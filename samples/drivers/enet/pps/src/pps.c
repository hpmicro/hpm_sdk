/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "board.h"
#include "hpm_enet_drv.h"

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
/** @brief Initialize PTP timer and PPS output */
static void ptp_init(void)
{
    enet_ptp_config_t cfg = {0};
    enet_ptp_ts_update_t timestamp = {0};

    /* Get default PTP config based on ENET PTP clock frequency */
    enet_get_default_ptp_config(BOARD_ENET_PPS, clock_get_frequency(BOARD_ENET_PPS_PTP_CLOCK), &cfg);
    cfg.ptp_timestamp = &timestamp;

#if defined(__PPS0_FIXED_OUTPUT_ENABLE) && __PPS0_FIXED_OUTPUT_ENABLE
    printf("Fixed Output Mode\n");
    timestamp.sec = 1651074120UL;
    timestamp.nsec = 0;
    /* Configure PPS0 control output frequency (PPS fields in cfg) */
    cfg.mode = enet_pps_fixed_output;
    cfg.param.control_freq = enet_pps_ctrl_bin_2hz_digital_1hz;
    cfg.pps_enable_output = true;
    enet_init_ptp(BOARD_ENET_PPS, &cfg);
#else
    /* PPSx flexible output: configurable pulse train (interval/width/target time) */
    printf("Flexible Output Mode\n");
    enet_pps_cmd_config_t cmd_cfg = {0};
    timestamp.sec = 1651074120UL;
    timestamp.nsec = 0;
    cmd_cfg.pps_interval = clock_get_frequency(BOARD_ENET_PPS_PTP_CLOCK) / 1000000;  /* 1MHz */
    cmd_cfg.pps_width    = cmd_cfg.pps_interval * 0.5;
    cmd_cfg.target_sec   = timestamp.sec + 2;
    cmd_cfg.target_nsec  = timestamp.nsec;
    cfg.mode = enet_pps_flexible_output;
    cfg.param.flexible.idx = BOARD_ENET_PPS_IDX;
    cfg.param.flexible.config = cmd_cfg;
    cfg.param.flexible.cmd = enet_pps_cmd_start_pulse_train;
    cfg.pps_enable_output = true;
    enet_init_ptp(BOARD_ENET_PPS, &cfg);
#endif
}

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize PTP clock */
    board_init_enet_ptp_clock(BOARD_ENET_PPS);

    /* Initialize GPIOs */
    board_init_enet_pins(BOARD_ENET_PPS);
    board_init_enet_pps_pins(BOARD_ENET_PPS);

    printf("This is an ethernet PPS%d demo\n", BOARD_ENET_PPS_IDX + 1);

    ptp_init();

    while (1) {

    }

    return 0;
}
