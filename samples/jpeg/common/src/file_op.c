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
#include <stdio.h>
#include "ff.h"
#include "file_op.h"
#include "hpm_common.h"
/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
static ATTR_PLACE_AT_NONCACHEABLE_BSS FIL s_file;

/*---------------------------------------------------------------------*
 * File data store a file type
 *---------------------------------------------------------------------
 */
bool file_store(uint8_t *fname, uint8_t *buf, uint32_t size)
{
    UINT cnt = 0;
    FRESULT stat;

    stat = f_open(&s_file, (const TCHAR *) fname, FA_WRITE | FA_OPEN_ALWAYS);
    if (stat == FR_OK) {
        f_write(&s_file, buf, size, &cnt);
        f_close(&s_file);
        printf("%s is opened, %d bytes.\n", fname, cnt);
        return true;
    } else {
        printf("fail to open file %s, status = %d\n", fname, stat);
        return false;
    }
}
