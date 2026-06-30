/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MQUICKJS_PORT_H
#define MQUICKJS_PORT_H

#include "hpm_common.h"
#include "hpm_debug_console.h"

/* Include mquickjs types - this header must be included after mquickjs.h
 * or we can include it here for the JS function declarations */
#include "mquickjs.h"

/* Memory configuration - adjust based on available RAM */
#define MQUICKJS_HEAP_SIZE      (32 * 1024)  /* 32KB heap for JS engine */

/* REPL configuration */
#define MQUICKJS_REPL_BUF_SIZE  256
#define MQUICKJS_PROMPT         "js> "

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Initialize mquickjs port layer
 */
void mquickjs_port_init(void);

/**
 * @brief Get time in milliseconds since boot
 * @return Time in milliseconds
 */
int64_t mquickjs_port_get_time_ms(void);

/**
 * @brief Read a line from console (non-blocking style)
 * @param buf Buffer to store the line
 * @param max_len Maximum length to read
 * @return Number of characters read, or -1 if no data available
 */
int mquickjs_port_readline(char *buf, int max_len);

/**
 * @brief Write data to console
 * @param data Data to write
 * @param len Length of data
 */
void mquickjs_port_write(const char *data, size_t len);

/**
 * @brief Check if a character is available
 * @return true if a character is available
 */
bool mquickjs_port_char_available(void);

/**
 * @brief Get a character from console (non-blocking)
 * @return Character read, or -1 if none available
 */
int mquickjs_port_getchar(void);

/**
 * @brief Put a character to console
 * @param c Character to output
 */
void mquickjs_port_putchar(char c);

/*
 * JavaScript stdlib functions - required by mqjs_stdlib.h
 * These are platform-specific implementations for HPM SDK
 */

/**
 * @brief JavaScript print() function implementation
 */
JSValue js_print(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/**
 * @brief JavaScript gc() function implementation
 */
JSValue js_gc(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/**
 * @brief JavaScript Date.now() implementation
 */
JSValue js_date_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/**
 * @brief JavaScript performance.now() implementation
 */
JSValue js_performance_now(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/**
 * @brief JavaScript load() function implementation (load and execute a script file)
 */
JSValue js_load(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/**
 * @brief JavaScript setTimeout() implementation
 */
JSValue js_setTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/**
 * @brief JavaScript clearTimeout() implementation
 */
JSValue js_clearTimeout(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/**
 * @brief JavaScript delayMs() implementation - blocking delay in milliseconds
 * @param ms Number of milliseconds to delay
 */
JSValue js_delayMs(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

#if defined(__cplusplus)
}
#endif

#endif /* MQUICKJS_PORT_H */
