/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#define TCP_DEST_PORT (5001U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void tcp_client_init(void);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TCP_CLIENT_H */