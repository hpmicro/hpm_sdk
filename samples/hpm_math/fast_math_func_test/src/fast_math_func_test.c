/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include <math.h>
#include "hpm_math.h"
#include "hpm_l1c_drv.h"

/* Test configuration */
#define TEST_LOOP_COUNT                 1000
#define HPM_FAST_MATH_TABLE_SIZE_MAX    (4096)
#define ACCURACY_TEST_STEP              (1e-6f)

/* Error thresholds for PASS/FAIL judgment */
#define ERROR_THRESHOLD_NONE            (1e-2f)   /* No interpolation */
#define ERROR_THRESHOLD_LINEAR          (1e-4f)   /* Linear interpolation */
#define ERROR_THRESHOLD_QUADRATIC       (1e-5f)   /* Quadratic interpolation */
#define ERROR_THRESHOLD_TAN_LINEAR      (1e-3f)   /* Tan linear */
#define ERROR_THRESHOLD_TAN_QUADRATIC   (1e-4f)   /* Tan quadratic */
#define ERROR_THRESHOLD_TAN_HYBRID      (2e-5f)   /* Tan hybrid (highest accuracy) */
#define ERROR_THRESHOLD_ATAN            (1e-4f)   /* Atan with range reduction */

/* Volatile variables to prevent compiler optimization */
volatile float g_float_result = 0.0f;
volatile float g_angle = 1.0f;

uint64_t delta_time;

static inline void start_time(void)
{
    delta_time = hpm_csr_get_core_mcycle();
}

static inline uint32_t get_end_time(void)
{
    delta_time = hpm_csr_get_core_mcycle() - delta_time;
    return (uint32_t)delta_time;
}

/* Fast math context and buffer */
hpm_math_fast_trig_context_t math_ctx;
float sin_table[HPM_FAST_MATH_TABLE_SIZE_MAX];

/* Global test pass flag */
static int g_all_tests_passed = 1;

static void check_result(const char *name, float max_err, float threshold)
{
    if (max_err > threshold) {
        printf("  [FAIL] %s: max_err=%.2e > threshold=%.2e\n", name, max_err, threshold);
        g_all_tests_passed = 0;
    } else {
        printf("  [PASS] %s: max_err=%.2e <= threshold=%.2e\n", name, max_err, threshold);
    }
}

int main(void)
{
    uint32_t ticks;
    float fr;
    float angle;
    uint32_t table_size = HPM_FAST_MATH_TABLE_SIZE_MAX;
    float result_buffer[TEST_LOOP_COUNT];

    board_init();
    printf("\n================================================\n");
    printf("HPMicro Fast Math Functions Performance Test\n");
    printf("================================================\n");
    printf("CPU Frequency: %d Hz\n", clock_get_frequency(clock_cpu0));
    printf("Test Loop Count: %d\n", TEST_LOOP_COUNT);
    printf("Accuracy Test Step: %.0e rad\n", ACCURACY_TEST_STEP);
    printf("------------------------------------------------\n\n");

    /* Initialize fast math lookup table */
    if (hpm_math_fast_trig_init(&math_ctx, table_size, sin_table) != status_success) {
        printf("Failed to initialize fast math context!\n");
        while (1) {
        }
    }
    printf("Fast math context initialized with table size: %u\n", table_size);
    printf("Memory usage: %u bytes\n", table_size * sizeof(float));
    printf("Resolution: %.6f rad (%.4f deg)\n\n",
           HPM_MATH_2_PI / table_size, 360.0f / table_size);

    /* Load volatile values to local variables */
    angle = g_angle;

    /* ================================================================ */
    /*                    PERFORMANCE TEST                               */
    /* ================================================================ */
    printf("================== PERFORMANCE TEST ==================\n\n");

    /* ---------- Sin Performance ---------- */
    printf("--- Sin Functions ---\n");

    /* Standard sinf */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = sinf(angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("sinf (stdlib):                  %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast sinf - no interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_sinf(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_sinf:             %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast sinf - linear interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_sinf_linear(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_sinf_linear:      %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast sinf - quadratic interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_sinf_quadratic(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_sinf_quadratic:   %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* ---------- Cos Performance ---------- */
    printf("\n--- Cos Functions ---\n");

    /* Standard cosf */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = cosf(angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("cosf (stdlib):                  %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast cosf - no interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_cosf(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_cosf:             %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast cosf - linear interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_cosf_linear(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_cosf_linear:      %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast cosf - quadratic interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_cosf_quadratic(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_cosf_quadratic:   %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* ---------- SinCos Performance ---------- */
    printf("\n--- SinCos Functions (optimized for both sin+cos) ---\n");

    float sin_result, cos_result;
    float sin_buffer[TEST_LOOP_COUNT];
    float cos_buffer[TEST_LOOP_COUNT];

    /* Fast sincosf - linear interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        hpm_math_fast_sincosf_linear(&math_ctx, angle, &sin_result, &cos_result);
        angle += 0.001f;
        sin_buffer[i] = sin_result;
        cos_buffer[i] = cos_result;
    }
    ticks = get_end_time();
    g_float_result = sin_result + cos_result;
    printf("hpm_math_fast_sincosf_linear:   %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("sin[%d]: %f, cos[%d]: %f\n", i, sin_buffer[i], i, cos_buffer[i]);
    }
    angle = g_angle;

    /* Fast sincosf - quadratic interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        hpm_math_fast_sincosf_quadratic(&math_ctx, angle, &sin_result, &cos_result);
        angle += 0.001f;
        sin_buffer[i] = sin_result;
        cos_buffer[i] = cos_result;
    }
    ticks = get_end_time();
    g_float_result = sin_result + cos_result;
    printf("hpm_math_fast_sincosf_quadratic:%5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("sin[%d]: %f, cos[%d]: %f\n", i, sin_buffer[i], i, cos_buffer[i]);
    }
    angle = g_angle;

    /* ---------- Tan Performance ---------- */
    printf("\n--- Tan Functions (using optimized sincosf) ---\n");

    /* Standard tanf */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = tanf(angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("tanf (stdlib):                  %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast tanf - linear interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_tanf_linear(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_tanf_linear:      %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast tanf - quadratic interpolation */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_tanf_quadratic(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_tanf_quadratic:   %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }
    angle = g_angle;

    /* Fast tanf - hybrid method (highest accuracy) */
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_tanf_hybrid(&math_ctx, angle);
        angle += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_tanf_hybrid:      %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }

    /* ---------- Atan Performance ---------- */
    printf("\n--- Atan Functions ---\n");

    /* Standard atanf */
    float atan_input = 0.5f;
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = atanf(atan_input);
        atan_input += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("atanf (stdlib):                 %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }

    /* Fast atanf */
    atan_input = 0.5f;
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_atanf(atan_input);
        atan_input += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_atanf:            %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }

    /* ---------- Atan2 Performance ---------- */
    printf("\n--- Atan2 Functions ---\n");

    /* Standard atan2f */
    float y_val = 0.5f, x_val = 1.0f;
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = atan2f(y_val, x_val);
        y_val += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("atan2f (stdlib):                %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }

    /* Fast atan2f */
    y_val = 0.5f;
    start_time();
    for (int i = 0; i < TEST_LOOP_COUNT; i++) {
        fr = hpm_math_fast_atan2f(y_val, x_val);
        y_val += 0.001f;
        result_buffer[i] = fr;
    }
    ticks = get_end_time();
    g_float_result = fr;
    printf("hpm_math_fast_atan2f:           %5d ticks (avg: %d/op)\n",
           ticks, ticks / TEST_LOOP_COUNT);
    for (int i = 0; i < TEST_LOOP_COUNT; i += 100) {
        printf("result_buffer[%d]: %f\n", i, result_buffer[i]);
    }

    /* ================================================================ */
    /*                    ACCURACY TEST                                  */
    /* ================================================================ */
    printf("\n================== ACCURACY TEST ==================\n");
    printf("Testing range: [0, 2*PI], step: %.0e rad\n\n", ACCURACY_TEST_STEP);

    /* Sin accuracy test */
    float max_err_sin_none = 0.0f, avg_err_sin_none = 0.0f;
    float max_err_sin_linear = 0.0f, avg_err_sin_linear = 0.0f;
    float max_err_sin_quad = 0.0f, avg_err_sin_quad = 0.0f;
    uint32_t sin_test_count = 0;

    uint32_t num_steps_sin = (uint32_t)(HPM_MATH_2_PI / ACCURACY_TEST_STEP);
    for (uint32_t i = 0; i < num_steps_sin; i++) {
        float theta = i * ACCURACY_TEST_STEP;
        float std_val = sinf(theta);

        float fast_none = hpm_math_fast_sinf(&math_ctx, theta);
        float fast_linear = hpm_math_fast_sinf_linear(&math_ctx, theta);
        float fast_quad = hpm_math_fast_sinf_quadratic(&math_ctx, theta);

        float err_none = fabsf(std_val - fast_none);
        float err_linear = fabsf(std_val - fast_linear);
        float err_quad = fabsf(std_val - fast_quad);

        if (err_none > max_err_sin_none) {
            max_err_sin_none = err_none;
        }
        if (err_linear > max_err_sin_linear) {
            max_err_sin_linear = err_linear;
        }
        if (err_quad > max_err_sin_quad) {
            max_err_sin_quad = err_quad;
        }

        avg_err_sin_none += err_none;
        avg_err_sin_linear += err_linear;
        avg_err_sin_quad += err_quad;
        sin_test_count++;
    }

    avg_err_sin_none /= sin_test_count;
    avg_err_sin_linear /= sin_test_count;
    avg_err_sin_quad /= sin_test_count;

    printf("--- Sin Accuracy (test points: %u) ---\n", sin_test_count);
    printf("Method              Max Error       Avg Error\n");
    printf("----------------------------------------------\n");
    printf("No interpolation    %.2e       %.2e\n", max_err_sin_none, avg_err_sin_none);
    printf("Linear interp       %.2e       %.2e\n", max_err_sin_linear, avg_err_sin_linear);
    printf("Quadratic interp    %.2e       %.2e\n", max_err_sin_quad, avg_err_sin_quad);

    /* Cos accuracy test */
    float max_err_cos_none = 0.0f, avg_err_cos_none = 0.0f;
    float max_err_cos_linear = 0.0f, avg_err_cos_linear = 0.0f;
    float max_err_cos_quad = 0.0f, avg_err_cos_quad = 0.0f;
    uint32_t cos_test_count = 0;

    uint32_t num_steps_cos = (uint32_t)(HPM_MATH_2_PI / ACCURACY_TEST_STEP);
    for (uint32_t i = 0; i < num_steps_cos; i++) {
        float theta = i * ACCURACY_TEST_STEP;
        float std_val = cosf(theta);

        float fast_none = hpm_math_fast_cosf(&math_ctx, theta);
        float fast_linear = hpm_math_fast_cosf_linear(&math_ctx, theta);
        float fast_quad = hpm_math_fast_cosf_quadratic(&math_ctx, theta);

        float err_none = fabsf(std_val - fast_none);
        float err_linear = fabsf(std_val - fast_linear);
        float err_quad = fabsf(std_val - fast_quad);

        if (err_none > max_err_cos_none) {
            max_err_cos_none = err_none;
        }
        if (err_linear > max_err_cos_linear) {
            max_err_cos_linear = err_linear;
        }
        if (err_quad > max_err_cos_quad) {
            max_err_cos_quad = err_quad;
        }

        avg_err_cos_none += err_none;
        avg_err_cos_linear += err_linear;
        avg_err_cos_quad += err_quad;
        cos_test_count++;
    }

    avg_err_cos_none /= cos_test_count;
    avg_err_cos_linear /= cos_test_count;
    avg_err_cos_quad /= cos_test_count;

    printf("\n--- Cos Accuracy (test points: %u) ---\n", cos_test_count);
    printf("Method              Max Error       Avg Error\n");
    printf("----------------------------------------------\n");
    printf("No interpolation    %.2e       %.2e\n", max_err_cos_none, avg_err_cos_none);
    printf("Linear interp       %.2e       %.2e\n", max_err_cos_linear, avg_err_cos_linear);
    printf("Quadratic interp    %.2e       %.2e\n", max_err_cos_quad, avg_err_cos_quad);

    /* Tan accuracy test (avoid near pi/2 singularities) */
    float max_err_tan_linear = 0.0f, avg_err_tan_linear = 0.0f;
    float max_err_tan_quad = 0.0f, avg_err_tan_quad = 0.0f;
    float max_err_tan_hermite = 0.0f, avg_err_tan_hermite = 0.0f;
    uint32_t tan_test_count = 0;

    uint32_t num_steps_tan = (uint32_t)(HPM_MATH_2_PI / ACCURACY_TEST_STEP);
    for (uint32_t i = 0; i < num_steps_tan; i++) {
        float theta = i * ACCURACY_TEST_STEP;
        /* Skip near singularities (pi/2, 3*pi/2) */
        float dist_to_pi2 = fabsf(theta - HPM_MATH_HALF_PI);
        float dist_to_3pi2 = fabsf(theta - 3.0f * HPM_MATH_HALF_PI);
        if (dist_to_pi2 < 0.1f || dist_to_3pi2 < 0.1f) {
            continue;
        }

        float std_val = tanf(theta);

        float fast_linear = hpm_math_fast_tanf_linear(&math_ctx, theta);
        float fast_quad = hpm_math_fast_tanf_quadratic(&math_ctx, theta);
        float fast_hybrid = hpm_math_fast_tanf_hybrid(&math_ctx, theta);

        float err_linear = fabsf(std_val - fast_linear);
        float err_quad = fabsf(std_val - fast_quad);
        float err_hybrid = fabsf(std_val - fast_hybrid);

        if (err_linear > max_err_tan_linear) {
            max_err_tan_linear = err_linear;
        }
        if (err_quad > max_err_tan_quad) {
            max_err_tan_quad = err_quad;
        }
        if (err_hybrid > max_err_tan_hermite) {
            max_err_tan_hermite = err_hybrid;
        }

        avg_err_tan_linear += err_linear;
        avg_err_tan_quad += err_quad;
        avg_err_tan_hermite += err_hybrid;
        tan_test_count++;
    }

    avg_err_tan_linear /= tan_test_count;
    avg_err_tan_quad /= tan_test_count;
    avg_err_tan_hermite /= tan_test_count;

    printf("\n--- Tan Accuracy (test points: %u, avoiding singularities) ---\n", tan_test_count);
    printf("Method              Max Error       Avg Error\n");
    printf("----------------------------------------------\n");
    printf("Linear interp       %.2e       %.2e\n", max_err_tan_linear, avg_err_tan_linear);
    printf("Quadratic interp    %.2e       %.2e\n", max_err_tan_quad, avg_err_tan_quad);
    printf("Hybrid method       %.2e       %.2e\n", max_err_tan_hermite, avg_err_tan_hermite);

    /* Atan accuracy test */
    float max_err_atan = 0.0f, avg_err_atan = 0.0f;
    uint32_t atan_test_count = 0;
    const float atan_start = -10.0f;
    const float atan_step = 0.0001f;
    uint32_t num_atan_steps = (uint32_t)((10.0f - (-10.0f)) / atan_step);

    for (uint32_t i = 0; i <= num_atan_steps; i++) {
        float x = atan_start + i * atan_step;
        float std_val = atanf(x);
        float fast_val = hpm_math_fast_atanf(x);

        float err = fabsf(std_val - fast_val);

        if (err > max_err_atan) {
            max_err_atan = err;
        }
        avg_err_atan += err;
        atan_test_count++;
    }

    avg_err_atan /= atan_test_count;

    printf("\n--- Atan Accuracy (range: [-10, 10], test points: %u) ---\n", atan_test_count);
    printf("Method              Max Error       Avg Error\n");
    printf("----------------------------------------------\n");
    printf("Fast (range red.)   %.2e       %.2e\n", max_err_atan, avg_err_atan);

    /* Atan2 accuracy test */
    float max_err_atan2 = 0.0f, avg_err_atan2 = 0.0f;
    uint32_t atan2_test_count = 0;
    const float atan2_step = 0.001f;
    uint32_t num_atan2_steps = (uint32_t)(HPM_MATH_2_PI / atan2_step);

    for (uint32_t i = 0; i < num_atan2_steps; i++) {
        float theta = i * atan2_step;
        float y = sinf(theta);
        float x = cosf(theta);

        float std_val = atan2f(y, x);
        float fast_val = hpm_math_fast_atan2f(y, x);

        float err = fabsf(std_val - fast_val);

        if (err > max_err_atan2) {
            max_err_atan2 = err;
        }
        avg_err_atan2 += err;
        atan2_test_count++;
    }

    avg_err_atan2 /= atan2_test_count;

    printf("\n--- Atan2 Accuracy (test points: %u) ---\n", atan2_test_count);
    printf("Method              Max Error       Avg Error\n");
    printf("----------------------------------------------\n");
    printf("Fast (range red.)   %.2e       %.2e\n", max_err_atan2, avg_err_atan2);

    /* ================================================================ */
    /*                    TEST RESULT SUMMARY                            */
    /* ================================================================ */
    printf("\n================== TEST RESULT SUMMARY ==================\n\n");

    printf("--- Sin/Cos Tests ---\n");
    check_result("sin (no interp)", max_err_sin_none, ERROR_THRESHOLD_NONE);
    check_result("sin (linear)", max_err_sin_linear, ERROR_THRESHOLD_LINEAR);
    check_result("sin (quadratic)", max_err_sin_quad, ERROR_THRESHOLD_QUADRATIC);
    check_result("cos (no interp)", max_err_cos_none, ERROR_THRESHOLD_NONE);
    check_result("cos (linear)", max_err_cos_linear, ERROR_THRESHOLD_LINEAR);
    check_result("cos (quadratic)", max_err_cos_quad, ERROR_THRESHOLD_QUADRATIC);

    printf("\n--- Tan Tests ---\n");
    check_result("tan (linear)", max_err_tan_linear, ERROR_THRESHOLD_TAN_LINEAR);
    check_result("tan (quadratic)", max_err_tan_quad, ERROR_THRESHOLD_TAN_QUADRATIC);
    check_result("tan (hybrid)", max_err_tan_hermite, ERROR_THRESHOLD_TAN_HYBRID);

    printf("\n--- Atan/Atan2 Tests ---\n");
    check_result("atan", max_err_atan, ERROR_THRESHOLD_ATAN);
    check_result("atan2", max_err_atan2, ERROR_THRESHOLD_ATAN);

    printf("\n================================================\n");
    if (g_all_tests_passed) {
        printf("*** ALL TESTS PASSED ***\n");
    } else {
        printf("*** SOME TESTS FAILED - Please check results above ***\n");
    }
    printf("================================================\n");

    while (1) {
        ;
    }

    return 0;
}
