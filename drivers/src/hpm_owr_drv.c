/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_owr_drv.h"

hpm_stat_t owr_sw_reset(OWR_Type *ptr)
{
    ptr->RESET |= OWR_RESET_SW_RESET_MASK;
    ptr->RESET &= ~OWR_RESET_SW_RESET_MASK;

    return status_success;
}

hpm_stat_t owr_init(OWR_Type *ptr, owr_config_t *config)
{
    uint32_t divider;

    /* OWR clock fixed at 1MHz */
    divider = config->clock_source_frequency / 1000000;

    if (divider < 3 || divider > 256) {
        return status_invalid_argument;
    }

    ptr->DIV_CFG = (ptr->DIV_CFG & ~OWR_DIV_CFG_DIV_CFG_MASK) | OWR_DIV_CFG_DIV_CFG_SET(divider - 1);
    ptr->GLB_CFG |= OWR_GLB_CFG_FORCE_CLK_ON_MASK;

    return status_success;
}

hpm_stat_t owr_reset_and_presence_pulses(OWR_Type *ptr)
{
    uint32_t timeout = OWR_RPP_TIMEOUT;

    ptr->CTRL |= OWR_CTRL_RPPBIT_MASK;

    while (OWR_CTRL_RPPBIT_GET(ptr->CTRL) && timeout--) {

    }

    if (timeout == 0) {
        return status_timeout;
    }

    return status_success;
}

hpm_stat_t owr_get_presence_status(OWR_Type *ptr, uint32_t *status)
{
    if (status == NULL) {
        return status_invalid_argument;
    }

    *status = OWR_CTRL_PSTBIT_GET(ptr->CTRL);

    return status_success;
}

hpm_stat_t owr_release_bus(OWR_Type *ptr)
{
    uint32_t status;
    uint32_t timeout = OWR_WR_TIMEOUT;

    owr_clear_irq_status(ptr, owr_irq_transmit_shift_register_empty);

    ptr->DATA = OWR_DATA_TXRX_DATA_SET(0xff);

    while (timeout--) {
        owr_get_irq_status(ptr, &status);
        if (status & owr_irq_transmit_shift_register_empty) {
            owr_clear_irq_status(ptr, owr_irq_transmit_shift_register_empty);
            ptr->DATA; /* dummy read */
            break;
        }
    }

    if (timeout == 0) {
        return status_timeout;
    }

    return status_success;
}

hpm_stat_t owr_write_data(OWR_Type *ptr, uint32_t data)
{
    uint32_t status;
    uint32_t timeout = OWR_WR_TIMEOUT;

    owr_clear_irq_status(ptr, owr_irq_transmit_shift_register_empty);

    ptr->DATA = OWR_DATA_TXRX_DATA_SET(data);

    while (timeout--) {
        owr_get_irq_status(ptr, &status);
        if (status & owr_irq_transmit_shift_register_empty) {
            owr_clear_irq_status(ptr, owr_irq_transmit_shift_register_empty);
            ptr->DATA; /* dummy read */
            break;
        }
    }

    if (timeout == 0) {
        return status_timeout;
    }

    return status_success;
}

hpm_stat_t owr_read_data(OWR_Type *ptr, uint32_t *data)
{
    uint32_t status;
    uint32_t timeout = OWR_WR_TIMEOUT;

    if (data == NULL) {
        return status_invalid_argument;
    }

    owr_release_bus(ptr);

    while (timeout--) {
        owr_get_irq_status(ptr, &status);
        if (status & owr_irq_receive_buff_full) {

            *data = OWR_DATA_TXRX_DATA_GET(ptr->DATA);
            owr_clear_irq_status(ptr, owr_irq_receive_buff_full);
            break;
        }
    }

    if (timeout == 0) {
        return status_timeout;
    }

    return status_success;
}

hpm_stat_t owr_clear_irq_status(OWR_Type *ptr, uint32_t mask)
{
    ptr->IRQ_STS = mask;

    return status_success;
}

hpm_stat_t owr_enable_interrupts(OWR_Type *ptr, uint32_t mask)
{
    ptr->IRQ_EN |= mask;

    return status_success;
}

hpm_stat_t owr_disable_interrupts(OWR_Type *ptr, uint32_t mask)
{
    ptr->IRQ_EN &= ~mask;

    return status_success;
}

hpm_stat_t owr_get_irq_status(OWR_Type *ptr, uint32_t *status)
{
    if (status == NULL) {
        return status_invalid_argument;
    }

    *status = ptr->IRQ_STS;

    return status_success;
}
