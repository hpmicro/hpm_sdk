/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include "mquickjs_port.h"
#ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
#include "hpm_gptmrv2_drv.h"
#else
#include "hpm_gptmr_drv.h"
#endif
#include "hpm_clock_drv.h"

#ifdef CONFIG_MQUICKJS

/* Need mquickjs headers for JS types */
#include "mquickjs.h"

/* Use machine timer or GPTMR for time tracking */
#if defined(HPM_GPTMR)
static GPTMR_Type *g_time_tmr = NULL;
static uint32_t g_time_tmr_ch = 0;
static uint32_t g_time_tmr_freq = 0;
#endif

void mquickjs_port_init(void)
{
    /* Console is already initialized by board_init() */
    /* Nothing extra needed here */
}

int64_t mquickjs_port_get_time_ms(void)
{
    /* Use the system tick or timer for time tracking */
    /* HPM SDK provides a simple way to get time via machine timer or GPTMR */
    /* For now, use a simple counter - this should be replaced with actual timer */
    /* On RISC-V, we can use mcycle counter */
    static int64_t ms_counter = 0;
    /* This is a simplified implementation - in production, use a proper timer */
    /* Or integrate with FreeRTOS tick count if available */
#ifdef HPM_CORE_MCYCLE
    /* RISC-V mcycle counter (assuming 1GHz clock, adjust for actual frequency) */
    uint64_t mcycle;
    __asm__ volatile("csrr %0, mcycle" : "=r"(mcycle));
    return (int64_t)(mcycle / 1000000);  /* Convert cycles to ms */
#else
    /* Fallback: increment counter on each call (not accurate) */
    return ms_counter++;
#endif
}

bool mquickjs_port_char_available(void)
{
    uint8_t c = console_try_receive_byte();
    /* The try_receive_byte returns 0 if no data, but 0 could be a valid char */
    /* We need a different approach - check UART status directly */
    /* For now, assume we always check and return true if read succeeds */
    return (c != 0);  /* Simplified check */
}

int mquickjs_port_getchar(void)
{
    int c = console_try_receive_byte();
    if (c > 0) {
        return c;
    }
    return -1;
}

void mquickjs_port_putchar(char c)
{
    console_send_byte((uint8_t)c);
}

void mquickjs_port_write(const char *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (data[i] == '\n') {
            console_send_byte('\r');
        }
        console_send_byte((uint8_t)data[i]);
    }
}

int mquickjs_port_readline(char *buf, int max_len)
{
    (void)max_len;
    static char rx_buf[MQUICKJS_REPL_BUF_SIZE];
    static int rx_pos = 0;
    int c;
    /* Non-blocking read - check for available character */
    c = console_try_receive_byte();
    if (c == 0) {
        /* No data available */
        return -1;
    }
    /* Echo the character */
    if (c == '\r' || c == '\n') {
        console_send_byte('\r');
        console_send_byte('\n');
        if (rx_pos > 0) {
            /* Copy and return the line */
            memcpy(buf, rx_buf, rx_pos);
            buf[rx_pos] = '\0';
            int len = rx_pos;
            rx_pos = 0;
            return len;
        }
        rx_pos = 0;
        return 0;
    } else if (c == 0x7F || c == '\b') {
        /* Backspace */
        if (rx_pos > 0) {
            rx_pos--;
            console_send_byte('\b');
            console_send_byte(' ');
            console_send_byte('\b');
        }
        return -1;
    } else if (c >= ' ' && rx_pos < MQUICKJS_REPL_BUF_SIZE - 1) {
        /* Regular character */
        rx_buf[rx_pos++] = (char)c;
        console_send_byte((uint8_t)c);
        return -1;
    }
    return -1;
}

/*
 * JavaScript stdlib function implementations for HPM SDK
 */

JSValue js_print(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    int i;
    for (i = 0; i < argc; i++) {
        if (i != 0) {
            mquickjs_port_putchar(' ');
        }
        /* Use JS_PrintValue for non-string values */
        if (JS_IsString(ctx, argv[i])) {
            /* For strings, convert and print directly */
            /* Note: mquickjs has different API, use JS_ToCString variant */
            const char *str;
            size_t len;
            str = JS_ToCStringLen(ctx, &len, argv[i], NULL);
            if (str) {
                mquickjs_port_write(str, len);
            }
        } else {
            /* For other values, use simple conversion */
            /* This is simplified - real implementation would need proper value printing */
            int int_val;
            if (!JS_ToInt32(ctx, &int_val, argv[i])) {
                char num_buf[16];
                snprintf(num_buf, sizeof(num_buf), "%d", int_val);
                mquickjs_port_write(num_buf, strlen(num_buf));
            }
        }
    }
    /* Send \r\n for proper terminal newline */
    mquickjs_port_putchar('\r');
    mquickjs_port_putchar('\n');
    return JS_UNDEFINED;
}

JSValue js_gc(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    (void)argc;
    (void)argv;
    JS_GC(ctx);
    return JS_UNDEFINED;
}

JSValue js_date_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    (void)argc;
    (void)argv;
    int64_t ms = mquickjs_port_get_time_ms();
    return JS_NewInt64(ctx, ms);
}

JSValue js_performance_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    (void)argc;
    (void)argv;
    int64_t ms = mquickjs_port_get_time_ms();
    return JS_NewInt64(ctx, ms);
}

/* load() - not supported on embedded without filesystem */
JSValue js_load(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    (void)argc;
    (void)argv;
    return JS_ThrowTypeError(ctx, "load() not supported on embedded platform");
}

/* Timer implementation - simplified for embedded */
#define MAX_TIMERS 4

typedef struct {
    int allocated;
    int func_ref;  /* Store function reference */
    int64_t timeout;
} js_timer_t;

static js_timer_t js_timers[MAX_TIMERS];

JSValue js_setTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    (void)argc;
    int delay, i;
    if (!JS_IsFunction(ctx, argv[0])) {
        return JS_ThrowTypeError(ctx, "not a function");
    }
    if (JS_ToInt32(ctx, &delay, argv[1])) {
        return JS_EXCEPTION;
    }
    for (i = 0; i < MAX_TIMERS; i++) {
        if (!js_timers[i].allocated) {
            js_timers[i].allocated = 1;
            js_timers[i].timeout = mquickjs_port_get_time_ms() + delay;
            /* Note: proper implementation needs to store function reference */
            return JS_NewInt32(ctx, i);
        }
    }
    return JS_ThrowInternalError(ctx, "too many timers");
}

JSValue js_clearTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    (void)argc;
    int timer_id;
    if (JS_ToInt32(ctx, &timer_id, argv[0])) {
        return JS_EXCEPTION;
    }
    if (timer_id >= 0 && timer_id < MAX_TIMERS) {
        js_timers[timer_id].allocated = 0;
    }
    return JS_UNDEFINED;
}

/* delayMs() - blocking delay in milliseconds */
JSValue js_delayMs(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    int delay_ms;
    if (argc < 1) {
        return JS_ThrowTypeError(ctx, "delayMs requires delay argument");
    }
    if (JS_ToInt32(ctx, &delay_ms, argv[0])) {
        return JS_EXCEPTION;
    }
    if (delay_ms <= 0) {
        return JS_NewInt32(ctx, 0);
    }
    clock_cpu_delay_ms(delay_ms);
    return JS_NewInt32(ctx, 0);
}

#endif /* CONFIG_MQUICKJS */
