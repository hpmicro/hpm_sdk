
/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <hpm_l1c_drv.h>
#include <hpm_interrupt.h>
#include <hpm_sysctl_drv.h>
#include <hpm_clock_drv.h>
#include "hpm_log.h"

typedef struct hpm_log hpm_log_t;

typedef struct hpm_log_fifo {
    char *buf;
    uint32_t size;
    volatile uint32_t w_index;
    volatile uint32_t r_index;
} hpm_log_fifo_t;

struct hpm_log_engine {
    volatile int enabled;
    volatile int transferring;
    void (*transfer_start)(const char *buf, uint32_t len);
    void (*transfer_block)(const char *buf, uint32_t len);
    char *transfer_buf;
    uint32_t max_transfer_size;
    hpm_log_fifo_t fifo;
    hpm_log_t *ctx;
};

struct hpm_log {
    char message[HPM_LOG_CONFIG_MAX_MESSAGE_LEN];
    uint64_t (*get_timestamp)(void);
    long (*critical_enter)(void);
    void (*critical_exit)(long val);
    hpm_log_engine_t engine[HPM_LOG_CONFIG_MAX_ENGINE_NUM];
};

static hpm_log_t hpm_log_ctx;

static void hpm_log_fifo_init(hpm_log_fifo_t *fifo, char *buf, uint32_t size)
{
    fifo->buf = buf;
    fifo->size = size;
    fifo->w_index = 0;
    fifo->r_index = 0;
}

static HPM_LOG_FAST_FUNC uint32_t hpm_log_fifo_get_len(hpm_log_fifo_t *fifo)
{
    uint32_t len;

    if (fifo->w_index == fifo->r_index) {
        len = 0;
    } else if (fifo->w_index > fifo->r_index) {
        len = fifo->w_index - fifo->r_index;
    } else {
        len = fifo->w_index + (fifo->size - fifo->r_index);
    }

    return len;
}

static HPM_LOG_FAST_FUNC void hpm_log_fifo_push(hpm_log_fifo_t *fifo, char *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        fifo->buf[fifo->w_index] = buf[i];
        fifo->w_index++;
        fifo->w_index %= fifo->size;

        /*
         * fifo overflow, so update r_index
         */
        if (fifo->r_index == fifo->w_index) {
            fifo->r_index++;
            fifo->r_index %= fifo->size;
        }
    }
}

static HPM_LOG_FAST_FUNC uint32_t hpm_log_fifo_pop(hpm_log_fifo_t *fifo, char *buf, uint32_t len)
{
    uint32_t max_len = hpm_log_fifo_get_len(fifo);

    max_len = max_len < len ? max_len : len;

    for (uint32_t i = 0; i < max_len; i++) {
        buf[i] = fifo->buf[fifo->r_index];
        fifo->r_index++;
        fifo->r_index %= fifo->size;
    }

    return max_len;
}

hpm_log_engine_t *hpm_log_engine_create(hpm_log_engine_config_t *cfg)
{
    hpm_log_engine_t *engine = NULL;
    hpm_log_t *ctx = &hpm_log_ctx;
    long critical;

    if (!cfg || !cfg->fifo_buf || !cfg->transfer_buf ||
        !cfg->max_transfer_size || !cfg->fifo_buf_size ||
        !cfg->transfer_start) {
        return NULL;
    }

    critical = ctx->critical_enter();
    for (int i = 0; i < HPM_LOG_CONFIG_MAX_ENGINE_NUM; i++) {
        if (!ctx->engine[i].ctx) {
            engine = &ctx->engine[i];
            break;
        }
    }

    if (engine) {
        engine->ctx = ctx;
        engine->enabled = 0;
        engine->transferring = 0;
        engine->max_transfer_size = cfg->max_transfer_size;
        engine->transfer_start = cfg->transfer_start;
        engine->transfer_block = cfg->transfer_block;
        engine->transfer_buf = cfg->transfer_buf;
        hpm_log_fifo_init(&engine->fifo, cfg->fifo_buf, cfg->fifo_buf_size);
    }

    ctx->critical_exit(critical);

    return engine;
}

int hpm_log_engine_destroy(hpm_log_engine_t *engine)
{
    hpm_log_t *ctx;
    long critical;

    if (!engine || !engine->ctx) {
        return -1;
    }

    ctx = engine->ctx;
    critical = ctx->critical_enter();
    engine->ctx = NULL;
    ctx->critical_exit(critical);

    return 0;
}

void hpm_log_engine_enable(hpm_log_engine_t *engine)
{
    hpm_log_t *ctx;
    long critical;

    if (!engine || !engine->ctx) {
        return;
    }

    ctx = engine->ctx;
    critical = ctx->critical_enter();
    engine->enabled = 1;
    ctx->critical_exit(critical);
}

void hpm_log_engine_disable(hpm_log_engine_t *engine)
{
    hpm_log_t *ctx;
    long critical;

    if (!engine || !engine->ctx) {
        return;
    }

    ctx = engine->ctx;
    critical = ctx->critical_enter();
    engine->enabled = 0;
    ctx->critical_exit(critical);
}

static HPM_LOG_FAST_FUNC void hpm_log_engine_transfer_start(hpm_log_engine_t *engine)
{
    hpm_log_t *ctx;
    uint32_t max_transfer_len;
    long critical;

    if (!engine || !engine->ctx || !engine->enabled) {
        return;
    }

    ctx = engine->ctx;
    critical = ctx->critical_enter();

    if (!engine->transferring) {
        max_transfer_len = hpm_log_fifo_pop(&engine->fifo, engine->transfer_buf, engine->max_transfer_size);
        if (max_transfer_len) {
            engine->transfer_start(engine->transfer_buf, max_transfer_len);
            engine->transferring = 1;
        }
    }

    ctx->critical_exit(critical);
}

HPM_LOG_FAST_FUNC void hpm_log_engine_transfer_finish(hpm_log_engine_t *engine)
{
    hpm_log_t *ctx;
    uint32_t max_transfer_len;
    long critical;

    if (!engine || !engine->ctx || !engine->enabled) {
        return;
    }

    ctx = engine->ctx;
    critical = ctx->critical_enter();
    engine->transferring = 0;

    max_transfer_len = hpm_log_fifo_pop(&engine->fifo, engine->transfer_buf, engine->max_transfer_size);
    if (max_transfer_len) {
        engine->transfer_start(engine->transfer_buf, max_transfer_len);
        engine->transferring = 1;
    }

    ctx->critical_exit(critical);
}

HPM_LOG_FAST_FUNC void hpm_log_engine_dump(hpm_log_engine_t *engine)
{
    hpm_log_t *ctx;
    uint32_t max_transfer_len;
    long critical;

    if (!engine || !engine->ctx || !engine->enabled || !engine->transfer_block) {
        return;
    }

    ctx = engine->ctx;
    critical = ctx->critical_enter();

    while ((max_transfer_len = hpm_log_fifo_pop(&engine->fifo, engine->transfer_buf, engine->max_transfer_size))) {
        engine->transfer_block(engine->transfer_buf, max_transfer_len);
    }

    ctx->critical_exit(critical);
}

static HPM_LOG_FAST_FUNC void hpm_log_transfer_start_all(hpm_log_t *ctx)
{
    for (int i = 0; i < HPM_LOG_CONFIG_MAX_ENGINE_NUM; i++) {
        hpm_log_engine_transfer_start(&ctx->engine[i]);
    }

}

static HPM_LOG_FAST_FUNC void hpm_log_fifo_push_all(hpm_log_t *ctx, char *message, uint32_t messge_len)
{
    for (int i = 0; i < HPM_LOG_CONFIG_MAX_ENGINE_NUM; i++) {
        if (ctx->engine[i].ctx && ctx->engine[i].enabled) {
            hpm_log_fifo_push(&ctx->engine[i].fifo, message, messge_len);
        }
    }
}

HPM_LOG_FAST_FUNC void hpm_log_write(int level, const char *func, const int line, const char *fmt, ...)
{
    static const char * const log_tag[] = {"UNKNOWN", "E", "W", "I", "D"};
    hpm_log_t *ctx = &hpm_log_ctx;
    uint32_t messge_len = 0;
    char *message = ctx->message;
    long critical;

    if (level > HPM_LOG_LEVEL_DEBUG) {
        level = 0;
    }

#if HPM_LOG_CONFIG_ENABLE_TIMESTAMP
    uint64_t timestamp = ctx->get_timestamp();
#endif

    critical = ctx->critical_enter();
    messge_len = snprintf(message, HPM_LOG_CONFIG_MAX_MESSAGE_LEN,
#if HPM_LOG_CONFIG_ENABLE_TIMESTAMP
        "%u.%06u "
#endif
        "[%s]"
#if HPM_LOG_CONFIG_ENABLE_FUNC_NAME
        " %s:%u: "
#endif
        " ",

#if HPM_LOG_CONFIG_ENABLE_TIMESTAMP
        (uint32_t)(timestamp / 1000000u), (uint32_t)(timestamp % 1000000u),
#endif

        log_tag[level]

#if HPM_LOG_CONFIG_ENABLE_FUNC_NAME
        , func, line
#endif
    );

    va_list ap;
    va_start(ap, fmt);
    messge_len += vsnprintf(message + messge_len, HPM_LOG_CONFIG_MAX_MESSAGE_LEN - messge_len, fmt, ap);
    va_end(ap);
    hpm_log_fifo_push_all(ctx, message, messge_len);
    ctx->critical_exit(critical);

    hpm_log_transfer_start_all(ctx);
}

HPM_LOG_FAST_FUNC void hpm_log_printf(const char *fmt, ...)
{
    hpm_log_t *ctx = &hpm_log_ctx;
    uint32_t messge_len;
    char *message = ctx->message;
    long critical;

    critical = ctx->critical_enter();
    va_list ap;
    va_start(ap, fmt);
    messge_len = vsnprintf(message, HPM_LOG_CONFIG_MAX_MESSAGE_LEN, fmt, ap);
    va_end(ap);
    hpm_log_fifo_push_all(ctx, message, messge_len);
    ctx->critical_exit(critical);

    hpm_log_transfer_start_all(ctx);
}

#if HPM_LOG_CONFIG_ENABLE_TIMESTAMP
static HPM_LOG_FAST_FUNC uint64_t hpm_log_cpu_get_us(void)
{
    static uint32_t ticks_per_us = 0;
    uint64_t result;
    uint32_t resultl_first = read_csr(CSR_MCYCLE);
    uint32_t resulth = read_csr(CSR_MCYCLEH);
    uint32_t resultl_second = read_csr(CSR_MCYCLE);

    if (resultl_first < resultl_second) {
        result = ((uint64_t)resulth << 32) | resultl_first;
    } else {
        resulth = read_csr(CSR_MCYCLEH);
        result = ((uint64_t)resulth << 32) | resultl_second;
    }

    if (!ticks_per_us) {
        ticks_per_us = clock_get_core_clock_ticks_per_us();
    }

    return result / ticks_per_us;
}
#endif

static HPM_LOG_FAST_FUNC long hpm_log_lock(void)
{
    return disable_global_irq(CSR_MSTATUS_MIE_MASK);
}

static HPM_LOG_FAST_FUNC void hpm_log_unlock(long restore)
{
    enable_global_irq(restore);
}

void hpm_log_init(hpm_log_config_t *cfg)
{
    hpm_log_t *ctx = &hpm_log_ctx;

#if HPM_LOG_CONFIG_ENABLE_TIMESTAMP
    if (!cfg || !cfg->get_timestamp) {
        ctx->get_timestamp = hpm_log_cpu_get_us;
    } else {
        ctx->get_timestamp = cfg->get_timestamp;
    }

#endif

    if (!cfg || !cfg->critical_enter) {
        ctx->critical_enter = hpm_log_lock;
    } else {
        ctx->critical_enter = cfg->critical_enter;
    }

    if (!cfg || !ctx->critical_exit) {
        ctx->critical_exit = hpm_log_unlock;
    } else {
        ctx->critical_exit = cfg->critical_exit;
    }
}

