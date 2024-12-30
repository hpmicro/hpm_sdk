/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef UDP_ECHO_H
#define UDP_ECHO_H

#ifndef UDP_LOCAL_PORT
#define UDP_LOCAL_PORT (5001U)
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void udp_echo_init(void);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* UDP_ECHO_H */