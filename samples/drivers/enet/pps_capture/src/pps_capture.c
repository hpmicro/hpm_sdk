/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "board.h"
#include "hpm_enet_drv.h"

enet_ptp_auxi_snapshot_status_t ptp_auxi_snapshot_status;
enet_ptp_ts_auxi_snapshot_t     ptp_auxi_snapshot_timestamp;
volatile bool tsis_interrupt_flag;

SDK_DECLARE_EXT_ISR_M(IRQn_ENET0, isr_pps_capture)
void isr_pps_capture(void)
{
    uint32_t status      = BOARD_ENET_AUXI_SNAPSHOT->DMA_STATUS;
    uint32_t intr_status = BOARD_ENET_AUXI_SNAPSHOT->INTR_STATUS;
    uint32_t ts_status   = BOARD_ENET_AUXI_SNAPSHOT->TS_STATUS;

    if (ENET_DMA_STATUS_TTI_GET(status)) {
        if (ENET_INTR_STATUS_TSIS_GET(intr_status)) {
            if (ENET_TS_STATUS_AUXTSTRIG_GET(ts_status)) {
                enet_get_ptp_auxi_snapshot_status(BOARD_ENET_AUXI_SNAPSHOT, &ptp_auxi_snapshot_status);
                enet_get_ptp_auxi_timestamp(BOARD_ENET_AUXI_SNAPSHOT, &ptp_auxi_snapshot_timestamp);
                tsis_interrupt_flag = true;
            }
        }
    }
}

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
    /* Initialize BSP */
    board_init();

    /* Initialize PTP clock */
    board_init_enet_ptp_clock(BOARD_ENET_AUXI_SNAPSHOT);

    /* Initialize GPIOs */
    board_init_enet_pins(BOARD_ENET_AUXI_SNAPSHOT);
    board_init_enet_pps_capture_pins(BOARD_ENET_AUXI_SNAPSHOT);
    board_enable_enet_irq(BOARD_ENET_AUXI_SNAPSHOT);

    printf("This is an ethernet PPS capture demo\n");

    /* Initialize PTP */
    enet_ptp_init();

    /* Enable PTP auxiliary snapshot trigger(s) */
    enet_enable_ptp_auxiliary_snapshot(BOARD_ENET_AUXI_SNAPSHOT, BOARD_ENET_AUXI_SNAPSHOT_IDX);

    while (1) {
        if (tsis_interrupt_flag) {
            printf("Auxiliary Trigger%d - ", ptp_auxi_snapshot_status.auxi_snapshot_id - 1);
            printf("Timestamp: %d.%d\n", ptp_auxi_snapshot_timestamp.sec, ptp_auxi_snapshot_timestamp.nsec);
            tsis_interrupt_flag = false;
        }
    }

    return 0;
}
