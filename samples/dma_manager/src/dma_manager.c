/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_dma_mgr.h"

#define DMA_RESOURCE_NUM (DMA_SOC_CHANNEL_NUM * DMA_SOC_MAX_COUNT)

dma_resource_t dma_resource_pools[DMA_RESOURCE_NUM];

typedef struct {
    uint8_t src[256];
    uint8_t dst[256];
    dma_mgr_chn_cb_t tc_callback;
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
    board_init();
    board_init_led_pins();

    show_description();

    dma_mgr_init();

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

void dma_channel_tc_callback(DMA_Type *ptr, uint32_t channel, void *user_data)
{
    (void)ptr;
    (void)channel;
    *(volatile bool *)user_data = true;
}

void show_description(void)
{
    static const char desc[] = "\n"
                        "=============================================================================\n"
                        "\n\n"
                        "    This example demonstrates the following features with DMA manager\n"
                        "\n"
                        "      1. Request/release DMA resource\n"
                        "      2. Setup DMA config\n"
                        "      3. Enable DMA interrupt\n"
                        "      4. DMA callback installation\n"
                        "\n\n"
                        "=============================================================================\n";
     printf("%s", desc);
}

void dma_manager_test_setup(void)
{
    dma_test_context *test_ctx;
    dma_mgr_chn_conf_t chg_config;

    for (uint32_t i = 0; i < ARRAY_SIZE(s_dma_test_ctx); i++) {
        test_ctx = &s_dma_test_ctx[i];
        memset(test_ctx, 0, sizeof(*test_ctx));
        test_ctx->tc_callback = dma_channel_tc_callback;
        for (uint32_t src_buf_idx = 0; src_buf_idx < ARRAY_SIZE(test_ctx->src); src_buf_idx++) {
            test_ctx->src[src_buf_idx] = src_buf_idx & 0xFF;
        }
    }

    for (uint32_t i = 0; i < ARRAY_SIZE(dma_resource_pools); i++) {
        dma_resource_t *resource = &dma_resource_pools[i];
        test_ctx = &s_dma_test_ctx[i];
        if (dma_mgr_request_resource(resource) == status_success) {
            dma_mgr_get_default_chn_config(&chg_config);
            chg_config.src_width = DMA_MGR_TRANSFER_WIDTH_WORD;
            chg_config.dst_width = DMA_MGR_TRANSFER_WIDTH_WORD;
            chg_config.src_addr = (uint32_t)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)test_ctx->src);
            chg_config.dst_addr = (uint32_t)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)test_ctx->dst);
            chg_config.size_in_byte = sizeof(test_ctx->src);
            dma_mgr_setup_channel(resource, &chg_config);

            dma_mgr_install_chn_tc_callback(resource, test_ctx->tc_callback, (void *)&test_ctx->has_done);
            dma_mgr_enable_chn_irq(resource, DMA_MGR_INTERRUPT_MASK_TC);
            dma_mgr_enable_dma_irq_with_priority(resource, 1);
        }
    }
}

bool dma_manager_test_execute(void)
{
    bool result = true;
    dma_test_context *test_ctx;

    for (uint32_t i = 0; i < ARRAY_SIZE(s_dma_test_ctx); i++) {
        dma_resource_t *resource = &dma_resource_pools[i];
        test_ctx = &s_dma_test_ctx[i];
        dma_mgr_enable_channel(resource);
        while (!test_ctx->has_done) {
            ;
        }
        if (memcmp(test_ctx->src, test_ctx->dst, sizeof(test_ctx->src)) != 0) {
            printf("DMA resource %d completed transfer, but error happened during data copy\n", i);
            result = false;
            break;
        } else {
            printf("DMA resource %d completed transfer, success\n", i);
        }
    }

    return result;
}

void dma_manager_test_teardown(void)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(dma_resource_pools); i++) {
        dma_resource_t *resource = &dma_resource_pools[i];
        dma_mgr_disable_dma_irq(resource);
        dma_mgr_disable_chn_irq(resource, DMA_MGR_INTERRUPT_MASK_ALL);
        dma_mgr_release_resource(resource);
    }
}
