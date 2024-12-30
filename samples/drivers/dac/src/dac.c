/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <math.h>
#include "board.h"
#include "hpm_dac_drv.h"

#define DAC_STEP_CFG_IDX DAC_STEP_CFG_STEP0

#define DAC_BUFF0_COUNT (DAC_SOC_MAX_BUFF_COUNT / 32)  /* 2048 */
#define DAC_BUFF1_COUNT (DAC_SOC_MAX_BUFF_COUNT / 32)  /* 2048 */

#define PI HPM_PI

static __RW uint8_t direct_cmpt_flag;
static __RW uint8_t step_running_flag;
static __RW uint8_t buffer_running_flag;
static __RW uint8_t buf1_cmpt_flag;
static __RW uint8_t buf0_cmpt_flag;

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(DAC_SOC_BUFF_ALIGNED_SIZE) static uint32_t buffer0[DAC_BUFF0_COUNT];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(DAC_SOC_BUFF_ALIGNED_SIZE) static uint32_t buffer1[DAC_BUFF1_COUNT];

SDK_DECLARE_EXT_ISR_M(BOARD_DAC_IRQn, isr_dac)
void isr_dac(void)
{
    uint32_t status;

    status = dac_get_status_flags(BOARD_DAC_BASE);
    dac_set_status_flags(BOARD_DAC_BASE, status);

    if (DAC_IRQ_STS_BUF1_CMPT_GET(status)) {
        buf1_cmpt_flag = 1;
    }

    if (DAC_IRQ_STS_BUF0_CMPT_GET(status)) {
        buf0_cmpt_flag = 1;
    }
}

static void init_common_config(dac_mode_t mode)
{
    dac_config_t config;

    dac_get_default_config(&config);
    config.dac_mode = mode;
    config.sync_mode = (clk_dac_src_ahb0 == clock_get_source(BOARD_APP_DAC_CLOCK_NAME)) ? true : false;

    dac_init(BOARD_DAC_BASE, &config);
}

static void set_direct_mode_config(void)
{
    printf("Set DAC to output data in direct mode\n");
    printf("DAC is outputting a triangle waveform in direct mode\n");

    /* triangle waveform */
    for (int32_t j = 0; j < 5000; j++) {
        for (float i = 0; i <= 10000; i += 2.5) { /* NOLINT */
            dac_set_direct_config(BOARD_DAC_BASE, (uint16_t)DAC_OUTPUT(i));
            board_delay_us(1);
        }

        for (float i = 10000; i >= 0; i -= 2.5) { /* NOLINT */
            dac_set_direct_config(BOARD_DAC_BASE, (uint16_t)DAC_OUTPUT(i));
            board_delay_us(1);
        }
    }

    dac_set_direct_config(BOARD_DAC_BASE, 0);
}

static void set_step_mode_config(void)
{
    dac_step_config_t config0;

    printf("Set DAC to output data in step mode\n");
    printf("DAC is outputting a saw tooth waveform in step mode\n");

    /* saw tooth waveform */
    config0.up_down = dac_step_up;
    config0.start_point = 0;
    config0.end_point = DAC_SOC_MAX_DATA;
    config0.step_num = 1;
    config0.round_mode = dac_round_mode_loop;

    dac_set_step_config(BOARD_DAC_BASE, DAC_STEP_CFG_IDX, &config0);
    dac_set_output_frequency(BOARD_DAC_BASE, clock_get_frequency(BOARD_APP_DAC_CLOCK_NAME), DAC_SOC_MAX_OUTPUT_FREQ);
}

static void set_buffer_mode_config(void)
{
    dac_buffer_config_t buffer_config;

    printf("Set DAC to output data in buffer mode\n");
    printf("DAC is outputting a sine waveform in buffer mode\n");

    buffer_config.buf_data_mode = dac_data_stru_1_point;
    buffer_config.burst = dac_burst_single;

    buffer_config.buf0.start_addr = (uint32_t)buffer0;
    buffer_config.buf0.stop = 0;
    buffer_config.buf0.len = (uint16_t)(sizeof(buffer0) / sizeof(uint32_t));

    buffer_config.buf1.start_addr = (uint32_t)buffer1;
    buffer_config.buf1.stop = 0;
    buffer_config.buf1.len = (uint16_t)(sizeof(buffer1) / sizeof(uint32_t));

    /* sine waveform */
    for (uint16_t i = 0; i < (DAC_SOC_MAX_DATA + 1) / 2; i += 1) {
        buffer0[i] = ((DAC_SOC_MAX_DATA + 1) / 2) * sin(i * 2 * PI / (DAC_SOC_MAX_DATA + 1)) + ((DAC_SOC_MAX_DATA + 1) / 2 - 1);
        buffer1[i] = ((DAC_SOC_MAX_DATA + 1) / 2) * sin((i + (DAC_SOC_MAX_DATA + 1) / 2) * 2 * PI / (DAC_SOC_MAX_DATA + 1)) + ((DAC_SOC_MAX_DATA + 1) / 2);
    }

    dac_set_buffer_config(BOARD_DAC_BASE, &buffer_config);
    dac_set_output_frequency(BOARD_DAC_BASE, clock_get_frequency(BOARD_APP_DAC_CLOCK_NAME), DAC_SOC_MAX_OUTPUT_FREQ);

    dac_enable_interrupts(BOARD_DAC_BASE, DAC_BUF1_COMPLETE_EVENT);
}

static void direct_mode_handler(void)
{
    if (direct_cmpt_flag == 0) {
        direct_cmpt_flag = 1;
        printf("DAC completed output\n");
    }
}

static void step_mode_handler(void)
{
    if (step_running_flag == 0) {
        step_running_flag = 1;
        dac_set_step_sw_trigger(BOARD_DAC_BASE, DAC_STEP_CFG_IDX);
    }
}

static void buffer_mode_handler(void)
{
    if (buffer_running_flag == 0) {
        buffer_running_flag = 1;
        dac_set_buffer_sw_trigger(BOARD_DAC_BASE);
    }

    if (buf1_cmpt_flag == 1) {
        buf1_cmpt_flag = 0;
    }
}

static void stop_handler(void)
{
    direct_cmpt_flag = 0;
    step_running_flag = 0;
    buffer_running_flag = 0;
    buf0_cmpt_flag = 0;
    buf1_cmpt_flag = 0;
}

static uint8_t get_dac_mode(void)
{
    uint8_t ch;

    while (1) {
        printf("1. Direct mode\n");
        printf("2. Step   mode\n");
        printf("3. Buffer mode\n");
        printf("Please enter the DAC mode code: ");
        printf("%c\n", ch = getchar());
        ch -= '0' + 1;
        if (ch > dac_mode_buffer) {
            printf("Output mode is not supported!\n");
        } else {
            return ch;
        }
    }
}

int main(void)
{
    uint8_t output_mode;

    /* Initialize Bsp */
    board_init();

    /* Set a log title */
    printf("This is a DAC demo:\n");

    /* Initialize a DAC clock */
    board_init_dac_clock(BOARD_DAC_BASE, false);

    /* Initialize a DAC pin */
    board_init_dac_pins(BOARD_DAC_BASE);

    while (1) {
        /* Get the output mode from a console */
        output_mode = get_dac_mode();

        /* Initialize a DAC peripheral with a common config */
        init_common_config(output_mode);

        /* Enable the DAC IRQ */
        intc_m_enable_irq(BOARD_DAC_IRQn);

        /* Enable DAC conversion */
        dac_enable_conversion(BOARD_DAC_BASE, true);

        /* Set a specified DAC output config */
        switch (output_mode) {
        case dac_mode_direct:
            set_direct_mode_config();
            break;

        case dac_mode_step:
            set_step_mode_config();
            break;

        case dac_mode_buffer:
            set_buffer_mode_config();
            break;

        default:
            break;
        }

        /* Main loop */
        while (1) {
            if (output_mode == dac_mode_direct) {
                direct_mode_handler();
            } else if (output_mode == dac_mode_step) {
                step_mode_handler();
            } else if (output_mode == dac_mode_buffer) {
                buffer_mode_handler();
            } else {
                printf("Output mode is not supported!\n");
            }

            if (console_try_receive_byte() == ' ') {
                dac_enable_conversion(BOARD_DAC_BASE, false);
                stop_handler();
                break;
            }
        }
    }
}
