/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_MCAN
#include "hpm_mcan_drv.h"
#include "hpm_canopen_mcan.h"
#else
#include "hpm_can_drv.h"
#include "hpm_canopen_can.h"
#endif
#include "can.h"
#include "CANopen.h"
#include "CO_driver.h"
#include "canopen_errno.h"
#include "hpm_ppor_drv.h"

#ifdef HPMSOC_HAS_HPMSDK_MCAN
#define HPM_CAN_EXT_FILTER_NUM_MAX (64U)
#define HPM_CAN_STD_FILTER_NUM_MAX (128U)
#define HPM_MCAN_NUM_TX_BUF_ELEMENTS (32U)
#define HPM_MCAN_NUM_RX_BUF_ELEMENTS (16U)
#else
#define HPM_CAN_NUM_TX_BUF_ELEMENTS (2U)
#define HPM_CAN_FILTER_NUM_MAX (16U)
#endif

struct hpm_master_receive_buf {
    uint8_t has_received_message;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    mcan_rx_message_t rx_buf;
#else
    can_receive_buf_t rx_buf;
#endif
};

struct hpm_can_config {
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    MCAN_Type *base;
#else
    CAN_Type *base;
#endif
};

struct hpm_can_data {
    struct can_driver_config common;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    mcan_config_t config;
    uint32_t ext_filter_count;
    uint16_t ext_filters_index[HPM_CAN_EXT_FILTER_NUM_MAX];
    mcan_filter_elem_t ext_filters[HPM_CAN_EXT_FILTER_NUM_MAX];
    uint32_t std_filter_count;
    uint16_t std_filters_index[HPM_CAN_STD_FILTER_NUM_MAX];
    mcan_filter_elem_t std_filters[HPM_CAN_STD_FILTER_NUM_MAX];

    /* RX callback */
    can_rx_callback_t rx_cb_ext[HPM_CAN_EXT_FILTER_NUM_MAX];
    void *rx_cb_arg_ext[HPM_CAN_EXT_FILTER_NUM_MAX];

    can_rx_callback_t rx_cb_std[HPM_CAN_STD_FILTER_NUM_MAX];
    void *rx_cb_arg_std[HPM_CAN_STD_FILTER_NUM_MAX];

        /* TX callback */
    can_tx_callback_t tx_fin_cb[HPM_MCAN_NUM_TX_BUF_ELEMENTS];
    void *tx_fin_cb_arg[HPM_MCAN_NUM_TX_BUF_ELEMENTS];
#else
    can_config_t config;
    uint32_t can_filter_count;
    can_filter_config_t filter_list[HPM_CAN_FILTER_NUM_MAX];

    /* RX callback */
    can_rx_callback_t rx_cb[HPM_CAN_FILTER_NUM_MAX];
    void *rx_cb_arg[HPM_CAN_FILTER_NUM_MAX];

    /* TX callback */
    can_tx_callback_t tx_fin_cb[HPM_CAN_NUM_TX_BUF_ELEMENTS];
    void *tx_fin_cb_arg[HPM_CAN_NUM_TX_BUF_ELEMENTS];
#endif
    uint32_t filter_rtr;
    uint32_t filter_rtr_mask;

    /* State change callback */
    can_state_change_callback_t state_change_cb;
    void *state_change_cb_data;
    volatile bool has_sent_out;
    bool started;
};

#ifdef HPMSOC_HAS_HPMSDK_MCAN
void canopen_init(struct canopen_context *CANdriverState, MCAN_Type *canptr, uint32_t baudrate);
#else
void canopen_init(struct canopen_context *CANdriverState, CAN_Type *canptr, uint32_t baudrate);
#endif
void enable_can_interrupt(void);
void config_leds(CO_NMT_t *nmt);
void hpm_sys_reboot(void);