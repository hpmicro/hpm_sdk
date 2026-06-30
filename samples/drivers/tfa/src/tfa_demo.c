/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_tfa_drv.h"
#include <math.h>

void show_help(void);

void tfa_inv_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = 1.0f/calculate_val;
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_inv_f32(calculate_val);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
          result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void tfa_sqrt_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = sqrt(calculate_val);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_sqrt_f32(calculate_val);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
          result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void tfa_sin_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = sin(HPM_2_PI * calculate_val);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_sin_f32(calculate_val);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
          result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void tfa_cos_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = cos(HPM_2_PI * calculate_val);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_cos_f32(calculate_val);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
          result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void tfa_pow2_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = pow(2, calculate_val);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_power2_f32(calculate_val);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
          result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void tfa_log2_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = log(calculate_val)/log(2);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_log2_f32(calculate_val);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
          result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void tfa_atan_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = atan(calculate_val);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_atan_f32(calculate_val) * HPM_2_PI;
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
          result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void tfa_invsqrt_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = 1.0f/sqrt(calculate_val);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_invsqrt_f32(calculate_val);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
        result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void tfa_atanpu2_test(void)
{
#if defined(TFA_SOC_ATANPU2_CORDIC_SUPPORT) && (TFA_SOC_ATANPU2_CORDIC_SUPPORT == 1)
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float x_val_in = 20.44f;
    float y_val_in = 1.66f;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    float result_val_math = atan2(y_val_in, x_val_in);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    float result_val_tfa = hpm_tfa_atanpu2_f32(y_val_in, x_val_in);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
        result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
#else
    printf("ANANPU2 TEST is not supported on this SOC\n");
#endif
}

void tfa_cordic32_test(void)
{
#if defined(TFA_SOC_ATANPU2_CORDIC_SUPPORT) && (TFA_SOC_ATANPU2_CORDIC_SUPPORT == 1)
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    int32_t x_val_in = 98;
    int32_t y_val_in = 12;

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    double result_math_angle = atan2((double)y_val_in, (double)x_val_in) * (180 / M_PI);
    double result_math_mod = sqrt((double)x_val_in * (double)x_val_in + (double)y_val_in * (double)y_val_in);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    cordic_res_t result_tfa_cordic = hpm_tfa_cordic_f32(x_val_in, y_val_in);
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("math angle value:%f, tfa angle value:%f, tfa and math angle diff value:%f,"
        "math mode value:%f, tfa mode value:%f, tfa and math mode diff value:%f,"
        "math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
        result_math_angle, result_tfa_cordic.angle, result_tfa_cordic.angle - result_math_angle,
        result_math_mod, result_tfa_cordic.mod, result_tfa_cordic.mod - result_math_mod,
        end_time_math - start_time_math, end_time_tfa - start_time_tfa);
#else
    printf("CORDIC32 OPERATION is not supported on this SOC\n");
#endif
}

void tfa_mixed_test(void)
{
    uint32_t start_time_math = 0;
    uint32_t start_time_tfa = 0;
    uint32_t end_time_math = 0;
    uint32_t end_time_tfa = 0;

    float calculate_val = 0.778f;
    float result_val_math = 0.0f;
    float result_val_tfa = 0.0f;

    /* calculate f(x) = sin(2pi*x) + cos(2pi*x) + log2(inv(2^x)) */

    /* calculate math value and record the time */
    start_time_math = read_csr(CSR_MCYCLE);
    result_val_math = sin(HPM_2_PI * calculate_val) + cos(HPM_2_PI * calculate_val) +
    log(1.0 / (pow(2, calculate_val)))/log(2);
    end_time_math = read_csr(CSR_MCYCLE);

    /* calculate math value and record the time */
    start_time_tfa = read_csr(CSR_MCYCLE);
    result_val_tfa = hpm_tfa_sin_f32(calculate_val) + hpm_tfa_cos_f32(calculate_val) +
    hpm_tfa_log2_f32(hpm_tfa_inv_f32(hpm_tfa_power2_f32(calculate_val)));
    end_time_tfa = read_csr(CSR_MCYCLE);

    printf("tfa and math diff value:%f, math calculation time:%d ticks, tfa calculation time:%d ticks.\n",
        result_val_tfa - result_val_math, end_time_math - start_time_math, end_time_tfa - start_time_tfa);
}

void handle_tfa_test(void)
{
    show_help();

    while (true) {
        char option = getchar();
        putchar(option);
        putchar('\n');

        switch (option) {
        default:
            show_help();
            break;
        case '0':
            tfa_inv_test();
            break;
        case '1':
            tfa_sqrt_test();
            break;
        case '2':
            tfa_sin_test();
            break;
        case '3':
            tfa_cos_test();
            break;
        case '4':
            tfa_pow2_test();
            break;
        case '5':
            tfa_log2_test();
            break;
        case '6':
            tfa_atan_test();
            break;
        case '7':
            tfa_invsqrt_test();
            break;
        case '8':
            tfa_atanpu2_test();
            break;
        case '9':
            tfa_cordic32_test();
            break;
        case 'A':
        case 'a':
            tfa_mixed_test();
            break;
        }
    }
}

void show_help(void)
{
    static const char help_info[] = ""
                                    "*********************************************************************************\n"
                                    "*                                                                               *\n"
                                    "*                         TFA Example Menu                                      *\n"
                                    "*                                                                               *\n"
                                    "* 0 - INV TEST                                                                  *\n"
                                    "* 1 - SQRT TEST                                                                 *\n"
                                    "* 2 - SIN TEST                                                                  *\n"
                                    "* 3 - COS TEST                                                                  *\n"
                                    "* 4 - POW2 TEST                                                                 *\n"
                                    "* 5 - LOG2 TEST                                                                 *\n"
                                    "* 6 - ATAN TEST                                                                 *\n"
                                    "* 7 - INVSQRT TEST                                                              *\n"
                                    "* 8 - ANANPU2 TEST                                                              *\n"
                                    "* 9 - CORDIC32 OPERATION                                                        *\n"
                                    "* A - MIXED OPERATION                                                           *\n"
                                    "*                                                                               *\n"
                                    "*********************************************************************************\n";
    printf("%s\n", help_info);
}

int main(void)
{
    board_init();

    handle_tfa_test();

    return 0;
}

