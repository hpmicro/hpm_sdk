/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_l1c_drv.h"
#include "hpm_ffa_drv.h"
#include "hpm_clock_drv.h"
#include "ffa_test_data.h"
#include <stdarg.h>

#define FFT_DEVIATION_MAX 16
#define FIR_DEVIATION_MAX 2

typedef struct {
    uint32_t num_points;
    uint32_t scale;
    const ffa_q31_t *gold_data;
    const ffa_q31_t *src;
    ffa_complex_q31_t *dst;
} ffa_fft_test_context_t;

void show_menu(void);
void test_fft_complex_q31(void);
void test_ifft_complex_q31(void);
void consttest_fir_real_q31_coeff_17taps_src_46taps(void);
static void l1c_dc_flush_cacheline_aligned(uint32_t src, uint32_t size);
static void l1c_dc_invalidate_cacheline_aligned(uint32_t src, uint32_t size);

/**
 * @brief Check the FFA operation result
 * @param [in] actual pointers to the actual output data
 * @param [in] golden pointers to the golden data
 * @param [in] scale scale of the golden data
 * @param [in] elem_num Number of elements for checking
 * @param [in] max_deviation Allowed deviation
 * @param [output] failed_index
 * @retval true Output data are as expected
 * @retval false Output data are not as expected
 */
static bool ffa_result_check(const int32_t *actual,
                             const int32_t *golden,
                             uint32_t scale,
                             uint32_t elem_num,
                             int32_t max_deviation,
                             uint32_t *failed_index);

ATTR_ALIGN(64) ffa_complex_q31_t ffa_fft_q31_dst[512];

ATTR_ALIGN(64) ffa_complex_q31_t ffa_ifft_q31_dst[512];

ATTR_ALIGN(64) ffa_q31_t ffa_fir_q31_coeff_17taps_src_48taps_dst[30];

ffa_fft_test_context_t fft_test_ctx[] = {
    {
        .num_points = 16,
        .scale = 4,
        .src = ffa_fft_complex_q31_16_point_src,
        .dst = ffa_fft_q31_dst,
        .gold_data = ffa_fft_complex_q31_16_point_result,
    },
    {
        .num_points = 32,
        .scale = 5,
        .src = ffa_fft_complex_q31_32_point_src,
        .dst = ffa_fft_q31_dst,
        .gold_data = ffa_fft_complex_q31_32_point_result,
    },
    {
        .num_points = 64,
        .scale = 6,
        .src = ffa_fft_complex_q31_64_point_src,
        .dst = ffa_fft_q31_dst,
        .gold_data = ffa_fft_complex_q31_64_point_result,
    },
    {
        .num_points = 256,
        .scale = 8,
        .src = ffa_fft_complex_q31_256_point_src,
        .dst = ffa_fft_q31_dst,
        .gold_data = ffa_fft_complex_q31_256_point_result,
    },
    {
        .num_points = 512,
        .scale = 0,
        .src = ffa_fft_complex_q31_512_point_src,
        .dst = ffa_fft_q31_dst,
        .gold_data = ffa_fft_complex_q31_512_point_result,
    },

};

ffa_fft_test_context_t ifft_test_ctx[] = {
    {
        .num_points = 8,
        .scale = 0,
        .src = ffa_ifft_complex_q31_8_point_src,
        .dst = ffa_ifft_q31_dst,
        .gold_data = ffa_ifft_complex_q31_8_point_result,
    },
    {
        .num_points = 16,
        .scale = 4,
        .src = ffa_ifft_complex_q31_16_point_src,
        .dst = ffa_ifft_q31_dst,
        .gold_data = ffa_ifft_complex_q31_16_point_result,
    },
    {
        .num_points = 32,
        .scale = 5,
        .src = ffa_ifft_complex_q31_32_point_src,
        .dst = ffa_ifft_q31_dst,
        .gold_data = ffa_ifft_complex_q31_32_point_result,
    }
    };

int main(void)
{
    board_init();
    clock_add_to_group(clock_ffa0, 0);
    show_menu();

    while (1) {
        char option = getchar();
        putchar(option);
        putchar('\n');
        switch (option) {
        case '1':
            test_fft_complex_q31();
            break;

        case '2':
            test_ifft_complex_q31();
            break;
        case '3':
            consttest_fir_real_q31_coeff_17taps_src_46taps();
            break;
        default:
            show_menu();
            break;
        }
    }
}

static void l1c_dc_flush_cacheline_aligned(uint32_t src, uint32_t size)
{
    uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(src);
    uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(src + size);
    uint32_t aligned_size = aligned_end - aligned_start;
    l1c_dc_flush(aligned_start, aligned_size);
}
static void l1c_dc_invalidate_cacheline_aligned(uint32_t src, uint32_t size)
{
    uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(src);
    uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(src + size);
    uint32_t aligned_size = aligned_end - aligned_start;
    l1c_dc_invalidate(aligned_start, aligned_size);
}

void show_menu(void)
{
    static const char menu_info[] = "\n"
                                    "*******************************************************************************\n"
                                    "*                                                                             *\n"
                                    "*    FFA Test demo                                                            *\n"
                                    "*    1. Complex Q31 FFT Test                                                  *\n"
                                    "*    2. 8-Point Complex Q31 IFFT Test                                         *\n"
                                    "*    3. Real Q31 FIR Test                                                     *\n"
                                    "*                                                                             *\n"
                                    "*******************************************************************************\n";
    printf("%s\n", menu_info);
}

static bool ffa_result_check(const int32_t *actual,
                             const int32_t *golden,
                             uint32_t scale,
                             uint32_t elem_num,
                             int32_t max_deviation,
                             uint32_t *failed_index)
{
    bool result = true;

    for (uint32_t i = 0; i < elem_num; i++) {
        if (abs(actual[i] - (golden[i] >> scale)) > max_deviation) {
            result = false;
            if (failed_index != NULL) {
                *failed_index = i;
            }
            printf("act:%08x, golden:%08x, index=%4d\n", actual[i], golden[i], i);
        }
    }
    return result;
}

void test_fft_complex_q31(void)
{
    printf("%s start...\n", __func__);
    fft_xfer_t xfer = { 0 };

    for (uint32_t i = 0; i < ARRAY_SIZE(fft_test_ctx); i++) {
        memset(&xfer, 0, sizeof(xfer));
        xfer.num_points = fft_test_ctx[i].num_points;
        xfer.src = fft_test_ctx[i].src;
        xfer.dst = fft_test_ctx[i].dst;
        xfer.is_ifft = false;
        xfer.src_data_type = FFA_DATA_TYPE_COMPLEX_Q31;
        xfer.dst_data_type = FFA_DATA_TYPE_COMPLEX_Q31;

        if (l1c_dc_is_enabled()) {
            l1c_dc_flush_cacheline_aligned((uint32_t) fft_test_ctx[i].src,
                                           fft_test_ctx[i].num_points * sizeof(ffa_complex_q31_t));
        }
        hpm_stat_t status = ffa_calculate_fft_blocking(HPM_FFA, &xfer);

        if (status != status_success) {
            printf("FFT calculation failed, error_code=%d\n", status);
        } else {
            if (l1c_dc_is_enabled()) {
                l1c_dc_invalidate_cacheline_aligned((uint32_t) fft_test_ctx[i].dst,
                                                    fft_test_ctx[i].num_points * sizeof(ffa_complex_q31_t));
            }
            bool is_equal = ffa_result_check((const int32_t *) fft_test_ctx[i].dst,
                                             (const int32_t *) fft_test_ctx[i].gold_data,
                                             fft_test_ctx[i].scale,
                                             fft_test_ctx[i].num_points * sizeof(ffa_complex_q31_t) / sizeof(int32_t),
                                             FFT_DEVIATION_MAX,
                                             NULL);
            printf("FFT test, %d points %s\n", fft_test_ctx[i].num_points, is_equal ? "PASSED" : "FAILED");
        }
    }
}

void test_ifft_complex_q31(void)
{
    printf("%s start...\n", __func__);
    fft_xfer_t xfer = { 0 };

    for (uint32_t i = 0; i < ARRAY_SIZE(ifft_test_ctx); i++) {
        memset(&xfer, 0, sizeof(xfer));
        xfer.num_points = ifft_test_ctx[i].num_points;
        xfer.src = ifft_test_ctx[i].src;
        xfer.dst = ifft_test_ctx[i].dst;
        xfer.is_ifft = true;
        xfer.src_data_type = FFA_DATA_TYPE_COMPLEX_Q31;
        xfer.dst_data_type = FFA_DATA_TYPE_COMPLEX_Q31;

        if (l1c_dc_is_enabled()) {
            l1c_dc_flush_cacheline_aligned((uint32_t) ifft_test_ctx[i].src,
                                           ifft_test_ctx[i].num_points * sizeof(ffa_complex_q31_t));
        }
        hpm_stat_t status = ffa_calculate_fft_blocking(HPM_FFA, &xfer);

        if (status != status_success) {
            printf("IFFT calculation failed, error_code=%d\n", status);
        } else {
            if (l1c_dc_is_enabled()) {
                l1c_dc_invalidate_cacheline_aligned((uint32_t) ifft_test_ctx[i].dst,
                                                    ifft_test_ctx[i].num_points * sizeof(ffa_complex_q31_t));
            }
            bool is_equal = ffa_result_check((const int32_t *) ifft_test_ctx[i].dst,
                                             (const int32_t *) ifft_test_ctx[i].gold_data,
                                             ifft_test_ctx[i].scale,
                                             ifft_test_ctx[i].num_points * sizeof(ffa_complex_q31_t) / sizeof(int32_t),
                                             FFT_DEVIATION_MAX,
                                             NULL);
            printf("IFFT test, %d points %s\n", ifft_test_ctx[i].num_points, is_equal ? "PASSED" : "FAILED");
        }
    }
}

void consttest_fir_real_q31_coeff_17taps_src_46taps(void)
{
    printf("%s start...\n", __func__);
    fir_xfer_t xfer = { 0 };
    xfer.coef_taps = ARRAY_SIZE(ffa_fir_q31_17_taps_coeff);
    xfer.input_taps = ARRAY_SIZE(ffa_fir_q31_46_taps_src);
    xfer.data_type = FFA_DATA_TYPE_REAL_Q31;
    xfer.src = &ffa_fir_q31_46_taps_src;
    xfer.coeff = &ffa_fir_q31_17_taps_coeff;
    xfer.dst = &ffa_fir_q31_coeff_17taps_src_48taps_dst;

    if (l1c_dc_is_enabled()) {
        l1c_dc_flush_cacheline_aligned((uint32_t) &ffa_fir_q31_17_taps_coeff,
                                       HPM_L1C_CACHELINE_ALIGN_UP(sizeof(ffa_fir_q31_17_taps_coeff)));
        l1c_dc_flush_cacheline_aligned((uint32_t) &ffa_fir_q31_46_taps_src,
                                       HPM_L1C_CACHELINE_ALIGN_UP(sizeof(ffa_fir_q31_46_taps_src)));
    }
    hpm_stat_t status = ffa_calculate_fir_blocking(HPM_FFA, &xfer);
    if (status != status_success) {
        printf("IR calculation failed, error_code=%d\n", status);
    } else {
        if (l1c_dc_is_enabled()) {
            l1c_dc_invalidate_cacheline_aligned((uint32_t) &ffa_fir_q31_coeff_17taps_src_48taps_dst,
                                                sizeof(ffa_fir_q31_coeff_17taps_src_48taps_dst));
        }
        bool is_equal = ffa_result_check((const int32_t *) &ffa_fir_q31_coeff_17taps_src_48taps_dst,
                                         (const int32_t *) &ffa_fir_q31_coeff_17taps_src48taps_result,
                                         0,
                                         sizeof(ffa_fir_q31_coeff_17taps_src_48taps_dst) / sizeof(int32_t),
                                         FIR_DEVIATION_MAX,
                                         NULL);
        printf("FIR test %s\n", is_equal ? "PASSED" : "FAILED");
    }
}
