/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_dmamux_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_debug_console.h"

#define APP_BOARD_GPTMR               BOARD_GPTMR_PWM
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_PWM_CHANNEL
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_PWM_IRQ
#define APP_GPTMR_DMA                 BOARD_APP_HDMA
#define APP_GPTMR_DMA_IRQ             BOARD_APP_HDMA_IRQ
#define APP_DMA_SRC_WIDTH             DMA_TRANSFER_WIDTH_WORD
#define APP_DMA_DST_WIDTH             DMA_TRANSFER_WIDTH_WORD
#define APP_BOARD_GPTMR_DMA_CH        (0U)
#define APP_BOARD_GPTMR_DMAMUX_CH     DMA_SOC_CHN_TO_DMAMUX_CHN(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH)

#define APP_FIXED_PULSE_WIDTH_US      (5U)
#define APP_MAX_CYCLE_US              (100U)
#define APP_MIN_CYCLE_US              (10U)
#define APP_CYCLE_COUNT               (280U)

static void dma_transfer_config(void);
static void gptmr_config(void);
static void generate_T_shape_data(void);

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(4) uint32_t shape_table[APP_CYCLE_COUNT];
volatile uint32_t gptmr_freq;
volatile uint32_t count;
volatile bool     dma_is_done;

SDK_DECLARE_EXT_ISR_M(APP_GPTMR_DMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile hpm_stat_t stat;
    stat = dma_check_transfer_status(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH);
    if (stat & DMA_CHANNEL_STATUS_TC) {
        dma_disable_channel(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH);
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
        dma_is_done = true;
    }
}

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, timer_isr)
void timer_isr(void)
{
    /* an reload update interrupt is to update the cmp0 value, output pulses with a fixed pulse width*/
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH))) {
        gptmr_disable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
        gptmr_update_cmp(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, 0, 1);
        gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    }
}

int main(void)
{
    count = 0;
    dma_is_done = false;
    board_init();
    printf("generate T_shape_accel_decel test\n");
    init_gptmr_pins(APP_BOARD_GPTMR);
    board_init_led_pins();

    gptmr_freq = board_init_gptmr_clock(APP_BOARD_GPTMR);

    generate_T_shape_data();
    dma_transfer_config();
    gptmr_config();
    while (1) {
        /* when the dma transfer reload value complete, need wait the last pulse complete*/
        if (dma_is_done) {
            if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH))) {
                gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
                dma_is_done = false;
                gptmr_stop_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
                board_led_toggle();
            }
        }
    }
    return 0;
}

static void dma_transfer_config(void)
{
    dma_channel_config_t ch_config = {0};
    dma_disable_channel(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH);
    dma_reset(APP_GPTMR_DMA);
    dma_default_channel_config(APP_GPTMR_DMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&shape_table[0]);
    ch_config.dst_addr = (uint32_t)&APP_BOARD_GPTMR->CHANNEL[APP_BOARD_GPTMR_CH].RLD;
    ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.src_width = APP_DMA_SRC_WIDTH;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.dst_width = APP_DMA_DST_WIDTH;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.size_in_byte = sizeof(shape_table);
    if (status_success != dma_setup_channel(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH, &ch_config, false)) {
        printf(" dma setup channel failed\n");
        return;
    }
}

static void gptmr_config(void)
{
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_stop_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    config.cmp_initial_polarity_high = false;
    config.dma_request_event = gptmr_dma_request_on_reload;
    config.reload = gptmr_freq / 1000000 * APP_FIXED_PULSE_WIDTH_US;
    config.enable_cmp_output = true;
    config.cmp[0] = gptmr_freq / 1000000 * APP_FIXED_PULSE_WIDTH_US + 100;
    config.cmp[1] = gptmr_freq / 1000000 * APP_FIXED_PULSE_WIDTH_US + 1;
    gptmr_channel_enable_dma_request(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, false);
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 4);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    dmamux_config(BOARD_APP_DMAMUX, APP_BOARD_GPTMR_DMAMUX_CH, BOARD_GPTMR_PWM_DMA_SRC, true);
    dma_enable_channel(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH);
    intc_m_enable_irq_with_priority(APP_GPTMR_DMA_IRQ, 4);
}

static void generate_T_shape_data(void)
{
    uint32_t i;
    uint32_t half_tmp     = APP_CYCLE_COUNT / 2;
    uint32_t quarter_tmp  = half_tmp / 2;
    uint32_t both_count   = quarter_tmp;
    uint32_t middle_count = APP_CYCLE_COUNT - (both_count * 2);
    uint32_t both_step    = (APP_MAX_CYCLE_US - APP_MIN_CYCLE_US) / both_count;

    for (i = 0; i < both_count; i += both_step) {
        shape_table[i] = APP_MAX_CYCLE_US - i;
    }
    for (i = 0; i < middle_count; i++) {
        shape_table[i + both_count] = APP_MIN_CYCLE_US;
    }
    for (i = 0; i < both_count; i += both_step) {
        shape_table[i + both_count + middle_count] = APP_MIN_CYCLE_US + i;
    }
    for (i = 0; i < APP_CYCLE_COUNT; i++) {
        shape_table[i] = gptmr_freq / 1000000 * shape_table[i];
        printf("shape_table[%d] :%d\n", i, shape_table[i]);
    }
}
