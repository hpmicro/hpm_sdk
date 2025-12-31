/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_CANOPEN_MCAN_H
#define HPM_CANOPEN_MCAN_H

#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_mcan_drv.h"
#include "hpm_ppor_drv.h"
#include "canopen_errno.h"
#include "can.h"
#include "CO_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HPM_CAN_EXT_FILTER_NUM_MAX (64U)
#define HPM_CAN_STD_FILTER_NUM_MAX (128U)

typedef struct hpm_can_config {
    MCAN_Type *base;
} hpm_can_config_t;

typedef struct can_info {
    MCAN_Type *can_base;
    uint32_t irq_num;
    uint32_t priority;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    uint32_t ram_base;
    uint32_t ram_size;
#endif
} can_info_t;

typedef struct hpm_master_receive_buf {
    uint8_t has_received_message;
    mcan_rx_message_t rx_buf;
} hpm_master_receive_buf_t;

typedef struct mcan_filter {
    uint16_t filters_index;
    mcan_filter_elem_t filters;
} mcan_filter_t;

typedef struct hpm_can_data {
    mcan_config_t config;
    uint32_t ext_filter_count;
    mcan_filter_t ext_filters[HPM_CAN_EXT_FILTER_NUM_MAX];
    uint32_t std_filter_count;
    mcan_filter_t std_filters[HPM_CAN_STD_FILTER_NUM_MAX];

    /* RX callback */
    can_rx_callback_t rx_cb_ext[HPM_CAN_EXT_FILTER_NUM_MAX];
    void *rx_cb_arg_ext[HPM_CAN_EXT_FILTER_NUM_MAX];
    can_rx_callback_t rx_cb_std[HPM_CAN_STD_FILTER_NUM_MAX];
    void *rx_cb_arg_std[HPM_CAN_STD_FILTER_NUM_MAX];

    uint32_t filter_rtr;
    uint32_t filter_rtr_mask;

    volatile bool has_sent_out;
    bool started;
} hpm_can_data_t;

int hpm_mcan_add_rx_filter(const struct device *dev, can_rx_callback_t callback, void *user_data, const struct can_filter *filter);
void hpm_mcan_remove_rx_filter(const struct device *dev, int filter_id);
int hpm_mcan_start(const struct device *dev);
int hpm_mcan_get_state(const struct device *dev, enum can_state *state, struct can_bus_err_cnt *err_cnt);
int hpm_mcan_send(const struct device *dev, const struct can_frame *frame);
void canopen_irq_handler(struct device *canopendevice);
int hpm_mcan_stop(const struct device *dev);
void hpm_mcan_init(struct canopen_context *candriverState, can_info_t *can_info, uint32_t baudrate, int index);
void hpm_canopen_reboot(void);

#ifdef __cplusplus
}
#endif

#endif /* HPM_CANOPEN_MCAN_H */