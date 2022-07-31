/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_dma_manager.h"

#define LED_FLASH_PERIOD_IN_MS 300

#define DMA_RESOURCE_NUM 16

hpm_dma_resource_t dma_resource_pools[DMA_RESOURCE_NUM];

typedef struct {
    uint8_t src[256];
    uint8_t dst[256];
    hpm_dma_channel_callback_t callback;
    volatile uint32_t has_done;
} dma_test_context;


ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) dma_test_context s_dma_test_ctx[DMA_RESOURCE_NUM];

void dma_channel_callback(DMA_Type *ptr, uint32_t channel, void *user_data, uint32_t int_stat);

void show_description(void);
void dma_manager_test_setup(void);
bool dma_manager_test_execute(void);
void dma_manager_test_teardown(void);

int main(void)
{
    int u;
    board_init();
    board_init_led_pins();

    show_description();

    dma_manager_init();

    uint32_t round = 0;
    bool result = true;
    while (round < 2) {
        dma_manager_test_setup();
        result = dma_manager_test_execute();
        if (!result) {
            break;
        }
        dma_manager_test_teardown();

        ++round;
    }

    printf("DMA Manager test %s\n", result ? "PASSED" : "FAILED");

    return 0;
}

void dma_channel_callback(DMA_Type *ptr, uint32_t channel, void *user_data, uint32_t int_stat)
{
    if (int_stat == DMA_CHANNEL_STATUS_TC) {
        *(volatile bool *)user_data = true;
    }
}

void show_description(void)
{
    static const char desc[] = "\n "
                        "============================================================================\n"
                        "\n\n"
                        "    This example demonstrates the following features with DMA manager\n"
                        "\n"
                        "      1. Request/release DMA resource\n"
                        "      2. Enable DMA interrupt\n"
                        "      3. DMA callback installation\n"
                        "\n\n"
                        "=============================================================================\n";
     printf("%s", desc);
}

void dma_manager_test_setup(void)
{
    dma_test_context *test_ctx;
    for (uint32_t i = 0; i < ARRAY_SIZE(s_dma_test_ctx); i++) {
        test_ctx = &s_dma_test_ctx[i];
        memset(test_ctx, 0, sizeof(*test_ctx));
        test_ctx->callback = dma_channel_callback;
        for (uint32_t src_buf_idx = 0; src_buf_idx < ARRAY_SIZE(test_ctx->src); src_buf_idx++) {
            test_ctx->src[src_buf_idx] = src_buf_idx & 0xFF;
        }
    }

    for (uint32_t i = 0; i < ARRAY_SIZE(dma_resource_pools); i++) {
        hpm_dma_resource_t *resource = &dma_resource_pools[i];
        test_ctx = &s_dma_test_ctx[i];
        if (dma_manager_request_resource(resource) == status_success) {
            dma_manager_install_interrupt_callback(resource, test_ctx->callback, &test_ctx->has_done);
            dma_manager_enable_dma_interrupt(resource, 1);
        }
    }
}

bool dma_manager_test_execute(void)
{
    bool result = true;
    dma_test_context *test_ctx;
    for (uint32_t i = 0; i < ARRAY_SIZE(s_dma_test_ctx); i++) {
        hpm_dma_resource_t *resource = &dma_resource_pools[i];
        test_ctx = &s_dma_test_ctx[i];

        uint32_t burst_len = 1UL << DMA_SOC_TRANSFER_WIDTH_MAX(resource->base);
        dma_start_memcpy(resource->base, resource->channel,
                (uint32_t)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, test_ctx->dst),
                (uint32_t)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, test_ctx->src),
                sizeof(test_ctx->src), burst_len);

    }

    for (uint32_t i = 0; i <  ARRAY_SIZE(s_dma_test_ctx); i++) {
        test_ctx = &s_dma_test_ctx[i];
        while (!test_ctx->has_done) {

        }
        if (memcmp(test_ctx->src, test_ctx->dst, sizeof(test_ctx->src)) != 0) {
            printf("DMA resource %d completed transfer, but error happened during data copy\n", i);
            result = false;
            break;
        } else {
            printf("DMA resource %d completed transfer, no errors\n", i);
        }
    }
    return result;
}

void dma_manager_test_teardown(void)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(dma_resource_pools); i++) {
        hpm_dma_resource_t *resource = &dma_resource_pools[i];
        dma_manager_disable_channel_interrupt(resource, 0xffff);
        dma_manager_release_resource(resource);
    }
}
