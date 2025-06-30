/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <assert.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_mcan_drv.h"
#include "can.h"

int hpm_mcan_add_rx_filter_ext(const struct device *dev, can_rx_callback_t callback, void *user_data, const struct can_filter *filter);

int hpm_mcan_add_rx_filter_std(const struct device *dev, can_rx_callback_t callback, void *user_data, const struct can_filter *filter);

void hpm_mcan_remove_rx_filter(const struct device *dev, int filter_id);

int hpm_mcan_set_timing(const struct device *dev, const struct can_timing *timing);

int hpm_mcan_set_mode(const struct device *dev, can_mode_t mode);

int hpm_mcan_start(const struct device *dev);

int hpm_mcan_get_state(const struct device *dev, enum can_state *state, struct can_bus_err_cnt *err_cnt);

int hpm_mcan_send(const struct device *dev, const struct can_frame *frame);

void canopen_irq_handler(struct device *canopendevice);