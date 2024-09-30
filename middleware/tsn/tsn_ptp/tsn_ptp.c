/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file    tsn_ptp.c
 * \author  zimmerli
 * \date    2019-01-14
 * \brief   IEEE 802.1as SW-Implementation, low-level
 *
 */

#include "tsn_ptp.h"

// Macros
// swap endianess
#define SWAP16(value) (((value & 0xFF) << 8 | (value >> 8)) & 0xFFFF)

// access memory
#define MEM8(addr) (*(uint8_t *)(addr))

#define NS_PER_SEC 1000000000

// ----------------------------------------------------------------
// inline functions

// 32 bit memory access and network to host order conversion
static inline uint32_t mem32_ntohl(uintptr_t addr)
{
    uint32_t tmp;
    uint8_t *ptr;
    ptr = (uint8_t *)addr;

    tmp = ptr[0];
    tmp = (tmp << 8) | ptr[1];
    tmp = (tmp << 8) | ptr[2];
    tmp = (tmp << 8) | ptr[3];

    return tmp;
}

// 32 bit memory access and host to network order conversion
static inline void mem32_htonl(uintptr_t addr, uint32_t value)
{
    uint8_t *ptr;
    ptr = (uint8_t *)addr;
    ptr[0] = (value >> 24) & 0xFF;
    ptr[1] = (value >> 16) & 0xFF;
    ptr[2] = (value >> 8) & 0xFF;
    ptr[3] = (value) & 0xFF;
}

// 16 bit memory access and network to host order conversion
static inline uint16_t mem16_ntohs(uintptr_t addr)
{
    uint16_t tmp;
    uint8_t *ptr;
    ptr = (uint8_t *)addr;
    tmp = ptr[0] << 8;
    tmp |= ptr[1];
    return tmp;
}

// 16 bit memory access and host to network order conversion
static inline void mem16_htons(uintptr_t addr, uint16_t value)
{
    uint8_t *ptr;
    ptr = (uint8_t *)addr;
    ptr[0] = (value >> 8) & 0xFF;
    ptr[1] = (value) & 0xFF;
}

// ----------------------------------------------------------------
/**
 * Destination MAC address used by all PTP communication
 */
const uint8_t tsn_ptp_macaddr[6] = { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E };

// ----------------------------------------------------------------
uint32_t tsn_ptp_set_eth_header(uintptr_t dstaddr, uint8_t *pMacAddr)
{
    uint8_t *pu8;
    uint32_t idx;

    // point pu8 to dstaddr
    pu8 = (uint8_t *)dstaddr;
    // dst addr
    for (idx = 0; idx < 6; ++idx) {
        pu8[idx] = tsn_ptp_macaddr[idx];
    }
    // src addr
    for (idx = 0; idx < 6; ++idx) {
        pu8[6 + idx] = pMacAddr[idx];
    }
    // eth type (MSB first)
    pu8[12] = ETH_TYPE_PTP >> 8;
    pu8[13] = ETH_TYPE_PTP & 0xFF;

    return 14;
}

uint32_t tsn_ptp_set_ptp_header(uintptr_t dstaddr, struct ptp_header_s *pPtp)
{
    uint32_t ofs;
    uint32_t idx;
    uint8_t *pSrc;

    // start at dstaddr
    ofs = 0;
    // set header
    MEM8(dstaddr + ofs) = pPtp->msgType | (pPtp->majorSdoId << 4);
    ofs++;
    MEM8(dstaddr + ofs) = pPtp->version | (pPtp->minorVersionPTP << 4);
    ofs++;
    mem16_htons(dstaddr + ofs, pPtp->msgLen);
    ofs += 2;
    MEM8(dstaddr + ofs) = pPtp->domainNumber;
    ofs++;
    MEM8(dstaddr + ofs) = pPtp->minorSdoId;
    ofs++;
    mem16_htons(dstaddr + ofs, SWAP16(pPtp->flags)); // flags used in network byte order in system
    ofs += 2;
    pSrc = (uint8_t *)(&(pPtp->correctionField));
    for (idx = 0; idx < 8; ++idx) {
        MEM8(dstaddr + ofs + idx) = pSrc[7 - idx];
    }
    ofs += idx;                    // inc by 8
    mem16_htons(dstaddr + ofs, 0); // reserved
    ofs += 2;
    mem16_htons(dstaddr + ofs, 0); // reserved
    ofs += 2;

    ofs += tsn_ptp_set_portid(dstaddr + ofs, &(pPtp->sourcePortId));

    mem16_htons(dstaddr + ofs, pPtp->sequenceId);
    ofs += 2;
    MEM8(dstaddr + ofs) = pPtp->control;
    ofs++;
    MEM8(dstaddr + ofs) = pPtp->logMsgInterval;
    ofs++;

    return ofs;
}

void tsn_ptp_adjust_timestamp(ptp_timestamp_t *pTimestamp, int32_t adjustNs)
{
    int32_t ns;
    ns = (int32_t)pTimestamp->nanoseconds;
    ns += adjustNs;
    while (ns < 0) {
        ns += NS_PER_SEC;
        pTimestamp->seconds--;
    }
    while (ns >= NS_PER_SEC) {
        ns -= NS_PER_SEC;
        pTimestamp->seconds++;
    }
    pTimestamp->nanoseconds = ns;
}

/**
 * Convert MAC address (EUI-48) to ClockIdentity (EUI-64)
 *
 * @param pMacAddr  Pointer to MAC address (u8[6])
 * @param pClockId  Pointer to ClockIdentity (u8[8])
 */
void tsn_ptp_mac_to_clockid(const uint8_t *pMacAddr, uint8_t *pClockId)
{
    uint32_t idx;
    for (idx = 0; idx < 3; idx++) {
        pClockId[idx] = pMacAddr[idx];
    };
    pClockId[3] = 0xFF;
    pClockId[4] = 0xFE;
    for (idx = 3; idx < 6; idx++) {
        pClockId[idx + 2] = pMacAddr[idx];
    };
}

/**
 * Copy timestamp (48 sec, 32 bit nanosec)to memory address
 *
 * @param dstaddr       Destination Address
 * @param pTimestamp    Pointer to ptp_timestamp_t
 * @return              returns number of written bytes (always 10)
 */
uint32_t tsn_ptp_set_tstamp(uintptr_t dstaddr, ptp_timestamp_t *pTimestamp)
{
    uint16_t *p16;

    // copy seconds
    p16 = (uint16_t *)(&pTimestamp->seconds);
    mem16_htons(dstaddr + 0, p16[2]);
    mem16_htons(dstaddr + 2, p16[1]);
    mem16_htons(dstaddr + 4, p16[0]);
    // copy nanoseconds
    p16 = (uint16_t *)(&pTimestamp->nanoseconds);
    mem16_htons(dstaddr + 6, p16[1]);
    mem16_htons(dstaddr + 8, p16[0]);

    return 10;
}

/**
 * Get Timestamp from PTP frame
 *
 * @param srcaddr       Source Address of timestamp
 * @param pTimestamp    pointer to ptp_timestamp_t structure
 */
void tsn_ptp_get_tstamp(uintptr_t srcaddr, ptp_timestamp_t *pTimestamp)
{
    uint16_t *pDst16;

    // copy seconds
    pDst16 = (uint16_t *)(&pTimestamp->seconds);
    pDst16[3] = 0; // only 48 bit seconds
    pDst16[2] = mem16_ntohs(srcaddr + 0);
    pDst16[1] = mem16_ntohs(srcaddr + 2);
    pDst16[0] = mem16_ntohs(srcaddr + 4);
    // copy nanoseconds
    pDst16 = (uint16_t *)(&pTimestamp->nanoseconds);
    pDst16[1] = mem16_ntohs(srcaddr + 6);
    pDst16[0] = mem16_ntohs(srcaddr + 8);
}

/**
 * Copy port identity to memory address
 *
 * @param dstaddr   Destination Address
 * @param pPortId   Pointer to port_id_t
 * @return          returns number of written bytes (always 10)
 */
uint32_t tsn_ptp_set_portid(uintptr_t dstaddr, port_id_t *pPortId)
{
    int idx;
    uintptr_t addrClkId;

    // ClockIdentity
    addrClkId = (uintptr_t)(&pPortId->clockIdentity);
    for (idx = 0; idx < 8; ++idx) {
        MEM8(dstaddr + idx) = MEM8(addrClkId + idx);
    }
    // Port Number
    mem16_htons(dstaddr + 8, pPortId->portNumber);

    return 10;
}

/**
 * Get PortIdentity from PTP frame
 *
 * @param srcaddr   address of portID within PTP frame
 * @param pPortId   pointer to portIdentity structure
 */
void tsn_ptp_get_portid(uintptr_t srcaddr, port_id_t *pPortId)
{
    int idx;
    uintptr_t addrClkId;

    // ClockIdentity
    addrClkId = (uintptr_t)(&pPortId->clockIdentity);
    for (idx = 0; idx < 8; ++idx) {
        MEM8(addrClkId + idx) = MEM8(srcaddr + idx);
    }
    // Port Number
    pPortId->portNumber = mem16_ntohs(srcaddr + 8);
}

/**
 * Get PTP flags from address
 * Flags are defined in network byte order within this stack
 *
 * @param srcaddr   address of flags within ptp header
 * @return          flags
 */
uint16_t tsn_ptp_get_flags(uintptr_t srcaddr)
{
    uint16_t flags;
    flags = mem16_ntohs(srcaddr);
    return SWAP16(flags);
}

/**
 * Set Pdelay-Request Payload, 20 reserved bytes (0x00)

 * @param dstaddr   Destination Address
 * @return          returns number of written bytes (always 20)
 */
uint32_t tsn_ptp_set_pdelay_req(uintptr_t dstaddr)
{
    uint32_t ofs;

    // 2*10 bytes reserved
    for (ofs = 0; ofs < 20; ++ofs) {
        MEM8(dstaddr + ofs) = 0x00;
    }
    return ofs;
}

/**
 * Set Pdelay-Response Payload, Rx timestamp of request, request port Id
 *
 * @param dstaddr       Destination Address
 * @param pRxTstamp     rx timestamp, pointer to ptp_timestamp_t
 * @param pReqPortId    requesting port id, pointer to port_id_t
 * @return              returns number of written bytes (always 20)
 */
uint32_t tsn_ptp_set_pdelay_resp(uintptr_t dstaddr, ptp_timestamp_t *pRxTstamp, port_id_t *pReqPortId)
{
    uint32_t ofs;

    ofs = 0;
    ofs += tsn_ptp_set_tstamp(dstaddr + ofs, pRxTstamp);
    ofs += tsn_ptp_set_portid(dstaddr + ofs, pReqPortId);
    return ofs;
}

/**
 * Set Pdelay-Response-Followup Payload, Tx timestamp of response, request port Id
 *
 * @param dstaddr           Destination Address
 * @param pRespOrigTstamp   tx timestamp, pointer to struct ptp_timestamp_s
 * @param pReqPortId        requesting port id, pointer to port_id_t
 * @return                  returns number of written bytes (always 20)
 */
uint32_t tsn_ptp_set_pdelay_fup(uintptr_t dstaddr, ptp_timestamp_t *pRespOrigTstamp, port_id_t *pReqPortId)
{
    uint32_t ofs;

    ofs = 0;
    ofs += tsn_ptp_set_tstamp(dstaddr + ofs, pRespOrigTstamp);
    ofs += tsn_ptp_set_portid(dstaddr + ofs, pReqPortId);
    return ofs;
}

/**
 * Set Sync Payload, 10 reserved bytes (0x00)
 *
 * @param dstaddr   Destination Address
 * @return          returns number of written bytes (always 10)
 */
uint32_t tsn_ptp_set_sync(uintptr_t dstaddr)
{
    uint32_t ofs;

    // 10 bytes reserved
    for (ofs = 0; ofs < 10; ++ofs) {
        MEM8(dstaddr + ofs) = 0x00;
    }
    return ofs;
}

/**
 * Copy FollowUp frame payload (precieOriginTimestamp + followup) to memory address
 *
 * @param dstaddr               Destination Address
 * @param pFollowup             Pointer to struct ptp_followup_tlv_s
 * @return                      returns number of written bytes (always 42)
 */
uint32_t tsn_ptp_set_followup(uintptr_t dstaddr, struct ptp_followup_s *pFollowup)
{
    uint32_t ofs;
    uint32_t idx;
    uint8_t orgId[6] = { 0x00, 0x80, 0xC2, 0x00, 0x00, 0x01 };

    ofs = 0;
    ofs += tsn_ptp_set_tstamp(dstaddr, &pFollowup->preciseOriginTimestamp); // preciseOriginTimestamp
    // TLV type + length
    mem16_htons(dstaddr + ofs + 0, PTP_TLV_TYPE_ORG_EXT);
    mem16_htons(dstaddr + ofs + 2, 28);
    ofs += 4;
    // Organization
    for (idx = 0; idx < 6; ++idx) {
        MEM8(dstaddr + ofs + idx) = orgId[idx];
    }
    ofs += idx;
    // cumScaledRateOfs
    mem32_htonl(dstaddr + ofs, (uint32_t)pFollowup->cumScaledRateOfs);
    ofs += 4;
    // gmTimeBaseIndicator
    mem16_htons(dstaddr + ofs, pFollowup->gmTimeBaseInd);
    ofs += 2;
    // lastGmPhaseChange  0, 0, ns[8], fns[2]
    mem16_htons(dstaddr + ofs + 0, pFollowup->lastGmPhaseCh.ns_msb);
    mem32_htonl(dstaddr + ofs + 2, pFollowup->lastGmPhaseCh.ns_lsb >> 32);
    mem32_htonl(dstaddr + ofs + 6, (uint32_t)pFollowup->lastGmPhaseCh.ns_lsb);
    mem16_htons(dstaddr + ofs + 10, pFollowup->lastGmPhaseCh.fns);
    ofs += 12;
    // scaledLastGmFreqChange
    mem32_htonl(dstaddr + ofs, (uint32_t)pFollowup->scaledLastGmFreqCh);
    ofs += 4;

    return ofs;
}

/**
 * Copy Signaling information to memory address
 *
 * @param dstaddr       Destination Address
 * @param pSignaling    Pointer to struct ptp_signaling_tlv_s
 * @return              returns number of written bytes (always 26)
 */
uint32_t tsn_ptp_set_signaling(uintptr_t dstaddr, struct ptp_signaling_tlv_s *pSignaling)
{
    uint32_t ofs;
    uint32_t idx;
    uint8_t orgId[6] = { 0x00, 0x80, 0xC2, 0x00, 0x00, 0x02 };

    ofs = 0;
    // targetPortIdentity - dummy - "value is 0xFF" (ieee802.1as-2011, 10.5.4.2.1)
    for (idx = 0; idx < 10; ++idx) {
        MEM8(dstaddr + ofs + idx) = 0xFF;
    }
    ofs += idx;
    // TLV type + length
    mem16_htons(dstaddr + ofs + 0, PTP_TLV_TYPE_ORG_EXT);
    mem16_htons(dstaddr + ofs + 2, 12);
    ofs += 4;
    // Organization
    for (idx = 0; idx < 6; ++idx) {
        MEM8(dstaddr + ofs + idx) = orgId[idx];
    }
    ofs += idx;
    // Intervals + flags
    MEM8(dstaddr + ofs + 0) = (uint8_t)(pSignaling->linkDelayInterval);
    MEM8(dstaddr + ofs + 1) = (uint8_t)(pSignaling->timeSyncInterval);
    MEM8(dstaddr + ofs + 2) = (uint8_t)(pSignaling->announceInterval);
    MEM8(dstaddr + ofs + 3) = pSignaling->flags;
    ofs += 4;
    mem16_htons(dstaddr + ofs, 0); // reserved
    ofs += 2;

    return ofs;
}

/**
 * Copy Announce information to memory address
 * @param dstaddr           Destination Address
 * @param pAnnounce         announce information, pointer to struct ptp_announce_s
 * @return                  returns number of written bytes (always 34+8*pathCount)
 */
uint32_t tsn_ptp_set_announce(uintptr_t dstaddr, struct ptp_announce_s *pAnnounce)
{
    uint32_t ofs;
    uint32_t idx;
    uint32_t tmp;
    uint8_t *p8;

    ofs = 0;
    // reserved 10 byte
    for (idx = 0; idx < 5; ++idx) {
        mem16_htons(dstaddr + ofs + idx * 2, 0); // reserved
    }
    ofs += (idx * 2);
    mem16_htons(dstaddr + ofs, (uint16_t)pAnnounce->currentUtcOfs); // currentUtcOfs
    ofs += 2;
    MEM8(dstaddr + ofs) = 0; // reserved
    ofs++;
    MEM8(dstaddr + ofs) = pAnnounce->gmPrio1; // prio1
    ofs++;

    // clockQual
    MEM8(dstaddr + ofs) = pAnnounce->clockQuality.clockClass; // clockQual.clockClass
    ofs++;
    MEM8(dstaddr + ofs) = pAnnounce->clockQuality.clockAccuracy; // clockQual.clockAccuracy
    ofs++;
    // clockQual.ofsScaledLogVariance
    mem16_htons(dstaddr + ofs, pAnnounce->clockQuality.offsetScaledLogVariance);
    ofs += 2;

    MEM8(dstaddr + ofs) = pAnnounce->gmPrio2; // prio2
    ofs++;
    // gmid
    p8 = (uint8_t *)(&pAnnounce->gmIdentity);
    for (idx = 0; idx < 8; ++idx) {
        MEM8(dstaddr + ofs + idx) = p8[idx];
    }
    ofs += 8;
    mem16_htons(dstaddr + ofs, pAnnounce->stepsRemoved);
    ofs += 2;
    MEM8(dstaddr + ofs) = pAnnounce->timeSource;
    ofs++;
    // TLV type + length
    mem16_htons(dstaddr + ofs, PTP_TLV_TYPE_PATHTRACE);
    ofs += 2;
    tmp = pAnnounce->pathCount * PTP_PATHTRACE_SZ;
    mem16_htons(dstaddr + ofs, tmp);
    ofs += 2;
    // copy path (ClockIdentity)
    for (idx = 0; idx < tmp; idx++) {
        MEM8(dstaddr + ofs + idx) = pAnnounce->pPathSequence[idx];
    }
    ofs += idx;
    return ofs;
}

/**
 * Copy Followup information from frame to structure
 * @param addr      address, to followup frame payload
 * @param pFollowup structure, where followup information is written
 * @return          0 on success, -1 on error
 */
int32_t tsn_ptp_get_followup(uintptr_t addr, struct ptp_followup_s *pFollowup)
{
    uint32_t tmp;
    uint32_t idx;

#ifdef TSN_PTP_CFG_FOLLOWUP_TLV_ORG_CHECK
    uint8_t orgId[6] = { 0x00, 0x80, 0xC2, 0x00, 0x00, 0x01 };
#endif

    // check param for NULL pointer
    if (pFollowup == 0)
        return -1;

    // offset = 0
    // precise origin timestamp
    tsn_ptp_get_tstamp(addr, &pFollowup->preciseOriginTimestamp);

    // offset = 10
    // CHECK TLV TYPE/LEN
    tmp = mem16_ntohs(addr + 10); // TLVTYPE
    if (tmp != PTP_TLV_TYPE_ORG_EXT)
        return -1;
    tmp = mem16_ntohs(addr + 12); // TLVLEN (exp 28)
    if (tmp != 28)
        return -1;

        // ORG
#ifdef TSN_PTP_CFG_FOLLOWUP_TLV_ORG_CHECK
    for (idx = 0; idx < 6; ++idx) {
        if (MEM8(addr + idx + 14) != orgId[idx])
            return -1;
    }
#endif // TSN_PTP_CFG_FOLLOWUP_TLV_ORG_CHECK

    // offset = 20
    pFollowup->cumScaledRateOfs = mem32_ntohl(addr + 20); // cumScaledRateOfs
    // offset = 24
    pFollowup->gmTimeBaseInd = mem16_ntohs(addr + 24); // gmTimeBaseIndicator
    // ofs = 26
    pFollowup->lastGmPhaseCh.ns_msb = mem16_ntohs(addr + 26 + 0);
    pFollowup->lastGmPhaseCh.ns_lsb = ((uint64_t)mem32_ntohl(addr + 26 + 2)) << 32;
    pFollowup->lastGmPhaseCh.ns_lsb |= mem32_ntohl(addr + 26 + 6);
    pFollowup->lastGmPhaseCh.fns = mem16_ntohs(addr + 26 + 10);
    // offset = 38
    pFollowup->scaledLastGmFreqCh = mem32_ntohl(addr + 38);

    return 0; // success
}

/**
 * Copy Announce frame information to structure
 * @param addr          pointer to payload address of frame (first byte of reserve-field)
 * @param pAnnounce     pointer to structure
 * @return              0 on success, -1 on error
 */
int32_t tsn_ptp_get_announce(uintptr_t addr, struct ptp_announce_s *pAnnounce)
{
    uint32_t idx;
    uint8_t *p8;
    uint16_t tlvType, tlvLen;

    // basic pointer check
    if (pAnnounce == 0) {
        return -1;
    }

    // reserved (10)
    // currentUtcOfs (2)
    pAnnounce->currentUtcOfs = mem16_ntohs(addr + 10);
    // rsv (1)
    // prio1 (1)
    pAnnounce->gmPrio1 = MEM8(addr + 13);
    // clock quality (4)
    pAnnounce->clockQuality.clockClass = MEM8(addr + 14);
    pAnnounce->clockQuality.clockAccuracy = MEM8(addr + 15);
    pAnnounce->clockQuality.offsetScaledLogVariance = mem16_ntohs(addr + 16);
    ;
    // prio2 (1)
    pAnnounce->gmPrio2 = MEM8(addr + 18);
    // gmid (8)
    p8 = (uint8_t *)(&pAnnounce->gmIdentity);
    for (idx = 0; idx < 8; ++idx) {
        p8[idx] = MEM8(addr + 19 + idx);
    }
    // stepsRemoved (2)
    pAnnounce->stepsRemoved = mem16_ntohs(addr + 27);
    // timesource (1)
    pAnnounce->timeSource = MEM8(addr + 29);
    // pathtrace TLV type/len (2/2)
    tlvType = mem16_ntohs(addr + 30);
    tlvLen = mem16_ntohs(addr + 32);

    if (tlvType == PTP_TLV_TYPE_PATHTRACE) {
        pAnnounce->pPathSequence = (uint8_t *)(addr + 34);
        pAnnounce->pathCount = tlvLen / PTP_PATHTRACE_SZ;
    } else {
        pAnnounce->pPathSequence = 0;
        pAnnounce->pathCount = 0;
    }

    return 0; // success
}

/**
 * Compare two SystemIdentities A and B
 * @param pSysIdA   pointer to SystemIdentitiy A
 * @param pSysIdB   pointer to SystemIdentitiy B
 * @return          0 when equal, else -1 if (A < B), else +1 if (A > B)
 */
int32_t tsn_ptp_cmp_systemid(ptp_system_id_t *pSysIdA, ptp_system_id_t *pSysIdB)
{
    // 10.3.2
    // a)
    if (pSysIdA->prio1 < pSysIdB->prio1)
        return -1;
    if (pSysIdA->prio1 > pSysIdB->prio1)
        return 1;
    // b)
    if (pSysIdA->clockQuality.clockClass < pSysIdB->clockQuality.clockClass)
        return -1;
    if (pSysIdA->clockQuality.clockClass > pSysIdB->clockQuality.clockClass)
        return 1;
    // c)
    if (pSysIdA->clockQuality.clockAccuracy < pSysIdB->clockQuality.clockAccuracy)
        return -1;
    if (pSysIdA->clockQuality.clockAccuracy > pSysIdB->clockQuality.clockAccuracy)
        return 1;
    // d)
    if (pSysIdA->clockQuality.offsetScaledLogVariance < pSysIdB->clockQuality.offsetScaledLogVariance)
        return -1;
    if (pSysIdA->clockQuality.offsetScaledLogVariance > pSysIdB->clockQuality.offsetScaledLogVariance)
        return 1;
    // e)
    if (pSysIdA->prio2 < pSysIdB->prio2)
        return -1;
    if (pSysIdA->prio2 > pSysIdB->prio2)
        return 1;
    // f
    if (pSysIdA->clockIdentity < pSysIdB->clockIdentity)
        return -1;
    if (pSysIdA->clockIdentity > pSysIdB->clockIdentity)
        return 1;

    return 0;
}

/**
 * Compare two priority vectors A and B
 * A is better than B when (A < B)
 *
 * @param pPrioA    pointer to priority vector A
 * @param pPrioB    pointer to priority vector B
 * @return          0 when equal, else -1 if (A < B), else +1 if (A > B)
 */
int32_t tsn_ptp_cmp_priovect(ptp_prio_vect_t *pPrioA, ptp_prio_vect_t *pPrioB)
{
    int32_t cmp;

    // 10.3.5
    // cmp systemId
    cmp = tsn_ptp_cmp_systemid(&(pPrioA->rootSystemId), &(pPrioB->rootSystemId));
    if (cmp != 0)
        return cmp;
    // cmp stepsRemoved
    if (pPrioA->stepsRemoved < pPrioB->stepsRemoved)
        return -1;
    if (pPrioA->stepsRemoved > pPrioB->stepsRemoved)
        return 1;
    // cmp sourcePortId
    if (pPrioA->sourcePortId.clockIdentity < pPrioB->sourcePortId.clockIdentity)
        return -1;
    if (pPrioA->sourcePortId.clockIdentity > pPrioB->sourcePortId.clockIdentity)
        return 1;
    if (pPrioA->sourcePortId.portNumber < pPrioB->sourcePortId.portNumber)
        return -1;
    if (pPrioA->sourcePortId.portNumber > pPrioB->sourcePortId.portNumber)
        return 1;
    // cmp portNum
    if (pPrioA->portNumber < pPrioB->portNumber)
        return -1;
    if (pPrioA->portNumber > pPrioB->portNumber)
        return 1;

    return 0;
}
