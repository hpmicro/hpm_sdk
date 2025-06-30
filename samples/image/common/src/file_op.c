/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------
 */
#include <stdio.h>
#include "ff.h"
#include "file_op.h"
#include "hpm_common.h"
#include "board.h"
#include "hpm_l1c_drv.h"

/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
static FIL s_file;

bool file_store(char *fname, uint8_t *buf, uint32_t size)
{
    UINT cnt = 0;
    FRESULT stat;

    stat = f_open(&s_file, (const TCHAR *) fname, FA_WRITE | FA_CREATE_ALWAYS);
    if (stat == FR_OK) {
        stat = f_write(&s_file, buf, size, &cnt);
        f_close(&s_file);
        if (stat != FR_OK || size != cnt) {
            printf("fail to stored file, status = %d, len = %d\n", stat, cnt);
            return false;
        } else {
            printf("%s is stored, %d bytes.\n", fname, cnt);
            return true;
        }
    } else {
        printf("fail to open file %s, status = %d\n", fname, stat);
        return false;
    }
}

bool file_restore(char *fname, uint8_t *buf, uint32_t size)
{
    UINT cnt = 0;
    FRESULT stat;

    stat = f_open(&s_file, (const TCHAR *) fname, FA_READ);
    if (stat == FR_OK) {
        stat = f_read(&s_file, buf, size, &cnt);
        f_close(&s_file);
        if (stat != FR_OK || size != cnt) {
            printf("fail to read file, status = %d, len = %d\n", stat, cnt);
            return false;
        } else {
            printf("%s is read, %d bytes.\n", fname, cnt);
            return true;
        }
    } else {
        printf("fail to open file %s, status = %d\n", fname, stat);
        return false;
    }
}

uint32_t file_get_size(char *fname)
{
    FRESULT stat;
    uint32_t size = 0;

    stat = f_open(&s_file, fname, FA_READ);
    if (stat != FR_OK) {
        return 0;
    }

    size = f_size(&s_file);
    f_close(&s_file);

    return size;
}
