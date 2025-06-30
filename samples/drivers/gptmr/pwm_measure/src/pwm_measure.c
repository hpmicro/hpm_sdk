/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"
#if (defined (DMA_SOC_MAX_COUNT) && (DMA_SOC_MAX_COUNT == 2)) && (defined (CONFIG_USE_DMA) && (CONFIG_USE_DMA == 1))
#include "hpm_dmamux_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#endif

#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME

#if (defined (DMA_SOC_MAX_COUNT) && (DMA_SOC_MAX_COUNT == 2)) && (defined (CONFIG_USE_DMA) && (CONFIG_USE_DMA == 1))
/* XDMA is 64bit width.so can transfer gptmr two 32bit register,it's include CAPPRD, CAPDTY. */
#define APP_GPTMR_DMA                 BOARD_APP_XDMA
#define APP_GPTMR_DMA_IRQ             BOARD_APP_XDMA_IRQ
#define APP_DMA_SRC_WIDTH             DMA_TRANSFER_WIDTH_DOUBLE_WORD
#define APP_DMA_DST_WIDTH             DMA_TRANSFER_WIDTH_DOUBLE_WORD
#define APP_GPTMR_DMA_SRC             BOARD_GPTMR_DMA_SRC
#define APP_DMA_CH                    (0U)
#define APP_DMAMUX_CH                 DMA_SOC_CHN_TO_DMAMUX_CHN(APP_GPTMR_DMA, APP_DMA_CH)
#define APP_MEASURE_COUNT             (10U)

typedef struct {
    uint32_t priiod;
    uint32_t duty;
} pwm_measure_cfg_t;

static void dma_transfer_config(void);

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(4) pwm_measure_cfg_t pwm_meas_table[APP_MEASURE_COUNT];
volatile uint32_t count;
volatile bool     dma_is_done;

SDK_DECLARE_EXT_ISR_M(APP_GPTMR_DMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile hpm_stat_t stat;
    stat = dma_check_transfer_status(APP_GPTMR_DMA, APP_DMA_CH);
    if (stat & DMA_CHANNEL_STATUS_TC) {
        dma_disable_channel(APP_GPTMR_DMA, APP_DMA_CH);
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
        dma_is_done = true;
    }
}
#endif

static void pwm_measure_config(void);
volatile uint32_t gptmr_freq;

int main(void)
{
    board_init();
    init_gptmr_pins(APP_BOARD_GPTMR);
    printf("gptmr pwm measure test\n");
#if (defined (DMA_SOC_MAX_COUNT) && (DMA_SOC_MAX_COUNT == 2)) && (defined (CONFIG_USE_DMA) && (CONFIG_USE_DMA == 1))
    dma_transfer_config();
#endif
    pwm_measure_config();
    while (1) {
        /* please make sure deplay time is more than one pwm cycle */
        board_delay_ms(200);
#if (defined (DMA_SOC_MAX_COUNT) && (DMA_SOC_MAX_COUNT == 2)) && (defined (CONFIG_USE_DMA) && (CONFIG_USE_DMA == 1))
        dma_enable_channel(APP_GPTMR_DMA, APP_DMA_CH);
        if (dma_is_done) {
            dma_is_done = false;
            gptmr_stop_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            board_led_toggle();
            for (uint32_t i = 0; i < sizeof(pwm_meas_table) / sizeof(pwm_measure_cfg_t); i++) {
                printf("[table_%d]measured frequency: %f\n", i, (float) gptmr_freq / pwm_meas_table[i].priiod);
                printf("[table_%d]measured duty cycle: %.2f%%\n", i, ((float) pwm_meas_table[i].duty / pwm_meas_table[i].priiod) * 100);
            }
        }
#else
        uint32_t period, duty;
        period = gptmr_channel_get_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_counter_type_measured_period);
        duty = gptmr_channel_get_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_counter_type_measured_duty_cycle);
        /* in pwm measure mode, the CAPF will set to 1 when capture the rising edge signal,so can used the flag when whether to collect pwm */
        if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_CAP_STAT_MASK(APP_BOARD_GPTMR_CH))) {
            gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_CAP_STAT_MASK(APP_BOARD_GPTMR_CH));
            printf("measured frequency: %f\n", (float) gptmr_freq / period);
            printf("measured duty cycle: %.2f%%\n", ((float) duty / period) * 100);
        }
#endif
    }
    return 0;
}

#if (defined (DMA_SOC_MAX_COUNT) && (DMA_SOC_MAX_COUNT == 2)) && (defined (CONFIG_USE_DMA) && (CONFIG_USE_DMA == 1))
static void dma_transfer_config(void)
{
    dma_channel_config_t ch_config = {0};
    dma_disable_channel(APP_GPTMR_DMA, APP_DMA_CH);
    dma_reset(APP_GPTMR_DMA);
    dma_default_channel_config(APP_GPTMR_DMA, &ch_config);
    ch_config.src_addr = (uint32_t)&APP_BOARD_GPTMR->CHANNEL[APP_BOARD_GPTMR_CH].CAPPRD;
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&pwm_meas_table[0].priiod);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_width = APP_DMA_SRC_WIDTH;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.dst_width = APP_DMA_DST_WIDTH;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.size_in_byte = sizeof(pwm_meas_table);
    if (status_success != dma_setup_channel(APP_GPTMR_DMA, APP_DMA_CH, &ch_config, false)) {
        printf(" dma setup channel failed\n");
        return;
    }
}
#endif

static void pwm_measure_config(void)
{
    gptmr_channel_config_t config;

    board_init_gptmr_clock(APP_BOARD_GPTMR);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
#if (defined (DMA_SOC_MAX_COUNT) && (DMA_SOC_MAX_COUNT == 2)) && (defined (CONFIG_USE_DMA) && (CONFIG_USE_DMA == 1))
    gptmr_stop_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    config.cmp_initial_polarity_high = false;
    config.dma_request_event = gptmr_dma_request_on_input_signal_toggle;
    config.mode = gptmr_work_mode_measure_width;
    gptmr_channel_enable_dma_request(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, false);
    dmamux_config(BOARD_APP_DMAMUX, APP_DMAMUX_CH, APP_GPTMR_DMA_SRC, true);
    intc_m_enable_irq_with_priority(APP_GPTMR_DMA_IRQ, 4);
#else
    config.mode = gptmr_work_mode_measure_width;
#endif
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
}


