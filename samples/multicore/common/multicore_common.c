/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "hpm_l1c_drv.h"
#include "hpm_misc.h"
#include "hpm_sysctl_drv.h"
#include "hpm_romapi.h"
#include "hpm_clock_drv.h"
#include "multicore_common.h"

ATTR_PLACE_AT_NONCACHEABLE static sdp_dma_ctx_t s_dma_ctx;

void multicore_release_cpu(uint8_t cpu_id, uint32_t start_addr)
{
    sdp_dma_ctx_t *p_sdp_ctx = (sdp_dma_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_dma_ctx);
    uint32_t sec_core_app_sys_addr = core_local_mem_to_sys_address(cpu_id, (uint32_t)start_addr);
    uint32_t sec_core_img_sys_addr = core_local_mem_to_sys_address(0, (uint32_t)sec_core_img);
    uint32_t aligned_start;
    uint32_t aligned_end;
    uint32_t aligned_size;

    if (!sysctl_is_cpu_released(HPM_SYSCTL, cpu_id)) {
        printf("\nCopying secondary core image to destination memory: %#x\n", sec_core_img_sys_addr);
        clock_add_to_group(clock_sdp, 0);
        rom_sdp_memcpy(p_sdp_ctx, (void *)sec_core_app_sys_addr, (void *)sec_core_img_sys_addr, sec_core_img_size);
        if (l1c_ic_is_enabled() || l1c_dc_is_enabled()) {
            aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(sec_core_img_sys_addr);
            aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(sec_core_img_sys_addr + sec_core_img_size);
            aligned_size = aligned_end - aligned_start;
            l1c_dc_flush(aligned_start, aligned_size);
        }

        sysctl_set_cpu_entry(HPM_SYSCTL, cpu_id, (uint32_t)start_addr);
        sysctl_release_cpu(HPM_SYSCTL, cpu_id);
    }
}

