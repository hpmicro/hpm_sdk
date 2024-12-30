/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef UDPECHO_H
#define UDPECHO_H

#ifndef UDP_ECHO_PORT
#define UDP_ECHO_PORT (5001U)
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void udp_echo_init(void);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* UDP_ECHO_H */