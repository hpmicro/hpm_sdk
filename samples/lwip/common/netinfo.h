/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef NETINFO_H
#define NETINFO_H

/* MAC ADDRESS */
#ifndef MAC0_CONFIG
#define MAC0_CONFIG 98:2c:bc:b1:9f:27
#endif

#ifndef MAC1_CONFIG
#define MAC1_CONFIG 98:2c:bc:b1:9f:37
#endif

/* Static IP ADDRESS */
#ifndef IP0_CONFIG
#define IP0_CONFIG 192.168.100.10
#endif

#ifndef IP1_CONFIG
#define IP1_CONFIG 192.168.200.10
#endif

/* Netmask */
#ifndef NETMASK0_CONFIG
#define NETMASK0_CONFIG 255.255.255.0
#endif

#ifndef NETMASK1_CONFIG
#define NETMASK1_CONFIG 255.255.255.0
#endif

/* Gateway Address */
#ifndef GW0_CONFIG
#define GW0_CONFIG 192.168.100.1
#endif

#ifndef GW1_CONFIG
#define GW1_CONFIG 192.168.200.1
#endif

/* Remote IP Address */
#ifndef REMOTE_IP0_CONFIG
#define REMOTE_IP0_CONFIG 192.168.100.5
#endif

#ifndef REMOTE_IP1_CONFIG
#define REMOTE_IP1_CONFIG 192.168.200.5
#endif

#endif  /* NETINFO_H */
