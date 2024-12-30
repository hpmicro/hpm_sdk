/*
 * Copyright (c) 2022 HPMicro
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
#ifdef HPMSOC_HAS_HPMSDK_FFA
#include "hpm_ffa_drv.h"
#endif
#include "hpm_l1c_drv.h"

typedef float fft_type_t;
#define FFT_PRECISION (0.001)
#define FFT_COMPLEX_MAX (2*1024)
#define FFT_COMPLEX_MAGNITUDE (FFT_COMPLEX_MAX/2)

fft_type_t fft_buf[FFT_COMPLEX_MAX * 2];
fft_type_t fft_buf_copy[FFT_COMPLEX_MAX];
fft_type_t fft_mag_output[FFT_COMPLEX_MAGNITUDE];

fft_type_t fft_buf_conversion[FFT_COMPLEX_MAX];

#ifdef HPMSOC_HAS_HPMSDK_FFA
ffa_q31_t ffa_buf[FFT_COMPLEX_MAX];
#else
q31_t ffa_buf[FFT_COMPLEX_MAX];
#endif

uint32_t run_times;
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

/**
 * @brief Three sine functions and a DC component are superimposed to form a waveform.
 * A*sin(2*pi*f/frc), A is the amplitude, f is the period, and frc is the sampling rate.
 * If the sampling rate is equal to the number of samples,
 * it will be easier to calculate the amplitude and frequency characteristics.
 *
 * @param[out] buf
 * @param[in] pt samples point
 */
void init_fft_inputbuf(fft_type_t *buf, uint16_t pt)
{
    uint16_t i;
    float flx;
    for (i = 0; i < pt; i++) {
        flx = 10 +
             11 * sinf(2 * HPM_MATH_PI * 8.0 * i / pt) +
             19 * sinf(2 * HPM_MATH_PI * 17.0 * i / pt) +
             23 * sinf(2 * HPM_MATH_PI * 24.0 * i / pt);
        buf[2*i] = flx;
        fft_buf_copy[2*i] = buf[2*i];
        fft_buf_conversion[2*i] = flx / pt;
        buf[2*i+1] = 0;
        fft_buf_copy[2*i+1] = 0;
        fft_buf_conversion[2*i+1] = 0;
    }
    hpm_dsp_convert_f32_q31(fft_buf_conversion, ffa_buf, 2 * pt);
}

void data_conversion_test(fft_type_t *buf, uint16_t pt)
{
    init_fft_inputbuf(buf, pt);
    printf("------------------------------------\r\n");
    printf("convert data from float to q31, nums:%d.\r\n", 2 * pt);
    start_time();
    hpm_dsp_convert_f32_q31(fft_buf_conversion, ffa_buf, 2 * pt);
    run_times = get_end_time();
    printf("total times:%d tick.\r\n", run_times);
    printf("convert data from q31 to float, nums:%d.\r\n", 2 * pt);
    start_time();
    hpm_dsp_convert_q31_f32(ffa_buf, fft_buf_conversion, 2 * pt);
    run_times = get_end_time();
    printf("total times:%d tick.\r\n", run_times);
    printf("------------------------------------\r\n");
}

void fft_printf(fft_type_t *buf, fft_type_t *output, uint16_t num)
{
    unsigned int i;
    hpm_dsp_cmag_f32(buf, output, num);
    printf("------------------------------------\r\n");
    printf("DC component amplitude:%f.\r\n", output[0]/num);
    for (i = 1; i <  num/2; i++) {
        if (output[i] > num) { /*limit the small amplitude*/
            printf("Frequency:%f, Amplitude:%f.\r\n", (float)i, output[i]/(num/2));
        }
    }
    printf("------------------------------------\r\n");
}

int main(void)
{
    uint8_t shift = 0;
    uint16_t point = 0;
    uint32_t err_num = 0;
    board_init();
    board_init_led_pins();
    printf("cpu0:\t\t %dHz\n\n\n", clock_get_frequency(clock_cpu0));
    /**
     * @brief DSP radix-4  2^i complex samples
     *
     */
    for (uint8_t i = 6; i <= 10; i += 2) {
        point = 1 << i;
        shift = i;
        init_fft_inputbuf(&fft_buf[0], point);
        start_time();
        hpm_dsp_cfft_rd4_f32(&fft_buf[0], shift);
        run_times = get_end_time();
        printf("dsp fft radix-4 Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        fft_printf(&fft_buf[0], &fft_mag_output[0], point);

        start_time();
        hpm_dsp_cifft_rd4_f32(&fft_buf[0], shift);
        run_times = get_end_time();
        printf("dsp ifft radix-4 Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        for (uint32_t m = 0; m < 2 * point; m++) {
            if ((fft_buf_copy[m] > fft_buf[m] + FFT_PRECISION) ||
            (fft_buf_copy[m] < fft_buf[m] - FFT_PRECISION)) {
                err_num++;
            }
        }
        printf("------------------------------------\r\n\r\n\r\n");
    }
    printf("**************************************\r\n\r\n\r\n\r\n");
    /**
     * @brief DSP radix-2  2^i complex samples
     *
     */
    for (uint8_t i = 6; i <= 10; i++) {
        point = 1 << i;
        shift = i;
        init_fft_inputbuf(&fft_buf[0], point);
        start_time();
        hpm_dsp_cfft_rd2_f32(&fft_buf[0], shift);
        run_times = get_end_time();
        printf("dsp fft radix-2 Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        fft_printf(&fft_buf[0], &fft_mag_output[0], point);

        start_time();
        hpm_dsp_cifft_rd2_f32(&fft_buf[0], shift);
        run_times = get_end_time();
        printf("dsp ifft radix-2 Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        for (uint32_t m = 0; m < 2 * point; m++) {
            if ((fft_buf_copy[m] > fft_buf[m] + FFT_PRECISION) ||
            (fft_buf_copy[m] < fft_buf[m] - FFT_PRECISION)) {
                err_num++;
            }
        }
        printf("------------------------------------\r\n\r\n\r\n");
    }
    printf("**************************************\r\n\r\n\r\n\r\n");
    /**
     * @brief Q31 DSP Radix-2  2^j complex samples
     *
     */
    for (uint8_t j = 6; j <= 10; j++) {
        point = 1 << j;
        shift = j;
        init_fft_inputbuf(&fft_buf[0], point);
        start_time();
        hpm_dsp_cfft_rd2_q31(&ffa_buf[0], shift);
        run_times = get_end_time();
        printf("dsp fft q31 radix-2 Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        hpm_dsp_convert_q31_f32(ffa_buf, fft_buf, point * 2);
        for (uint32_t i = 0; i < 2 * point; i++) {
#ifdef __zcc__
            fft_buf[i] = fft_buf[i] * point * point;
#else
            fft_buf[i] = fft_buf[i] * point * point * 2;
#endif
        }
        fft_printf(&fft_buf[0], &fft_mag_output[0], point);

        start_time();
        hpm_dsp_cifft_rd2_q31(ffa_buf, shift);
        run_times = get_end_time();
        hpm_dsp_convert_q31_f32(ffa_buf, fft_buf, point * 2);
        for (uint32_t i = 0; i < 2 * point; i++) {
            fft_buf[i] = fft_buf[i] * point;
        }
        printf("dsp ifft q31 radix-2 Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        for (uint32_t m = 0; m < 2 * point; m++) {
            if ((fft_buf_copy[m] > fft_buf[m] + FFT_PRECISION) ||
            (fft_buf_copy[m] < fft_buf[m] - FFT_PRECISION)) {
                err_num++;
            }
        }
        printf("------------------------------------\r\n\r\n\r\n");
    }
    printf("**************************************\r\n\r\n\r\n\r\n");
#ifdef HPMSOC_HAS_HPMSDK_FFA
    clock_add_to_group(clock_ffa0, 0);

    /**
     * @brief Q31 FFA  2^j complex samples
     *
     */
    for (uint8_t j = 6; j <= 9; j++) {
        point = 1 << j;
        shift = j;
        init_fft_inputbuf(&fft_buf[0], point);
        if (l1c_dc_is_enabled()) {
            l1c_dc_flush(HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) &ffa_buf[0]),
                2 * (HPM_L1C_CACHELINE_ALIGN_UP((point * sizeof(q31_t) + &ffa_buf[0])) -
                HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) &ffa_buf[0])));
        }
        start_time();
        hpm_ffa_cfft_q31(&ffa_buf[0], shift);
        run_times = get_end_time();
        if (l1c_dc_is_enabled()) {
            l1c_dc_invalidate(HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) &ffa_buf[0]),
                2 * (HPM_L1C_CACHELINE_ALIGN_UP((point * sizeof(q31_t) + &ffa_buf[0])) -
                HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) &ffa_buf[0])));
        }
        printf("dsp ffa fft q31 Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        hpm_dsp_convert_q31_f32(ffa_buf, fft_buf, point * 2);
        for (uint32_t i = 0; i < 2 * point; i++) {
            fft_buf[i] = fft_buf[i] * point * point;
        }
        fft_printf(&fft_buf[0], &fft_mag_output[0], point);

        if (l1c_dc_is_enabled()) {
            l1c_dc_flush(HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) &ffa_buf[0]),
                2 * (HPM_L1C_CACHELINE_ALIGN_UP((point * sizeof(q31_t) + &ffa_buf[0])) -
                HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) &ffa_buf[0])));
        }
        start_time();
        hpm_ffa_cifft_q31(ffa_buf, shift);
        run_times = get_end_time();
        if (l1c_dc_is_enabled()) {
            l1c_dc_invalidate(HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) &ffa_buf[0]),
                2 * (HPM_L1C_CACHELINE_ALIGN_UP((point * sizeof(q31_t) + &ffa_buf[0])) -
                HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) &ffa_buf[0])));
        }
        hpm_dsp_convert_q31_f32(ffa_buf, fft_buf, point * 2);
        for (uint32_t i = 0; i < 2 * point; i++) {
            fft_buf[i] = fft_buf[i] * point * point;
        }
        printf("dsp ffa ifft q31 Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        for (uint32_t m = 0; m < 2 * point; m++) {
            if ((fft_buf_copy[m] > fft_buf[m] + FFT_PRECISION) ||
            (fft_buf_copy[m] < fft_buf[m] - FFT_PRECISION)) {
                err_num++;
            }
        }
        printf("------------------------------------\r\n\r\n\r\n");
    }
    printf("**************************************\r\n\r\n\r\n\r\n");
#endif
    /**
     * @brief Software colley-tukey.  2^j complex samples
     *
     */
    for (uint8_t i = 6; i <= 10; i++) {
        point = 1 << i;
        shift = i;
        init_fft_inputbuf(&fft_buf[0], point);
        start_time();
        hpm_software_cfft_float(&fft_buf[0], shift);
        run_times = get_end_time();
        printf("Software fft  cooley tukey Total samples: %d.\r\n", point);
        printf("total times:%d tick.\r\n", run_times);
        fft_printf(&fft_buf[0], &fft_mag_output[0], point);
    }
    printf("**************************************\r\n\r\n\r\n\r\n");
/* conversion test */
    for (uint8_t i = 6; i <= 10; i++) {
        point = 1 << i;
        data_conversion_test(&fft_buf[0], point);
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
