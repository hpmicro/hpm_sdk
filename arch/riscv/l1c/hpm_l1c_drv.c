/*
 * Copyright (c) 2021-2022,2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_l1c_drv.h"
#include <assert.h>


#define ASSERT_ADDR_SIZE(addr, size) do { \
                                             assert(address % HPM_L1C_CACHELINE_SIZE == 0); \
                                             assert(size % HPM_L1C_CACHELINE_SIZE == 0); \
                                        } while (0)

static void l1c_op(uint8_t opcode, uint32_t address, uint32_t size)
{
#if defined(HPM_SDK_L1C_NO_REENTRANT) && HPM_SDK_L1C_NO_REENTRANT
    register uint32_t i, tmp;
    for (i = 0, tmp = 0; tmp < size; i++) {
        l1c_cctl_address_cmd(opcode, address + i * HPM_L1C_CACHELINE_SIZE);
        tmp += HPM_L1C_CACHELINE_SIZE;
    }
#else
    register uint32_t next_address;
    next_address = address;
    l1c_cctl_address(next_address);
    while ((next_address < (address + size)) && (next_address >= address)) {
        l1c_cctl_cmd(opcode);
        next_address = l1c_cctl_get_address();
    }
#endif
}

void l1c_dc_enable(void)
{
    if (!l1c_dc_is_enabled()) {
#ifdef L1C_DC_DISABLE_WRITEAROUND_ON_ENABLE
        l1c_dc_disable_writearound();
#else
        l1c_dc_enable_writearound();
#endif
        set_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_DPREF_EN_MASK | HPM_MCACHE_CTL_DC_EN_MASK);
    }
}

void l1c_dc_disable(void)
{
    if (l1c_dc_is_enabled()) {
        clear_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_DC_EN_MASK);
    }
}

void l1c_ic_enable(void)
{
    if (!l1c_ic_is_enabled()) {
        set_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_IPREF_EN_MASK
                              | HPM_MCACHE_CTL_CCTL_SUEN_MASK
                              | HPM_MCACHE_CTL_IC_EN_MASK);
    }
}

void l1c_ic_disable(void)
{
    if (l1c_ic_is_enabled()) {
        clear_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_IC_EN_MASK);
    }
}

void l1c_fence_i(void)
{
    __asm("fence.i");
}

void l1c_dc_invalidate_all(void)
{
    l1c_cctl_cmd(HPM_L1C_CCTL_CMD_L1D_INVAL_ALL);
}

void l1c_dc_writeback_all(void)
{
    l1c_cctl_cmd(HPM_L1C_CCTL_CMD_L1D_WB_ALL);
}

void l1c_dc_flush_all(void)
{
    l1c_cctl_cmd(HPM_L1C_CCTL_CMD_L1D_WBINVAL_ALL);
}

void l1c_dc_fill_lock(uint32_t address, uint32_t size)
{
    ASSERT_ADDR_SIZE(address, size);
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_LOCK, address, size);
}

void l1c_dc_unlock(uint32_t address, uint32_t size)
{
    ASSERT_ADDR_SIZE(address, size);
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_UNLOCK, address, size);
}

void l1c_dc_invalidate(uint32_t address, uint32_t size)
{
    ASSERT_ADDR_SIZE(address, size);
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_INVAL, address, size);
}

void l1c_dc_writeback(uint32_t address, uint32_t size)
{
    ASSERT_ADDR_SIZE(address, size);
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_WB, address, size);
}

void l1c_dc_flush(uint32_t address, uint32_t size)
{
    ASSERT_ADDR_SIZE(address, size);
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_WBINVAL, address, size);
}

void l1c_ic_invalidate(uint32_t address, uint32_t size)
{
    ASSERT_ADDR_SIZE(address, size);
    l1c_op(HPM_L1C_CCTL_CMD_L1I_VA_INVAL, address, size);
}

void l1c_ic_fill_lock(uint32_t address, uint32_t size)
{
    ASSERT_ADDR_SIZE(address, size);
    l1c_op(HPM_L1C_CCTL_CMD_L1I_VA_LOCK, address, size);
}

void l1c_ic_unlock(uint32_t address, uint32_t size)
{
    ASSERT_ADDR_SIZE(address, size);
    l1c_op(HPM_L1C_CCTL_CMD_L1I_VA_UNLOCK, address, size);
}

void l1c_dc_enable_writearound(void)
{
    set_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_DC_WAROUND_MASK);
}

void l1c_dc_disable_writearound(void)
{
    clear_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_DC_WAROUND_MASK);
}
