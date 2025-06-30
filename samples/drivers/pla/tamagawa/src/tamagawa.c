/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_math.h"
#include "hpm_debug_console.h"
#include "hpm_pla_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_spi_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#include "tamagawa_proc.h"
#include "hpm_gpio_drv.h"
#include "hpm_qei_drv.h"
#include "hpm_hall_drv.h"
#include "hpm_l1c_drv.h"

#define TEST_MOTOR_PWM_BASE           (HPM_PWM0)
#define TEST_MOTOR_PWM_CMP            (0U)
#define TEST_MOTOR_PWM_TRG_PLA_CMP    (8U)
#define TEST_MOTOR_PWM_TRG_PLA_CHN      (8U)
#define TEST_MOTOR_PWM_INT_CMP    (9U)
#define TEST_MOTOR_PWM_INT_CHN    (9U)
#define TEST_MOTOR_PWM_PIN            (0U)
#define TEST_MOTOR_PWM_IN_MS          (0.1)
#define TEST_MOTOR_PWM_TRG_PLA_TRG  (HPM_TRGM0_INPUT_SRC_PWM0_CH8REF)
#define TEST_MOTOR_PWM_IRQ       IRQn_PWM0

#define TEST_MOTOR_PWM_CAP_CMP_CHN        (12U)
#define TEST_MOTOR_PWM_CAP_REFRESH_DMA_LINK_CMP    HPM_TRGM0_DMA_SRC_PWM0_CMP12
#define TEST_MOTOR_PWM_CAP_REFRESH_DMA_LINK_TRGMNUM     TRGM_TRGOCFG_PWM_IN12

#define PLA_TMGW_PWM_IN_CHN    pla_filter1_inchn0
#define PLA_TMGW_QEI_IN_CHN    pla_filter1_inchn1
#define PLA_TMGW_MOTOR_TRG_IN_CHN    pla_filter1_inchn2
#define PLA_TMGW_SPI_MOSI_DATA_TRG_IN_CHN    pla_filter1_inchn3
#define PLA_TMGW_LEVEL1_FILTER_IN_END    pla_filter1_inchn7
#define PLA_TMGW_LEVEL1_FILTER_OUT_END    pla_filter1_outchn7
#define PLA_CLK_PERIOD_IN_MS (0.0004)

#define PLA_TMGW_SPI_DATA_LEN_IN_BIT    (10)
#define PLA_TMGW_SPI_DMA_TRANS_DATA_WIDTH     DMA_TRANSFER_WIDTH_HALF_WORD


#define SET_DMA_LINK_PTR(addr)  DMA_CHCTRL_LLPOINTER_LLPOINTERL_SET(core_local_mem_to_sys_address(HPM_CORE0, addr) >> 3)

/**
 * @brief if define ,will enabled dma refresh by dma_link.
 * Enables the current option by default.
 */
#define PLA_DMA_REFRESH_BY_DMA_LINK 1

/**
 * @brief ahb sram holds some variables for dma handling to peripherals, which will make the handling faster.
 *
 */
#define ATTR_PLACE_AT_AHBSRAM ATTR_PLACE_AT(".ahb_sram")
#define ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(alignment) \
    ATTR_PLACE_AT_AHBSRAM ATTR_ALIGN(alignment)

uint32_t pla_tmgw_qei_phcmp[TMGW_CMD_NUM][2];
uint32_t pla_tmgw_comm_completed = 1;
ATTR_RAMFUNC_WITH_ALIGNMENT(8) volatile uint32_t tmgw_status_completed;

ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) uint32_t tmgw_dma_link_src_cs_data;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_qei_cr_assert;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_qei_cr_release;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_qei_dma_size;
ATTR_RAMFUNC_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_qei_source_addr;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_qei_dma_enable;

ATTR_RAMFUNC_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_spitx_dma_size;
ATTR_RAMFUNC_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_spitx_source_addr;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_spitx_dma_enable;
ATTR_RAMFUNC_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_spirx_dma_size;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_spirx_source_addr;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_spirx_dma_enable;

ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_hall_cr_release;

ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_link_dma_size;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_link_source_addr;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_link_dma_enable;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_src_link_dma_linked;

ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_srcbuf;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile uint32_t tmgw_dma_link_dstbuf;

ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile DMA_Type tmgw_dma_link_src_dma_struct;
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) volatile QEI_Type tmgw_dma_link_src_qei_struct;


#define CS_ENABLE()     gpio_write_pin(PLA_TMGW_SPI_CS_GPIO_CTRL, PLA_TMGW_SPI_CS_GPIO_INDEX, PLA_TMGW_SPI_CS_GPIO_PIN, 0);
#define CS_DISABLE()    gpio_write_pin(PLA_TMGW_SPI_CS_GPIO_CTRL, PLA_TMGW_SPI_CS_GPIO_INDEX, PLA_TMGW_SPI_CS_GPIO_PIN, 1);
#define TMGW_POWER_ENABLE()      gpio_write_pin(PLA_TMGW_POWER_GPIO_CTRL, PLA_TMGW_POWER_GPIO_INDEX, PLA_TMGW_POWER_GPIO_PIN, 0);
#define TMGW_POWER_DISABLE()     gpio_write_pin(PLA_TMGW_POWER_GPIO_CTRL, PLA_TMGW_POWER_GPIO_INDEX, PLA_TMGW_POWER_GPIO_PIN, 1);

uint32_t reload;
uint32_t reload_trg;
ATTR_RAMFUNC volatile uint16_t receive_buff[20];
ATTR_RAMFUNC_WITH_ALIGNMENT(8) volatile uint32_t hall_buff[1];
ATTR_PLACE_AT_AHBSRAM_WITH_ALIGNMENT(8) dma_linked_descriptor_t tmgw_dma_linked_descriptors[PLA_TMGW_DMA_LINK_NUM];
void pla_tmgw_init(void);
hpm_stat_t qei_position_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, QEI_Type *qei_x, uint32_t src, uint8_t data_width, uint32_t size);
void init_qei(void);
hpm_stat_t tmgw_dma_chained_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, uint32_t src, uint32_t dst, uint8_t data_width, uint32_t size);
void tamagawa_get_time(uint32_t *times);

uint8_t tmgw_cmd;

ATTR_RAMFUNC_WITH_ALIGNMENT(8) uint16_t tmgw_cmd_sendbuf[TMGW_CMD_NUM][TMGW_CMD_SEND_FRAME_MAX_LEN];

tmgw_proc_message_t tmgw_msg;
/**
 * @brief The array information is related to the Tamagawa protocol.
 *
 */
tmgw_cmd_cfg_t tmgw_cmd_cfgbuf[TMGW_CMD_NUM] = {
    [tmgw_cmd_id0] = {1, 6, &tmgw_cmd_sendbuf[tmgw_cmd_id0][0], &receive_buff[0]},
    [tmgw_cmd_id1] = {1, 6, &tmgw_cmd_sendbuf[tmgw_cmd_id1][0], &receive_buff[0]},
    [tmgw_cmd_id2] = {1, 4, &tmgw_cmd_sendbuf[tmgw_cmd_id2][0], &receive_buff[0]},
    [tmgw_cmd_id3] = {1, 11, &tmgw_cmd_sendbuf[tmgw_cmd_id3][0], &receive_buff[0]},
    [tmgw_cmd_id7] = {1, 6, &tmgw_cmd_sendbuf[tmgw_cmd_id7][0], &receive_buff[0]},
    [tmgw_cmd_id8] = {1, 6, &tmgw_cmd_sendbuf[tmgw_cmd_id8][0], &receive_buff[0]},
    [tmgw_cmd_idc] = {1, 6, &tmgw_cmd_sendbuf[tmgw_cmd_idc][0], &receive_buff[0]},
    [tmgw_cmd_wr_eeprom] = {4, 4, &tmgw_cmd_sendbuf[tmgw_cmd_wr_eeprom][0], &receive_buff[0]},
    [tmgw_cmd_rd_eeprom] = {3, 4, &tmgw_cmd_sendbuf[tmgw_cmd_rd_eeprom][0], &receive_buff[0]},
};

/**
 * @brief Runtime information, mainly the current command.
 *
 */
tmgw_dev_cfg_t tmgw_dev = {
    .run_cmd = tmgw_cmd_id3,
    .buf_cfg = &tmgw_cmd_cfgbuf[0],
    .hall_buf = &hall_buff[0],
    .comm_time_delay = 0,
    .comm_time_delay_config = 0,
    .comm_time_delay_init = 0
};

/**
 * @brief Only need to call when the command is updated, normal use does not need to call
 *
 */
void refresh_cmd(uint8_t cmd)
{
    static int32_t new_cmd = -1;

    if (new_cmd != -1) {
        tmgw_dev.run_cmd = new_cmd;
    }
    new_cmd = cmd;
    if (new_cmd != tmgw_dev.run_cmd) {
        tmgw_dma_link_src_spitx_dma_size = (tmgw_dev.buf_cfg[new_cmd].tx_num + 1);
        tmgw_dma_link_src_spitx_source_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE,
            (uint32_t)tmgw_dev.buf_cfg[new_cmd].send_buf);
        tmgw_dma_link_src_spirx_dma_size = (tmgw_dev.buf_cfg[new_cmd].rx_num + tmgw_dev.buf_cfg[new_cmd].tx_num);
        tmgw_dma_link_src_qei_source_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE,
            (uint32_t)&pla_tmgw_qei_phcmp[new_cmd][0]);
    }

}

/**
 * @brief Update the communication start time.
 *  Keep the moment of the encoder latching angle consistent by compensating the time.
 *
 * @note The zero moment of tick time is the zero moment of the pwm cycle.
 *
 * @param tick The compensation time, in pwm peripheral clock,
 *  ranges up to the number of clocks in one cycle of the pwm,
 *  and a time of pwm cycle turns off communication.
 */
void refresh_comm_start_time(uint32_t tick)
{
    pwm_cmp_update_cmp_value(TEST_MOTOR_PWM_BASE, TEST_MOTOR_PWM_TRG_PLA_CMP, tick, 0);
}

hpm_stat_t spi_tx_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, SPI_Type *spi_ptr, uint32_t src, uint8_t data_width, uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = (uint32_t)&spi_ptr->DATA;
    config.dst_fixed = true;
    config.src = src;
    config.src_fixed = false;
    config.data_width = data_width;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config, true);
}

hpm_stat_t spi_rx_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, SPI_Type *spi_ptr, uint32_t dst, uint8_t data_width, uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = dst;
    config.dst_fixed = false;
    config.src = (uint32_t)&spi_ptr->DATA;
    config.src_fixed = true;
    config.data_width = data_width;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config, true);
}

void init_spi_cfg(SPI_Type *ptr)
{
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    uint8_t cmd = 0x0;
    uint32_t addr = 0x0;
    hpm_stat_t stat;

    spi_slave_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = PLA_TMGW_SPI_DATA_LEN_IN_BIT;
    format_config.common_config.data_merge = false;
    format_config.common_config.mosi_bidir = false;
    format_config.common_config.lsb = false;
    format_config.common_config.mode = spi_slave_mode;
    format_config.common_config.cpol = spi_sclk_low_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(ptr, &format_config);

    /* set SPI control config for master */
    spi_slave_get_default_control_config(&control_config);
    control_config.slave_config.slave_data_only = true;
    control_config.common_config.tx_dma_enable = true;
    control_config.common_config.rx_dma_enable = true;
    control_config.common_config.trans_mode = spi_trans_write_read_together;
    control_config.common_config.data_phase_fmt = spi_single_io_mode;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;

    stat = spi_setup_dma_transfer(ptr,
                        &control_config,
                        &cmd, &addr,
                        tmgw_dev.buf_cfg[tmgw_dev.run_cmd].tx_num + 1,
                        tmgw_dev.buf_cfg[tmgw_dev.run_cmd].rx_num + tmgw_dev.buf_cfg[tmgw_dev.run_cmd].tx_num);
    if (stat != status_success) {
        printf("spi setup dma transfer failed\n");
        while (1) {
        }
    }

    dmamux_config(PLA_TMGW_SPI_DMAMUX, PLA_TMGW_SPI_TX_DMAMUX_CH, PLA_TMGW_SPI_TX_DMA_REQ, true);
    stat = spi_tx_trigger_dma(PLA_TMGW_SPI_DMA,
                            PLA_TMGW_SPI_TX_DMA_CH,
                            PLA_TMGW_SPI,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)tmgw_dev.buf_cfg[tmgw_dev.run_cmd].send_buf),
                            PLA_TMGW_SPI_DMA_TRANS_DATA_WIDTH,
                            2 * (tmgw_dev.buf_cfg[tmgw_dev.run_cmd].tx_num + 1));
    if (stat != status_success) {
        printf("spi tx trigger dma failed\n");
        while (1) {
        }
    }

    /* setup spi rx trigger dma transfer*/
    dmamux_config(PLA_TMGW_SPI_DMAMUX, PLA_TMGW_SPI_RX_DMAMUX_CH, PLA_TMGW_SPI_RX_DMA_REQ, true);
    stat = spi_rx_trigger_dma(PLA_TMGW_SPI_DMA,
                            PLA_TMGW_SPI_RX_DMA_CH,
                            PLA_TMGW_SPI,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)receive_buff),
                            PLA_TMGW_SPI_DMA_TRANS_DATA_WIDTH,
                            2 * (tmgw_dev.buf_cfg[tmgw_dev.run_cmd].rx_num + tmgw_dev.buf_cfg[tmgw_dev.run_cmd].tx_num));
    if (stat != status_success) {
        printf("spi rx trigger dma failed\n");
        while (1) {
        }
    }

    gpio_set_pin_output(PLA_TMGW_SPI_CS_GPIO_CTRL, PLA_TMGW_SPI_CS_GPIO_INDEX,
                           PLA_TMGW_SPI_CS_GPIO_PIN);
    CS_ENABLE();

}

/**
 * @brief Configure pla's channel to implement the Domokawa protocol parsing function.
 *
 */
void pla_tmgw_init(void)
{
    pla_filter_cfg_t filter_level1_chn_cfg;
    pla_filter_cfg_t filter_level2_chn_cfg;
    pla_filter_cfg_t filter_level3_chn_cfg;
    pla_aoi_16to8_chn_cfg_t aoi_16to8_chn_cfg;
    pla_aoi_8to7_chn_cfg_t aoi_8to7_chn_cfg;
    pla_ff_cfg_t pla_ff_cfg;

/*******************************************************************************
 * @brief filter 0
 *
 * *****************************************************************************
 */
    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.sync_edge_filter_disable = true;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    pla_set_filter1_in(PLA_TMGW_COUNTER, PLA_TMGW_PWM_IN_CHN, &filter_level1_chn_cfg);

    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.sync_edge_filter_disable = false;
    filter_level1_chn_cfg.edge_dect_en = true;
    filter_level1_chn_cfg.pose_edge_dect_en = true;
    filter_level1_chn_cfg.filter_reverse = false;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    pla_set_filter1_in(PLA_TMGW_COUNTER, PLA_TMGW_QEI_IN_CHN, &filter_level1_chn_cfg);

    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.sync_edge_filter_disable = false;
    filter_level1_chn_cfg.edge_dect_en = true;
    filter_level1_chn_cfg.pose_edge_dect_en = true;
    filter_level1_chn_cfg.filter_reverse = false;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    pla_set_filter1_in(PLA_TMGW_COUNTER, PLA_TMGW_MOTOR_TRG_IN_CHN, &filter_level1_chn_cfg);

    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.sync_edge_filter_disable = false;
    filter_level1_chn_cfg.edge_dect_en = true;
    filter_level1_chn_cfg.nege_edge_dect_en = true;
    filter_level1_chn_cfg.filter_reverse = false;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    pla_set_filter1_in(PLA_TMGW_COUNTER, PLA_TMGW_SPI_MOSI_DATA_TRG_IN_CHN, &filter_level1_chn_cfg);

    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_height;
    for (uint16_t i = 4; i <= PLA_TMGW_LEVEL1_FILTER_IN_END; i++) {
        pla_set_filter1_in(PLA_TMGW_COUNTER, i, &filter_level1_chn_cfg);
    }
    filter_level1_chn_cfg.val = 0;
    filter_level1_chn_cfg.sync_edge_filter_disable = true;
    filter_level1_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = 0; i <= PLA_TMGW_LEVEL1_FILTER_OUT_END; i++) {
        pla_set_filter1_out(PLA_TMGW_COUNTER, i, &filter_level1_chn_cfg);
    }

/*******************************************************************************
 * @brief channel 0
 *
 * *****************************************************************************
 */

    /**
     * @brief pla 16to8 channel 0 PWM
     *
     */
    aoi_16to8_chn_cfg.chn = pla_chn_0;
    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_0;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_0].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_0].signal = pla_level1_filter_out_0;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_10].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_10].signal = pla_level1_filter_out_10;
    pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);

    /**
     * @brief pla 16to8 channel 2-7
     *
     */
    for (uint16_t j = pla_aoi_16to8_chn_2; j <= pla_aoi_16to8_chn_7; j++) {
        aoi_16to8_chn_cfg.chn = pla_chn_0;
        aoi_16to8_chn_cfg.aoi_16to8_chn = j;
        for (uint16_t i = pla_level1_filter_out_0; i < pla_level1_filter_out_15; i++) {
            aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
            aoi_16to8_chn_cfg.input[i].signal = i;
        }
        pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);
    }

    filter_level2_chn_cfg.val = 0;
    filter_level2_chn_cfg.sync_edge_filter_disable = true;
    filter_level2_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter2_chn0; i <= pla_filter2_chn7; i++) {
        pla_set_filter2(PLA_TMGW_COUNTER, pla_chn_0, i, &filter_level2_chn_cfg);
    }

    /**
     * @brief pla 8to7
     *
     */

    /**
     * @brief chn0
     *
     */
    aoi_8to7_chn_cfg.chn = pla_chn_0;
    aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_0;
    for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
        aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
        aoi_8to7_chn_cfg.input[i].signal = i;
    }
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].signal = pla_level2_filter_out_0;
    pla_set_aoi_8to7_one_channel(PLA_TMGW_COUNTER, &aoi_8to7_chn_cfg);

    /**
     * @brief filter level3
     *
     */
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.sync_edge_filter_disable = true;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter3_chn0; i < pla_filter3_chn1; i++) {
        pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_0, i, &filter_level3_chn_cfg);
    }
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_0, pla_filter3_chn2, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_0, pla_filter3_chn3, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_0, pla_filter3_chn4, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_0, pla_filter3_chn5, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_0, pla_filter3_chn6, &filter_level3_chn_cfg);
    pla_ff_cfg.val = 0;
    pla_ff_cfg.sel_cfg_ff_type = pla_ff_type_level3_filter0;
    pla_ff_cfg.sel_clk_source = 0;
    pla_set_ff(PLA_TMGW_COUNTER, pla_chn_0, &pla_ff_cfg);

/*******************************************************************************
 * @brief channel 1 WR_EN QEI_IN TRG_IN JK
 *
 * *****************************************************************************
 */
    /**
     * @brief pla 16to8 channel 0
     *
     */
    aoi_16to8_chn_cfg.chn = pla_chn_1;
    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_0;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_1].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_1].signal = pla_level1_filter_out_1;
    pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);

    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_1;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_2].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_2].signal = pla_level1_filter_out_2;
    pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);

    /**
     * @brief pla 16to8 channel 2-7
     *
     */
    for (uint16_t j = pla_aoi_16to8_chn_2; j <= pla_aoi_16to8_chn_7; j++) {
        aoi_16to8_chn_cfg.chn = pla_chn_1;
        aoi_16to8_chn_cfg.aoi_16to8_chn = j;
        for (uint16_t i = pla_level1_filter_out_0; i < pla_level1_filter_out_15; i++) {
            aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
            aoi_16to8_chn_cfg.input[i].signal = i;
        }
        pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);
    }

    filter_level2_chn_cfg.val = 0;
    filter_level2_chn_cfg.sync_edge_filter_disable = true;
    filter_level2_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter2_chn0; i <= pla_filter2_chn7; i++) {
        pla_set_filter2(PLA_TMGW_COUNTER, pla_chn_1, i, &filter_level2_chn_cfg);
    }

    /**
     * @brief pla 8to7
     *
     */

    /**
     * @brief chn0
     *
     */
    aoi_8to7_chn_cfg.chn = pla_chn_1;
    for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
        aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
        aoi_8to7_chn_cfg.input[i].signal = i;
    }
    aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_5;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].signal = pla_level2_filter_out_0;
    pla_set_aoi_8to7_one_channel(PLA_TMGW_COUNTER, &aoi_8to7_chn_cfg);
    for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
        aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
        aoi_8to7_chn_cfg.input[i].signal = i;
    }
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].signal = pla_level2_filter_out_1;
    aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_6;
    pla_set_aoi_8to7_one_channel(PLA_TMGW_COUNTER, &aoi_8to7_chn_cfg);

    /**
     * @brief filter level3
     *
     */
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.sync_edge_filter_disable = true;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter3_chn0; i <= pla_filter3_chn6; i++) {
        pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_1, i, &filter_level3_chn_cfg);
    }
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_1, pla_filter3_chn2, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_1, pla_filter3_chn3, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_1, pla_filter3_chn4, &filter_level3_chn_cfg);

    pla_ff_cfg.val = 0;
    pla_ff_cfg.sel_cfg_ff_type = pla_ff_type_jk_ff;
    pla_ff_cfg.sel_clk_source = 0;
    pla_set_ff(PLA_TMGW_COUNTER, pla_chn_1, &pla_ff_cfg);

/*******************************************************************************
 * @brief WR_EN OR RD_EN --> CLK_EN channel 2
 *
 * *****************************************************************************
 */
    /**
     * @brief pla 16to8 channel 0
     *
     */
    aoi_16to8_chn_cfg.chn = pla_chn_2;
    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_0;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_9].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_9].signal = pla_level1_filter_out_9;
    pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);

    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_1;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_12].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_12].signal = pla_level1_filter_out_12;
    pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);


    /**
     * @brief pla 16to8 channel 2-7
     *
     */
    for (uint16_t j = pla_aoi_16to8_chn_2; j <= pla_aoi_16to8_chn_7; j++) {
        aoi_16to8_chn_cfg.chn = pla_chn_2;
        aoi_16to8_chn_cfg.aoi_16to8_chn = j;
        for (uint16_t i = pla_level1_filter_out_0; i < pla_level1_filter_out_15; i++) {
            aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
            aoi_16to8_chn_cfg.input[i].signal = i;
        }
        pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);
    }

    filter_level2_chn_cfg.val = 0;
    filter_level2_chn_cfg.sync_edge_filter_disable = true;
    filter_level2_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter2_chn0; i <= pla_filter2_chn7; i++) {
        pla_set_filter2(PLA_TMGW_COUNTER, pla_chn_2, i, &filter_level2_chn_cfg);
    }

    /**
     * @brief pla 8to7
     *
     */

    /**
     * @brief chn0
     *
     */
    aoi_8to7_chn_cfg.chn = pla_chn_2;
    for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
        aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
        aoi_8to7_chn_cfg.input[i].signal = i;
    }
    aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_0;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].signal = pla_level2_filter_out_0;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].signal = pla_level2_filter_out_1;
    pla_set_aoi_8to7_one_channel(PLA_TMGW_COUNTER, &aoi_8to7_chn_cfg);

    /**
     * @brief filter level3
     *
     */
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.sync_edge_filter_disable = true;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter3_chn0; i <= pla_filter3_chn6; i++) {
        pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_2, i, &filter_level3_chn_cfg);
    }
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_2, pla_filter3_chn2, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_2, pla_filter3_chn3, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_2, pla_filter3_chn4, &filter_level3_chn_cfg);

    pla_ff_cfg.val = 0;
    pla_ff_cfg.sel_cfg_ff_type = pla_ff_type_level3_filter0;
    pla_ff_cfg.sel_clk_source = 0;
    pla_set_ff(PLA_TMGW_COUNTER, pla_chn_2, &pla_ff_cfg);

/*******************************************************************************
 * @brief channel 6 RDEN--RDEND QEI_IN JK
 *
 * *****************************************************************************
 */
    /**
     * @brief pla 16to8 channel 0
     *
     */
    aoi_16to8_chn_cfg.chn = pla_chn_6;
    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_0;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_1].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_1].signal = pla_level1_filter_out_1;
    pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);

    /**
     * @brief pla 16to8 channel 1-7
     *
     */
    for (uint16_t j = pla_aoi_16to8_chn_1; j <= pla_aoi_16to8_chn_7; j++) {
        aoi_16to8_chn_cfg.chn = pla_chn_6;
        aoi_16to8_chn_cfg.aoi_16to8_chn = j;
        for (uint16_t i = pla_level1_filter_out_0; i < pla_level1_filter_out_15; i++) {
            aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
            aoi_16to8_chn_cfg.input[i].signal = i;
        }
        pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);
    }

    filter_level2_chn_cfg.val = 0;
    filter_level2_chn_cfg.sync_edge_filter_disable = true;
    filter_level2_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter2_chn0; i <= pla_filter2_chn7; i++) {
        pla_set_filter2(PLA_TMGW_COUNTER, pla_chn_6, i, &filter_level2_chn_cfg);
    }
    filter_level2_chn_cfg.val = 0;
    filter_level2_chn_cfg.sync_edge_filter_disable = false;
    filter_level2_chn_cfg.edge_dect_en = true;
    filter_level2_chn_cfg.pose_edge_dect_en = true;
    filter_level2_chn_cfg.filter_reverse = false;
    filter_level2_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    pla_set_filter2(PLA_TMGW_COUNTER, pla_chn_6, pla_filter2_chn0, &filter_level2_chn_cfg);

    /**
     * @brief pla 8to7
     *
     */

    /**
     * @brief chn0
     *
     */
    aoi_8to7_chn_cfg.chn = pla_chn_6;
    for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
        aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
        aoi_8to7_chn_cfg.input[i].signal = i;
    }
    aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_0;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].signal = pla_level2_filter_out_0;
    pla_set_aoi_8to7_one_channel(PLA_TMGW_COUNTER, &aoi_8to7_chn_cfg);

    /**
     * @brief filter level3
     *
     */
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.sync_edge_filter_disable = true;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter3_chn0; i <= pla_filter3_chn6; i++) {
        pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_6, i, &filter_level3_chn_cfg);
    }
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_6, pla_filter3_chn2, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_6, pla_filter3_chn3, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_6, pla_filter3_chn4, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_6, pla_filter3_chn5, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_6, pla_filter3_chn6, &filter_level3_chn_cfg);

    pla_ff_cfg.val = 0;
    pla_ff_cfg.sel_cfg_ff_type = pla_ff_type_adder_minus;
    pla_ff_cfg.sel_adder_minus = 0;
    pla_ff_cfg.sel_clk_source = 0;
    pla_set_ff(PLA_TMGW_COUNTER, pla_chn_6, &pla_ff_cfg);

/*******************************************************************************
 * @brief channel 4 CLK_RD_EN D-FF WR_EN MOSI_IN RD_CLK_IN
 *
 * *****************************************************************************
 */
    /**
     * @brief pla 16to8 channel 0
     *
     */
    aoi_16to8_chn_cfg.chn = pla_chn_4;
    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_0;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_3].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_3].signal = pla_level1_filter_out_3;
    pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);

    aoi_16to8_chn_cfg.aoi_16to8_chn = pla_aoi_16to8_chn_1;
    for (uint16_t i = pla_level1_filter_out_0; i <= pla_level1_filter_out_15; i++) {
        aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
        aoi_16to8_chn_cfg.input[i].signal = i;
    }
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_14].op = pla_aoi_operation_and_1;
    aoi_16to8_chn_cfg.input[pla_level1_filter_out_14].signal = pla_level1_filter_out_14;
    pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);

    /**
     * @brief pla 16to8 channel 2-7
     *
     */
    for (uint16_t j = pla_aoi_16to8_chn_2; j <= pla_aoi_16to8_chn_7; j++) {
        aoi_16to8_chn_cfg.chn = pla_chn_4;
        aoi_16to8_chn_cfg.aoi_16to8_chn = j;
        for (uint16_t i = pla_level1_filter_out_0; i < pla_level1_filter_out_15; i++) {
            aoi_16to8_chn_cfg.input[i].op = pla_aoi_operation_or_1;
            aoi_16to8_chn_cfg.input[i].signal = i;
        }
        pla_set_aoi_16to8_one_channel(PLA_TMGW_COUNTER, &aoi_16to8_chn_cfg);
    }

    filter_level2_chn_cfg.val = 0;
    filter_level2_chn_cfg.sync_edge_filter_disable = true;
    filter_level2_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter2_chn0; i <= pla_filter2_chn7; i++) {
        pla_set_filter2(PLA_TMGW_COUNTER, pla_chn_4, i, &filter_level2_chn_cfg);
    }

    /**
     * @brief pla 8to7
     *
     */

    /**
     * @brief chn0
     *
     */
    aoi_8to7_chn_cfg.chn = pla_chn_4;
    for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
        aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
        aoi_8to7_chn_cfg.input[i].signal = i;
    }
    aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_0].signal = pla_level2_filter_out_0;
    pla_set_aoi_8to7_one_channel(PLA_TMGW_COUNTER, &aoi_8to7_chn_cfg);

    aoi_8to7_chn_cfg.chn = pla_chn_4;
    for (uint16_t i = pla_level2_filter_out_0; i <= pla_level2_filter_out_7; i++) {
        aoi_8to7_chn_cfg.input[i].op = pla_aoi_operation_and_0;
        aoi_8to7_chn_cfg.input[i].signal = i;
    }
    aoi_8to7_chn_cfg.aoi_8to7_chn = pla_aoi_8to7_chn_2;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].op = pla_aoi_operation_and_1;
    aoi_8to7_chn_cfg.input[pla_level2_filter_out_1].signal = pla_level2_filter_out_1;
    pla_set_aoi_8to7_one_channel(PLA_TMGW_COUNTER, &aoi_8to7_chn_cfg);

    /**
     * @brief filter level3
     *
     */
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.sync_edge_filter_disable = true;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_disable;
    for (uint16_t i = pla_filter3_chn0; i <= pla_filter3_chn6; i++) {
        pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_4, i, &filter_level3_chn_cfg);
    }
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_4, pla_filter3_chn0, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_height;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_4, pla_filter3_chn5, &filter_level3_chn_cfg);
    filter_level3_chn_cfg.val = 0;
    filter_level3_chn_cfg.software_inject = pla_filter_sw_inject_low;
    pla_set_filter3(PLA_TMGW_COUNTER, pla_chn_4, pla_filter3_chn6, &filter_level3_chn_cfg);

    pla_ff_cfg.val = 0;
    pla_ff_cfg.sel_cfg_ff_type = pla_ff_type_latch;
    pla_ff_cfg.sel_clk_source = 1;
    pla_set_ff(PLA_TMGW_COUNTER, pla_chn_4, &pla_ff_cfg);

/**
 *--------------------------------------------------------------------------------
 *
 */


    /**
     * @brief Construct a new pla channel enable object
     *
     */
    pla_channel_enable(PLA_TMGW_COUNTER, pla_chn_0);
    pla_channel_enable(PLA_TMGW_COUNTER, pla_chn_1);
    pla_channel_enable(PLA_TMGW_COUNTER, pla_chn_2);
    pla_channel_enable(PLA_TMGW_COUNTER, pla_chn_3);
    pla_channel_enable(PLA_TMGW_COUNTER, pla_chn_4);
    pla_channel_enable(PLA_TMGW_COUNTER, pla_chn_5);
    pla_channel_enable(PLA_TMGW_COUNTER, pla_chn_6);
    pla_channel_enable(PLA_TMGW_COUNTER, pla_chn_7);
}

void init_pwm_pla_trgm(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = PLA_TMGW_TRGM_CLK_IN_TRG;
    trgm_output_config(ptr, PLA_TMGW_TRGM_CLK_To_PLA_TRGNUM, &trgm_output_cfg);
    /* trig0 p5 clk*/
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = PLA_TMGW_OUT_TRG;
    trgm_output_config(ptr, PLA_TMGW_IO_TRG_NUM, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1 << PLA_TMGW_IO_TRG_NUM);

    /* clear pwm */
    trgm_output_cfg.invert = true;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = PLA_TMGW_PWM_SYNCI_TRG;
    trgm_output_config(ptr, PLA_TMGW_PWM_SYNCI_TRGNUM, &trgm_output_cfg);

    /**485-DIR p01*/
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = PLA_TMGW_SPI_485_DIR_TRG;
    trgm_output_config(ptr, PLA_TMGW_SPI_485_DIR_TRGNUM, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1 << PLA_TMGW_SPI_485_DIR_TRGNUM);

    /**MOSI SIG PLA_in.p03*/
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = PLA_TMGW_SPI_MOSI_DATA_TRG;
    trgm_output_config(ptr, PLA_TMGW_SPI_MOSI_DATA_TRGNUM, &trgm_output_cfg);

    /*HAll time*/
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input = PLA_TMGW_HALL_TIME_TRG;
    trgm_output_config(ptr, TRGM_TRGOCFG_HALL_U, &trgm_output_cfg);

    /*DMA LINK*/
    trgm_output_cfg.invert = true;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input = PLA_TMGW_DMA_LINK_TRGM_INPUT;
    trgm_output_config(ptr, TEST_MOTOR_PWM_CAP_REFRESH_DMA_LINK_TRGMNUM, &trgm_output_cfg);

    /** test point */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLA0_OUT6;
    trgm_output_config(ptr, TRGM_TRGOCFG_TRGM_OUT2, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1 << TRGM_TRGOCFG_TRGM_OUT2);
}

void tmgw_dma_refresh_by_sw(void)
{
        CS_DISABLE();
        PLA_TMGW_QEI_DMA->CHCTRL[PLA_TMGW_QEI_DMACH].TRANSIZE = 2;
        PLA_TMGW_QEI_DMA->CHCTRL[PLA_TMGW_QEI_DMACH].SRCADDR =
            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&pla_tmgw_qei_phcmp[tmgw_dev.run_cmd][0]);
        PLA_TMGW_QEI_DMA->CHCTRL[PLA_TMGW_QEI_DMACH].CTRL |= DMA_CHCTRL_CTRL_ENABLE_MASK;
        PLA_TMGW_QEI_BASE->CR |= QEI_CR_RSTCNT_MASK;
        PLA_TMGW_QEI_BASE->CR &= ~QEI_CR_RSTCNT_MASK;

        PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_TX_DMA_CH].TRANSIZE = (tmgw_dev.buf_cfg[tmgw_dev.run_cmd].tx_num + 1);
        PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_TX_DMA_CH].SRCADDR =
            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)tmgw_dev.buf_cfg[tmgw_dev.run_cmd].send_buf);

        PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_RX_DMA_CH].TRANSIZE =
            (tmgw_dev.buf_cfg[tmgw_dev.run_cmd].rx_num + tmgw_dev.buf_cfg[tmgw_dev.run_cmd].tx_num);
        PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_RX_DMA_CH].DSTADDR = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&receive_buff);
        PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_RX_DMA_CH].CTRL |= DMA_CHCTRL_CTRL_ENABLE_MASK;
        PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_TX_DMA_CH].CTRL |= DMA_CHCTRL_CTRL_ENABLE_MASK;


        PLA_TMGW_HALL_DMA->CHCTRL[PLA_TMGW_HALL_DMA_CH].TRANSIZE = PLA_TMGW_HALL_TRAN_SIZE;
        PLA_TMGW_HALL_DMA->CHCTRL[PLA_TMGW_HALL_DMA_CH].CTRL |= DMA_CHCTRL_CTRL_ENABLE_MASK;
        PLA_TMGW_HALL_BASE->CR |= HALL_CR_RSTCNT_MASK;
        PLA_TMGW_HALL_BASE->CR &= ~HALL_CR_RSTCNT_MASK;
        tmgw_status_completed = 1;
        CS_ENABLE();
}

SDK_DECLARE_EXT_ISR_M(TEST_MOTOR_PWM_IRQ, pwm_isr)
void pwm_isr(void)
{
    volatile uint32_t irq_status;

    irq_status = pwm_get_status(TEST_MOTOR_PWM_BASE);
    pwm_clear_status(TEST_MOTOR_PWM_BASE, irq_status);
    if ((irq_status & PWM_IRQ_CMP(TEST_MOTOR_PWM_INT_CMP))) {
        tamagawa_get_time((uint32_t *)&hall_buff[0]);
        tmgw_message_process(&tmgw_status_completed, &tmgw_dev, &tmgw_msg);
        refresh_comm_start_time(tmgw_dev.comm_time_delay_config);
#ifdef PLA_DMA_REFRESH_BY_DMA_LINK
        refresh_cmd(tmgw_cmd);
#else
        tmgw_dma_refresh_by_sw();
#endif
    }

}

/**
 * @brief Test motor
 *
 */
void init_pwm_trg(void)
{
    uint8_t cmp_index = 0;
    uint32_t duty;
    pwm_cmp_config_t cmp_config[5] = {0};
    pwm_config_t pwm_config = {0};
    uint32_t freq_trg;
    pwm_output_channel_t pwm_output_ch_cfg;
    trgm_output_t trgm_output_cfg;

    freq_trg = clock_get_frequency(PLA_TMGW_PWM_CLOCK_NAME);
    reload_trg = freq_trg / 1000 * TEST_MOTOR_PWM_IN_MS - 1;

    pwm_stop_counter(TEST_MOTOR_PWM_BASE);
    pwm_get_default_pwm_config(TEST_MOTOR_PWM_BASE, &pwm_config);

    /*
     * reload and start counter
     */
    pwm_set_reload(TEST_MOTOR_PWM_BASE, 0, reload_trg);
    pwm_set_start_count(TEST_MOTOR_PWM_BASE, 0, 0);

    /*
     * config cmp1 and cmp2 and cmp3
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload_trg + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload_trg + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[2].mode = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = reload_trg;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;
    /*
     * config pwm
     */
    cmp_index = TEST_MOTOR_PWM_CMP;
    if (status_success != pwm_setup_waveform(TEST_MOTOR_PWM_BASE, TEST_MOTOR_PWM_PIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while (1) {
            ;
        }
    }
    pwm_load_cmp_shadow_on_match(TEST_MOTOR_PWM_BASE, cmp_index + 3,  &cmp_config[2]);
    pwm_start_counter(TEST_MOTOR_PWM_BASE);
    pwm_issue_shadow_register_lock_event(TEST_MOTOR_PWM_BASE);
    duty = reload_trg - 1;
    pwm_update_raw_cmp_central_aligned(TEST_MOTOR_PWM_BASE, cmp_index, cmp_index + 1, (reload_trg - duty) >> 1, (reload_trg + duty) >> 1);

/**
 * @brief pwm trig output
 *
 */
    pwm_output_ch_cfg.cmp_start_index = TEST_MOTOR_PWM_TRG_PLA_CMP;
    pwm_output_ch_cfg.cmp_end_index   = TEST_MOTOR_PWM_TRG_PLA_CMP;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(TEST_MOTOR_PWM_BASE, TEST_MOTOR_PWM_TRG_PLA_CHN, &pwm_output_ch_cfg);

    /*
     * config cmp = RELOAD + 1
     */
    tmgw_dev.comm_time_delay_init = reload_trg >> 2;
    cmp_config[3].mode = pwm_cmp_mode_output_compare;
    cmp_config[3].cmp = tmgw_dev.comm_time_delay_init;
    cmp_config[3].update_trigger = pwm_shadow_register_update_on_hw_event;

    pwm_load_cmp_shadow_on_match(TEST_MOTOR_PWM_BASE, TEST_MOTOR_PWM_TRG_PLA_CMP, &cmp_config[3]);

    pwm_start_counter(TEST_MOTOR_PWM_BASE);
    pwm_issue_shadow_register_lock_event(TEST_MOTOR_PWM_BASE);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = TEST_MOTOR_PWM_TRG_PLA_TRG;
    trgm_output_config(PLA_TMGW_TRGM, PLA_TMGW_IN_MOTOR_TRG_NUM, &trgm_output_cfg);
/**
 * @brief pwm interrupt
 * PWM interrupt time to be after dma completion
 *
 */

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[4].mode = pwm_cmp_mode_output_compare;
    cmp_config[4].cmp = (reload_trg * 9) / 10;
    cmp_config[4].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_load_cmp_shadow_on_match(TEST_MOTOR_PWM_BASE, TEST_MOTOR_PWM_INT_CMP, &cmp_config[4]);

    pwm_start_counter(TEST_MOTOR_PWM_BASE);
    pwm_issue_shadow_register_lock_event(TEST_MOTOR_PWM_BASE);
    intc_m_enable_irq_with_priority(TEST_MOTOR_PWM_IRQ, 1);

}

void config_pwm_input_capture(void)
{
    pwm_cmp_config_t cmp_config = {0};

    cmp_config.mode = pwm_cmp_mode_input_capture;
    pwm_config_cmp(TEST_MOTOR_PWM_BASE, TEST_MOTOR_PWM_CAP_CMP_CHN, &cmp_config);
    pwm_enable_dma_request(TEST_MOTOR_PWM_BASE, PWM_IRQ_CMP(TEST_MOTOR_PWM_CAP_CMP_CHN));
}

void init_pwm_clk(void)
{
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;
    trgm_output_t trgm_output_cfg;

    pwm_stop_counter(PLA_TMGW_PWM_BASE);
    pwm_get_default_pwm_config(PLA_TMGW_PWM_BASE, &pwm_config);

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 0;
    pwm_config.invert_output = false;
    /*
     * reload and start counter
     */
    pwm_set_reload(PLA_TMGW_PWM_BASE, 0, reload);
    pwm_set_start_count(PLA_TMGW_PWM_BASE, 0, 0);

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = PLA_TMGW_PWM_CMP;  /* start channel */
    pwm_output_ch_cfg.cmp_end_index   = PLA_TMGW_PWM_CMP;  /* end channel */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(PLA_TMGW_PWM_BASE, PLA_TMGW_PWM_CHN, &pwm_output_ch_cfg);

    /*
     * config cmp = RELOAD + 1
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload >> 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_load_cmp_shadow_on_match(PLA_TMGW_PWM_BASE, PLA_TMGW_PWM_CMP, &cmp_config[0]);
    pwm_enable_reload_at_synci(PLA_TMGW_PWM_BASE);

    pwm_start_counter(PLA_TMGW_PWM_BASE);
    pwm_issue_shadow_register_lock_event(PLA_TMGW_PWM_BASE);

    trgm_output_cfg.invert = true;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = PLA_TMGW_CLK_PWM_TRG;
    trgm_output_config(PLA_TMGW_CLK_TRGM, PLA_TMGW_CLK_TRG_NUM, &trgm_output_cfg);

    /**
     * @brief pwm reset
     *
     */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = PLA_TMGW_PWM_SYNCI_IN_TRG;
    trgm_output_config(PLA_TMGW_CLK_TRGM, PLA_TMGW_PWM_SYNCI_IN_TRGNUM, &trgm_output_cfg);

}

hpm_stat_t qei_position_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, QEI_Type *qei_x, uint32_t src, uint8_t data_width, uint32_t size)
{
    dma_handshake_config_t config;
    config.ch_index = ch_num;
    config.dst = (uint32_t)&qei_x->PHCMP;
    config.dst_fixed = true;
    config.src = src;
    config.src_fixed = false;
    config.data_width = data_width;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config, true);
}

void init_qei(void)
{
    trgm_output_t config = {0};
    config.invert = false;
    config.input = PLA_TMGW_QEI_TRGM_QEI_A_SRC;
    /**
     * Trgm in pin from pla
     */
    trgm_output_config(PLA_TMGW_QEI_TRGM, TRGM_TRGOCFG_QEI_A, &config);

    qei_counter_reset_assert(PLA_TMGW_QEI_BASE);
    qei_phase_config(PLA_TMGW_QEI_BASE, PLA_TMGW_QEI_MOTOR_PHASE_COUNT_MAX,
            qei_z_count_inc_on_z_input_assert, false);
    qei_phase_cmp_set(PLA_TMGW_QEI_BASE, pla_tmgw_qei_phcmp[tmgw_dev.run_cmd][0] - 0xc0000000,
            false, qei_rotation_dir_cmp_ignore);
    qei_load_read_trigger_event_enable(PLA_TMGW_QEI_BASE, QEI_EVENT_POSITION_COMPARE_FLAG_MASK);
    qei_set_work_mode(PLA_TMGW_QEI_BASE, qei_work_mode_ud);
    qei_output_trigger_event_enable(PLA_TMGW_QEI_BASE, QEI_EVENT_POSITION_COMPARE_FLAG_MASK);
    qei_counter_reset_release(PLA_TMGW_QEI_BASE);
    /**
     * @brief Construct a new qei dma request enable object
     *
     */
    qei_dma_request_enable(PLA_TMGW_QEI_BASE, QEI_EVENT_POSITION_COMPARE_FLAG_MASK);
    dmamux_config(PLA_TMGW_QEI_DMAMUX, PLA_TMGW_QEI_DMAMUX_CH, PLA_TMGW_QEI_DMAREQ, true);
    qei_position_trigger_dma(PLA_TMGW_QEI_DMA,
                            PLA_TMGW_QEI_DMACH,
                            PLA_TMGW_QEI_BASE,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&pla_tmgw_qei_phcmp[tmgw_dev.run_cmd][1]),
                            DMA_TRANSFER_WIDTH_WORD,
                            sizeof(pla_tmgw_qei_phcmp[tmgw_dev.run_cmd][1]));
    trgm_dma_request_config(PLA_TMGW_QEI_TRGM, TRGM_DMACFG_0, HPM_TRGM0_DMA_SRC_QEI0);

    /**
     * @brief Trig out pin to pla
     *
     */
    config.invert = false;
    config.input = PLA_TMGW_QEI_TRGM_QEI_TRG0;
    trgm_output_config(PLA_TMGW_QEI_TRGM, PLA_TMGW_QEI_TRGM_QEI_PLA_IN, &config);
}

void tamagawa_get_time(uint32_t *time)
{
    *time = hall_get_u_history0(PLA_TMGW_HALL_BASE);
}

void tmgw_hall_init(void)
{
    hall_counter_reset_assert(PLA_TMGW_HALL_BASE);
    hall_counter_reset_release(PLA_TMGW_HALL_BASE);
}

hpm_stat_t tmgw_dma_chained_trigger_dma(DMA_Type *dma_ptr, uint8_t ch_num, uint32_t src, uint32_t dst, uint8_t data_width, uint32_t size)
{

    hpm_stat_t stat = status_success;
    dma_channel_config_t config = {0};
    dma_default_channel_config(dma_ptr, &config);

    config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;

    config.src_width = data_width;
    config.dst_width = data_width;
    config.src_addr = src;
    config.dst_addr = dst;
    config.size_in_byte = size;
    /*  In DMA handshake case, source burst size must be 1 transfer, that is 0. */
    config.src_burst_size = 0;
    config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_linked_descriptors[0]);
    stat = dma_setup_channel(dma_ptr, ch_num, &config, true);
    if (stat != status_success) {
        return stat;
    }
    return stat;

}


/**
 * @brief Must be placed after all peripheral initialization is complete.
 *
 */
void tmgw_dma_chained(void)
{
    uint8_t pos = 0;
    /**
     * @brief SPI_CS_DISABLE
     *
     */
    tmgw_dma_link_src_cs_data = (1 << PLA_TMGW_SPI_CS_GPIO_PIN);
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_cs_data);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_SPI_CS_GPIO_CTRL->DO[PLA_TMGW_SPI_CS_GPIO_INDEX].SET);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief QEI DMA set transfer size
     *
     */
    pos++;
    tmgw_dma_link_src_qei_dma_size = ARRAY_SIZE(pla_tmgw_qei_phcmp[tmgw_dev.run_cmd]);
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_qei_dma_size);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_QEI_DMA->CHCTRL[PLA_TMGW_QEI_DMACH].TRANSIZE);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief QEI DMA set source addr
     *
     */
    pos++;
    tmgw_dma_link_src_qei_source_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&pla_tmgw_qei_phcmp[tmgw_dev.run_cmd][0]);
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_qei_source_addr);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_QEI_DMA->CHCTRL[PLA_TMGW_QEI_DMACH].SRCADDR);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief QEI DMA enable
     *
     */
    pos++;
    tmgw_dma_link_src_qei_dma_enable = PLA_TMGW_QEI_DMA->CHCTRL[PLA_TMGW_QEI_DMACH].CTRL;
    tmgw_dma_link_src_qei_dma_enable |= DMA_CHCTRL_CTRL_ENABLE_MASK;
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_qei_dma_enable);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_QEI_DMA->CHCTRL[PLA_TMGW_QEI_DMACH].CTRL);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

   /**
     * @brief QEI counter_reset assert
     *
     */
    pos++;
    tmgw_dma_link_src_qei_cr_assert = PLA_TMGW_QEI_BASE->CR;
    tmgw_dma_link_src_qei_cr_assert |= QEI_CR_RSTCNT_MASK;
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_qei_cr_assert);
    tmgw_dma_linked_descriptors[pos].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_QEI_BASE->CR);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
   tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief qei counter reset release
     *
     */
    pos++;
    tmgw_dma_link_src_qei_cr_release = PLA_TMGW_QEI_BASE->CR;
    tmgw_dma_link_src_qei_cr_release &= ~QEI_CR_RSTCNT_MASK;
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_qei_cr_release);
    tmgw_dma_linked_descriptors[pos].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_QEI_BASE->CR);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);


    /**
     * @brief SPI DMA TX RX
     *
     */

    /**
     * @brief SPI DMA set tx transfer size
     *
     */
    pos++;
    tmgw_dma_link_src_spitx_dma_size = (tmgw_dev.buf_cfg[tmgw_dev.run_cmd].tx_num + 1);
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_spitx_dma_size);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_TX_DMA_CH].TRANSIZE);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief SPI DMA set tx source addr
     *
     */
    pos++;
    tmgw_dma_link_src_spitx_source_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)tmgw_dev.buf_cfg[tmgw_dev.run_cmd].send_buf);
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_spitx_source_addr);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_TX_DMA_CH].SRCADDR);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
   tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief SPI DMA set rx transfer size
     *
     */
    pos++;
    tmgw_dma_link_src_spirx_dma_size = (tmgw_dev.buf_cfg[tmgw_dev.run_cmd].rx_num + tmgw_dev.buf_cfg[tmgw_dev.run_cmd].tx_num + 1);
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_spirx_dma_size);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_RX_DMA_CH].TRANSIZE);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
   tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief SPI DMA set rx source addr
     *
     */
    pos++;
    tmgw_dma_link_src_spirx_source_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&receive_buff[0]);
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_spirx_source_addr);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_RX_DMA_CH].DSTADDR);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
   tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief SPI tx DMA enable
     *
     */
    pos++;
    tmgw_dma_link_src_spitx_dma_enable = PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_TX_DMA_CH].CTRL;
    tmgw_dma_link_src_spitx_dma_enable |= DMA_CHCTRL_CTRL_ENABLE_MASK;
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_spitx_dma_enable);
    tmgw_dma_linked_descriptors[pos].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_TX_DMA_CH].CTRL);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief SPI rx DMA enable
     *
     */
    pos++;
    tmgw_dma_link_src_spirx_dma_enable = PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_RX_DMA_CH].CTRL;
    tmgw_dma_link_src_spirx_dma_enable |= DMA_CHCTRL_CTRL_ENABLE_MASK;
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_spirx_dma_enable);
    tmgw_dma_linked_descriptors[pos].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_SPI_DMA->CHCTRL[PLA_TMGW_SPI_RX_DMA_CH].CTRL);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
   tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);


    /**
     * @brief hall counter reset release
     *
     */
    pos++;
    tmgw_dma_link_src_hall_cr_release = PLA_TMGW_HALL_BASE->CR;
    tmgw_dma_link_src_hall_cr_release &= ~HALL_CR_RSTCNT_MASK;
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_hall_cr_release);
    tmgw_dma_linked_descriptors[pos].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_HALL_BASE->CR);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief SPI_CS_ENABLE
     *
     */
    pos++;
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_src_cs_data);
    tmgw_dma_linked_descriptors[pos].dst_addr =
        core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&PLA_TMGW_SPI_CS_GPIO_CTRL->DO[PLA_TMGW_SPI_CS_GPIO_INDEX].CLEAR);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief Communication completion status flag bit
     *
     */
    pos++;

    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&pla_tmgw_comm_completed);
    tmgw_dma_linked_descriptors[pos].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_status_completed);
    tmgw_dma_linked_descriptors[pos].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED);
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[pos + 1]);

    /**
     * @brief DMA Link config
     *
     */
    pos++;

    tmgw_dma_link_src_link_dma_enable = 0;
    tmgw_dma_link_src_link_dma_enable |= DMA_CHCTRL_CTRL_ENABLE_MASK
                                                |DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(0)
                                                | DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
                                                | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
                                                | DMA_CHCTRL_CTRL_SRCMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
                                                | DMA_CHCTRL_CTRL_DSTMODE_SET(DMA_HANDSHAKE_MODE_HANDSHAKE)
                                                | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
                                                | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
                                                | DMA_CHCTRL_CTRL_SRCREQSEL_SET(PLA_TMGW_DMA_LINK_DMAMUX_CH)
                                                | DMA_CHCTRL_CTRL_DSTREQSEL_SET(PLA_TMGW_DMA_LINK_DMAMUX_CH);
    tmgw_dma_linked_descriptors[pos].trans_size = 1;
    tmgw_dma_linked_descriptors[pos].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_srcbuf);
    tmgw_dma_linked_descriptors[pos].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_dstbuf);
    tmgw_dma_linked_descriptors[pos].ctrl = tmgw_dma_link_src_link_dma_enable;
    tmgw_dma_linked_descriptors[pos].linked_ptr = SET_DMA_LINK_PTR((uint32_t)&tmgw_dma_linked_descriptors[0]);
    if (l1c_dc_is_enabled()) {
        l1c_dc_flush_all();
    }

    tmgw_dma_chained_trigger_dma(PLA_TMGW_DMA_LINK_DMA, PLA_TMGW_DMA_LINK_DMA_CH,
            core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_srcbuf),
            core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tmgw_dma_link_dstbuf),
            DMA_TRANSFER_WIDTH_WORD,
            PLA_TMGW_DMA_LINK_TRAN_SIZE);
    dmamux_config(PLA_TMGW_DMA_LINK_DMAMUX, PLA_TMGW_DMA_LINK_DMAMUX_CH, PLA_TMGW_DMA_LINK_DMA_REQ, true);
    trgm_dma_request_config(PLA_TMGW_DMA_LINK_TRGM, TRGM_DMACFG_2, TEST_MOTOR_PWM_CAP_REFRESH_DMA_LINK_CMP);

}

int main(void)
{
    uint32_t freq;

    board_init();
    board_init_spi_clock(PLA_TMGW_SPI);
    init_pla_tamagawa_pins();
    init_pla_pins();

    gpio_set_pin_output(PLA_TMGW_POWER_GPIO_CTRL, PLA_TMGW_POWER_GPIO_INDEX, PLA_TMGW_POWER_GPIO_PIN);
    TMGW_POWER_ENABLE();
    tmgw_cmd_init_data((tmgw_cmd_cfg_t *)tmgw_dev.buf_cfg);
    /**
     * @brief eeprom write test
     *
     */
    /**
     *   tmgw_cmd_send_write_eeprom(111, 11, &tmgw_dev.buf_cfg[tmgw_cmd_wr_eeprom]);
     *   tmgw_dev.run_cmd = tmgw_cmd_wr_eeprom;
    */
    tmgw_cmd_qei_phcmp_init(&pla_tmgw_qei_phcmp[0], (tmgw_cmd_cfg_t *)tmgw_dev.buf_cfg);

    tmgw_hall_init();
    init_pwm_pla_trgm(PLA_TMGW_TRGM);
    pla_tmgw_init();
    init_qei();
    init_spi_cfg(PLA_TMGW_SPI);
    freq = clock_get_frequency(PLA_TMGW_PWM_CLOCK_NAME);
    reload = freq / 1000 * PLA_CLK_PERIOD_IN_MS - 1;
    init_pwm_clk();
    init_pwm_trg();
    config_pwm_input_capture();
#ifdef PLA_DMA_REFRESH_BY_DMA_LINK
    tmgw_dma_chained();
#endif
    pwm_enable_irq(TEST_MOTOR_PWM_BASE, PWM_IRQ_CMP(TEST_MOTOR_PWM_INT_CMP));
    tmgw_cmd = tmgw_dev.run_cmd;
    while (1) {
        if (tmgw_msg.is_completed) {
            tmgw_msg.is_completed = false;
            switch (tmgw_msg.cmd) {
            case tmgw_cmd_id0:
            case tmgw_cmd_id7:
            case tmgw_cmd_id8:
            case tmgw_cmd_idc:
                printf("cmd:%d time:%d, abs pos:%d.\r\n", tmgw_msg.cmd, tmgw_dev.comm_time_delay, tmgw_msg.abs);
            break;
            case tmgw_cmd_id1:
                printf("cmd:%d time:%d, abm pos:%d.\r\n", tmgw_msg.cmd, tmgw_dev.comm_time_delay, tmgw_msg.abm);
            break;
            case tmgw_cmd_id2:
                printf("cmd:%d time:%d, id:%d.\r\n", tmgw_msg.cmd, tmgw_dev.comm_time_delay, tmgw_msg.id);
            break;
            case tmgw_cmd_id3:
                printf("cmd:%d time:%d, abs pos:%d, abm pos:%d, id:%d.\r\n", tmgw_msg.cmd, tmgw_dev.comm_time_delay, tmgw_msg.abs, tmgw_msg.abm, tmgw_msg.id);
            break;
            case tmgw_cmd_wr_eeprom:
                if (tmgw_msg.is_eeprom_busy) {
                    printf("eeprom write fail, bus busy.\r\n");
                } else {
                    printf("eeprom wtire address:%d, data:%d.\r\n", tmgw_msg.eeprom_addr, tmgw_msg.eeprom_data);
                }
            break;
            case tmgw_cmd_rd_eeprom:
                if (tmgw_msg.is_eeprom_busy) {
                    printf("eeprom read fail, bus busy.\r\n");
                } else {
                    printf("eeprom read address:%d, data:%d.\r\n", tmgw_msg.eeprom_addr, tmgw_msg.eeprom_data);
                }
            break;
            default:
                printf("undefined cmd %d.\r\n", tmgw_msg.cmd);
            break;
            }
        }
        if (tmgw_msg.is_crc_error) {
            tmgw_msg.is_crc_error = false;
            printf("crc error.\r\n");
        }
        if (tmgw_msg.is_cmd_error) {
            tmgw_msg.is_cmd_error = false;
            printf("Inconsistent receive and send commands.\r\n");
        }
    };
    return 0;
}
