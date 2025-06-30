/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <math.h>
#include "board.h"
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_dmamux_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif


#define APP_BOARD_GPTMR               BOARD_GPTMR
#define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
#define APP_BOARD_GPTMR_CLOCK         BOARD_GPTMR_CLK_NAME

#define APP_BOARD_TIMER_CH            (APP_BOARD_GPTMR_CH + 1)
#define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ

#ifndef CONFIG_SENT_IDLE_INTERVAL_US
#define CONFIG_SENT_IDLE_INTERVAL_US  (1000U)
#endif

#define APP_GPTMR_DMAMUX              BOARD_APP_DMAMUX
#if defined(DMA_SOC_MAX_COUNT) && (DMA_SOC_MAX_COUNT == 2)
/* XDMA is 64bit width.so can transfer gptmr two 32bit register,it's include CAPPRD, CAPDTY. */
#define APP_GPTMR_DMA                 BOARD_APP_XDMA
#define APP_GPTMR_DMA_IRQ             BOARD_APP_XDMA_IRQ
#define APP_GPTMR_DMA_MUX_CH          DMAMUX_MUXCFG_XDMA_MUX0
#define APP_DMA_SRC_WIDTH             DMA_TRANSFER_WIDTH_DOUBLE_WORD
#define APP_DMA_DST_WIDTH             DMA_TRANSFER_WIDTH_DOUBLE_WORD
#define APP_GPTMR_DMA_SRC             BOARD_GPTMR_DMA_SRC
#else
/* HDMA is 32bit width.so can transfer gptmr one 32bit register,it's only include CAPPRD */
#define APP_GPTMR_DMA                 BOARD_APP_HDMA
#define APP_GPTMR_DMA_IRQ             BOARD_APP_HDMA_IRQ
#define APP_GPTMR_DMA_MUX_CH          DMAMUX_MUXCFG_HDMA_MUX0
#define APP_DMA_SRC_WIDTH             DMA_TRANSFER_WIDTH_WORD
#define APP_DMA_DST_WIDTH             DMA_TRANSFER_WIDTH_WORD
#define APP_GPTMR_DMA_SRC             BOARD_GPTMR_DMA_SRC
#endif
#define APP_DMA_CH                    (0U)
#define APP_MEASURE_COUNT             (2048U)


/* sent signal clock tick time: 3us ±20% */
#ifndef SENT_CLCOK_TICK_MAX_US
#define SENT_CLCOK_TICK_MAX_US        ((float)3.6)
#endif
#ifndef SENT_CLCOK_TICK_MIN_US
#define SENT_CLCOK_TICK_MIN_US        ((float)2.4)
#endif
/* A sequence of one up to six 4 bit Data nibble pulses */
#define SENT_MAX_NIBBLE_COUNT                   (8U)
#define SENT_SYNC_CALI_TICKS                   (56U)
/* include status nibble, data nibble, crc nibble */
#define SENT_MIN_DATA_TICKS                    (12U)
#define SENT_MAX_DATA_TICKS                    (27U)
/*pause nibble: Minimum Length 12 ticks, Maximum Length 768 ticks */
#define SENT_MIN_PAUSE_TICKS                   (12U)
#define SENT_MAX_PAUSE_TICKS                   (768U)
typedef enum {
    sent_sync_table_num = 0,
    sent_data_table_num,
    sent_pause_table_num,
} sent_sequence_table_t;

typedef struct {
    uint32_t period;
#if defined(DMA_SOC_MAX_COUNT) && (DMA_SOC_MAX_COUNT == 2)
    uint32_t duty;
#endif
} pwm_measure_cfg_t;

typedef struct {
    float max_value;
    float min_value;
} sent_us_tick_t;

typedef struct {
    uint8_t status;
    uint8_t data_nibble[SENT_MAX_NIBBLE_COUNT];
    uint8_t data_len;
    uint8_t crc;
    bool pause;
} sent_data_code_t;

/*slot channel max frame lenth is 18*/
typedef struct {
    sent_data_code_t code[18];
    uint8_t sent_data_len;
} sent_data_format_t;

static void dma_transfer_config(void);

/* use J2716 recommends*/
static uint8_t crc4_calculate(uint8_t *data, uint8_t len);

static hpm_stat_t pwm_process_sent(float *pwm_priiod_table, uint16_t pwm_count, sent_data_format_t *sent);

const uint8_t crc4_table_byte[16]= {
    0, 13, 7, 10, 14, 3, 9, 4, 1, 12, 6, 11, 15, 2, 8, 5
};

sent_us_tick_t sent_ticks_table[] = {
    {((float)SENT_SYNC_CALI_TICKS * SENT_CLCOK_TICK_MAX_US), ((float)SENT_SYNC_CALI_TICKS * SENT_CLCOK_TICK_MIN_US)},
    {((float)SENT_MAX_DATA_TICKS * SENT_CLCOK_TICK_MAX_US), ((float)SENT_MIN_DATA_TICKS * SENT_CLCOK_TICK_MIN_US)},
    {((float)SENT_MAX_PAUSE_TICKS * SENT_CLCOK_TICK_MAX_US), ((float)SENT_MIN_PAUSE_TICKS * SENT_CLCOK_TICK_MIN_US)}
};

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) pwm_measure_cfg_t pwm_meas_table[APP_MEASURE_COUNT];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) float sent_signal_table[APP_MEASURE_COUNT];


volatile uint32_t count;
volatile bool     dma_is_done;
volatile uint32_t dma_actual_len;
static volatile uint32_t record_dma_count = 0;
SDK_DECLARE_EXT_ISR_M(APP_GPTMR_DMA_IRQ, isr_dma)
void isr_dma(void)
{
    volatile hpm_stat_t stat;
    stat = dma_check_transfer_status(APP_GPTMR_DMA, APP_DMA_CH);
    if (stat & DMA_CHANNEL_STATUS_TC) {
        dma_disable_channel(APP_GPTMR_DMA, APP_DMA_CH);
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
        dma_is_done = true;
        dma_actual_len = APP_MEASURE_COUNT;
    }
}

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr)
void tick_ms_isr(void)
{
    uint32_t current_dma_count = 0;
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_TIMER_CH))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_TIMER_CH));
        current_dma_count = dma_get_remaining_transfer_size(APP_GPTMR_DMA, APP_DMA_CH);
        if ((current_dma_count == record_dma_count) && (current_dma_count != APP_MEASURE_COUNT)) {
            dma_is_done = true;
            dma_transfer_config();
            dma_actual_len = current_dma_count;
            dma_disable_channel(APP_GPTMR_DMA, APP_DMA_CH);
        }
        record_dma_count = current_dma_count;
    }
}


static void pwm_measure_config(void);
static void timer_config(void);

volatile uint32_t gptmr_freq;

sent_data_format_t sent_format_data;

uint16_t sent_format_count = 0;

int main(void)
{
    char buffer[100];
    uint16_t buf_len = 0;
    uint16_t len = 0;
    board_init();
    init_gptmr_pins(APP_BOARD_GPTMR); 
    printf("sent signal decode demo\n");
    timer_config();
    dma_transfer_config();
    record_dma_count = dma_get_remaining_transfer_size(APP_GPTMR_DMA, APP_DMA_CH);
    pwm_measure_config();
    while (1) {
        dma_enable_channel(APP_GPTMR_DMA, APP_DMA_CH);
        if (dma_is_done) {
            dma_is_done = false;
             for (uint32_t i = 0; i < (APP_MEASURE_COUNT - dma_actual_len); i++) {
                /* convert the cycle count value to microseconds (us). */
                sent_signal_table[i] = (float)(((float)1000000 / gptmr_freq) * pwm_meas_table[i].period);
             }
            if (pwm_process_sent(sent_signal_table, APP_MEASURE_COUNT - dma_actual_len, &sent_format_data) != status_success) {
                continue;
            }
            for (uint8_t j = 0; j < sent_format_data.sent_data_len; j++) {
                len = sprintf(&buffer[buf_len], "stat:%02x data:", sent_format_data.code[j].status);
                buf_len += len;
                for (uint8_t k = 0; k < sent_format_data.code[j].data_len; k++) {
                    len = sprintf(&buffer[buf_len], "%02x ", sent_format_data.code[j].data_nibble[k]);
                    buf_len += len;
                }
                len = sprintf(&buffer[buf_len], "crc:%02x ", sent_format_data.code[j].crc);
                buf_len += len;
                printf("%s pause:%s \n\n", buffer, (sent_format_data.code[j].pause) ? "true" : "false");
            }
            buf_len = 0;
        }
    }
    return 0;
}

static void dma_transfer_config(void)
{
    dma_channel_config_t ch_config = {0};
    dma_disable_channel(APP_GPTMR_DMA, APP_DMA_CH);
    dma_reset(APP_GPTMR_DMA);
    dma_default_channel_config(APP_GPTMR_DMA, &ch_config);
    ch_config.src_addr = (uint32_t)&APP_BOARD_GPTMR->CHANNEL[APP_BOARD_GPTMR_CH].CAPPRD;
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&pwm_meas_table[0].period);
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

static void pwm_measure_config(void)
{
    gptmr_channel_config_t config;

    clock_add_to_group(APP_BOARD_GPTMR_CLOCK, 0);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
    gptmr_stop_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    config.cmp_initial_polarity_high = false;
    config.dma_request_event = gptmr_dma_request_on_input_signal_toggle;
    config.mode = gptmr_work_mode_measure_width;
    gptmr_channel_enable_dma_request(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, false);
    dmamux_config(APP_GPTMR_DMAMUX, APP_GPTMR_DMA_MUX_CH, APP_GPTMR_DMA_SRC, true);
    intc_m_enable_irq_with_priority(APP_GPTMR_DMA_IRQ, 4);
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);
    gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
}

static void timer_config(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    clock_add_to_group(APP_BOARD_GPTMR_CLOCK, 0);
    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
    config.reload = gptmr_freq / 1000000 * CONFIG_SENT_IDLE_INTERVAL_US;
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_TIMER_CH, &config, false);
    gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_TIMER_CH);

    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_TIMER_CH));
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

static uint8_t crc4_calculate(uint8_t *data, uint8_t len)
{
    uint8_t result  = 0x03;
    uint8_t tableNo = 0;
    int i = 0;
    if (data == NULL || len == 0) {
        return 0;
    }
    for(i = 0; i < len; i++) {
        tableNo = result ^ data[i];
        result  = crc4_table_byte[tableNo];
    }
    return result;
}

static hpm_stat_t pwm_process_sent(float *pwm_priiod_table, uint16_t pwm_count, sent_data_format_t *sent)
{
    bool find_sync = false;
    bool find_next_sync = false;
    bool find_pause = false;
    volatile bool find_crc = false;
    uint8_t data_len = 0;
    uint8_t data[SENT_MAX_NIBBLE_COUNT + 3] = {0}; /* include status and crc4 and pause */
    uint16_t i;
    float min_data_us_ticks, max_data_us_ticks;
    float min_sync_us_ticks, max_sync_us_ticks;
    float min_pause_us_ticks, max_pause_us_ticks;
    uint16_t sent_fomat_count = 0;
    float sent_ticks = 0, clock_tick_us = 0, tick_error = 0;

    if (pwm_priiod_table == NULL) {
        return status_invalid_argument;
    }
    min_data_us_ticks = sent_ticks_table[sent_data_table_num].min_value;
    max_data_us_ticks = sent_ticks_table[sent_data_table_num].max_value;
    min_sync_us_ticks = sent_ticks_table[sent_sync_table_num].min_value;
    max_sync_us_ticks = sent_ticks_table[sent_sync_table_num].max_value;
    min_pause_us_ticks = sent_ticks_table[sent_pause_table_num].min_value;
    max_pause_us_ticks = sent_ticks_table[sent_pause_table_num].max_value;
    for (i = 0; i < pwm_count; i++) {
        /* Check if current period time falls within the sync pulse range */
        if ((pwm_priiod_table[i] >= min_sync_us_ticks) && (pwm_priiod_table[i] <= max_sync_us_ticks)) {
            /** If two consecutive periods fall within the sync pulse range, consider it as a pause pulse */
            if (i == (pwm_count - 1) ||
                ((pwm_priiod_table[i + 1] >= min_sync_us_ticks) && (pwm_priiod_table[i + 1] <= max_sync_us_ticks))) {
                find_pause = true;
            } else {
                clock_tick_us = pwm_priiod_table[i] / SENT_SYNC_CALI_TICKS;
                tick_error = clock_tick_us * 0.2; /* tick error ±20 */
                min_data_us_ticks = (SENT_MIN_DATA_TICKS * clock_tick_us) - tick_error;
                max_data_us_ticks = (SENT_MAX_DATA_TICKS * clock_tick_us) + tick_error;
                min_pause_us_ticks = (SENT_MIN_PAUSE_TICKS * clock_tick_us) - tick_error;
                max_pause_us_ticks = (SENT_MAX_PAUSE_TICKS * clock_tick_us) + tick_error;
                if (find_sync == true) {
                    find_next_sync = true;
                    find_sync = false;
                } else {
                    find_sync = true;
                    find_next_sync = false;
                }
            }
        } else if ((pwm_priiod_table[i] >= min_data_us_ticks) && (pwm_priiod_table[i] <= max_data_us_ticks)) {
            /* One 4 bit Status and Serial Communication nibble and data nibble pulse of 12 to 27 clock ticks */
            sent_ticks = pwm_priiod_table[i] / clock_tick_us;
            data[data_len] = ((int)round(sent_ticks) - SENT_MIN_DATA_TICKS);
            /* If enough data has been collected and CRC validation passes, set CRC validation flag */
            if (find_crc == true) {
                find_pause = true;
                data_len--; /* pause is not included in data_len */
            } else if ((data_len > 3) && (data[data_len] == crc4_calculate(&data[1], data_len - 1))) {
                find_crc = true;
            }
            data_len++;
        } else {
            /* Check if current period time falls within the pause pulse range */
            if ((pwm_priiod_table[i] >= min_pause_us_ticks) && (pwm_priiod_table[i] <= max_pause_us_ticks)) {
                find_pause = true;
            } else {
                find_pause = false;
            }
        }
        if ((find_next_sync == true) || (i == (pwm_count - 1))) {
            if (data_len < 3) {
                return status_invalid_argument;
            }
            if (data[data_len - 1] == crc4_calculate(&data[1], data_len - 2)) {
                sent->code[sent_fomat_count].crc = data[data_len - 1];
                sent->code[sent_fomat_count].data_len = data_len - 2;
                sent->code[sent_fomat_count].status = data[0];
                memcpy(sent->code[sent_fomat_count].data_nibble, &data[1], sent->code[sent_fomat_count].data_len);
                sent->code[sent_fomat_count].pause = find_pause;
                sent_fomat_count++;
                sent->sent_data_len = sent_fomat_count;
                /* last sent pulse parsed successfully */
                if (i == (pwm_count - 1)) {
                    return status_success;
                }
            }
            find_crc = false;
            find_pause = false;
            data_len = 0; /* reset data_len */
        }
    }
    return status_fail;
}
