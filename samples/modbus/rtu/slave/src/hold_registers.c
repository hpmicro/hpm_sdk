/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "agile_modbus.h"
#include "agile_modbus_slave_util.h"

static uint16_t _tab_registers[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

static int get_map_buf(void *buf, int bufsz)
{
    (void)bufsz;
    uint16_t *ptr = (uint16_t *)buf;
    for (uint32_t i = 0; i < sizeof(_tab_registers) / sizeof(_tab_registers[0]); i++) {
        ptr[i] = _tab_registers[i];
    }
    return 0;
}

static int set_map_buf(int index, int len, void *buf, int bufsz)
{
    (void)bufsz;
    uint16_t *ptr = (uint16_t *)buf;
    for (int i = 0; i < len; i++) {
        _tab_registers[index + i] = ptr[index + i];
    }
    return 0;
}

const agile_modbus_slave_util_map_t hold_register_maps[1] = {
    {0x00, 0x09, get_map_buf, set_map_buf}};
