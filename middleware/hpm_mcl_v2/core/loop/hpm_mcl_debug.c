/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_mcl_debug.h"

uint8_t mcl_debug_fifo[MCL_DEBUG_FIFO + 2] = {0xaa, 0xbb};

void hpm_mcl_debug_init(mcl_debug_t *debug)
{
    debug->cfg.data.addr = &mcl_debug_fifo[2];
}

void hpm_mcl_debug_send_scope(mcl_debug_t *debug)
{
    debug->cfg.callback.write_buf(mcl_debug_fifo, debug->cfg.cnt * MCL_DEBUG_DATA_TYPE_GET_SIZE(debug->cfg.data_type) + 2);
}

void hpm_mcl_debug_update_data(mcl_debug_t *debug, uint8_t chn, void *data, mcl_debug_data_type_t type)
{
    switch (type) {
    case mcl_debug_data_i8:
        debug->cfg.data.i8[chn] = *(int8_t *)data;
        break;
    case mcl_debug_data_i16:
        debug->cfg.data.i16[chn] = *(int16_t *)data;
        break;
    case mcl_debug_data_i32:
        debug->cfg.data.i32[chn] = *(int32_t *)data;
        break;
    case mcl_debug_data_f32:
        debug->cfg.data.f32[chn] = *(float *)data;
        break;
    }
}
