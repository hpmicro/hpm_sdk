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
#include "board.h"
#include "hpm_l1c_drv.h"

/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
static ATTR_PLACE_AT_NONCACHEABLE_BSS FIL s_file;
ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE) static uint32_t s_file_aligned_buf[FILE_BUF_SIZE / sizeof(uint32_t)];

/*---------------------------------------------------------------------*
 * File data store a file type
 *---------------------------------------------------------------------
 */

static bool file_write_unaligned(FIL *fp, uint8_t *buf, uint32_t size)
{
    FRESULT stat;
    UINT wlen;
    uint32_t w_size;
    uint32_t remain_size = size;
    uint32_t aligned_start = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)s_file_aligned_buf);

    for (uint32_t len = 0; len < size;) {
        w_size = FILE_BUF_SIZE > remain_size ? remain_size : FILE_BUF_SIZE;
        memcpy(s_file_aligned_buf, buf + len, w_size);

        /*
         * if w_size < FILE_SECTOR_SZIE, we need to write back the cache line before writing to the file.
         * FIL.buf[FF_MAX_SS] be used to store the last sector data and FIL is stored in noncacheable memory
         * so we needn't to write back the cache line.
         * data in boundary of cache line and less than a cache line size will be written by FIL.buf[FF_MAX_SS],
         * so we don't care cache.
         */
        if (w_size > FILE_SECTOR_SZIE)
            l1c_dc_writeback(aligned_start, HPM_L1C_CACHELINE_ALIGN_DOWN(w_size));

        stat = f_write(fp, s_file_aligned_buf, w_size, &wlen);
        if (stat != FR_OK || wlen != w_size) {
            printf("fail to write file, status = %d, w_size = %d\n", stat, w_size);
            return false;
        }
        len += w_size;
        remain_size -= w_size;
    }

    return true;
}

static bool file_write_aligned(FIL *fp, uint8_t *buf, uint32_t size)
{
    FRESULT stat;
    UINT len;

    uint32_t aligned_start = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)buf);

    if (size > FILE_SECTOR_SZIE)
        l1c_dc_writeback(aligned_start, HPM_L1C_CACHELINE_ALIGN_DOWN(size));

    stat = f_write(fp, buf, size, &len);
    if (stat != FR_OK || len != size) {
        printf("fail to write file, status = %d, len = %d\n", stat, len);
        return false;
    }

    return true;
}

static bool file_read_unaligned(FIL *fp, uint8_t *buf, uint32_t size)
{
    FRESULT stat;
    UINT rlen = 0;
    uint32_t r_size;
    uint32_t remain_size = size;
    uint32_t aligned_start = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)s_file_aligned_buf);

    for (uint32_t len = 0; len < size;) {
        r_size = FILE_BUF_SIZE > remain_size ? remain_size : FILE_BUF_SIZE;

        /*
         * if r_size < FILE_SECTOR_SZIE, we need to invalidate the cache line before reading to the file.
         * FIL.buf[FF_MAX_SS] be used to store the last sector data and FIL is stored in noncacheable memory
         * so we needn't to invalidate the cache line.
         * data in boundary of cache line and less than a cache line size will be copy from FIL.buf[FF_MAX_SS],
         * so we don't care cache.
         */
        if (r_size > FILE_SECTOR_SZIE)
            l1c_dc_invalidate(aligned_start, HPM_L1C_CACHELINE_ALIGN_DOWN(r_size));

        stat = f_read(fp, s_file_aligned_buf, r_size, &rlen);
        if (stat != FR_OK || rlen != r_size) {
            printf("fail to read file, status = %d, r_size = %d\n", stat, r_size);
            return false;
        }

        memcpy(buf + len, s_file_aligned_buf, r_size);
        len += r_size;
        remain_size -= r_size;
    }

    return true;
}

static bool file_read_aligned(FIL *fp, uint8_t *buf, uint32_t size)
{
    FRESULT stat;
    UINT len;

    uint32_t aligned_start = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)buf);

    if (size > FILE_SECTOR_SZIE)
        l1c_dc_invalidate(aligned_start, HPM_L1C_CACHELINE_ALIGN_DOWN(size));

    stat = f_read(fp, buf, size, &len);
    if (stat != FR_OK || len != size) {
        printf("fail to read file, status = %d, len = %d\n", stat, len);
        return false;
    }

    return true;
}

bool file_store(char *fname, uint8_t *buf, uint32_t size)
{
    FRESULT stat;
    bool ret;

    stat = f_open(&s_file, (const TCHAR *) fname, FA_WRITE | FA_OPEN_ALWAYS);
    if (stat == FR_OK) {
        if ((uint32_t)buf % HPM_L1C_CACHE_SIZE)
            ret = file_write_unaligned(&s_file, buf, size);
        else
            ret = file_write_aligned(&s_file, buf, size);

        if (!ret) {
            return false;
        } else {
            f_close(&s_file);
            printf("%s is stored, %d bytes.\n", fname, size);
            return true;
        }
        return true;
    } else {
        printf("fail to open file %s, status = %d\n", fname, stat);
        return false;
    }
}

bool file_restore(char *fname, uint8_t *buf, uint32_t size)
{
    FRESULT stat;
    bool ret;

    stat = f_open(&s_file, (const TCHAR *) fname, FA_READ);
    if (stat == FR_OK) {
        if ((uint32_t)buf % HPM_L1C_CACHE_SIZE)
            ret = file_read_unaligned(&s_file, buf, size);
        else
            ret = file_read_aligned(&s_file, buf, size);
        if (!ret)
            return false;
        else {
            f_close(&s_file);
            printf("%s is read, %d bytes.\n", fname, size);
            return true;
        }
        return true;
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
