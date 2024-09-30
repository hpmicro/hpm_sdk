/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file
 * \author Phil Seipt
 * \date 07.09.2021
 * \brief Holding global defines for the TSN RTOS framework
 *
 */

#ifndef TSN_RTOS_CONFIG_H_
#define TSN_RTOS_CONFIG_H_

/* Kernel NET configuration */
#define USE_TSN_RTOS_NET_ETHTOOL  (1)
#define USE_TSN_RTOS_NET_BRIDGE   (1)
#define USE_TSN_RTOS_NET_CB       (1)
#define USE_TSN_RTOS_NET_PSFP     (1)
#define USE_TSN_RTOS_NET_FDFIFO   (1)
#define USE_TSN_RTOS_NET_MONITOR  (1)
#define USE_TSN_RTOS_NET_TC       (1)
#define USE_TSN_RTOS_NET_PTP4SYNC (1)
#define USE_TSN_RTOS_NET_LWIP     (1)

/* Kernel DRIVER configuration */
#define USE_TSN_RTOS_DRIVER_IPMS_TSN         (1)
#define USE_TSN_RTOS_DRIVER_SWITCH           (1)
#define USE_TSN_RTOS_DRIVER_LEARNING         (0)
#define USE_TSN_RTOS_DRIVER_LEARNING_DEBUG   (0)
#define USE_TSN_RTOS_DRIVER_ALT_UARTDRV      (1)
#define USE_TSN_RTOS_DRIVER_ALT_MSGDMA       (1)
#define USE_TSN_RTOS_DRIVER_ALT_FPGAMGR_GPIO (1)

/* Kernel TOOL configuration */
#define USE_TSN_RTOS_TOOL_BRIDGE    (1)
#define USE_TSN_RTOS_TOOL_CB        (1)
#define USE_TSN_RTOS_TOOL_PSFP      (1)
#define USE_TSN_RTOS_TOOL_CLI       (1)
#define USE_TSN_RTOS_TOOL_CLOCK     (1)
#define USE_TSN_RTOS_TOOL_ECHO      (0)
#define USE_TSN_RTOS_TOOL_ETHDUMP   (1)
#define USE_TSN_RTOS_TOOL_ETHTEST   (0)
#define USE_TSN_RTOS_TOOL_ETHTOOL   (1)
#define USE_TSN_RTOS_TOOL_FDFIFO    (1)
#define USE_TSN_RTOS_TOOL_IF        (1)
#define USE_TSN_RTOS_TOOL_LINKTEST  (1)
#define USE_TSN_RTOS_TOOL_LLDP      (1)
#define USE_TSN_RTOS_TOOL_MONITOR   (1)
#define USE_TSN_RTOS_TOOL_PSFP      (1)
#define USE_TSN_RTOS_TOOL_PTPDOMAIN (1)
#define USE_TSN_RTOS_TOOL_RTPING    (1)
#define USE_TSN_RTOS_TOOL_TC        (1)
#define USE_TSN_RTOS_TOOL_TEMPLATE  (0)
#define USE_TSN_RTOS_TOOL_TIMEPULSE (1)
#define USE_TSN_RTOS_TOOL_UTIL      (1)

#endif /* TSN_RTOS_CONFIG_H_ */
