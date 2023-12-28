/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "common_lwip.h"
#include "netconf.h"
#include "lwip.h"
#include "lwip/init.h"
#include "ptpd.h"

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT] ; /* Ethernet Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT] ; /* Ethernet Tx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */

enet_desc_t desc;
uint32_t localtime;
uint8_t mac[ENET_MAC];
/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
static void enet_ptp_init(void)
{
    enet_ptp_config_t config;
    enet_ptp_ts_update_t timestamp;

    /* initial the ptp function */
    config.ssinc = 2 * ENET_ONE_SEC_IN_NANOSEC / clock_get_frequency(ENET_PTP_CLK);
    config.timestamp_rollover_mode = enet_ts_dig_rollover_control;
    config.update_method = enet_ptp_time_fine_update;
    config.addend = 0x80000000;

    enet_init_ptp(ENET, &config);

    /* set the initial timestamp */
    timestamp.sec = 1651074120UL;
    timestamp.nsec = 0;
    enet_set_ptp_timestamp(ENET, &timestamp);
}

static hpm_stat_t enet_init(ENET_Type *ptr)
{
    enet_int_config_t int_config = {.int_enable = 0, .int_mask = 0};
    enet_mac_config_t enet_config;
    enet_tx_control_config_t enet_tx_control_config;

    #if defined(RGMII) && RGMII
        #if defined(__USE_DP83867) && __USE_DP83867
        dp83867_config_t phy_config;
        #else
        rtl8211_config_t phy_config;
        #endif
    #else
        #if defined(__USE_DP83848) && __USE_DP83848
        dp83848_config_t phy_config;
        #else
        rtl8201_config_t phy_config;
        #endif
    #endif

    /* Initialize td, rd and the corresponding buffers */
    memset((uint8_t *)dma_tx_desc_tab, 0x00, sizeof(dma_tx_desc_tab));
    memset((uint8_t *)dma_rx_desc_tab, 0x00, sizeof(dma_rx_desc_tab));
    memset((uint8_t *)rx_buff, 0x00, sizeof(rx_buff));
    memset((uint8_t *)tx_buff, 0x00, sizeof(tx_buff));

    desc.tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_tx_desc_tab);
    desc.rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_rx_desc_tab);

    desc.tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)tx_buff);
    desc.tx_buff_cfg.count = ENET_TX_BUFF_COUNT;
    desc.tx_buff_cfg.size = ENET_TX_BUFF_SIZE;

    desc.rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rx_buff);
    desc.rx_buff_cfg.count = ENET_RX_BUFF_COUNT;
    desc.rx_buff_cfg.size = ENET_RX_BUFF_SIZE;

    /*Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(ENET, &enet_tx_control_config);
    enet_tx_control_config.enable_ttse = true;

    /* Set the control config for tx descriptor */
    memcpy(&desc.tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Get MAC address */
    enet_get_mac_address(mac);

    /* Set MAC0 address */
    enet_config.mac_addr_high[0] = mac[5] << 8 | mac[4];
    enet_config.mac_addr_low[0]  = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    enet_config.valid_max_count  = 1;

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(ENET);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    int_config.mmc_intr_mask_rx = 0x03ffffff;   /* Disable all mmc rx interrupt events */
    int_config.mmc_intr_mask_tx = 0x03ffffff;   /* Disable all mmc tx interrupt events */

    /* Initialize enet controller */
    enet_controller_init(ptr, ENET_INF_TYPE, &desc, &enet_config, &int_config);

    /* Initialize phy */
    #if defined(RGMII) && RGMII
        #if defined(__USE_DP83867) && __USE_DP83867
        dp83867_reset(ptr);
        dp83867_basic_mode_default_config(ptr, &phy_config);
        if (dp83867_basic_mode_init(ptr, &phy_config) == true) {
        #else
        rtl8211_reset(ptr);
        rtl8211_basic_mode_default_config(ptr, &phy_config);
        if (rtl8211_basic_mode_init(ptr, &phy_config) == true) {
        #endif
    #else
        #if defined(__USE_DP83848) && __USE_DP83848
        dp83848_reset(ptr);
        dp83848_basic_mode_default_config(ptr, &phy_config);
        if (dp83848_basic_mode_init(ptr, &phy_config) == true) {
        #else
        rtl8201_reset(ptr);
        rtl8201_basic_mode_default_config(ptr, &phy_config);
        if (rtl8201_basic_mode_init(ptr, &phy_config) == true) {
        #endif
    #endif
            printf("Enet phy init passed !\n");
            return status_success;
        } else {
            printf("Enet phy init failed !\n");
            return status_fail;
        }
}

static void local_timer_callback(void)
{
    localtime += LWIP_APP_TIMER_INTERVAL;
}

void timer_callback(void)
{
    local_timer_callback();
    sys_timer_callback();
}

/*---------------------------------------------------------------------*
 * Main
 *---------------------------------------------------------------------*/
int main(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize PTP clock */
    board_init_enet_ptp_clock(ENET);

    /* Initialize GPIOs */
    board_init_enet_pins(ENET);

    #if defined(ENET_PPS_PINOUT) && ENET_PPS_PINOUT
    /* Initialize PPS pins */
    board_init_enet_pps_pins(BOARD_ENET_PPS);
    #endif

    /* Reset an enet PHY */
    board_reset_enet_phy(ENET);

    printf("This is an ethernet demo: PTP V1 Master\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    #if defined(RGMII) && RGMII
    /* Set RGMII clock delay */
    board_init_enet_rgmii_clock_delay(ENET);
    #else
    /* Set RMII reference clock */
    board_init_enet_rmii_reference_clock(ENET, BOARD_ENET_RMII_INT_REF_CLK);
    printf("Reference Clock: %s\n", BOARD_ENET_RMII_INT_REF_CLK ? "Internal Clock" : "External Clock");
    #endif

    /* Initialize a board timer */
    board_timer_create(LWIP_APP_TIMER_INTERVAL, timer_callback);

    /* Initialize MAC and DMA */
    if (enet_init(ENET) == 0) {
        /* Initialize PTP */
        enet_ptp_init();

        #if defined(ENET_PPS_PINOUT) && ENET_PPS_PINOUT
        enet_set_pps0_control_output(BOARD_ENET_PPS, enet_pps_ctrl_pps);
        #endif

        /* Initialize the Lwip stack */
        lwip_init();
        netif_config();
        user_notification(&gnetif);

        /* Start services */
        enet_services(&gnetif);

        /* Initialize ptpd */
        ptpd_Init();

        while (1) {
            enet_common_handler(&gnetif);
            ptpd_periodic_handle(localtime);
        }
    } else {
        printf("Enet initialization fails !!!\n");
        while (1) {

        }
    }

    return 0;
}
