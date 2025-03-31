/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "hpm_ecat_phy.h"
#include "hpm_gpio_drv.h"

void ecat_phy_reset(void)
{
    gpio_write_pin(BOARD_ECAT_PHY0_RESET_GPIO, BOARD_ECAT_PHY0_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY0_RESET_PIN_INDEX, JL11X1_RESET_LEVEL);
#if BOARD_ECAT_SUPPORT_PORT1
    gpio_write_pin(BOARD_ECAT_PHY1_RESET_GPIO, BOARD_ECAT_PHY1_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY1_RESET_PIN_INDEX, JL11X1_RESET_LEVEL);
#endif
#if BOARD_ECAT_SUPPORT_PORT2
    gpio_write_pin(BOARD_ECAT_PHY2_RESET_GPIO, BOARD_ECAT_PHY2_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY2_RESET_PIN_INDEX, JL11X1_RESET_LEVEL);
#endif
    board_delay_ms(JL11X1_RESET_HOLD_TIME_MS);
    gpio_write_pin(BOARD_ECAT_PHY0_RESET_GPIO, BOARD_ECAT_PHY0_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY0_RESET_PIN_INDEX, !JL11X1_RESET_LEVEL);
#if BOARD_ECAT_SUPPORT_PORT1
    gpio_write_pin(BOARD_ECAT_PHY1_RESET_GPIO, BOARD_ECAT_PHY1_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY1_RESET_PIN_INDEX, !JL11X1_RESET_LEVEL);
#endif
#if BOARD_ECAT_SUPPORT_PORT2
    gpio_write_pin(BOARD_ECAT_PHY2_RESET_GPIO, BOARD_ECAT_PHY2_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY2_RESET_PIN_INDEX, !JL11X1_RESET_LEVEL);
#endif
    board_delay_ms(JL11X1_RESET_WAIT_TIME_MS);
}

void ecat_phy_keep_reset(void)
{
    gpio_write_pin(BOARD_ECAT_PHY0_RESET_GPIO, BOARD_ECAT_PHY0_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY0_RESET_PIN_INDEX, JL11X1_RESET_LEVEL);
#if BOARD_ECAT_SUPPORT_PORT1
    gpio_write_pin(BOARD_ECAT_PHY1_RESET_GPIO, BOARD_ECAT_PHY1_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY1_RESET_PIN_INDEX, JL11X1_RESET_LEVEL);
#endif
#if BOARD_ECAT_SUPPORT_PORT2
    gpio_write_pin(BOARD_ECAT_PHY2_RESET_GPIO, BOARD_ECAT_PHY2_RESET_GPIO_PORT_INDEX, BOARD_ECAT_PHY2_RESET_PIN_INDEX, JL11X1_RESET_LEVEL);
#endif
    board_delay_ms(JL11X1_RESET_HOLD_TIME_MS);
}

/* config PHY LED mode (JL1111 Page 7 Register 19, bits[5:4] to 0) */
static hpm_stat_t ecat_jl1111_phy_config_led_mode(ESC_Type *ptr, uint8_t phy_addr)
{
    hpm_stat_t stat;

    /* config LED mode to 00 */
    stat = esc_mdio_write(ptr, phy_addr, JL11X1_PAGESEL_REG_OFFSET, JL11X1_WOLEN_REG_OFFSET / 32);
    if (stat != status_success) {
        return stat;
    }

    stat = esc_mdio_write(ptr, phy_addr, JL11X1_WOLEN_REG_OFFSET % 32, JL11X1_WOLEN_REG_VALUE);
    if (stat != status_success) {
        return stat;
    }

    stat = esc_mdio_write(ptr, phy_addr, JL11X1_PAGESEL_REG_OFFSET, 0);
    if (stat != status_success) {
        return stat;
    }

    return status_success;
}

static hpm_stat_t ecat_jl1111_phy_disable_broadcast_response(ESC_Type *ptr, uint8_t phy_addr, uint8_t phy_offset)
{
    hpm_stat_t stat;

    /* foreach phyid which is not broadcast, disable broadcast */
    if (phy_addr + phy_offset > 0) {
        stat = esc_mdio_write(ptr, phy_addr, JL11X1_PAGESEL_REG_OFFSET, 128);
        if (stat != status_success) {
            return stat;
        }

        uint16_t value = (phy_addr + phy_offset) << 5 | 0x1F;
        stat = esc_mdio_write(ptr, phy_addr, 19, value);
        if (stat != status_success) {
            return stat;
        }

        stat = esc_mdio_write(ptr, phy_addr, JL11X1_PAGESEL_REG_OFFSET, 0);
        if (stat != status_success) {
            return stat;
        }
    }

    return status_success;
}

hpm_stat_t ecat_phy_config(ESC_Type *ptr)
{
    hpm_stat_t stat;
    stat = ecat_jl1111_phy_config_led_mode(ptr, BOARD_ECAT_PORT0_PHY_ADDR); /* config Port0 PHY LED mode  */
    if (stat != status_success) {
        return stat;
    }

    stat = ecat_jl1111_phy_disable_broadcast_response(ptr, BOARD_ECAT_PORT0_PHY_ADDR, BOARD_ECAT_PHY_ADDR_OFFSET); /* disable broadcast if phyid is not broadcast */
    if (stat != status_success) {
        return stat;
    }

#if defined(BOARD_ECAT_SUPPORT_PORT1) && BOARD_ECAT_SUPPORT_PORT1
    stat = ecat_jl1111_phy_config_led_mode(ptr, BOARD_ECAT_PORT1_PHY_ADDR); /* config Port1 PHY LED mode */
    if (stat != status_success) {
        return stat;
    }

    stat = ecat_jl1111_phy_disable_broadcast_response(ptr, BOARD_ECAT_PORT1_PHY_ADDR, BOARD_ECAT_PHY_ADDR_OFFSET); /* disable broadcast if phyid is not broadcast */
    if (stat != status_success) {
        return stat;
    }
#endif
#if defined(BOARD_ECAT_SUPPORT_PORT2) && BOARD_ECAT_SUPPORT_PORT2
    stat = ecat_jl1111_phy_config_led_mode(ptr, BOARD_ECAT_PORT2_PHY_ADDR);  /* config Port2 PHY LED mode */
    if (stat != status_success) {
        return stat;
    }

    stat = ecat_jl1111_phy_disable_broadcast_response(ptr, BOARD_ECAT_PORT2_PHY_ADDR, BOARD_ECAT_PHY_ADDR_OFFSET); /* disable broadcast if phyid is not broadcast */
    if (stat != status_success) {
        return stat;
    }
#endif
    return status_success;
}