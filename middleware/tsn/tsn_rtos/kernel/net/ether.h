/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   ether.h
 * \author zimmerli
 * \date   2020-01-20
 * \brief  Ethernet Types
 *
 */

#ifndef ETHER_H_
#define ETHER_H_

/* 802.1Q Annex I */
/* the traffic acronyms describes the traffic priority in range [0=lowest .. 7=highest] */
#define ETH_PRIO_BG 1 /* Background */
#define ETH_PRIO_BE 0 /* Best Effort */
#define ETH_PRIO_EE 2 /* Excellent Effort */
#define ETH_PRIO_CA 3 /* Critical Application */
#define ETH_PRIO_VI 4 /* Video */
#define ETH_PRIO_VO 5 /* Voice */
#define ETH_PRIO_IC 6 /* Internetwork Control */
#define ETH_PRIO_NC 7 /* Network Control */

extern const char *ETH_PRIO_NAMES[8];

#define VLAN_TAG(pcp, dei, vid) (((pcp & 0x07) << 13) | ((dei & 0x01) << 12) | (vid & 0xFFF))

#define ETH_TYPE_IP   0x0800
#define ETH_TYPE_VLAN 0x8100

#define ETH_TYPE_LOCALEXP1 0x88B5
#define ETH_TYPE_LOCALEXP2 0x88B6

#define ETH_TYPE_LLDP 0x88CC
#define ETH_TYPE_UADP 0xB62C

#endif /* ETHER_H_ */
