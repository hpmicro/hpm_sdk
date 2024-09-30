/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file    tsn_ptp.h
 * \author  zimmerli
 * \date    2019-01-14
 * \brief   IEEE 802.1as SW-Implementation, low-level, (Header)
 *
 */
#ifndef TSN_PTP_H_
#define TSN_PTP_H_

#include <stdint.h>
#include "tsn_ptp_types.h"
#include "tsn_ptp_config.h"

extern const uint8_t tsn_ptp_macaddr[6];

/**
 * EtherType used by PTP communication
 */
#define ETH_TYPE_PTP 0x88F7

#define PTP_MAJOR_SDO_ID 0x1
#define PTP_MINOR_SDO_ID 0x0
#define PTP_VERSION      0x2

#define PTP_MSGTYPE_SYNC        0x0
#define PTP_MSGTYPE_PDELAY_REQ  0x2
#define PTP_MSGTYPE_PDELAY_RESP 0x3
#define PTP_MSGTYPE_FOLLOWUP    0x8
#define PTP_MSGTYPE_PDELAY_FUP  0xA
#define PTP_MSGTYPE_ANNOUNCE    0xB
#define PTP_MSGTYPE_SIGNALING   0xC

#define PTP_MSGTYPE_MASK 0xF

#define PTP_CONTROL_SYNC     0x0
#define PTP_CONTROL_FOLLOWUP 0x2
#define PTP_CONTROL_OTHER    0x5

#define PTP_HEADER_SZ           34
#define PTP_PAYLOAD_SZ_PDELAY   20
#define PTP_PAYLOAD_SZ_SYNC     10
#define PTP_PAYLOAD_SZ_FOLLOWUP 42
#define PTP_PAYLOAD_SZ_SIGNAL   26
#define PTP_PAYLOAD_SZ_ANNOUNCE 34

#define PTP_ALLOW_LOST_RESP    3
#define PTP_ALLOW_PREQ_FAULTS  9
#define PTP_INIT_INTVAL_SYNC   -3
#define PTP_INIT_INTVAL_ANNC   0
#define PTP_INIT_INTVAL_PDELAY 0
#define PTP_SYNC_ADJ_FREQ_IVAL 1000000000LL
#define PTP_SYNC_RATE_OFS_TH   (0x3000000)

#define PTP_TLV_TYPE_ORG_EXT   0x3
#define PTP_TLV_TYPE_PATHTRACE 0x8

#define PTP_PATHTRACE_SZ 8

#define PTP_FLAG_SIGNALING_CNRR (1 << 1) //!< flag computeNeighborRateRatio
#define PTP_FLAG_SIGNALING_CNPD (1 << 2) //!< flag computeNeighborPropDelay

/**
 * Type of source of time used by clock
 * Information only attribute, not used for GM selection,
 * see Table 8-3 for time source enumeration,
 * see 7.6.2.6 of IEEE Std 1588 for more details
 */
enum ptp_timesource_enum {
    PTP_SRC_ATOMIC_CLOCK = 0x10,
    PTP_SRC_GPS = 0x20,
    PTP_SRC_TERREST_RADIO = 0x30,
    PTP_SRC_PTP = 0x40,
    PTP_SRC_NTP = 0x50,
    PTP_SRC_HAND_SET = 0x60,
    PTP_SRC_OTHER = 0x90,
    PTP_SRC_INTERNAL_OSC = 0xA0
};

/**
 * \brief PTP Flags, Uint16
 *
 * defined and used in network byte order
 */
enum ptp_flags_enum {
    PTP_FLAG_TWO_STEP = (1 << 1),
    PTP_FLAG_LEAP61 = (1 << (8 + 0)),
    PTP_FLAG_LEAP59 = (1 << (8 + 1)),
    PTP_FLAG_CURUTCO_VALID = (1 << (8 + 2)),
    PTP_FLAG_PTP_TIMESCALE = (1 << (8 + 3)),
    PTP_FLAG_TIME_TRACEABLE = (1 << (8 + 4)),
    PTP_FLAG_FREQ_TRACEABLE = (1 << (8 + 5))
};

/**
 * \brief PTP Port state enumeration
 *
 * see IEEE1588
 * see IEEE802.1as - 14.6.3
 */
enum ptp_port_role_enum {
    PTP_PORT_ROLE_DISABLED = 3,
    PTP_PORT_ROLE_MASTER = 6,
    PTP_PORT_ROLE_PASSIVE = 7,
    PTP_PORT_ROLE_SLAVE = 9
};

/**
 * \brief PTP header information
 */
struct ptp_header_s {
    uint8_t msgType;
    uint8_t majorSdoId; // transportSpecific in 802.1AS-2011

    uint8_t version;
    uint8_t minorVersionPTP;
    uint16_t msgLen;
    uint8_t domainNumber;
    uint8_t minorSdoId; // reserved in 802.1AS-2011
    uint16_t flags;
    int64_t correctionField;
    port_id_t sourcePortId;
    uint16_t sequenceId;
    uint8_t control;
    int8_t logMsgInterval;
};

/**
 * \brief PTP FollowUp message information
 */
struct ptp_followup_s {
    ptp_timestamp_t preciseOriginTimestamp;
    int32_t cumScaledRateOfs; // cumulativeScaledRateOffset
    uint16_t gmTimeBaseInd;   // gmTimeBaseIndicator
    scaled_ns_t lastGmPhaseCh;
    int32_t scaledLastGmFreqCh;
};

/**
 * \brief PTP signaling TLV information
 */
struct ptp_signaling_tlv_s {
    int8_t linkDelayInterval;
    int8_t timeSyncInterval;
    int8_t announceInterval;
    uint8_t flags;
};

/**
 * \brief PTP Announce information
 */
struct ptp_announce_s {
    int16_t currentUtcOfs;
    uint8_t gmPrio1;
    uint8_t gmPrio2;
    clock_quality_t clockQuality;
    clock_id gmIdentity;
    uint16_t stepsRemoved;
    uint8_t timeSource;
    uint16_t pathCount;
    uint8_t *pPathSequence;
};

/**
 * Set ethernet header information in destination memory
 *
 * @param dstaddr   Destination Address
 * @param pMacAddr  Pointer to MAC address (expected array of 6 uint8_t)
 * @return          returns length of header in bytes (always 14)
 */
uint32_t tsn_ptp_set_eth_header(uintptr_t dstaddr, uint8_t *pMacAddr);

/**
 * Set PTP header information in destination memory
 *
 * @param dstaddr   Destination Address
 * @param pPtp      Pointer to struct ptp_header_s, containing header information
 * @return          returns length of header in bytes (always 34)
 */
uint32_t tsn_ptp_set_ptp_header(uintptr_t dstaddr, struct ptp_header_s *pPtp);

/**
 * Adjust PTP timestamp by nanosecond value (primarily used for ingress/egress latency adjustment)
 *
 * @param pTimestamp    pointer to timestamp
 * @param adjustNs      adjustement value in nanoseconds, must not exceed 1 second (10^9-1)
 */
void tsn_ptp_adjust_timestamp(ptp_timestamp_t *pTimestamp, int32_t adjustNs);

void tsn_ptp_mac_to_clockid(const uint8_t *pMacAddr, uint8_t *pClockId);
uint32_t tsn_ptp_set_tstamp(uintptr_t dstaddr, ptp_timestamp_t *pTimestamp);
void tsn_ptp_get_tstamp(uintptr_t srcaddr, ptp_timestamp_t *pTimestamp);
uint32_t tsn_ptp_set_portid(uintptr_t dstaddr, port_id_t *pPortId);
void tsn_ptp_get_portid(uintptr_t srcaddr, port_id_t *pPortId);
uint16_t tsn_ptp_get_flags(uintptr_t srcaddr);

uint32_t tsn_ptp_set_pdelay_req(uintptr_t dstaddr);
uint32_t tsn_ptp_set_pdelay_resp(uintptr_t dstaddr, ptp_timestamp_t *pRxTstamp, port_id_t *pReqPortId);
uint32_t tsn_ptp_set_pdelay_fup(uintptr_t dstaddr, ptp_timestamp_t *pRespOrigTstamp, port_id_t *pReqPortId);
uint32_t tsn_ptp_set_sync(uintptr_t dstaddr);
uint32_t tsn_ptp_set_followup(uintptr_t dstaddr, struct ptp_followup_s *pFollowup);
uint32_t tsn_ptp_set_signaling(uintptr_t dstaddr, struct ptp_signaling_tlv_s *pSignaling);
uint32_t tsn_ptp_set_announce(uintptr_t dstaddr, struct ptp_announce_s *pAnnounce);

int32_t tsn_ptp_get_followup(uintptr_t addr, struct ptp_followup_s *pFollowup);
int32_t tsn_ptp_get_announce(uintptr_t addr, struct ptp_announce_s *pAnnounce);

int32_t tsn_ptp_cmp_systemid(ptp_system_id_t *pSysIdA, ptp_system_id_t *pSysIdB);
int32_t tsn_ptp_cmp_priovect(ptp_prio_vect_t *pPrioA, ptp_prio_vect_t *pPrioB);

#endif /* TSN_PTP_H_ */
