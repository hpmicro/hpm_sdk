/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_CANOPEN_CAN_H
#define HPM_CANOPEN_CAN_H

#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_can_drv.h"
#include "hpm_ppor_drv.h"
#include "can.h"
#include "CO_driver.h"
#include "canopen_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HPM_CAN_FILTER_NUM_MAX (16U)

typedef struct can_info {
    CAN_Type *can_base;
    uint32_t irq_num;
    uint32_t priority;
} can_info_t;

typedef struct hpm_master_receive_buf {
    uint8_t has_received_message;
    can_receive_buf_t rx_buf;
} hpm_master_receive_buf_t;

typedef struct hpm_can_config {
    CAN_Type *base;
} hpm_can_config_t;

typedef struct hpm_can_data {
    can_config_t config;
    uint32_t can_filter_count;
    can_filter_config_t filter_list[HPM_CAN_FILTER_NUM_MAX];

    /* RX callback */
    can_rx_callback_t rx_cb[HPM_CAN_FILTER_NUM_MAX];
    void *rx_cb_arg[HPM_CAN_FILTER_NUM_MAX];

    uint32_t filter_rtr;
    uint32_t filter_rtr_mask;

    volatile bool has_sent_out;
    bool started;
} hpm_can_data_t;

int hpm_can_add_rx_filter(const struct device *dev, can_rx_callback_t callback, void *user_data, const struct can_filter *filter);
void hpm_can_remove_rx_filter(const struct device *dev, int filter_id);
int hpm_can_start(const struct device *dev);
int hpm_can_get_state(const struct device *dev, enum can_state *state, struct can_bus_err_cnt *err_cnt);
int hpm_can_send(const struct device *dev, const struct can_frame *frame);
void canopen_irq_handler(struct device *canopendevice);
void hpm_can_init(struct canopen_context *candriverState, can_info_t *can_info, uint32_t baudrate, int index);
int hpm_can_stop(const struct device *dev);
void hpm_canopen_reboot(void);

#ifdef __cplusplus
}
#endif

#endif /* HPM_CANOPEN_CAN_H */