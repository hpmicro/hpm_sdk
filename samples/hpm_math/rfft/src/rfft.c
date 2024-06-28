/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_gpio_drv.h"
#include <math.h>
#include "hpm_math.h"
#include "hpm_l1c_drv.h"

typedef float rfft_type_t;
#define FFT_PRECISION (0.001)
#define RFFT_COMPLEX_MAX (2*1024)
#define RFFT_COMPLEX_MAGNITUDE (RFFT_COMPLEX_MAX/2)

#define RFFT_SIN_BUFF_LEN   3
#define RFFT_SIN_AMP_1    11
#define RFFT_SIN_AMP_2    19
#define RFFT_SIN_AMP_3    13

#define RFFT_SIN_FREQC_1    8.0
#define RFFT_SIN_FREQC_2    17.0
#define RFFT_SIN_FREQC_3    24.0

rfft_type_t rfft_buf[RFFT_COMPLEX_MAX * 2];
rfft_type_t rfft_mag_output[RFFT_COMPLEX_MAGNITUDE];
rfft_type_t rfft_sin_amp_buf[RFFT_SIN_BUFF_LEN] = {RFFT_SIN_AMP_1, RFFT_SIN_AMP_2, RFFT_SIN_AMP_3};
rfft_type_t rfft_sin_freqc_buf[RFFT_SIN_BUFF_LEN] = {RFFT_SIN_FREQC_1, RFFT_SIN_FREQC_2, RFFT_SIN_FREQC_3};

uint32_t run_times;
uint32_t err_num;
uint64_t delta_time;

void start_time(void)
{
    delta_time = hpm_csr_get_core_mcycle();
}

uint32_t get_end_time(void)
{
    delta_time = hpm_csr_get_core_mcycle() - delta_time;
    return delta_time;
}

void init_rfft_inputbuf(rfft_type_t *buf, uint16_t pt)
{
    uint16_t i;
    float flx;

    for (i = 0; i < pt; i++) {
        flx = 5 +
            RFFT_SIN_AMP_1 * sinf(2 * HPM_MATH_PI * RFFT_SIN_FREQC_1 * i / pt) +
            RFFT_SIN_AMP_2 * sinf(2 * HPM_MATH_PI * RFFT_SIN_FREQC_2 * i / pt) +
            RFFT_SIN_AMP_3 * sinf(2 * HPM_MATH_PI * RFFT_SIN_FREQC_3 * i / pt);
        buf[i] = flx;
    }
}

void rfft_printf(rfft_type_t *buf, rfft_type_t *output, uint16_t num)
{
    unsigned int i, j = 0;
    hpm_dsp_cmag_f32(buf, output, num);
    printf("------------------------------------\r\n");
    printf("DC component amplitude:%f.\r\n", output[0]/(num * 2));
    for (i = 1; i <  num; i++) {
        if (output[i] > num) { /*limit the small amplitude*/
            if ((output[i]/num > rfft_sin_amp_buf[j] + FFT_PRECISION) ||
            (output[i]/num < rfft_sin_amp_buf[j] - FFT_PRECISION)) {
                err_num++;
            } else if ((i > rfft_sin_freqc_buf[j] + FFT_PRECISION) ||
            (i < rfft_sin_freqc_buf[j] - FFT_PRECISION)) {
                err_num++;
            }
            j++;
            printf("Frequency:%f, Amplitude:%f.\r\n", (float)i, output[i]/num);
        }
    }
    printf("------------------------------------\r\n");
}

int main(void)
{
    uint8_t shift = 0;
    uint16_t point = 0;

    err_num = 0;

    board_init();
    board_init_led_pins();
    printf("cpu0:\t\t %dHz\n\n\n", clock_get_frequency(clock_cpu0));
    for (uint8_t i = 6; i <= 10; i++) {
        point = 1 << i;
        shift = i;
        init_rfft_inputbuf(&rfft_buf[0], point);
        start_time();
        hpm_dsp_rfft_f32(&rfft_buf[0], shift);
        run_times = get_end_time();
        printf("Rfft demo: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        rfft_printf(&rfft_buf[0], &rfft_mag_output[0], point >> 1);
    }
    if (err_num > 0) {
        printf("ERROR NUM :%d.\r\n", err_num);
    } else {
        printf("PASS.\r\n");
    }
    printf("----------------END--------------------\r\n");

    while (1) {
        ;
    }
    return 0;
}
