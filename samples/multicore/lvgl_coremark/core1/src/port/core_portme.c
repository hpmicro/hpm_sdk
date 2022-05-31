/*
 *   Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *   Original Author: Shay Gal-on
 */
#include <stdio.h>
#include "coremark.h"
#include "core_portme.h"
#include "board.h"
#include "hpm_uart_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_debug_console.h"
#include "board.h"

#define COREMARK_CTX (coremark_context_t *)(0xBF000UL)

#if USE_SNPRINTF
#include <stdio.h>
#include <stdarg.h>
void ee_printf(char *format, ...)
{
    char g_temp[1024];

    coremark_context_t *ctx = COREMARK_CTX;

    uint32_t len;
    va_list args;
    va_start(args, format);
    len = vsnprintf(g_temp, sizeof(g_temp), format, args);
    va_end(args);
    memcpy((void *) &ctx->s_buffer[ctx->current_idx], g_temp, len);
    if (ctx->print_out) {
        ctx->printf_callback("%s\n", g_temp);
    }
    ctx->current_idx += len;
}
#endif

#define ITERATIONS (0)
#define CLOCKS_PER_SEC (clock_get_frequency(clock_mchtmr0))

#if VALIDATION_RUN
volatile ee_s32 seed1_volatile = 0x3415;
volatile ee_s32 seed2_volatile = 0x3415;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN

volatile ee_s32 seed1_volatile = 0x0;

volatile ee_s32 seed2_volatile = 0x0;

volatile ee_s32 seed3_volatile = 0x66;

#endif
#if PROFILE_RUN
volatile ee_s32 seed1_volatile = 0x8;
volatile ee_s32 seed2_volatile = 0x8;
volatile ee_s32 seed3_volatile = 0x8;
#endif

volatile ee_s32 seed4_volatile = ITERATIONS;

volatile ee_s32 seed5_volatile = 0;
/* Porting : Timing functions
 *       How to capture time and convert to seconds must be ported to whatever is
 *  supported by the platform. e.g. Read value from on board RTC, read value from
 *  cpu clock cycles performance counter etc. Sample implementation for standard
 *  time.h and windows.h definitions included.
 */
CORETIMETYPE barebones_clock(void)
{
    return (CORETIMETYPE) HPM_MCHTMR->MTIME;
}
/* Define : TIMER_RES_DIVIDER
 *       Divider to trade off timer resolution and total time that can be
 *  measured.
 *
 *       Use lower values to increase resolution, but make sure that overflow
 *  does not occur. If there are issues with the return value overflowing,
 *  increase this value.
 */
#define GETMYTIME(_t) (*_t = barebones_clock())
#define MYTIMEDIFF(fin, ini) ((fin) - (ini))
/* #define TIMER_RES_DIVIDER          1 */
/* #define SAMPLE_TIME_IMPLEMENTATION 1 */
/* #define EE_TICKS_PER_SEC           (CLOCKS_PER_SEC / TIMER_RES_DIVIDER) */
#ifndef TIMER_CLOCK_NAME
#define TIMER_CLOCK_NAME clock_mchtmr1
#endif

/** Define Host specific (POSIX), or target specific global time variables. */

/* Function : start_time
 *       This function will be called right before starting the timed portion of
 *  the benchmark.
 *
 *       Implementation may be capturing a system timer (as implemented in the
 *  example code) or zeroing some system parameters - e.g. setting the cpu clocks
 *  cycles to 0.
 */
void start_time(void)
{
    coremark_context_t *ctx = COREMARK_CTX;
    GETMYTIME(&ctx->start_time_val);
}
/* Function : stop_time
 *       This function will be called right after ending the timed portion of the
 *  benchmark.
 *
 *       Implementation may be capturing a system timer (as implemented in the
 *  example code) or other system parameters - e.g. reading the current value of
 *  cpu cycles counter.
 */
void stop_time(void)
{
    coremark_context_t *ctx = COREMARK_CTX;
    GETMYTIME(&ctx->stop_time_val);
}
/* Function : get_time
 *       Return an abstract "ticks" number that signifies time on the system.
 *
 *       Actual value returned may be cpu cycles, milliseconds or any other
 *  value, as long as it can be converted to seconds by <time_in_secs>. This
 *  methodology is taken to accommodate any hardware or simulated platform.
 *  The sample implementation returns milliseconds by default, and the resolution is
 *  controlled by <TIMER_RES_DIVIDER>
 */
CORE_TICKS get_time(void)
{
    coremark_context_t *ctx = COREMARK_CTX;
    CORE_TICKS elapsed = (CORE_TICKS) (MYTIMEDIFF(ctx->stop_time_val, ctx->start_time_val));
    return elapsed;
}
/* Function : time_in_secs
 *       Convert the value returned by get_time to seconds.
 *
 *       The <secs_ret> type is used to accommodate systems with no support for
 *  floating point. Default implementation implemented by the EE_TICKS_PER_SEC
 *  macro above.
 */
secs_ret time_in_secs(CORE_TICKS ticks)
{
    coremark_context_t *ctx = COREMARK_CTX;
    secs_ret retval = ((secs_ret) ticks) / (secs_ret) ctx->tmr_freq;
    return retval;
}

const ee_u32 default_num_contexts = 1;

/* Function : portable_init
 *       Target specific initialization code
 *       Test for some common mistakes.
 */
void portable_init(core_portable *p, int *argc, char *argv[])
{
    coremark_context_t *ctx = COREMARK_CTX;
    memset(ctx, 0, sizeof(*ctx));
    ctx->current_idx = 0;
    ctx->s_buffer[0] = '\0';
    ctx->print_out = true;
    ctx->printf_callback = printf;

    board_init_uart(HPM_UART6);
    clock_add_to_group(clock_uart6, 1);
    console_config_t config;
    config.type = console_type_uart;
    config.base = HPM_UART6_BASE;
    config.src_freq_in_hz = board_init_uart_clock(HPM_UART6);
    config.baudrate = 115200;
    console_init(&config);
    uint32_t m_hartid = read_csr(CSR_MHARTID);
    clock_name_t mtmr_clk = (m_hartid == 1) ? clock_mchtmr1 : clock_mchtmr0;
    ctx->tmr_freq = clock_get_frequency(mtmr_clk);
    if (!ctx->tmr_freq) {
        ee_printf("ERROR! make sure TIMER_CLOCK_NAME is defined properly!\n");
    }

    if (sizeof(ee_ptr_int) != sizeof(ee_u8 *)) {
        ee_printf(
            "ERROR! Please define ee_ptr_int to a type that holds a "
            "pointer!\n");
    }
    if (sizeof(ee_u32) != 4) {
        ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\n");
    }
    p->portable_id = 1;
}



typedef void (*app_func_t)(void);

extern uint32_t _start[];

/* Function : portable_fini
 *       Target specific final code
 */
void portable_fini(core_portable *p)
{
    coremark_context_t *ctx = COREMARK_CTX;
    ctx->has_done = true;
    if (ctx->print_out && ctx->printf_callback) {
        ctx->printf_callback("%s\n", (const char *) ctx->s_buffer);
    }

    while (HPM_SYSCTL->CPU[1].GPR[3] == 0) {
        for (volatile uint32_t i = 0; i < 10000; i++) {
            __asm__ volatile("nop");
        }
    }
    HPM_SYSCTL->CPU[1].GPR[3] = 0;
    app_func_t start = (app_func_t) _start;
    printf("Re-running the coremark ...\n");
    start();
    while (1) {

    }
}
