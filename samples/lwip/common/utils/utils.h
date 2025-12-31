/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>


#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

bool fetch_ip_addr_from_serial_terminal(uint8_t netif_num, uint8_t *buff, uint32_t length);

#if defined __cplusplus
}
#endif /* __cplusplus */

 #endif /* UTILS_H */