/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_spi_sdcard.h"
#include "spi_sd_adapt.h"
#if defined(USE_DMA_TRANSFER) && (USE_DMA_TRANSFER == 1)
#include "hpm_dma_mgr.h"
#endif
#include "ff.h"
#include "diskio.h"

typedef enum {
    create_txt    = 1,
    read_1line_txt,
    dire_test,
    large_file_write,
} fatfs_test_e;

typedef struct {
    uint8_t num;
    char *num_str;
} test_number_t;

#define TEST_DIR_NAME "hpmicro_sd_test_dir0"

static void fatfs_test(fatfs_test_e num);
static FRESULT sd_mount_fs(void);
static FRESULT sd_write_file(void);
static FRESULT sd_read_file(void);
static FRESULT sd_dir_test(void);
static FRESULT sd_big_file_test(void);
const char *show_error_string(FRESULT fresult);
static void show_help(void);

FATFS s_sd_disk;
FIL s_file;
FRESULT fatfs_result;

const TCHAR driver_num_buf[4] = {DEV_SD + '0', ':', '/', '\0'};

const test_number_t test_table[] = {
    {create_txt,                "*        1 - Create hello.txt                                 *\n"},
    {read_1line_txt,            "*        2 - Read 1st line from hello.txt                     *\n"},
    {dire_test,                 "*        3 - Directory related test                           *\n"},
    {large_file_write,          "*        4 - Large file write test                            *\n"},
};

int main(void)
{
    hpm_stat_t sta;
    board_init();
#if defined(USE_DMA_TRANSFER) && (USE_DMA_TRANSFER == 1)
    dma_mgr_init();
#endif
    sta = spi_sd_init();
    if (sta != status_success) {
        printf("spi sdcard init fail!\n");
        while (1) {
        }
    }

    show_help();
    fatfs_result = sd_mount_fs();
    if (fatfs_result == FR_NO_FILESYSTEM) {
        printf("There is no File system available\n");
        while (1) {
        }
    }

    while (true) {
        char option = getchar();
        int i;
        int table_count = sizeof(test_table) / sizeof(test_number_t);
        fatfs_test_e num = (option - '0');
        putchar(option);
        putchar('\n');
        for (i = 0; i < table_count; i++) {
            if (num == test_table[i].num) {
                fatfs_test(num);
                break;
            }
        }
        if (i >= table_count) {
            show_help();
        }
    }
    return 0;
}

static void fatfs_test(fatfs_test_e num)
{
    switch (num) {
    case create_txt:
        fatfs_result = sd_write_file();
        break;
    case read_1line_txt:
        fatfs_result = sd_read_file();
        break;
    case dire_test:
        fatfs_result = sd_dir_test();
        break;
    case large_file_write:
        fatfs_result = sd_big_file_test();
        break;
    default:
        break;
    }
}

static FRESULT sd_mount_fs(void)
{
    FRESULT fresult = f_mount(&s_sd_disk, driver_num_buf, 1);
    if (fresult == FR_OK) {
        printf("SD card has been mounted successfully\n");
        fresult = f_chdrive(driver_num_buf);
    } else {
        printf("Failed to mount SD card, cause: %s\n", show_error_string(fresult));
    }
    return fresult;
}

static FRESULT sd_write_file(void)
{
    FRESULT fresult = f_open(&s_file, "readme.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (fresult != FR_OK) {
        printf("Create new file failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Create new file successfully, status=%d\n", fresult);
    }
    char hello_str[] = "Hello, this is SPI SD card FATFS demo\n";
    UINT byte_written;
    fresult = f_write(&s_file, hello_str, sizeof(hello_str), &byte_written);
    if (fresult != FR_OK) {
        printf("Write file failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Write file　operation is successfully\n");
    }

    f_close(&s_file);

    return fresult;
}

static FRESULT sd_read_file(void)
{
    FRESULT fresult = f_open(&s_file, "readme.txt", FA_READ);
    if (fresult != FR_OK) {
        printf("Open file failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Open file successfully\n");
    }

    if (fresult != FR_OK) {
        return fresult;
    }

    TCHAR str[100];
    f_gets(str, sizeof(str), &s_file);
    printf("%s\n", str);

    f_close(&s_file);

    return fresult;
}

static FRESULT sd_big_file_test(void)
{
    ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint8_t buf[4096];
    FRESULT fresult = f_open(&s_file, "big_file.bin", FA_WRITE | FA_CREATE_ALWAYS);
    if (fresult != FR_OK) {
        printf("Create new file failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Create new file successfully, now writing.....\n");
    }

    uint32_t write_size = 1024UL * 1024UL * 5UL;

    for (uint32_t i = 0; i < sizeof(buf); i++) {
        buf[i] = i & 0xFF;
    }
    while (write_size > 0) {
        UINT byte_written;
        fresult = f_write(&s_file, buf, sizeof(buf), &byte_written);
        if (fresult != FR_OK) {
            printf("Write file failed, cause: %s\n", show_error_string(fresult));
            return fresult;
        }

        write_size -= byte_written;
    }
    printf("Write file operation is successful\n");

    f_close(&s_file);

    return fresult;
}


static FRESULT sd_dir_test(void)
{
    FRESULT fresult = f_mkdir(TEST_DIR_NAME);
    if (fresult != FR_OK) {
        printf("Creating new directory failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Creating new directory succeeded\n");
    }

    fresult = f_rmdir(TEST_DIR_NAME);
    if (fresult != FR_OK) {
        printf("Removing new directory failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Removing new directory succeeded\n");
    }

    return fresult;
}

const char *show_error_string(FRESULT fresult)
{
    const char *result_str;

    switch (fresult) {
    case FR_OK:
        result_str = "succeeded";
        break;
    case FR_DISK_ERR:
        result_str = "A hard error occurred in the low level disk I/O level";
        break;
    case FR_INT_ERR:
        result_str = "Assertion failed";
        break;
    case FR_NOT_READY:
        result_str = "The physical drive cannot work";
        break;
    case FR_NO_FILE:
        result_str = "Could not find the file";
        break;
    case FR_NO_PATH:
        result_str = "Could not find the path";
        break;
    case FR_INVALID_NAME:
        result_str = "Tha path name format is invalid";
        break;
    case FR_DENIED:
        result_str = "Access denied due to prohibited access or directory full";
        break;
    case FR_EXIST:
        result_str = "Access denied due to prohibited access";
        break;
    case FR_INVALID_OBJECT:
        result_str = "The file/directory object is invalid";
        break;
    case FR_WRITE_PROTECTED:
        result_str = "The physical drive is write protected";
        break;
    case FR_INVALID_DRIVE:
        result_str = "The logical driver number is invalid";
        break;
    case FR_NOT_ENABLED:
        result_str = "The volume has no work area";
        break;
    case FR_NO_FILESYSTEM:
        result_str = "There is no valid FAT volume";
        break;
    case FR_MKFS_ABORTED:
        result_str = "THe f_mkfs() aborted due to any problem";
        break;
    case FR_TIMEOUT:
        result_str = "Could not get a grant to access the volume within defined period";
        break;
    case FR_LOCKED:
        result_str = "The operation is rejected according to the file sharing policy";
        break;
    case FR_NOT_ENOUGH_CORE:
        result_str = "LFN working buffer could not be allocated";
        break;
    case FR_TOO_MANY_OPEN_FILES:
        result_str = "Number of open files > FF_FS_LOCK";
        break;
    case FR_INVALID_PARAMETER:
        result_str = "Given parameter is invalid";
        break;
    default:
        result_str = "Unknown error";
        break;
    }
    return result_str;
}

static void show_help(void)
{
    static const char help_info[] = "\n"
                                    "---------------------------------------------------------------\n"
                                    "*                                                             *\n"
                                    "*                   spi sdcard fatfs demo                     *\n"
                                    "*                                                             *\n";

    static const char help_info_ending[] = "*-------------------------------------------------------------*\n";

    printf("%s", help_info);
    for (uint32_t i = 0; i < sizeof(test_table) / sizeof(test_number_t); i++) {
        printf("%s", test_table[i].num_str);
    }
    printf("%s", help_info_ending);
}
