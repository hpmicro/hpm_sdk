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
#include "hpm_canopen.h"

extern struct device hpm_canopen_dev;
struct hpm_can_config hpm_canopen_config = {0};
struct hpm_can_data hpm_canopen_data = {0};
uint32_t press_counter =0;

#ifdef HPMSOC_HAS_HPMSDK_MCAN
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
ATTR_PLACE_AT(".ahb_sram") uint32_t board_app_mcan_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
static mcan_msg_buf_attr_t s_can_info[] = {
    {
        .ram_base = (uint32_t) &board_app_mcan_msg_buf,
        .ram_size = sizeof(board_app_mcan_msg_buf),
    },
};
#endif
#endif

static void hpmicro_can_remove_rx_filter(const struct device *dev, int filter_id)
{
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    hpm_mcan_remove_rx_filter(dev, filter_id);
#else
    hpm_can_remove_rx_filter(dev, filter_id);
#endif
}

static int hpmicro_can_add_rx_filter(const struct device *dev, can_rx_callback_t callback, void *user_data, const struct can_filter *filter)
{
    int filter_id = -ENOSPC;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    if ((filter->flags & CAN_FILTER_IDE) != 0U) {
		filter_id = hpm_mcan_add_rx_filter_ext(dev, callback, user_data, filter);
	} else {
		filter_id = hpm_mcan_add_rx_filter_std(dev, callback, user_data, filter);
	}
#else
    filter_id = hpm_can_add_rx_filter(dev, callback, user_data, filter);
#endif
    return filter_id;
}

static int hpmicro_can_set_timing(const struct device *dev, const struct can_timing *timing)
{
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    return hpm_mcan_set_timing(dev, timing);
#else
    return hpm_can_set_timing(dev, timing);
#endif
}

static int hpm_can_get_core_clock(const struct device *dev, uint32_t *rate)
{
    const struct hpm_can_config *cfg = dev->config;
    if (rate != NULL) {
        *rate = board_init_can_clock(cfg->base);
    }
    return 0;
}

static int hpmicro_can_set_mode(const struct device *dev, can_mode_t mode)
{
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    hpm_mcan_set_mode(dev, mode);
#else
    hpm_can_set_mode(dev, mode);
#endif
    return 0;
}

static int hpmicro_can_start(const struct device *dev)
{
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    hpm_mcan_start(dev);
#else
    hpm_can_start(dev);
#endif
    return 0;
}

static int hpmicro_can_stop(const struct device *dev)
{
    const struct hpm_can_config *config = dev->config;
    struct hpm_can_data *data = dev->data;

    if (!data->started) {
        return -EALREADY;
    }
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    mcan_deinit(config->base);
#else
    can_deinit(config->base);
#endif

    data->started = false;

    return 0;
}

static int hpmicro_can_get_max_filters(const struct device *dev, bool ide)
{
    ARG_UNUSED(dev);
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    if (ide) {
        return 16;
    } else {
        return 16;
    }
#else
    ARG_UNUSED(ide);
    return 16;
#endif
}

static int hpmicro_can_get_state(const struct device *dev,
                             enum can_state *state,
                             struct can_bus_err_cnt *err_cnt)
{
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    hpm_mcan_get_state(dev, state, err_cnt);
#else
    hpm_can_get_state(dev, state, err_cnt);
#endif
    return 0;
}

static int hpmicro_can_send(const struct device *dev,
                        const struct can_frame *frame,
                        k_timeout_t timeout,
                        can_tx_callback_t callback,
                        void *user_data)
{
    ARG_UNUSED(timeout);
    ARG_UNUSED(callback);
    ARG_UNUSED(user_data);
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    hpm_mcan_send(dev, frame);
#else
    hpm_can_send(dev, frame);
#endif
    return 0;
}

static const struct can_driver_api hpm_can_driver_api = {
    .get_capabilities = NULL,
    .set_mode = hpmicro_can_set_mode,
    .set_timing = hpmicro_can_set_timing,
    .send = hpmicro_can_send,
    .start = hpmicro_can_start,
    .stop = hpmicro_can_stop,
    .add_rx_filter = hpmicro_can_add_rx_filter,
    .remove_rx_filter = hpmicro_can_remove_rx_filter,
    .get_state = hpmicro_can_get_state,
    .set_state_change_callback = NULL,
    .get_core_clock = hpm_can_get_core_clock,
    .get_max_filters = hpmicro_can_get_max_filters,

    .timing_min = {
        .sjw = 1,
        .prop_seg = 1,
        .phase_seg1 = 1,
        .phase_seg2 = 2,
        .prescaler = 1,
    },
    .timing_max = {
        .sjw = 16,
        .prop_seg = 8,
        .phase_seg1 = 56,
        .phase_seg2 = 32,
        .prescaler = 256,
    },
};

#ifdef HPMSOC_HAS_HPMSDK_MCAN
void canopen_init(struct canopen_context *CANdriverState, MCAN_Type *canptr, uint32_t baudrate)
#else
void canopen_init(struct canopen_context *CANdriverState, CAN_Type *canptr, uint32_t baudrate)
#endif
{
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    mcan_deinit(canptr);
    mcan_config_t can_config;
    hpm_stat_t  status;
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
    status = mcan_set_msg_buf_attr(canptr, &s_can_info[0]);
    if (status != status_success) {
        printf("Error was detected during setting message buffer attribute, please check the arguments\n");
    }
#endif
    mcan_get_default_config(canptr, &can_config);
    can_config.mode = mcan_mode_normal;
#else
    can_deinit(canptr);
    can_config_t can_config;
    can_get_default_config(&can_config);
    can_config.mode = can_mode_normal;
#endif

    can_config.baudrate = baudrate;

    uint32_t can_src_clk_freq = board_init_can_clock(canptr);
    board_init_can(canptr);

#ifdef HPMSOC_HAS_HPMSDK_MCAN
    status = mcan_init(canptr, &can_config, can_src_clk_freq);
#else
    hpm_stat_t status = can_init(canptr, &can_config, can_src_clk_freq);
#endif
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    hpm_canopen_config.base = canptr;
    hpm_canopen_data.config = can_config;
    hpm_canopen_data.started = false;
    hpm_canopen_data.filter_rtr = 0;
    hpm_canopen_data.filter_rtr_mask = 0;
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    hpm_canopen_data.ext_filter_count = 0;
    hpm_canopen_data.std_filter_count = 0;
#else
    hpm_canopen_data.can_filter_count = 0;
#endif
    hpm_canopen_dev.config = &hpm_canopen_config;
    hpm_canopen_dev.data = &hpm_canopen_data;
    hpm_canopen_dev.api = &hpm_can_driver_api;

    CANdriverState->dev = &hpm_canopen_dev;
}
