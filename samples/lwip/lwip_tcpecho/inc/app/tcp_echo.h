/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCPECHO_H
#define TCPECHO_H

#define TCP_ECHO_PORT (5001U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void tcp_echo_init(void);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TCP_ECHO_H */