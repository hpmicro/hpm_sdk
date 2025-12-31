/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_canopen.h"

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
    filter_id = hpm_mcan_add_rx_filter(dev, callback, user_data, filter);
#else
    filter_id = hpm_can_add_rx_filter(dev, callback, user_data, filter);
#endif
    return filter_id;
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
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    hpm_mcan_stop(dev);
#else
    hpm_can_stop(dev);
#endif
    return 0;
}

static int hpmicro_can_get_max_filters(const struct device *dev, bool ide)
{
    ARG_UNUSED(dev);
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    if (ide) {
        return HPM_CAN_STD_FILTER_NUM_MAX;
    } else {
        return HPM_CAN_EXT_FILTER_NUM_MAX;
    }
#else
    ARG_UNUSED(ide);
    return HPM_CAN_FILTER_NUM_MAX;
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

const struct can_driver_api hpm_can_driver_api = {
    .send = hpmicro_can_send,
    .start = hpmicro_can_start,
    .stop = hpmicro_can_stop,
    .add_rx_filter = hpmicro_can_add_rx_filter,
    .remove_rx_filter = hpmicro_can_remove_rx_filter,
    .get_state = hpmicro_can_get_state,
    .get_max_filters = hpmicro_can_get_max_filters,
};

void canopen_controller_init(struct canopen_context *candriverstate, void *canptr, uint32_t baudrate, int index)
{
#ifdef HPMSOC_HAS_HPMSDK_MCAN
    hpm_mcan_init(candriverstate, canptr, baudrate, index);
#else
    hpm_can_init(candriverstate, canptr, baudrate, index);
#endif
}