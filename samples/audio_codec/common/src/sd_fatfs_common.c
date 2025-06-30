/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------
 */
#include "board.h"
#include "ff.h"
#include "diskio.h"
#include "sd_fatfs_common.h"

/*---------------------------------------------------------------------*
 * Define
 *---------------------------------------------------------------------
 */
#define FIL_SEARCH_LENGTH 128

/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
static FATFS s_sd_disk;
static const TCHAR c_driver_num_buf[4] = { DEV_SD + '0', ':', '/', '\0'};
static uint8_t s_search_file_cnt;
static char s_search_file_buff[FIL_SEARCH_NUM][FIL_SEARCH_LENGTH];

/*---------------------------------------------------------------------*
 * Define function
 *---------------------------------------------------------------------
 */
/*---------------------------------------------------------------------*
 * SD FatFs module reads image file data
 *---------------------------------------------------------------------
 */
FRESULT sd_mount_fs(void)
{
    FRESULT fresult;

    fresult = f_mount(&s_sd_disk, c_driver_num_buf, 1);
    if (fresult == FR_OK) {
        printf("SD card has been mounted successfully\n");
    } else {
        printf("Failed to mount SD card, error code:%d\n", fresult);
        return fresult;
    }
    /* change to newly mounted drive */
    fresult = f_chdrive(c_driver_num_buf);
    /* set root directory as current directory */
    f_chdir("/");
    return fresult;
}

FRESULT sd_choose_music(char *target_filetype, char *filename)
{
    DIR dir;
    FILINFO fil;
    FRESULT rsl;
    char *ret;
    char option = '0';

    rsl = f_opendir(&dir, c_driver_num_buf);
    if (rsl != FR_OK) {
        return rsl;
    }

    printf("\r\n\r\n***********Music List**********\r\n");
    printf("\r\n**Enter any non-numeric key to change pages or music number to choose music**\r\n");
    printf("\r\n\r\n***********Page**********\r\n");
    s_search_file_cnt = 0;
    do {
        rsl = f_readdir(&dir, &fil);
        if ((rsl != FR_OK) || (fil.fname[0] == 0)) {
            break;
        }
        if (fil.fattrib == AM_DIR) {
            continue;
        }
        ret = strstr(fil.fname, target_filetype);
        if (ret == NULL) {
            continue;
        } else {
            strncpy((char *)s_search_file_buff[s_search_file_cnt], fil.fname, FIL_SEARCH_LENGTH);
            printf("%d: %s\r\n\r\n", s_search_file_cnt, s_search_file_buff[s_search_file_cnt]);
            s_search_file_cnt++;
        }
        if (s_search_file_cnt >= 10) {
            option = getchar();
            if (('0' <= option) && (option <= '9')) { /* choose music */
                break;
            } else { /* change page */
                memset(s_search_file_buff, 0, sizeof(s_search_file_buff));
                s_search_file_cnt = 0;
                printf("\r\n\r\n***********Page**********\r\n");
            }
        }
    } while (1);

    rsl = FR_NO_FILE;
    if ((s_search_file_cnt != 10) && (s_search_file_cnt != 0)) { /* choose remaining music */
        option = getchar();
    }
    if (('0' <= option) && (option <= '9')) {
        printf("%c\r\n\r\n", option);
        strncpy(filename, (char *)s_search_file_buff[option - '0'], FIL_SEARCH_LENGTH);
        rsl = FR_OK;
    }

    return rsl;
}

FRESULT sd_search_music_name(char *target_filetype1, char *target_filetype2)
{
    DIR dir;
    FILINFO fil;
    FRESULT rsl;
    char *ret1, *ret2;

    rsl = f_opendir(&dir, c_driver_num_buf);
    if (rsl != FR_OK) {
        return rsl;
    }

    printf("\r\n\r\n***********Music List**********\r\n");
    s_search_file_cnt = 0;
    do {
        rsl = f_readdir(&dir, &fil);
        if ((rsl != FR_OK) || (fil.fname[0] == 0)) {
            break;
        }
        if (fil.fattrib == AM_DIR) {
            continue;
        }
        ret1 = strstr(fil.fname, target_filetype1);
        ret2 = strstr(fil.fname, target_filetype2);
        if ((ret1 == NULL) && (ret2 == NULL)) {
            continue;
        } else {
            strncpy((char *)s_search_file_buff[s_search_file_cnt], fil.fname, FIL_SEARCH_LENGTH);
            s_search_file_cnt++;
        }
        if (s_search_file_cnt >= FIL_SEARCH_NUM) {
            break;
        }
    } while (1);

    if ((s_search_file_cnt > 0) && (rsl == FR_OK)) {
        rsl = FR_OK;
    } else {
        rsl = FR_NO_FILE;
    }

    return rsl;
}

uint8_t sd_get_search_file_cnt(void)
{
    return s_search_file_cnt;
}

char *sd_get_search_file_buff_ptr(uint8_t index)
{
    return (char *)s_search_file_buff[index];
}
