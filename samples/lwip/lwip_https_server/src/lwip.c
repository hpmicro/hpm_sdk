/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "lwip.h"
#include "common.h"
#include "httpd_mbedtls.h"
#include "lwip/init.h"
#include "netconf.h"
#include "sys_arch.h"

#include "mbedtls/entropy.h"

#include "hpm_l1c_drv.h"
#include "hpm_rng_drv.h"
#include "mbedtls/certs.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_cache.h"
#include "mbedtls/x509.h"

#define DEBUG_LEVEL 0
#define PRINTF printf
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT]; /* Ethernet Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT]; /* Ethernet Tx DMA Descriptor */

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE)
__RW uint8_t rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE)
__RW uint8_t tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */

enet_desc_t desc;
uint8_t mac[ENET_MAC];

const char *pers = "ssl_server";
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_ssl_context ssl;
mbedtls_ssl_config conf;
mbedtls_x509_crt srvcert;
mbedtls_pk_context pkey;
#if defined(MBEDTLS_SSL_CACHE_C)
mbedtls_ssl_cache_context cache;
#endif

#if defined(__USE_ENET_RECEIVE_INTERRUPT) && __USE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag;
#endif

struct netif gnetif;

/* Entropy poll callback for a hardware source */
#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    hpm_stat_t stat;
    ((void)data);

    stat = rng_rand_wait(HPM_RNG, output, len);
    if (stat == status_success) {
        *olen = len;
        return status_success;
    } else {
        *olen = len;
        return status_success;
    }
}

#endif

static void my_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void)ctx);
    ((void)level);

    PRINTF("\r\n%s, at line %d in file %s\n", str, line, file);
}

hpm_stat_t enet_init(ENET_Type *ptr)
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

    /* Set the control config for tx descriptor */
    memcpy(&desc.tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Get MAC address */
    enet_get_mac_address(mac);

    /* Set MAC0 address */
    enet_config.mac_addr_high[0] = mac[5] << 8 | mac[4];
    enet_config.mac_addr_low[0] = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    enet_config.valid_max_count = 1;

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(ENET);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    #if defined(__USE_ENET_RECEIVE_INTERRUPT) && __USE_ENET_RECEIVE_INTERRUPT
    /* Enable Enet IRQ */
    board_enable_enet_irq(ENET);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ENET, &int_config);
    #endif

    /* Initialize enet controller */
    if (enet_controller_init(ptr, ENET_INF_TYPE, &desc, &enet_config, &int_config) != status_success) {
        return status_fail;
    }

#if defined(__USE_ENET_RECEIVE_INTERRUPT) && __USE_ENET_RECEIVE_INTERRUPT
    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(ENET);
#endif

/* Initialize phy */
#if defined(RGMII) && RGMII
#if defined(__USE_DP83867) && __USE_DP83867
    dp83867_reset(ptr);
#ifdef __DISABLE_AUTO_NEGO
    dp83867_set_mdi_crossover_mode(ENET, enet_phy_mdi_crossover_manual_mdix);
#endif
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

int main(void)
{
    hpm_stat_t stat;
    /* Initialize BSP */
    board_init();

#if defined(CONFIG_MBEDTLS_USE_HPM_SDP) && CONFIG_MBEDTLS_USE_HPM_SDP
    clock_add_to_group(clock_sdp, BOARD_RUNNING_CORE & 0x1);
#endif

    clock_add_to_group(clock_rng, BOARD_RUNNING_CORE & 0x1);
    stat = rng_init(HPM_RNG);
    if (stat) {
        printf("failed to rng_init: 0x%x\n", stat);
    }

    /* Initialize GPIOs */
    board_init_enet_pins(ENET);

    /* Reset an enet PHY */
    board_reset_enet_phy(ENET);

#if defined(__USE_ENET_RECEIVE_INTERRUPT) && __USE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: https server (Interrupt Usage)\n");
#else
    printf("This is an ethernet demo: https server (Polling Usage)\n");
#endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

/* Set RGMII clock delay */
#if defined(RGMII) && RGMII
    board_init_enet_rgmii_clock_delay(ENET);
#elif defined(RMII) && RMII
    /* Set RMII reference clock */
    board_init_enet_rmii_reference_clock(ENET, BOARD_ENET_RMII_INT_REF_CLK);
    printf("Reference Clock: %s\n", BOARD_ENET_RMII_INT_REF_CLK ? "Internal Clock" : "External Clock");
#endif
    /* Start a board timer */
    board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

    /* Initialize MAC and DMA */
    if (enet_init(ENET) == 0) {
        /* Initialize the Lwip stack */
        lwip_init();
        netif_config(&gnetif);

        /* Start services */
        enet_services(&gnetif);

        /*
         * mbedTLS - setup
         */

        mbedtls_ssl_init(&ssl);
        mbedtls_ssl_config_init(&conf);

        mbedtls_x509_crt_init(&srvcert);
        mbedtls_pk_init(&pkey);
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&ctr_drbg);

#if defined(MBEDTLS_DEBUG_C)
        mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

        /*
         * 1. Load the certificates and private RSA key
         */

        /*
         * This demonstration program uses embedded test certificates.
         * Instead, you may want to use mbedtls_x509_crt_parse_file() to read the
         * server and CA certificates, as well as mbedtls_pk_parse_keyfile().
         */
        int ret;
        ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *)mbedtls_test_srv_crt, mbedtls_test_srv_crt_len);
        if (ret != 0) {
            printf(" failed\r\n  !  mbedtls_x509_crt_parse returned %d\r\n\r\n", ret);
            goto exit;
        }

#if (!defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED))
        ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *)mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
        if (ret != 0) {
            printf(" failed\r\n  !  mbedtls_x509_crt_parse returned %d\r\n\r\n", ret);
            goto exit;
        }
#endif

        ret = mbedtls_pk_parse_key(&pkey, (const unsigned char *)mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0);
        if (ret != 0) {
            printf(" failed\r\n  !  mbedtls_pk_parse_key returned %d\r\n\r\n", ret);
            goto exit;
        }

        /*
         * 2. Seeding the random number generator
         */
        ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, strlen(pers));
        if (ret != 0) {
            PRINTF(" failed\r\n  ! mbedtls_ctr_drbg_seed returned %d\r\n", ret);
            goto exit;
        }

        /*
         * 3. Setting up the SSL data.
         */
        ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
        if (ret != 0) {
            PRINTF(" failed\r\n  ! mbedtls_ssl_config_defaults returned %d\r\n\r\n", ret);
            goto exit;
        }
        mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
        mbedtls_ssl_conf_dbg(&conf, my_debug, NULL);
#if defined(MBEDTLS_SSL_CACHE_C)
        mbedtls_ssl_conf_session_cache(&conf, &cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
#endif
        mbedtls_ssl_conf_ca_chain(&conf, srvcert.next, NULL);
        ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey);
        if (ret != 0) {
            PRINTF(" failed\r\n  ! mbedtls_ssl_conf_own_cert returned %d\r\n\r\n", ret);
            goto exit;
        }
        ret = mbedtls_ssl_setup(&ssl, &conf);
        if (ret != 0) {
            PRINTF(" failed\r\n  ! mbedtls_ssl_setup returned %d\r\n\r\n", ret);
            goto exit;
        }
        httpd_mbedtls_init(&ssl);

        PRINTF("\r\n************************************************\r\n");
        PRINTF(" mbedTLS HTTPS Server example\r\n");
        PRINTF("************************************************\r\n");

        while (1) {
            enet_common_handler(&gnetif);
        }
    } else {
        printf("Enet initialization fails !!!\n");
        while (1) {
        }
    }

    return 0;
exit:
    while (1) {
    }
}