/*
 * Copyright (c) 2023-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_sysctl_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#include "hpm_gptmrv2_drv.h"
#else
#include "hpm_gptmr_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_debug_console.h"
#include "math.h"

#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#define GPTMR_CHANNEL_CR_CEN_MASK     GPTMRV2_CHANNEL_CR_CEN_MASK
#endif
#define APP_BOARD_GPTMR               BOARD_GPTMR_PWM
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_PWM_CHANNEL
#if (DMA_SOC_MAX_COUNT == 2) && defined(HPM_XDMA_BASE)
#define APP_GPTMR_DMA                 BOARD_APP_DMA1
#define APP_GPTMR_DMA_IRQ             BOARD_APP_DMA1_IRQ
#define APP_DMA_SRC_WIDTH             DMA_TRANSFER_WIDTH_DOUBLE_WORD
#define APP_DMA_DST_WIDTH             DMA_TRANSFER_WIDTH_DOUBLE_WORD
#else
#define APP_GPTMR_DMA                 BOARD_APP_DMA0
#define APP_GPTMR_DMA_IRQ             BOARD_APP_DMA0_IRQ
#define APP_DMA_SRC_WIDTH             DMA_TRANSFER_WIDTH_WORD
#define APP_DMA_DST_WIDTH             DMA_TRANSFER_WIDTH_WORD
#endif
#define APP_BOARD_GPTMR_DMA_CH        (0U)
#define APP_BOARD_GPTMR_DMAMUX_CH     DMA_SOC_CHN_TO_DMAMUX_CHN(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH)

#ifndef APP_CMP_INITIAL_POLARITY_HIGH
#define APP_CMP_INITIAL_POLARITY_HIGH  (0U)
#endif

/* Defines the offset value for the timer's compare register (CMP0), in timer ticks. */
#ifndef APP_OFFSET_CMP0_TICK
#define APP_OFFSET_CMP0_TICK            (100U)
#endif

#ifndef APP_GPTMR_TARGET_FREQ
#define APP_GPTMR_TARGET_FREQ         (10000000UL) /* 10MHz, for PWM resolution */
#endif

/* Specifies the maximum cycle duration for the PWM signal, in microseconds (us) */
#define APP_MAX_CYCLE_US                (100U)

/* Specifies the minimum cycle duration for the PWM signal, in microseconds (us) */
#define APP_MIN_CYCLE_US                (10U)

/* Defines the total number of cycles used to generate the PWM signal or sine wave */
#define APP_CYCLE_COUNT                  (100U)

/* If DMA_SOC_MAX_COUNT=2 and XDMA is used
 * XDMA's transfer width can be 64-bit, allowing transmission of CMP1 and reload values to adjust duty cycle and frequency
 */
typedef struct {
#if (DMA_SOC_MAX_COUNT == 2) && defined(HPM_XDMA_BASE)
    uint32_t compare_value;
#endif
    uint32_t reload;
} pwm_generate_cfg_t;

static void dma_transfer_config(void);
static void gptmr_config(void);
static void generate_T_shape_data(void);

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) pwm_generate_cfg_t shape_table[APP_CYCLE_COUNT];
/* descriptor should be DMA_LINKED_DESCRIPTOR_ALIGN aligned */
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(DMA_LINKED_DESCRIPTOR_ALIGN) dma_linked_descriptor_t descriptors[1];
volatile uint32_t gptmr_freq;
volatile bool     dma_is_done;
volatile uint32_t first_compare_value;
static  uint32_t  cmp0_offset_tick;
ATTR_PLACE_AT_NONCACHEABLE volatile uint32_t gptmr_cr_reg_value;

SDK_DECLARE_EXT_ISR_M(APP_GPTMR_DMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile hpm_stat_t stat;
    stat = dma_check_transfer_status(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH);
    if (stat & DMA_CHANNEL_STATUS_TC) {
        /* when DMA transfer is complete, DMA is automatically disabled, so the dma_disable_channel API is not needed.*/
        dma_is_done = true;
    }
}

int main(void)
{
    dma_is_done = false;
    board_init();
    printf("generate T_shape_accel_decel test\n");
    gptmr_freq = board_init_gptmr_clock(APP_BOARD_GPTMR);

#if APP_CMP_INITIAL_POLARITY_HIGH
    /* When the reset level is low and the GPTMR output compare is high, to avoid an initial pulse edge
     * configure the polarity level during GPTMR pinmux initialization.
     */
    gptmr_channel_set_cmp_initial_polarity(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, true);
#endif
    init_gptmr_pins(APP_BOARD_GPTMR);
    board_init_led_pins();

    generate_T_shape_data();
    dma_transfer_config();
    gptmr_config();
    while (1) {
        if (dma_is_done) {
            board_delay_ms(500);
            dma_is_done = false;
            dma_transfer_config();
            gptmr_config();
            board_led_toggle();
        }
    }
    return 0;
}

static void dma_transfer_config(void)
{
    dma_channel_config_t ch_config = {0};
    dma_default_channel_config(APP_GPTMR_DMA, &ch_config);
    ch_config.dst_addr = (uint32_t)&APP_BOARD_GPTMR->CHANNEL[APP_BOARD_GPTMR_CH].CR;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.size_in_byte = 1 << DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&gptmr_cr_reg_value);
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.linked_ptr = 0;
    /* In the last pulse, use DMA linked configuration to disable the timer counter and stop PWM output. */
    dma_config_linked_descriptor(APP_GPTMR_DMA, &descriptors[0], APP_BOARD_GPTMR_DMA_CH, &ch_config);
#if (DMA_SOC_MAX_COUNT == 2) && defined(HPM_XDMA_BASE)
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&shape_table[1].compare_value);
    ch_config.dst_addr = (uint32_t)&APP_BOARD_GPTMR->CHANNEL[APP_BOARD_GPTMR_CH].CMP[1];
    ch_config.size_in_byte = sizeof(shape_table);
#else
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&shape_table[1].reload);
    ch_config.dst_addr = (uint32_t)&APP_BOARD_GPTMR->CHANNEL[APP_BOARD_GPTMR_CH].RLD;
    ch_config.size_in_byte = sizeof(shape_table);
#endif
    ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.src_width = APP_DMA_SRC_WIDTH;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.dst_width = APP_DMA_DST_WIDTH;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.linked_ptr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&descriptors[0]);
    dmamux_config(BOARD_APP_DMAMUX, APP_BOARD_GPTMR_DMAMUX_CH, BOARD_GPTMR_PWM_DMA_SRC, true);
    if (status_success != dma_setup_channel(APP_GPTMR_DMA, APP_BOARD_GPTMR_DMA_CH, &ch_config, true)) {
        printf(" dma setup channel failed\n");
        return;
    }
    intc_m_enable_irq_with_priority(APP_GPTMR_DMA_IRQ, 4);
}

static void gptmr_config(void)
{
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_channel_enable(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, false);
    gptmr_stop_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
    config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
#endif
    config.cmp_initial_polarity_high = APP_CMP_INITIAL_POLARITY_HIGH;
    config.dma_request_event = gptmr_dma_request_on_reload;
    config.enable_cmp_output = true;
    /* Set the initial reload value from the first entry in the shape table */
    config.reload = shape_table[0].reload - cmp0_offset_tick;
    config.cmp[0] = cmp0_offset_tick;
    /* Set the initial compare value from the first entry in the shape table */
    config.cmp[1] = first_compare_value + cmp0_offset_tick;
    if (gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, true) != status_success) {
        printf("config gptmr channel failed\n");
        return;
    }
    gptmr_cr_reg_value = APP_BOARD_GPTMR->CHANNEL[APP_BOARD_GPTMR_CH].CR & ~GPTMR_CHANNEL_CR_CEN_MASK;
}

static void generate_T_shape_data(void)
{
    uint32_t i = 0;
    float angle = 0.0, pwm_time_us = 0.0;
    uint32_t period = 0;
    uint32_t timer_freq = 0;
#if (DMA_SOC_MAX_COUNT == 1) || (DMA_SOC_MAX_COUNT == 2 && !defined(HPM_XDMA_BASE))
    uint32_t min_reload = UINT32_MAX;
#endif
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
    /*
     * GPTMRV2 uses a prescaler to reduce the timer clock from gptmr_freq to APP_GPTMR_TARGET_FREQ,
     * so APP_OFFSET_CMP0_TICK must be scaled down proportionally:
     *   cmp0_offset_tick = APP_OFFSET_CMP0_TICK * (target_freq / gptmr_freq)
     *                    = APP_OFFSET_CMP0_TICK / (gptmr_freq / APP_GPTMR_TARGET_FREQ)
     * e.g. 100 * (10MHz / 200MHz) = 5
     */
    timer_freq = gptmr_freq / (gptmr_freq / APP_GPTMR_TARGET_FREQ);
    cmp0_offset_tick = APP_OFFSET_CMP0_TICK / (gptmr_freq / APP_GPTMR_TARGET_FREQ);
#else
    timer_freq = gptmr_freq;
    cmp0_offset_tick = APP_OFFSET_CMP0_TICK;
#endif
    if (cmp0_offset_tick < 1) {
        cmp0_offset_tick = 1;
    }
    for (i = 0; i < APP_CYCLE_COUNT; i++) {
        /* Calculate the angle for the sine wave, linearly decreasing from PI to 0 */
        angle = 3.1415926 - ((float)i / (float)APP_CYCLE_COUNT) * 3.1415926;

        /* Clamp the angle to the range [0, PI] */
        if (angle < 0.0)  {
            angle = 0.0;
        }
        if (angle > 3.1415926) {
            angle = 3.1415926;
        }

        /* Calculate the PWM period based on the sine wave */
        pwm_time_us = APP_MAX_CYCLE_US - (APP_MAX_CYCLE_US - APP_MIN_CYCLE_US) * sin(angle);

        /* Convert the period from microseconds to timer ticks */
        period = (uint32_t)(pwm_time_us * timer_freq / 1000000) - 1;

        /* Ensure the period is not less than the minimum allowed value */
        if (period < 5 * (timer_freq / 1000000)) {
            period = 5 * (timer_freq / 1000000);
        }
        /* Configure the compare value and reload value for the PWM signal */
        shape_table[i].reload = period - cmp0_offset_tick;
#if (DMA_SOC_MAX_COUNT == 2) && defined(HPM_XDMA_BASE)
        shape_table[i].compare_value = period / 2 + cmp0_offset_tick;
        printf("shape_table[%d] compare_value:%d, reload:%d\n", i, shape_table[i].compare_value, shape_table[i].reload);
#endif
    }
#if (DMA_SOC_MAX_COUNT == 1) || (DMA_SOC_MAX_COUNT == 2 && !defined(HPM_XDMA_BASE))
    for (i = 0; i < APP_CYCLE_COUNT; i++) {
        if (shape_table[i].reload < min_reload) {
            min_reload = shape_table[i].reload;
        }
    }
    first_compare_value = (min_reload > 1) ? ((min_reload - 1) / 2) : 1;
    for (i = 0; i < APP_CYCLE_COUNT; i++) {
        printf("shape_table[%d] compare_value:%d, reload:%d\n", i, first_compare_value, shape_table[i].reload);
    }
#else
    first_compare_value = shape_table[0].compare_value;
#endif
}
