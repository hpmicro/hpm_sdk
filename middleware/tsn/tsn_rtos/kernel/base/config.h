/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file
 * \author sep
 * \date   2022-05-17
 * \brief  TSN RTOS config parameter checker
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <tsn_rtos_config.h>

#ifndef USE_TSN_RTOS_NET_ETHTOOL
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_ETHTOOL"
#endif

#ifndef USE_TSN_RTOS_NET_BRIDGE
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_BRIDGE"
#endif

#ifndef USE_TSN_RTOS_NET_CB
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_CB"
#endif

#ifndef USE_TSN_RTOS_NET_PSFP
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_PSFP"
#endif

#ifndef USE_TSN_RTOS_NET_FDFIFO
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_FDFIFO"
#endif

#ifndef USE_TSN_RTOS_NET_MONITOR
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_MONITOR"
#endif

#ifndef USE_TSN_RTOS_NET_TC
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_TC"
#endif

#ifndef USE_TSN_RTOS_NET_PTP4SYNC
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_PTP4SYNC"
#endif

#ifndef USE_TSN_RTOS_NET_LWIP
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_NET_LWIP"
#endif

/* Kernel DRIVER configuration */

#if (USE_TSN_RTOS_DRIVER_SWITCH == 1)

#ifndef USE_TSN_RTOS_DRIVER_LEARNING
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_DRIVER_LEARNING"
#endif

#endif

/* Kernel TOOL configuration */

#ifndef USE_TSN_RTOS_TOOL_BRIDGE
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_BRIDGE"
#endif

#ifndef USE_TSN_RTOS_TOOL_CB
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_CB"
#endif

#ifndef USE_TSN_RTOS_TOOL_PSFP
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_PSFP"
#endif

#ifndef USE_TSN_RTOS_TOOL_CLI
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_CLI"
#endif

#ifndef USE_TSN_RTOS_TOOL_CLOCK
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_CLOCK"
#endif

#ifndef USE_TSN_RTOS_TOOL_ECHO
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_ECHO"
#endif

#ifndef USE_TSN_RTOS_TOOL_ETHDUMP
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_ETHDUMP"
#endif

#ifndef USE_TSN_RTOS_TOOL_ETHTEST
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_ETHTEST"
#endif

#ifndef USE_TSN_RTOS_TOOL_ETHTOOL
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_ETHTOOL"
#endif

#ifndef USE_TSN_RTOS_TOOL_FDFIFO
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_FDFIFO"
#endif

#ifndef USE_TSN_RTOS_TOOL_IF
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_IF"
#endif

#ifndef USE_TSN_RTOS_TOOL_LINKTEST
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_LINKTEST"
#endif

#ifndef USE_TSN_RTOS_TOOL_LLDP
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_LLDP"
#endif

#ifndef USE_TSN_RTOS_TOOL_MONITOR
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_MONITOR"
#endif

#ifndef USE_TSN_RTOS_TOOL_PTPDOMAIN
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_PTPDOMAIN"
#endif

#ifndef USE_TSN_RTOS_TOOL_RTPING
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_RTPING"
#endif

#ifndef USE_TSN_RTOS_TOOL_TC
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_TC"
#endif

#ifndef USE_TSN_RTOS_TOOL_TEMPLATE
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_TEMPLATE"
#endif

#ifndef USE_TSN_RTOS_TOOL_TIMEPULSE
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_TIMEPULSE"
#endif

#ifndef USE_TSN_RTOS_TOOL_UTIL
#error "TSN RTOS: missing config parameter USE_TSN_RTOS_TOOL_UTIL"
#endif

#endif /* CONFIG_H_ */
