/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_i2c.h"
#include "hpm_clock_drv.h"
#include <stdlib.h>

typedef struct {
    I2C_Type *i2c_ptr;
    clock_name_t i2c_clock_name;
#if USE_I2C_DMA_MGR
    uint8_t dmamux_src;
    dma_resource_t dma_resource;
    hpm_i2c_dma_complete_cb dma_complete;
#endif
} hpm_i2c_cfg_t;

static hpm_i2c_cfg_t i2c_cfg_table[] = {
#if defined(HPM_I2C0)
    {
        .i2c_ptr = HPM_I2C0,
        .i2c_clock_name = clock_i2c0,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C0,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C1)
    {
        .i2c_ptr = HPM_I2C1,
        .i2c_clock_name = clock_i2c1,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C1,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C2)
    {
        .i2c_ptr = HPM_I2C2,
        .i2c_clock_name = clock_i2c2,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C2,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C3)
    {
        .i2c_ptr = HPM_I2C3,
        .i2c_clock_name = clock_i2c3,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C3,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C4)
    {
        .i2c_ptr = HPM_I2C4,
        .i2c_clock_name = clock_i2c4,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C4,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C5)
    {
        .i2c_ptr = HPM_I2C5,
        .i2c_clock_name = clock_i2c5,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C5,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C6)
    {
        .i2c_ptr = HPM_I2C6,
        .i2c_clock_name = clock_i2c6,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C6,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C7)
    {
        .i2c_ptr = HPM_I2C7,
        .i2c_clock_name = clock_i2c7,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C7,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C8)
    {
        .i2c_ptr = HPM_I2C8,
        .i2c_clock_name = clock_i2c8,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C8,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C9)
    {
        .i2c_ptr = HPM_I2C9,
        .i2c_clock_name = clock_i2c9,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C9,
        .dma_complete = NULL,
#endif
    },
#endif

#if defined(HPM_I2C10)
    {
        .i2c_ptr = HPM_I2C10,
        .i2c_clock_name = clock_i2c10,
#if USE_I2C_DMA_MGR
        .dmamux_src = HPM_DMA_SRC_I2C10,
        .dma_complete = NULL,
#endif
    },
#endif
};

static hpm_i2c_cfg_t *hpm_i2c_get_cfg_obj(I2C_Type *ptr)
{
    hpm_i2c_cfg_t *obj;
    uint8_t i = 0;
    for (i = 0; i < (sizeof(i2c_cfg_table) / sizeof(hpm_i2c_cfg_t)); i++) {
        obj = &i2c_cfg_table[i];
        if (obj->i2c_ptr == ptr) {
            return obj;
        }
    }
    return NULL;
}


static void hpm_i2c_master_phase_config(I2C_Type *ptr, uint16_t device_addr, uint8_t flags, uint32_t transfer_size, bool enable_dma)
{
    if (flags & I2C_NO_ADDRESS) {
        i2c_master_disable_addr_phase(ptr);
    } else {
        i2c_master_enable_addr_phase(ptr);
        i2c_master_set_slave_address(ptr, device_addr);
    }
    if (flags & I2C_RD) {
        i2c_set_direction(ptr, I2C_DIR_MASTER_READ);
    } else {
        i2c_set_direction(ptr, I2C_DIR_MASTER_WRITE);
    }
    /* start signal */
    if (flags & I2C_NO_START) {
        i2c_master_disable_start_phase(ptr);
    } else {
        i2c_master_enable_start_phase(ptr);
    }
    /* end signal*/
    if (flags & I2C_NO_STOP) {
        i2c_master_disable_stop_phase(ptr);
    } else {
        i2c_master_enable_stop_phase(ptr);
    }
    if (transfer_size > 0) {
        i2c_master_enable_data_phase(ptr);
        i2c_set_data_count(ptr, transfer_size);
    } else {
        i2c_master_disable_data_phase(ptr);
    }
    if (enable_dma) {
        i2c_dma_enable(ptr);
    } else {
        i2c_dma_disable(ptr);
    }
}

static void hpm_i2c_release_bus(I2C_Type *ptr)
{
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    hpm_i2c_master_phase_config(ptr, 0, I2C_NO_ADDRESS | I2C_NO_START, 0, false);
    i2c_master_issue_data_transmission(ptr);
}

static void hpm_i2c_get_default_init_config(hpm_i2c_initialize_config_t *config)
{
    config->is_10bit_addressing = false;
    config->communication_mode = i2c_master;
    config->speed = i2c_speed_100khz;
}

void hpm_i2c_get_default_init_context(hpm_i2c_context_t *context)
{
    context->addr_endianness = i2c_master_addr_little_endian;
    hpm_i2c_get_default_init_config(&context->init_config);
}

hpm_stat_t hpm_i2c_initialize(hpm_i2c_context_t *context)
{
    hpm_stat_t stat = status_success;
    i2c_config_t cfg;
    uint32_t freq;
    hpm_i2c_initialize_config_t *config = &context->init_config;
    I2C_Type *ptr = context->base;
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(ptr);
    if (obj == NULL) {
        return status_invalid_argument;
    }
    switch (config->speed) {
    case i2c_speed_100khz:
        cfg.i2c_mode = i2c_mode_normal;
        break;
    case i2c_speed_400khz:
        cfg.i2c_mode = i2c_mode_fast;
        break;
    case i2c_speed_1Mhz:
        cfg.i2c_mode = i2c_mode_fast_plus;
        break;
    default:
        stat = status_invalid_argument;
        break;
    }
    if (stat != status_success) {
        return stat;
    }
    cfg.is_10bit_addressing = config->is_10bit_addressing;
    freq = clock_get_frequency(obj->i2c_clock_name);
    if (config->communication_mode == i2c_master) {
        stat = i2c_init_master(obj->i2c_ptr, freq, &cfg);
    } else {
        stat = i2c_init_slave(obj->i2c_ptr, freq, &cfg, config->slave_address);
    }
    return stat;
}

hpm_stat_t hpm_i2c_master_probe_slave_address(hpm_i2c_context_t *context, uint16_t device_address)
{
    I2C_Type *ptr = context->base;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    uint64_t expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL;
    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_master_issue_data_transmission(ptr);
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_WR, 0, false);
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);
    return status_success;
}


hpm_stat_t hpm_i2c_master_addr_write_blocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size, uint32_t timeout)
{
    uint32_t left;
    uint8_t *p = (uint8_t *)&addr;
    I2C_Type *ptr = context->base;

    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;

    if (((addr_size == 0) || (addr_size > sizeof(uint32_t))) ||
        ((buf_size == 0) || (buf_size > I2C_SOC_TRANSFER_COUNT_MAX)) ||
        ((addr_size + buf_size) > I2C_SOC_TRANSFER_COUNT_MAX)) {
        return status_invalid_argument;
    }

    while (i2c_get_status(ptr) & I2C_STATUS_BUSBUSY_MASK) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            return status_timeout;
        }
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_WR, (addr_size + buf_size), false);

    left = addr_size;
    while (left) {
        if (context->addr_endianness == i2c_master_addr_big_endian) {
            i2c_write_byte(ptr, p[left - 1]);
        } else {
            i2c_write_byte(ptr, *(p++));
        }
        left--;
    }
    i2c_master_issue_data_transmission(ptr);

   /* Before starting to transmit data, judge addrhit to ensure that the slave address exists on the bus. */
   /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL; /* 500Us */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);

    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    left = buf_size;
    while (left) {
        if (!(i2c_get_status(ptr) & I2C_STATUS_FIFOFULL_MASK)) {
            i2c_write_byte(ptr, *(buf++));
            left--;
        } else {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                hpm_i2c_release_bus(ptr);
                return status_timeout;
            }
        }
    }

    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    while (!(i2c_get_status(ptr) & I2C_STATUS_CMPL_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_timeout;
        }
    }
    return status_success;
}

hpm_stat_t hpm_i2c_master_addr_read_blocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size, uint32_t timeout)
{
    uint32_t left;
    uint8_t *p = (uint8_t *)&addr;
    I2C_Type *ptr = context->base;

    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;

    if (((addr_size == 0) || (addr_size > sizeof(uint32_t))) ||
        ((buf_size == 0) || (buf_size > I2C_SOC_TRANSFER_COUNT_MAX)) ||
        ((addr_size + buf_size) > I2C_SOC_TRANSFER_COUNT_MAX)) {
        return status_invalid_argument;
    }

    while (i2c_get_status(ptr) & I2C_STATUS_BUSBUSY_MASK) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            return status_timeout;
        }
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_WR | I2C_NO_STOP, addr_size, false);

    left = addr_size;
    while (left) {
        if (context->addr_endianness == i2c_master_addr_big_endian) {
            i2c_write_byte(ptr, p[left - 1]);
        } else {
            i2c_write_byte(ptr, *(p++));
        }
        left--;
    }
    i2c_master_issue_data_transmission(ptr);

   /* Before starting to transmit data, judge addrhit to ensure that the slave address exists on the bus. */
   /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL; /* 500Us */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    while (!(i2c_get_status(ptr) & I2C_STATUS_CMPL_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_timeout;
        }
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_RD, buf_size, false);
    i2c_master_issue_data_transmission(ptr);

    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    left = buf_size;
    while (left) {
        if (!(i2c_get_status(ptr) & I2C_STATUS_FIFOEMPTY_MASK)) {
            *(buf++) = i2c_read_byte(ptr);
            left--;
        } else {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                hpm_i2c_release_bus(ptr);
                return status_timeout;
            }
        }
    }
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    while (!(i2c_get_status(ptr) & I2C_STATUS_CMPL_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_timeout;
        }
    }
    return status_success;
}

hpm_stat_t hpm_i2c_master_write_blocking(hpm_i2c_context_t *context, uint16_t device_address,
                            uint8_t *buf, uint32_t size, uint32_t timeout)
{
    hpm_stat_t stat = status_success;
    uint32_t left;

    I2C_Type *ptr = context->base;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;

    if (size > I2C_SOC_TRANSFER_COUNT_MAX) {
        return status_invalid_argument;
    }

    while (i2c_get_status(ptr) & I2C_STATUS_BUSBUSY_MASK) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            return status_timeout;
        }
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_WR, size, false);

    i2c_master_issue_data_transmission(ptr);

   /* Before starting to transmit data, judge addrhit to ensure that the slave address exists on the bus. */
   /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL; /* 500Us */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);
    /* when size is zero, it's probe slave device, so directly return success */
    if (size == 0) {
        return status_success;
    }

    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    left = size;
    while (left) {
        if (!(i2c_get_status(ptr) & I2C_STATUS_FIFOFULL_MASK)) {
            i2c_write_byte(ptr, *(buf++));
            left--;
        } else {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                hpm_i2c_release_bus(ptr);
                return status_i2c_no_addr_hit;
            }
        }
    }

    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    while (!(i2c_get_status(ptr) & I2C_STATUS_CMPL_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_timeout;
        }
    }

    if (i2c_get_data_count(ptr) && (size)) {
        return status_i2c_transmit_not_completed;
    }

    return stat;
}

hpm_stat_t hpm_i2c_master_read_blocking(hpm_i2c_context_t *context, const uint16_t device_address,
                           uint8_t *buf, const uint32_t size, uint32_t timeout)
{
    hpm_stat_t stat = status_success;
    uint32_t left;

    I2C_Type *ptr = context->base;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;

    if (size > I2C_SOC_TRANSFER_COUNT_MAX) {
        return status_invalid_argument;
    }

    while (i2c_get_status(ptr) & I2C_STATUS_BUSBUSY_MASK) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            return status_timeout;
        }
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_RD, size, false);
    i2c_master_issue_data_transmission(ptr);

   /* Before starting to transmit data, judge addrhit to ensure that the slave address exists on the bus. */
   /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL; /* 500Us */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);
    /* when size is zero, it's probe slave device, so directly return success */
    if (size == 0) {
        return status_success;
    }

    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    left = size;
    while (left) {
        if (!(i2c_get_status(ptr) & I2C_STATUS_FIFOEMPTY_MASK)) {
            *(buf++) = i2c_read_byte(ptr);
            left--;
        } else {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                hpm_i2c_release_bus(ptr);
                return status_timeout;
            }
        }
    }

    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    while (!(i2c_get_status(ptr) & I2C_STATUS_CMPL_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_timeout;
        }
    }

    if (i2c_get_data_count(ptr) && (size)) {
        return status_i2c_transmit_not_completed;
    }

    return stat;
}

hpm_stat_t hpm_i2c_slave_write_blocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size, uint32_t timeout)
{
    uint32_t left;
    I2C_Type *ptr = context->base;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    if (((size == 0) || (size > I2C_SOC_TRANSFER_COUNT_MAX))) {
        return status_invalid_argument;
    }

    /* wait for address hit */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            return status_timeout;
        }
    }
    i2c_dma_disable(ptr);
    /* W1C, clear CMPL bit and clear ADDRHIT bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK | I2C_STATUS_ADDRHIT_MASK);
    i2c_clear_fifo(ptr);
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    left = size;
    while (left) {
        if (!(i2c_get_status(ptr) & I2C_STATUS_FIFOFULL_MASK)) {
            i2c_write_byte(ptr, *(buf++));
            left--;
        } else {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                return status_timeout;
            }
        }
    }
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    while (!(i2c_get_status(ptr) & I2C_STATUS_CMPL_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            return status_timeout;
        }
    }
    /* clear status, CMPL must to be cleared at slave mode before next transaction */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);

    if (i2c_get_data_count(ptr) != size) {
        return status_i2c_transmit_not_completed;
    }

    return status_success;
}

hpm_stat_t hpm_i2c_slave_read_blocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size, uint32_t timeout)
{
    uint32_t left;
    I2C_Type *ptr = context->base;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    if (((size == 0) || (size > I2C_SOC_TRANSFER_COUNT_MAX))) {
        return status_invalid_argument;
    }

    /* wait for address hit */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            return status_timeout;
        }
    }
    i2c_dma_disable(ptr);
    /* W1C, clear CMPL bit and clear ADDRHIT bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK | I2C_STATUS_ADDRHIT_MASK);
    i2c_clear_fifo(ptr);
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    left = size;
    while (left) {
        if (!(i2c_get_status(ptr) & I2C_STATUS_FIFOEMPTY_MASK)) {
            *(buf++) = i2c_read_byte(ptr);
            left--;
        } else {
            if (hpm_csr_get_core_cycle() > expected_ticks) {
                return status_i2c_no_addr_hit;
            }
        }
    }
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 1000UL * timeout;
    while (!(i2c_get_status(ptr) & I2C_STATUS_CMPL_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            return status_timeout;
        }
    }
    /* clear status, CMPL must to be cleared at slave mode before next transaction */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);

    if (i2c_get_data_count(ptr) != size) {
        return status_i2c_transmit_not_completed;
    }

    return status_success;
}

#if USE_I2C_DMA_MGR
void hpm_i2c_dma_channel_tc_callback(DMA_Type *ptr, uint32_t channel, void *user_data)
{
    hpm_i2c_cfg_t *obj = (hpm_i2c_cfg_t *)user_data;
    if ((obj->dma_resource.channel == channel) &&
        (obj->dma_resource.base == ptr) &&
        (obj->dma_complete != NULL)) {
        obj->dma_complete(channel);
    }
}

static void hpm_i2c_read_trigger_dma(hpm_i2c_cfg_t *obj, uint32_t addr, uint32_t size)
{
    dma_mgr_set_chn_dst_addr(&obj->dma_resource, addr);
    dma_mgr_set_chn_dst_work_mode(&obj->dma_resource, DMA_MGR_HANDSHAKE_MODE_NORMAL);
    dma_mgr_set_chn_dst_addr_ctrl(&obj->dma_resource, DMA_MGR_ADDRESS_CONTROL_INCREMENT);
    dma_mgr_set_chn_src_addr(&obj->dma_resource, (uint32_t)&obj->i2c_ptr->DATA);
    dma_mgr_set_chn_src_work_mode(&obj->dma_resource, DMA_MGR_HANDSHAKE_MODE_HANDSHAKE);
    dma_mgr_set_chn_src_addr_ctrl(&obj->dma_resource, DMA_MGR_ADDRESS_CONTROL_FIXED);
    dma_mgr_set_chn_transize(&obj->dma_resource, size);
    dma_mgr_enable_channel(&obj->dma_resource);
}

static void hpm_i2c_write_trigger_dma(hpm_i2c_cfg_t *obj, uint32_t addr, uint32_t size)
{
    dma_mgr_set_chn_src_addr(&obj->dma_resource, addr);
    dma_mgr_set_chn_src_work_mode(&obj->dma_resource, DMA_MGR_HANDSHAKE_MODE_NORMAL);
    dma_mgr_set_chn_src_addr_ctrl(&obj->dma_resource, DMA_MGR_ADDRESS_CONTROL_INCREMENT);
    dma_mgr_set_chn_dst_addr(&obj->dma_resource, (uint32_t)&obj->i2c_ptr->DATA);
    dma_mgr_set_chn_dst_work_mode(&obj->dma_resource, DMA_MGR_HANDSHAKE_MODE_HANDSHAKE);
    dma_mgr_set_chn_dst_addr_ctrl(&obj->dma_resource, DMA_MGR_ADDRESS_CONTROL_FIXED);
    dma_mgr_set_chn_transize(&obj->dma_resource, size);
    dma_mgr_enable_channel(&obj->dma_resource);
}

hpm_stat_t hpm_i2c_dma_mgr_install_callback(hpm_i2c_context_t *context, hpm_i2c_dma_complete_cb complete)
{
    dma_mgr_chn_conf_t chg_config;
    I2C_Type *ptr = context->base;
    dma_resource_t *resource = NULL;
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(ptr);
    if (obj == NULL) {
        return status_invalid_argument;
    }
    dma_mgr_get_default_chn_config(&chg_config);
    chg_config.src_width = DMA_MGR_TRANSFER_WIDTH_BYTE;
    chg_config.dst_width = DMA_MGR_TRANSFER_WIDTH_BYTE;
    /* i2c dma config */
    resource = &obj->dma_resource;
    if (dma_mgr_request_resource(resource) == status_success) {
        chg_config.src_mode = DMA_MGR_HANDSHAKE_MODE_HANDSHAKE;
        chg_config.src_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_FIXED;
        chg_config.src_addr = (uint32_t)&ptr->DATA;
        chg_config.dst_mode = DMA_MGR_HANDSHAKE_MODE_NORMAL;
        chg_config.dst_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_INCREMENT;
        chg_config.en_dmamux = true;
        chg_config.dmamux_src = obj->dmamux_src;
        dma_mgr_setup_channel(resource, &chg_config);
        dma_mgr_install_chn_tc_callback(resource, hpm_i2c_dma_channel_tc_callback, (void *)obj);
        dma_mgr_enable_chn_irq(resource, DMA_MGR_INTERRUPT_MASK_TC);
        dma_mgr_enable_dma_irq_with_priority(resource, 1);
        obj->dma_complete = complete;
    }
    return status_success;
}

hpm_stat_t hpm_i2c_slave_read_nonblocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size)
{
    hpm_stat_t stat = status_success;
    uint32_t buf_addr;
    I2C_Type *ptr = context->base;
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(ptr);
    if ((obj == NULL) || (size > I2C_SOC_TRANSFER_COUNT_MAX)) {
        return status_invalid_argument;
    }
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_dma_disable(ptr);
    buf_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buf);
    i2c_set_data_count(ptr, size);
    i2c_dma_enable(ptr);
    hpm_i2c_read_trigger_dma(obj, buf_addr, size);
    return stat;
}

hpm_stat_t hpm_i2c_slave_write_nonblocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size)
{
    hpm_stat_t stat = status_success;
    uint32_t buf_addr;
    I2C_Type *ptr = context->base;
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(ptr);
    if ((obj == NULL) || (size > I2C_SOC_TRANSFER_COUNT_MAX)) {
        return status_invalid_argument;
    }
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_dma_disable(ptr);
    buf_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buf);
    hpm_i2c_write_trigger_dma(obj, buf_addr, size);
    i2c_set_data_count(ptr, size);
    i2c_dma_enable(ptr);
    return stat;
}

hpm_stat_t hpm_i2c_master_addr_write_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size)
{
    hpm_stat_t stat = status_success;
    uint32_t left;
    I2C_Type *ptr = context->base;
    uint8_t *p = (uint8_t *)&addr;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = 0;
    uint32_t buf_addr;
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(ptr);
    if (((addr_size == 0) || (addr_size > sizeof(uint32_t))) ||
        ((buf_size == 0) || (buf_size > I2C_SOC_TRANSFER_COUNT_MAX)) ||
        ((addr_size + buf_size) > I2C_SOC_TRANSFER_COUNT_MAX) ||
        (obj == NULL)) {
        return status_invalid_argument;
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_WR, (addr_size + buf_size), false);

    left = addr_size;
    while (left) {
        if (context->addr_endianness == i2c_master_addr_big_endian) {
            i2c_write_byte(ptr, p[left - 1]);
        } else {
            i2c_write_byte(ptr, *(p++));
        }
        left--;
    }
    i2c_master_issue_data_transmission(ptr);

   /* Before starting to transmit data, judge addrhit to ensure that the slave address exists on the bus. */
   /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL; /* 500Us */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);
    buf_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buf);
    hpm_i2c_write_trigger_dma(obj, buf_addr, buf_size);
    i2c_dma_enable(ptr);
    return stat;
}

hpm_stat_t hpm_i2c_master_addr_read_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size)
{
    hpm_stat_t stat = status_success;
    uint32_t left;
    I2C_Type *ptr = context->base;
    uint8_t *p = (uint8_t *)&addr;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = 0;
    uint32_t buf_addr;
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(ptr);
    if (((addr_size == 0) || (addr_size > sizeof(uint32_t))) ||
        ((buf_size == 0) || (buf_size > I2C_SOC_TRANSFER_COUNT_MAX)) ||
        ((addr_size + buf_size) > I2C_SOC_TRANSFER_COUNT_MAX) ||
        (obj == NULL)) {
        return status_invalid_argument;
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_WR | I2C_NO_STOP, (addr_size), false);
    left = addr_size;
    while (left) {
        if (context->addr_endianness == i2c_master_addr_big_endian) {
            i2c_write_byte(ptr, p[left - 1]);
        } else {
            i2c_write_byte(ptr, *(p++));
        }
        left--;
    }
    i2c_master_issue_data_transmission(ptr);

   /* Before starting to transmit data, judge addrhit to ensure that the slave address exists on the bus. */
   /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL; /* 500Us */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);

    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL;
    while (!(i2c_get_status(ptr) & I2C_STATUS_CMPL_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_timeout;
        }
    }
    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);

    buf_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buf);
    hpm_i2c_read_trigger_dma(obj, buf_addr, buf_size);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_RD, buf_size, true);
    i2c_master_issue_data_transmission(ptr);
    return stat;
}


hpm_stat_t hpm_i2c_master_write_nonblocking(hpm_i2c_context_t *context, uint16_t device_address,
                            uint8_t *buf, uint32_t size)
{
    hpm_stat_t stat = status_success;
    uint32_t buf_addr;
    I2C_Type *ptr = context->base;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = 0;
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(ptr);
    if ((size > I2C_SOC_TRANSFER_COUNT_MAX) || (obj == NULL)) {
        return status_invalid_argument;
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_WR, size, false);

    i2c_master_issue_data_transmission(ptr);

   /* Before starting to transmit data, judge addrhit to ensure that the slave address exists on the bus. */
   /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL; /* 500Us */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);
    i2c_dma_enable(ptr);
    buf_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buf);
    hpm_i2c_write_trigger_dma(obj, buf_addr, size);
    return stat;
}

hpm_stat_t hpm_i2c_master_read_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address,
                           uint8_t *buf, const uint32_t size)
{
    hpm_stat_t stat = status_success;
    uint32_t buf_addr;
    I2C_Type *ptr = context->base;
    uint32_t ticks_per_us = clock_get_core_clock_ticks_per_us();
    uint64_t expected_ticks = 0;
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(ptr);
    if ((size > I2C_SOC_TRANSFER_COUNT_MAX) || (obj == NULL)) {
        return status_invalid_argument;
    }

    /* W1C, clear CMPL bit to avoid blocking the transmission */
    i2c_clear_status(ptr, I2C_STATUS_CMPL_MASK);
    i2c_clear_fifo(ptr);
    hpm_i2c_master_phase_config(ptr, device_address, I2C_RD, size, false);
    i2c_master_issue_data_transmission(ptr);

   /* Before starting to transmit data, judge addrhit to ensure that the slave address exists on the bus. */
   /* i2c speed min is 100Kbps, and mem address max is 4 byte, 10us * (4 * 8) = 320us,  so 500us is enough */
    expected_ticks = hpm_csr_get_core_cycle() + (uint64_t)ticks_per_us * 500UL; /* 500Us */
    while (!(i2c_get_status(ptr) & I2C_STATUS_ADDRHIT_MASK)) {
        if (hpm_csr_get_core_cycle() > expected_ticks) {
            hpm_i2c_release_bus(ptr);
            return status_i2c_no_addr_hit;
        }
    }
    i2c_clear_status(ptr, I2C_STATUS_ADDRHIT_MASK);
    i2c_dma_enable(ptr);
    buf_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)buf);
    hpm_i2c_read_trigger_dma(obj, buf_addr, size);
    return stat;
}
dma_resource_t *hpm_i2c_get_dma_mgr_resource(hpm_i2c_context_t *context)
{
    hpm_i2c_cfg_t *obj = hpm_i2c_get_cfg_obj(context->base);
    if (obj != NULL) {
        return &obj->dma_resource;
    }
    return NULL;
}

#endif

