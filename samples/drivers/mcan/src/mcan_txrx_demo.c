/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file demonstrates bulk message transmission on the CAN bus:
 *   board_can_send_multiple_classic_can_messages() - Sends 2048 classic CAN 2.0 frames
 *   board_can_send_multiple_canfd_messages()       - Sends 2048 CANFD frames with
 *                                                    manually specified bit-timing
 */

#include "mcan_app_common.h"

void board_can_send_multiple_classic_can_messages(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return;
    }
    mcan_get_default_config(ptr, &can_config);
    can_config.baudrate = 1000000; /* 1Mbps */
    can_config.mode = mcan_mode_normal;
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    tx_buf.dlc = 8;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i | (i << 4);
    }
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        status = mcan_transmit_blocking(ptr, &tx_buf);
        if (status != status_success) {
            printf("CAN sent CAN2.0 message failed, error_code:%d\n", status);
            return;
        }
    }
    printf("Sent messages with ID from 0 to 2047 out\n");
}

void board_can_send_multiple_canfd_messages(void)
{
    MCAN_Type *ptr = BOARD_APP_CAN_BASE;
    mcan_config_t can_config;
    hpm_stat_t status;
    if (mcan_setup_msg_buf(ptr) != status_success) {
        return;
    }
    board_init_can(ptr);
    uint32_t can_src_clk_freq = board_init_can_clock(ptr);
    mcan_get_default_config(ptr, &can_config);
    can_config.use_lowlevel_timing_setting = true;

    /* Prescalers and bit-timing parameters for the two supported clock rates.
     * Formulae:
     *   baudrate     = clk / prescaler / (1 + seg1 + seg2)
     *   sample point = (1 + seg1) / (1 + seg1 + seg2) * 100%
     *
     * Target:
     *   Arbitration: 1 Mbps,  sample point 75%  → seg1=29, seg2=10, sjw=2
     *   Data:        2 Mbps,  sample point 75%  → seg1=14, seg2=5,  sjw=2
     */
    uint16_t prescaler, prescaler_fd;
    uint16_t seg1 = 29, seg2 = 10, sjw = 2;
    uint16_t seg1_fd = 14, seg2_fd = 5, sjw_fd = 2;

    if (can_src_clk_freq == 40000000UL) {
        prescaler    = 1;
        prescaler_fd = 1;
    } else if (can_src_clk_freq == 80000000UL) {
        prescaler    = 2;
        prescaler_fd = 2;
    } else {
        printf("Unsupported clock frequency for the CAN-FD test");
        return;
    }

    can_config.can_timing.prescaler = prescaler;
    can_config.can_timing.num_seg1 = seg1;
    can_config.can_timing.num_seg2 = seg2;
    can_config.can_timing.num_sjw = sjw;

    can_config.canfd_timing.prescaler = prescaler_fd;
    can_config.canfd_timing.num_seg1 = seg1_fd;
    can_config.canfd_timing.num_seg2 = seg2_fd;
    can_config.canfd_timing.num_sjw = sjw_fd;
    /* Note: Check the CAN Transceiver datasheet carefully, if the loop delay exceeds 1 data bit time
     * Please enable the Transmitter Delay Compensation feature and configure the `ssp_offset` properly
     * Below is an example setting.
     * */
#ifdef CANFD_ENABLE_TRANSMITTER_DELAY_COMPENSATION
    can_config.canfd_timing.enable_tdc = true;
    /* If the ssp_offset is set to 0, then the following configuration is used by default */
    can_config.tdc_config.ssp_offset = seg1_fd + 1;
    can_config.tdc_config.filter_window_length = can_config.tdc_config.ssp_offset;
#else
    can_config.canfd_timing.enable_tdc = false;
#endif
    can_config.enable_canfd = true;
    /* Mirror the canfd_timing.enable_tdc flag into can_config.enable_tdc so
     * the driver's top-level mcan_init() path also enables the TDC hardware
     * block.  Both fields must be consistent. */
    can_config.enable_tdc = can_config.canfd_timing.enable_tdc;
    can_config.mode = mcan_mode_normal;
    mcan_get_default_ram_config(ptr, &can_config.ram_config, true);
    status = mcan_init(ptr, &can_config, can_src_clk_freq);
    if (status != status_success) {
        printf("CAN initialization failed, error code: %d\n", status);
        return;
    }

    mcan_tx_frame_t tx_buf;
    memset(&tx_buf, 0, sizeof(tx_buf));
    /* DLC 15 maps to the 64-byte CANFD payload size (see ISO 11898-1 Table 7).
     * Values 9-15 are only valid with enable_canfd = true. */
    tx_buf.dlc = 15;
    uint32_t msg_len = mcan_get_message_size_from_dlc(tx_buf.dlc);
    for (uint32_t i = 0; i < msg_len; i++) {
        tx_buf.data_8[i] = i;
    }
    for (uint32_t i = 0; i < 2048; i++) {
        tx_buf.std_id = i;
        tx_buf.canfd_frame = 1;
        tx_buf.bitrate_switch = 1;
        status = mcan_transmit_blocking(ptr, &tx_buf);
        if (status != status_success) {
            printf("CAN sent CANFD message failed, error_code:%d\n", status);
            return;
        }
    }
    printf("Sent CANFD messages with ID from 0 to 2047 out\n");
}
