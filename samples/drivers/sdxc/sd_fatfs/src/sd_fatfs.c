/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_sdmmc_sd.h"
#include "ff.h"
#include "diskio.h"

extern sd_card_t g_sd;

FATFS s_sd_disk;
FIL s_file;
DIR s_dir;
FRESULT fatfs_result;
BYTE work[FF_MAX_SS];

const TCHAR driver_num_buf[3] = {DEV_SD + '0', ':', '/'};

#define TEST_DIR_NAME "hpmicro_sd_test_dir0"

void show_menu(void);

const char *show_error_string(FRESULT fresult);

static FRESULT sd_mount_fs(void);

static FRESULT sd_mkfs(void);

static FRESULT sd_write_file(void);

static FRESULT sd_read_file(void);

static FRESULT sd_dir_test(void);

static FRESULT sd_big_file_test(void);

int main(void)
{
    board_init();

    show_menu();

    fatfs_result = sd_mount_fs();
    if (fatfs_result == FR_NO_FILESYSTEM) {
        printf("There is no File system available, making file system...\n");
        fatfs_result = sd_mkfs();
    }

    while (1) {
        char option = getchar();

        switch (option) {
        case '1':
            fatfs_result = sd_mkfs();
            break;
        case '2':
            fatfs_result = sd_write_file();
            break;
        case '3':
            fatfs_result = sd_read_file();
            break;
        case '4':
            fatfs_result = sd_dir_test();
            break;
        case 's':
            fatfs_result = sd_big_file_test();
            break;
        default:
            show_menu();
            break;
        }
    }

}

void show_menu(void)
{
    const char menu_str[] = "SD FATFS demo\n-----------------------------------\n"
                            "1 - Format the SD card with FATFS\n"
                            "2 - Create hello.txt\n"
                            "3 - Read 1st line from hello.txt\n"
                            "4 - Directory related test\n"
                            "s - Large file write test\n"
                            "Others - Show menu\n";

    printf(menu_str);
}

static FRESULT sd_mount_fs(void)
{
    FRESULT fresult = f_mount(&s_sd_disk, driver_num_buf, 1);
    if (fresult == FR_OK) {
        printf("SD card has been mounted successfully\n");
    } else {
        printf("Failed to mount SD card, cause: %s\n", show_error_string(fresult));
    }

    fresult = f_chdrive(driver_num_buf);
    return fresult;
}

static FRESULT sd_mkfs(void)
{
    printf("Formatting the SD card, depending on the SD card capacity, the formatting process may take a long time\n");
    FRESULT fresult = f_mkfs(driver_num_buf, NULL, work, sizeof(work));
    if (fresult != FR_OK) {
        printf("Making File system failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Making file system is successful\n");
    }

    return fresult;
}

static FRESULT sd_write_file(void)
{
    FRESULT fresult = f_open(&s_file, "readme.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (fresult != FR_OK) {
        printf("Create new file failed, cause: %d\n", show_error_string(fresult));
    } else {
        printf("Create new file successfully, status=%d\n", fresult);
    }
    char hello_str[] = "Hello, this is SD card FATFS demo\n";
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
    FRESULT fresult = f_open(&s_file, "big_file.bin", FA_WRITE | FA_CREATE_ALWAYS);
    if (fresult != FR_OK) {
        printf("Create new file failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Create new file successfully\n");
    }

    uint32_t write_size = 1024UL * 1024UL * 100UL;
    static uint8_t buf[32768];
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
