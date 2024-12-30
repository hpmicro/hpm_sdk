/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "flashstress_lib.h"
#include <stdio.h>
#include <string.h>

/****************************************************************/
#include <board.h>
#include "hpm_clock_drv.h"
#include "hpm_mchtmr_drv.h"

static uint64_t interface_get_tick_us(void)
{
    static int is_inited = 0;
    static uint32_t div;
    uint32_t freq = 0;
    uint64_t tick_us;

    if (!is_inited) {
        is_inited = 1;
        board_ungate_mchtmr_at_lp_mode();
        freq = clock_get_frequency(clock_mchtmr0);
        div = freq / 1000000;
    }

    tick_us = mchtmr_get_count(HPM_MCHTMR) / div;
    return tick_us;
}
/****************************************************************/


static uint64_t tick_elapesd(uint64_t start, uint64_t current)
{
    uint64_t tick;
    if (start >= current) {
        tick = start - current;
    } else {
        tick = current + (0xffffffffffffffffull - start);
    }
    return tick;
}

#define FLASHSTRESS_LOG(...) printf(__VA_ARGS__)
#define CONFIG_CONTEXT_NUM 1
#define CONFIG_DATA_BUF_SIZE 4096
#define CONFIG_NAME_LEN 64

struct flashstress_context {
    int is_used;
    char name[CONFIG_NAME_LEN + 1];
    struct flashstress_driver drv;
    uint8_t data_buf[CONFIG_DATA_BUF_SIZE];
};

static struct flashstress_context flashstress_ctxs[CONFIG_CONTEXT_NUM];

static struct flashstress_context *flashstress_context_alloc(void)
{
    struct flashstress_context *ctx = NULL;

    for (int i = 0; i < CONFIG_CONTEXT_NUM; i++) {
        if (!flashstress_ctxs[i].is_used) {
            flashstress_ctxs[i].is_used = 1;
            ctx = &flashstress_ctxs[i];
            break;
        }
    }

    return ctx;
}

static void flashstress_context_free(struct flashstress_context *ctx)
{
    if (!ctx) {
        return;
    }

    ctx->is_used = 0;
}

struct flashstress_context *flashstress_create(struct flashstress_driver *drv, const char *name)
{
    struct flashstress_context *ctx = NULL;

    if (!drv || !name || !drv->erase_block || !drv->read ||
        !drv->write || !drv->read ||
        !drv->get_flash_block_size_bytes ||
        !drv->get_flash_chip_size_bytes) {
        FLASHSTRESS_LOG("FLASHSTRESS: ERR: drv is invalid!!!\n");
        return ctx;
    }

    if (!drv->erase_chip) {
        FLASHSTRESS_LOG("[%s]: WARNNING: <erase chip> api invalid!\n", name);
    }

    ctx = flashstress_context_alloc();
    if (!ctx) {
        FLASHSTRESS_LOG("FLASHSTRESS: ERROR: [%s] context alloc failed!!!\n", name);
    }

    ctx->drv = *drv;
    strncpy(ctx->name, name, CONFIG_NAME_LEN);

    return ctx;
}

void flashstress_destroy(struct flashstress_context *ctx)
{
    flashstress_context_free(ctx);
}

int flashstress_erase_chip(struct flashstress_context *ctx)
{
    struct flashstress_driver *drv = &ctx->drv;
    uint32_t chip_size_bytes = drv->get_flash_chip_size_bytes();
    uint32_t block_size_bytes = drv->get_flash_block_size_bytes();
    uint8_t erase_value = drv->get_flash_erase_value();
    uint32_t offset;
    uint32_t i;
    uint64_t erase_time_us;
    uint64_t start_time_us;
    float erase_time_s;

    if (drv->erase_chip) {
        start_time_us = interface_get_tick_us();
        if (drv->erase_chip()) {
            FLASHSTRESS_LOG("[%s]: ERROR: <erase_chip> FAILED!!!\n");
            return -1;
        }
        erase_time_us = tick_elapesd(start_time_us, interface_get_tick_us());
    } else {
        FLASHSTRESS_LOG("[%s]: WARNNING: <erase chip> api invalid, <erase block> api to be used!\n", ctx->name);
        offset = 0;
        start_time_us = interface_get_tick_us();
        while (offset < chip_size_bytes) {
            if (drv->erase_block(offset)) {
                FLASHSTRESS_LOG("[%s]: ERROR: <erase_block> offset: 0x%X FAILED!!!\n", ctx->name, offset);
                return -1;
            }
            offset += block_size_bytes;
        }
        erase_time_us = tick_elapesd(start_time_us, interface_get_tick_us());
    }

    /*
     * verify erase
     */
    offset = 0;
    while (offset < chip_size_bytes) {
        memset(ctx->data_buf, !erase_value, CONFIG_DATA_BUF_SIZE);
        if (drv->read(offset, ctx->data_buf, CONFIG_DATA_BUF_SIZE)) {
            FLASHSTRESS_LOG("[%s]: ERROR: <erase verify read api> offset: 0x%X FAILED!!!\n", ctx->name, offset);
            return -1;
        }

        for (i = 0; i < CONFIG_DATA_BUF_SIZE; i++) {
            if (ctx->data_buf[i] != erase_value) {
                FLASHSTRESS_LOG("[%s]: ERROR: <ease verify data> 0x%X!= 0x%X FAILED!!!\n", ctx->name, ctx->data_buf[i], erase_value);
                return -1;
            }
        }
        offset += CONFIG_DATA_BUF_SIZE;
    }

    erase_time_s = erase_time_us / 1000000.0f;
    FLASHSTRESS_LOG("[%s]: [erase_chip] PASSED!!!\n", ctx->name);
    FLASHSTRESS_LOG("[%s]: [erase_chip] take %f seconds!!!\n", ctx->name, erase_time_s);

    return 0;
}

int flashstress_erase_all_blocks(struct flashstress_context *ctx)
{
    struct flashstress_driver *drv = &ctx->drv;
    uint32_t chip_size_bytes = drv->get_flash_chip_size_bytes();
    uint32_t block_size_bytes = drv->get_flash_block_size_bytes();
    uint8_t erase_value = drv->get_flash_erase_value();
    uint32_t offset;
    uint32_t i;
    uint64_t erase_time_us;
    uint64_t start_time_us;
    float erase_time_s;

    offset = 0;
    start_time_us = interface_get_tick_us();
    while (offset < chip_size_bytes) {
        if (drv->erase_block(offset)) {
            FLASHSTRESS_LOG("[%s]: ERROR: <erase_block> offset: 0x%X FAILED!!!\n", offset);
            return -1;
        }
        offset += block_size_bytes;
    }
    erase_time_us = tick_elapesd(start_time_us, interface_get_tick_us());
    erase_time_s = erase_time_us / 1000000.0f;

    /*
     * verify erase
     */
    offset = 0;
    while (offset < chip_size_bytes) {
        memset(ctx->data_buf, !erase_value, CONFIG_DATA_BUF_SIZE);
        if (drv->read(offset, ctx->data_buf, CONFIG_DATA_BUF_SIZE)) {
            FLASHSTRESS_LOG("[%s]: ERROR: <erase verify read api> offset: 0x%X FAILED!!!\n", ctx->name, offset);
            return -1;
        }

        for (i = 0; i < CONFIG_DATA_BUF_SIZE; i++) {
            if (ctx->data_buf[i] != erase_value) {
                FLASHSTRESS_LOG("[%s]: ERROR: <ease verify data> offset: 0x%X FAILED!!!\n", ctx->name, offset);
                return -1;
            }
        }

        offset += CONFIG_DATA_BUF_SIZE;
    }

    FLASHSTRESS_LOG("[%s]: [erase all blocks] PASSED\n", ctx->name);
    FLASHSTRESS_LOG("[%s]: [erase all blocks] take time: %f seconds\n", ctx->name, erase_time_s);

    return 0;
}

int flashstress_write_read(struct flashstress_context *ctx, uint8_t verify_data)
{
    struct flashstress_driver *drv = &ctx->drv;
    uint32_t chip_size_bytes = drv->get_flash_chip_size_bytes();
    uint32_t block_size_bytes = drv->get_flash_block_size_bytes();
    uint32_t offset;
    uint32_t i;
    uint64_t read_time_us;
    uint64_t write_time_us;
    uint64_t start_time_us;
    float read_time_s;
    float read_speed;
    float write_time_s;
    float write_speed;

    uint32_t write_len = block_size_bytes;

    while (1) {
        if (write_len <= CONFIG_DATA_BUF_SIZE) {
            break;
        } else {
            write_len /= 2;
        }
    }

    if (write_len == 0) {
        FLASHSTRESS_LOG("[%s]: ERROR: write len = %u!!!\n", ctx->name, write_len);
        return -1;
    }

    /*
     * write flash
     */
    memset(ctx->data_buf, verify_data, write_len);
    offset = 0;
    start_time_us = interface_get_tick_us();
    while (offset < chip_size_bytes) {
        if (drv->write(offset, ctx->data_buf, write_len)) {
            FLASHSTRESS_LOG("[%s]: ERROR: <write> offset: 0x%X FAILED!!!\n", ctx->name, offset);
            return -1;
        }
        offset += write_len;
    }
    write_time_us = tick_elapesd(start_time_us, interface_get_tick_us());
    write_time_s = write_time_us / 1000000.0f;
    write_speed = (write_time_us == 0) ? -1.0 : (chip_size_bytes / write_time_s / 1024.0 / 1024.0);

    /*
     * verify flash data
     */
    offset = 0;
    while (offset < chip_size_bytes) {
        memset(ctx->data_buf, 0xff, CONFIG_DATA_BUF_SIZE);
        if (drv->read(offset, ctx->data_buf, CONFIG_DATA_BUF_SIZE)) {
            FLASHSTRESS_LOG("[%s]: ERROR: <write verify read api> offset: 0x%X FAILED!!!\n", ctx->name, offset);
            return -1;
        }

        for (i = 0; i < CONFIG_DATA_BUF_SIZE; i++) {
            if (ctx->data_buf[i] != verify_data) {
                FLASHSTRESS_LOG("[%s]: ERROR: <write verify data> offset: 0x%X FAILED!!!\n", ctx->name, offset);
                return -1;
            }
        }

        offset += CONFIG_DATA_BUF_SIZE;
    }

    FLASHSTRESS_LOG("[%s]: [write] PASSED!!!\n", ctx->name);
    FLASHSTRESS_LOG("[%s]: [write] take time: %f, speed: %f MiB/s\n", ctx->name, write_time_s, write_speed);

    /*
     * only read speed test, don't care data
     */
    offset = 0;
    start_time_us = interface_get_tick_us();
    while (offset < chip_size_bytes) {
        if (drv->read(offset, ctx->data_buf, CONFIG_DATA_BUF_SIZE)) {
            FLASHSTRESS_LOG("[%s]: ERROR: <read speed read api> offset: 0x%X FAILED!!!\n", ctx->name, offset);
            return -1;
        }
        offset += CONFIG_DATA_BUF_SIZE;
    }
    read_time_us = tick_elapesd(start_time_us, interface_get_tick_us());
    read_time_s = read_time_us / 1000000.0f;
    read_speed = (read_time_us == 0) ? -1.0 : (chip_size_bytes / read_time_s / 1024.0 / 1024.0);

    /*
     * verify data
     */
    offset = 0;
    while (offset < chip_size_bytes) {
        memset(ctx->data_buf, 0xff, CONFIG_DATA_BUF_SIZE);
        if (drv->read(offset, ctx->data_buf, CONFIG_DATA_BUF_SIZE)) {
            FLASHSTRESS_LOG("[%s]: ERROR: <read verify read api> offset: 0x%X FAILED!!!\n", ctx->name, offset);
            return -1;
        }

        for (i = 0; i < CONFIG_DATA_BUF_SIZE; i++) {
            if (ctx->data_buf[i] != verify_data) {
                FLASHSTRESS_LOG("[%s]: ERROR: <read verify data> offset: 0x%X FAILED!!!\n", ctx->name, offset);
                return -1;
            }
        }

        offset += CONFIG_DATA_BUF_SIZE;
    }
    FLASHSTRESS_LOG("[%s]: [read] PASSED!!!\n", ctx->name);
    FLASHSTRESS_LOG("[%s]: [read] take time: %f, speed: %f MiB/s\n", ctx->name, read_time_s, read_speed);

    return 0;
}


int flashstress_run(struct flashstress_context *ctx)
{
    uint32_t chip_size_bytes = ctx->drv.get_flash_chip_size_bytes();
    uint32_t block_size_bytes = ctx->drv.get_flash_block_size_bytes();

    FLASHSTRESS_LOG("\n=============================================\n");
    FLASHSTRESS_LOG("[%s]: chip size: %u\n", ctx->name, chip_size_bytes);
    FLASHSTRESS_LOG("[%s]: block size: %u\n", ctx->name, block_size_bytes);
    FLASHSTRESS_LOG("=============================================\n");

    if (flashstress_erase_chip(ctx) < 0) {
        goto ERROR;
    }

    if (flashstress_write_read(ctx, 0x55) < 0) {
        goto ERROR;
    }

    if (flashstress_erase_all_blocks(ctx) < 0) {
        goto ERROR;
    }

    if (flashstress_write_read(ctx, 0xAA) < 0) {
        goto ERROR;
    }

    return 0;

ERROR:
    FLASHSTRESS_LOG("[%s]: FAILED\n\n", ctx->name);
    return -1;
}

