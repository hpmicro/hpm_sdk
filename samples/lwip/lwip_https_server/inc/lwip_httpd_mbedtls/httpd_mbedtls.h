/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LWIP_HDR_APPS_HTTPD_MBEDTLS_H
#define LWIP_HDR_APPS_HTTPD_MBEDTLS_H

#include "mbedtls/ssl.h"

#ifdef __cplusplus
extern "C" {
#endif

void httpd_mbedtls_init(mbedtls_ssl_context *ssl);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_HTTPD_H */
