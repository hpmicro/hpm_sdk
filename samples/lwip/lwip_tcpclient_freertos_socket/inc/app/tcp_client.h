/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#ifndef TCP_DEST_PORT
#define TCP_DEST_PORT (5001U)
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void tcp_client_init(void *arg);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TCP_CLIENT_H */