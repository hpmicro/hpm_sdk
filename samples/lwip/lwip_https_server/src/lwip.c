/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

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

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    printf("This is an ethernet demo: https server (Interrupt Usage)\n");
#else
    printf("This is an ethernet demo: https server (Polling Usage)\n");
#endif

    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    if (enet_init(ENET) == 0) {
        /* Initialize lwIP stack */
        lwip_init();

        /* Initialize network interface */
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

        /* Start a board timer */
        board_timer_create(LWIP_APP_TIMER_INTERVAL, sys_timer_callback);

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