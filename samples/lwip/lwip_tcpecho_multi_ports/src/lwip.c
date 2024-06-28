/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "common.h"
#include "netconf.h"
#include "sys_arch.h"
#include "lwip.h"
#include "lwip/init.h"
#include "tcp_echo.h"

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag[BOARD_ENET_COUNT];
#endif

typedef struct {
    enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT];
    enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT];
    uint8_t        rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE];
    uint8_t        tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE];
} enet_desc_init_t;

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
enet_desc_init_t desc_init[BOARD_ENET_COUNT];
enet_desc_t desc[BOARD_ENET_COUNT];
uint8_t mac[BOARD_ENET_COUNT][ENET_MAC];

struct netif gnetif[BOARD_ENET_COUNT];

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
static void enet_desc_init(enet_desc_t *pdesc, enet_desc_init_t *pdesc_init)
{
    pdesc->tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->dma_tx_desc_tab);
    pdesc->rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->dma_rx_desc_tab);

    pdesc->tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->tx_buff);
    pdesc->tx_buff_cfg.count  = ENET_TX_BUFF_COUNT;
    pdesc->tx_buff_cfg.size   = ENET_TX_BUFF_SIZE;

    pdesc->rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->rx_buff);
    pdesc->rx_buff_cfg.count  = ENET_RX_BUFF_COUNT;
    pdesc->rx_buff_cfg.size   = ENET_RX_BUFF_SIZE;
}

static hpm_stat_t enet_init(uint8_t idx)
{
    enet_mac_config_t        enet_config;
    enet_tx_control_config_t enet_tx_control_config;
    enet_int_config_t        int_config = {0};
    enet_base_t              *base;
    enet_inf_type_t          itf;

    if (idx > BOARD_ENET_COUNT) {
        return status_invalid_argument;
    }

    base = board_get_enet_base(idx);
    itf  = board_get_enet_phy_itf(idx);

    /* Initialize td, rd and the corresponding buffers */
    enet_desc_init(&desc[idx], &desc_init[idx]);

    /* Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(base, &enet_tx_control_config);

    /* Set the control config for tx descriptor */
    memcpy(&desc[idx].tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Get a default MAC address */
    enet_get_mac_address(idx, mac[idx]);

    /* Set MAC0 address */
    enet_set_mac_address(&enet_config, mac[idx]);

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(base);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    /* Enable Enet IRQ */
    board_enable_enet_irq(base);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ENET, &int_config);
    #endif

    /* Initialize Enet MAC */
    if (enet_controller_init(base, itf, &desc[idx], &enet_config, &int_config) != status_success) {
        printf("Enet%d MAC init failed!\n", idx);
        return status_fail;
    }

    /* Initialize Enet PHY */
    if (board_init_enet_phy(base) != status_success) {
        printf("Enet%d PHY init failed!\n", idx);
        return status_fail;
    }

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(base);
    #endif

    return status_success;
}

/*---------------------------------------------------------------------*
 * Main
/ *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_multiple_enet_pins();

    /* Reset Enet PHYs */
    board_reset_multiple_enet_phy();

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: TCP Echo On Multiple Ports (Interrupt Usage)\n");
    #else
    printf("This is an ethernet demo: TCP Echo On Multiple Ports (Polling Usage)\n");
    #endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    /* Initialize Enet Clock */
    board_init_multiple_enet_clock();

    /* Initialize MAC and DMA */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        if (enet_init(i) == status_success) {
            printf("Enet%d init passed!\n", i);
        } else {
            printf("Enet%d init failed!\n", i);
        }
    }

    /* Start a board timer */
    board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

    /* Initialize the Lwip stack */
    lwip_init();

    /* Initialize network setting, services and apps */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        netif_config(&gnetif[i], i);
        enet_services(&gnetif[i]);
        tcp_echo_init(&gnetif[i]);
    }

    while (1) {
        for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
            enet_common_handler(&gnetif[i]);
        }
    }

    return 0;
}
