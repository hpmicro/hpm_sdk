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
static hpm_stat_t ecat_jl1111_phy_config(ESC_Type *ptr, uint8_t phy_addr)
{
    hpm_stat_t stat;

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

hpm_stat_t ecat_phy_config(ESC_Type *ptr)
{
    hpm_stat_t stat;
    stat = ecat_jl1111_phy_config(ptr, 0); /* PHY address 0 */
    if (stat != status_success) {
        return stat;
    }
#if defined(BOARD_ECAT_SUPPORT_PORT1) && BOARD_ECAT_SUPPORT_PORT1
    stat = ecat_jl1111_phy_config(ptr, 1); /* PHY address 1 */
    if (stat != status_success) {
        return stat;
    }
#endif
#if defined(BOARD_ECAT_SUPPORT_PORT2) && BOARD_ECAT_SUPPORT_PORT2
    stat = ecat_jl1111_phy_config(ptr, 2);  /* PHY address 2 */
    if (stat != status_success) {
        return stat;
    }
#endif
    return status_success;
}