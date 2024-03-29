/*
 * Copyright (c) 2023 HPMicro
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
static void enet_ptp_init(void)
{
    enet_ptp_config_t config;
    enet_ptp_ts_update_t timestamp;

    /* initial the ptp function */
    config.ssinc = ENET_ONE_SEC_IN_NANOSEC / clock_get_frequency(BOARD_ENET_PPS_PTP_CLOCK);
    config.timestamp_rollover_mode = enet_ts_dig_rollover_control;
    config.update_method = enet_ptp_time_fine_update;
    config.addend = 0xffffffff;

    enet_init_ptp(BOARD_ENET_PPS, &config);

    /* set the initial timestamp */
    timestamp.sec = 1651074120UL;
    timestamp.nsec = 0;
    enet_set_ptp_timestamp(BOARD_ENET_PPS, &timestamp);
}

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------*/
int main(void)
{
#if !defined(__PPS0_CONTROL_OUTPUT_ENABLE) || !__PPS0_CONTROL_OUTPUT_ENABLE
    enet_ptp_ts_system_t timestamp;
#endif

    /* Initialize BSP */
    board_init();

    /* Initialize PTP clock */
    board_init_enet_ptp_clock(BOARD_ENET_PPS);

    /* Initialize GPIOs */
    board_init_enet_pins(BOARD_ENET_PPS);
    board_init_enet_pps_pins(BOARD_ENET_PPS);

    printf("This is an ethernet PPS%d demo\n", BOARD_ENET_PPS_IDX + 1);

    /* Initialize PTP */
    enet_ptp_init();

#if defined(__PPS0_CONTROL_OUTPUT_ENABLE) && __PPS0_CONTROL_OUTPUT_ENABLE
    printf("Control Output Mode\n");
    enet_set_pps0_control_output(BOARD_ENET_PPS, enet_pps_ctrl_bin_2hz_digital_1hz);
#else
    printf("Flexible Output Mode\n");
    enet_get_ptp_timestamp(BOARD_ENET_PPS, &timestamp);
    enet_pps_cmd_config_t cmd_cfg;
    cmd_cfg.pps_interval = clock_get_frequency(BOARD_ENET_PPS_PTP_CLOCK) / 1000000;  /* Set target frequency at 1MHz */
    cmd_cfg.pps_width    = cmd_cfg.pps_interval * 0.5;                               /* Set duty cycle to 50% */
    cmd_cfg.target_sec   = timestamp.sec + 2;
    cmd_cfg.target_nsec  = timestamp.nsec;

    enet_set_ppsx_config(BOARD_ENET_PPS, &cmd_cfg, BOARD_ENET_PPS_IDX);
    enet_set_ppsx_command(BOARD_ENET_PPS, enet_pps_cmd_start_pulse_train, BOARD_ENET_PPS_IDX);
#endif

    while (1) {

    }

    return 0;
}
