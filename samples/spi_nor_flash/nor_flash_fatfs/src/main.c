/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "ff.h"
#include "diskio.h"
#if defined(USE_FREERTOS) && USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "hpm_uart_drv.h"
#if SERIAL_NOR_USE_DMA_MGR
#include "hpm_dma_mgr.h"
#endif


typedef enum {
    create_txt    = 1,
    read_1line_txt,
    dire_test,
    large_file_write,
    format_fs,
} fatfs_test_e;

typedef struct {
    uint8_t num;
    char *num_str;
} test_number_t;

#define TEST_DIR_NAME "hpmicro_spi_nor_flash_test_dir0"

static FRESULT spi_nor_flash_mkfs(void);
static void fatfs_test(fatfs_test_e num);
static FRESULT spi_nor_flash_mount_fs(void);
static FRESULT spi_nor_flash_write_file(void);
static FRESULT spi_nor_flash_read_file(void);
static FRESULT spi_nor_flash_dir_test(void);
static FRESULT spi_nor_flash_big_file_test(void);
const char *show_error_string(FRESULT fresult);
static void show_help(void);
static void fatfs_task(void *param);
static int console_getchar_nonblocking(char *c);

ATTR_PLACE_AT_NONCACHEABLE FATFS s_spi_nor_flash_disk;
ATTR_PLACE_AT_NONCACHEABLE FIL s_file;
ATTR_PLACE_AT_NONCACHEABLE DIR s_dir;
FRESULT fatfs_result;
BYTE work[FF_MAX_SS];
const TCHAR driver_num_buf[4] = {DEV_SPI_NOR + '0', ':', '/', '\0'};

const test_number_t test_table[] = {
    {create_txt,                "*        1 - Create hello.txt                                 *\n"},
    {read_1line_txt,            "*        2 - Read 1st line from hello.txt                     *\n"},
    {dire_test,                 "*        3 - Directory related test                           *\n"},
    {large_file_write,          "*        4 - Large file write test                            *\n"},
    {format_fs,                 "*        5 - format fs system                            *\n"},
};



static void fatfs_task(void *param)
{
    (void)param;
    fatfs_result = spi_nor_flash_mount_fs();
    if (fatfs_result == FR_NO_FILESYSTEM) {
        spi_nor_flash_mkfs();
        fatfs_result = spi_nor_flash_mount_fs();
        if (fatfs_result == FR_NO_FILESYSTEM) {
            printf("There is no File system available\n");
#if defined(USE_FREERTOS) && USE_FREERTOS
            vTaskDelete(NULL);
#else
            return;
#endif
        }
    }

    show_help();
    while (true) {
        char option;
        if (!console_getchar_nonblocking(&option)) {
#if defined(USE_FREERTOS) && USE_FREERTOS
            vTaskDelay(pdMS_TO_TICKS(10));
#endif
            continue;
        }
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
}

void hpm_spi_nor_udelay(uint32_t us)
{
#if defined(USE_FREERTOS) && USE_FREERTOS
    /* Delay for a specified number of microseconds */
    vTaskDelay(pdMS_TO_TICKS(us));
#else
    board_delay_us(us);
#endif
}
int main(void)
{
    board_init();
#if SERIAL_NOR_USE_DMA_MGR
    dma_mgr_init();
#endif

#if defined(USE_FREERTOS) && USE_FREERTOS
    if (xTaskCreate(fatfs_task, "fatfs_task", 512, NULL, (tskIDLE_PRIORITY + 1), NULL) != pdPASS) {
        printf("Create fatfs_task failed\n");
        while (1) {
        }
    }

    vTaskStartScheduler();
#else
    fatfs_task(NULL);
#endif
    while (1) {
    }
    return 0;
}

static void fatfs_test(fatfs_test_e num)
{
    switch (num) {
    case create_txt:
        fatfs_result = spi_nor_flash_write_file();
        break;
    case read_1line_txt:
        fatfs_result = spi_nor_flash_read_file();
        break;
    case dire_test:
        fatfs_result = spi_nor_flash_dir_test();
        break;
    case large_file_write:
        fatfs_result = spi_nor_flash_big_file_test();
        break;
    case format_fs:
        fatfs_result = spi_nor_flash_mkfs();
        break;
    default:
        break;
    }
}

static FRESULT spi_nor_flash_mkfs(void)
{
    printf("Formatting the SPI Nor flash, depending on the SPI Nor flash capacity, the formatting process may take a long time\n");
    FRESULT fresult = f_mkfs(driver_num_buf, NULL, work, sizeof(work));
    if (fresult != FR_OK) {
        printf("Making File system failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Making file system is successful\n");
    }

    return fresult;
}

static FRESULT spi_nor_flash_mount_fs(void)
{
    FRESULT fresult = f_mount(&s_spi_nor_flash_disk, driver_num_buf, 1);
    if (fresult == FR_OK) {
        printf("SPI Nor flash has been mounted successfully\n");
        fresult = f_chdrive(driver_num_buf);
    } else {
        printf("Failed to mount SPI Nor flash, cause: %s\n", show_error_string(fresult));
    }
    return fresult;
}

static FRESULT spi_nor_flash_write_file(void)
{
    FRESULT fresult = f_open(&s_file, "readme.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (fresult != FR_OK) {
        printf("Create new file failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Create new file successfully, status=%d\n", fresult);
    }
    char hello_str[] = "Hello, this is SPI SPI Nor flash FATFS demo\n";
    UINT byte_written;
    fresult = f_write(&s_file, hello_str, sizeof(hello_str), &byte_written);
    if (fresult != FR_OK) {
        printf("Write file failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Write file operation is successfully\n");
    }

    f_close(&s_file);

    return fresult;
}

static FRESULT spi_nor_flash_read_file(void)
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

static FRESULT spi_nor_flash_big_file_test(void)
{
    FRESULT fresult = f_open(&s_file, "big_file.bin", FA_WRITE | FA_CREATE_ALWAYS);
    if (fresult != FR_OK) {
        printf("Create new file failed, cause: %s\n", show_error_string(fresult));
    } else {
        printf("Create new file successfully, now writing.....\n");
    }

    uint32_t write_size = 1024UL * 1024UL * 1UL;
    static uint8_t buf[4096];
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


static FRESULT spi_nor_flash_dir_test(void)
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
        result_str = "The path name format is invalid";
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
        result_str = "The f_mkfs() aborted due to any problem";
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
                                    "*                   spi nor flash fatfs demo                  *\n"
                                    "*                                                             *\n";

    static const char help_info_ending[] = "*-------------------------------------------------------------*\n";

    printf("%s", help_info);
    for (uint32_t i = 0; i < sizeof(test_table) / sizeof(test_number_t); i++) {
        printf("%s", test_table[i].num_str);
    }
    printf("%s", help_info_ending);
}

static int console_getchar_nonblocking(char *c)
{
    if (c == NULL) {
        return 0;
    }
    if (uart_receive_byte(BOARD_CONSOLE_UART_BASE, (uint8_t *)c) == status_success) {
        return 1;
    }
    return 0;
}

