/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_canopen.h"
#include "hpm_ppor_drv.h"

struct hpm_master_receive_buf {
    uint8_t has_received_message;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    mcan_rx_message_t rx_buf;
#else
    can_receive_buf_t rx_buf;
#endif
};

void hpm_sys_reboot(void);
uint32_t find_slave_device(void);
CO_ReturnError_t set_slave_device_mode(CO_CANmodule_t *CANmodule);
void PDO_test(uint32_t node_id);
void SDO_test(CO_CANmodule_t *CANmodule, uint32_t node_id);