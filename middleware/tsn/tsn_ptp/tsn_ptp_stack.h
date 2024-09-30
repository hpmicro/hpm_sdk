/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file    tsn_ptp_stack.h
 * \author  zimmerli
 * \date    2019-01-17
 * \brief   IEEE 802.1as software protocol stack (header)
 *
 */
#ifndef TSN_PTP_PROTO_H_
#define TSN_PTP_PROTO_H_

#include "tsn_ptp.h"

/**
 * \brief Timer event enumeration
 *
 * Timer index used by functions hookSetTimer and tsn_ptp_stack_timerevent
 */
typedef enum ptp_timer_event {
    PTP_TMR_EVENT_TXPDREQ = 0,
    PTP_TMR_EVENT_TXANNC = 1,
    PTP_TMR_EVENT_TXSYNC = 2,
    PTP_TMR_EVENT_RXANNC = 3,
    PTP_TMR_EVENT_RXSYNC = 4,
} ptp_timer_event_t;

/**
 * \brief Intsance status event enumeration
 *
 */
typedef enum {
    PTP_STS_EVENT_SYNCLOST,
    PTP_STS_EVENT_SYNCRECV,
    PTP_STS_EVENT_PHASE_CHANGE,
    PTP_STS_EVENT_FREQ_CHANGE
} ptp_status_event_t;

/**
 * \brief Per-Port global variables
 * IEEE 802.1as 10.2.4
 * no delayAsymmetry (10.2.4.8)
 * no pttPortEnable
 * neighborPropDelay accuracy and range large enough using uint32_t
 */
typedef struct {
    uint8_t asCapable;                //!< bool
    uint8_t enabled;                  //!< bool
    int8_t curLogSyncInterval;        //!< currentLogSyncInterval
    int32_t neighborRateOfs32;        //!< (neighborRateRatio-1.0)<<32
    uint32_t meanLinkDelay;           //!< PeerDelay in ns
    uint8_t computeNeighborRateRatio; //!< bool
    uint8_t computeMeanLinkDelay;     //!< bool
    uint16_t thisPort;                //!< port number
    uint8_t selectedRole;             //!< port role
    port_id_t thisPortId;             //!< port identity
    uint8_t delayMechanism;           //!< delayMechanism IEEE 802.1as-2020 14.8.5
    uint16_t portIndex;               //!< internal port index variable
    uint8_t rcvdPortStateInd;         //!< bool enabled external port role
    uint8_t rcvdPortState;            //!< external port role
} port_glob_t;

/**
 * \brief Media-Dependent global variables
 *
 * IEEE 802.1as 11.2.12
 * initialLogPdelayReqInterval: fixed constant
 * pdelayReqInterval: calc from curLogPdelayReqInterval
 * allowedLostResponses: fixed constant
 * allowedFaults: fixed constatnt
 */
typedef struct {
    int8_t curLogPdelayReqInterval; //!< curLogPdelayReqInterval
    uint8_t isMeasuringDelay;       //!< bool, measunring PeerDelay
    uint32_t meanLinkDelayTresh;    //!< PerrDelay threshold in ns
    uint16_t syncSeqId;             //!< syncSeqId
    uint8_t detectedFaults;         //!< detectedFaults
    uint8_t asCapableAcrossDomains; //!< bool, AsCapable for all domains
} md_glob_t;

/**
 * \brief Pdelay-response-statemachine internals
 */
typedef struct {
    uint16_t seqid;      //!< sequenceID
    port_id_t srcPortId; //!< sourcePortIdentity
} md_pdelay_resp_sm;

/**
 * \brief Pdelay-request-statemachine internals
 */
typedef struct {
    uint16_t seqid;                 //!< sequence id
    uint64_t time_local_txreq;      //!< t1 - pdelayReqEventEgressTimestamp
    uint64_t time_local_rxresp;     //!< t4 - pdelayRespEventIngressTimestamp
    uint64_t time_peer_rxreq;       //!< t2
    uint64_t time_peer_txresp;      //!< t3
    uint64_t saved_local_rxresp;    //!< helper for calc neighbor rate ratio
    uint64_t saved_peer_txresp;     //!< helper for calc neighbor rate ratio
    uint8_t measIsRunning;          //!< bool
    uint16_t lostResponses;         //!< lostResponses counter
    uint8_t neighborRateRatioValid; //!< bool
    uint8_t eventflags;             //!< events, control of calculation
} md_pdelay_req_sm;

/**
 * \brief Sync-Receive-statemachine internals
 */
typedef struct {
    uint16_t seqid;       //!< sequenceId
    uint8_t waitFup;      //!< waiting_for_follow_up
    port_sync_sync_t pss; //!< PortSyncSync structure to PortSyncRecv sm
} md_sync_receive_sm;

/**
 * \brief Sync-Send-statemachine internals
 */
typedef struct {
    uint16_t seqid;       //!< next sequence id
    port_sync_sync_t pss; //!< PortSyncSync structure
} md_sync_send_sm;

/**
 * \brief Time informations
 */
typedef struct {
    uint16_t flags;     //!< leap59, leap61, curUtcOfsVal, timeTrace, freqTrace
    int16_t curUtcOfs;  //!< difference (TAI - UTC) in seconds
    uint8_t timeSource; //!< timeSource, see enum ptp_timesource_enum
} time_info_t;

/**
 * Maximum announce message payload words (uint32)
 */
#define PTP_ANNC_PAYLOAD_SZ_U32 (((PTP_PAYLOAD_SZ_ANNOUNCE + 8 * TSN_PTP_CFG_PATHTRACE_MAX) + 3) / 4)

/**
 * \brief Announce Message Information
 */
typedef struct {
    uint8_t newInfo;                              //!< boolean flag
    uint8_t infoIs;                               //!< disabled, aged, mine, received
    ptp_prio_vect_t portPriority;                 //!< port priority vector
    uint16_t portStepsRemoved;                    //!< stepsRemoved
    time_info_t annInfo;                          //!< annInfo
    uint16_t txSeqId;                             //!< txSeqId
    int8_t curLogInterval;                        //!< curLogInterval
    uint32_t payloadBuf[PTP_ANNC_PAYLOAD_SZ_U32]; //!< only used for building announce payload
} annc_info_sm;

/**
 * \brief Per-Port strcuture
 */
typedef struct {
    port_glob_t port;              //!< port variables
    md_glob_t md;                  //!< media-dependent variables
    md_pdelay_resp_sm mdPdResp;    //!< mdPdResp
    md_pdelay_req_sm mdPdReq;      //!< mdPdReq
    md_sync_receive_sm mdSyncRecv; //!< mdSyncRecv
    md_sync_send_sm mdSyncSend;    //!< mdSyncSend
    annc_info_sm anncInfo;         //!< anncInfo
} tsn_ptp_port_t;

/**
 * \brief LocalClock strcuture
 */
typedef struct {
    uint64_t offset_ns;
    scaled_ns_t lastPhaseChange;
    int32_t rateOfs;
    uint32_t fractOfs;
    uint64_t last_localtime;
} tsn_ptp_clock_t;

/**
 * \brief SiteSync strcuture
 */
typedef struct {
    uint8_t hasSavedSync;
    int32_t gmRateOfs; // (ratio-1.0)<<32
    int64_t savedSyncReceiptTime;
    int64_t savedSyncReceiptLocalTime;
    scaled_ns_t clkSrcPhOfs;
    uint16_t clkTimeBaseInd;
    int32_t clkFrqOfsScaled;
    int32_t gmOffset;
    int32_t gmOffsetSum;
    int8_t logSyncIval;
    uint8_t hasSyncRcvd;
} tsn_ptp_sync_t;

/**
 * \brief PTP Stack main management structure
 *
 * includes time-aware system global variables, 10.2.3
 */
typedef struct {
    void *cbParam; //!< callback parameter for hook functions
    void (*hookTxFrame)(void *cbParam, uint16_t portIndex, struct ptp_header_s *pPtpHeader, uint8_t *pPayload,
                        uint32_t payloadLen); //!< hook, called for sending frame
    void (*hookGetLocalTime)(void *cbParam, ptp_timestamp_t *localtime);
    void (*hookGetLocalOffset)(void *cbParam, ptp_timestamp_t *offset);
    void (*hookStatusEvent)(void *cbParam, ptp_status_event_t event, void *param);
    void (*hookSetTimer)(void *cbParam, ptp_timer_event_t timer, uint32_t id, int8_t logInterval,
                         uint8_t mult);            //!< hook, called for settingh timer; logInterval 127: disable
    clock_id thisClock;                            //!< clockIdentity of system, 10.2.3.22
    tsn_ptp_port_t port[TSN_PTP_CFG_PORTNUM];      //!< port structures
    clock_id pathTrace[TSN_PTP_CFG_PATHTRACE_MAX]; //!< pathTrace, 10.3.8.21
    uint32_t pathTraceCount;                       //!< pathTraceCount
    ptp_system_id_t systemId;                      //!< systemId
    ptp_prio_vect_t gmPriority;                    //!< current GM priority vector
    uint16_t gmChanges;                            //!< counter, incrementing when GM changes
    uint16_t masterStepsRemoved;                   //!< masterStepsRemoved
    time_info_t sysInfo;                           //!< sysInfo
    time_info_t announceInfo;                      //!< announceInfo
    uint8_t selectedRole0;                         //!< role of system, see enum ptp_port_role_enum
    uint8_t domainNumber;                          //!< domain number of PTP instance
    tsn_ptp_sync_t siteSync;                       //!< sync structure
    tsn_ptp_clock_t localClock;                    //!< local clock structure of PTP instance
    uint8_t enableInvoke;                          //!< enable the status timing invoke calls
} tsn_ptp_instance_t;

/**
 * TSN PTP Stack initialization
 * Must be called before first use of stack
 * @param pStack        pointer to stack
 * @param pMacAddr      pointer to MAC address (uint8_t[6])
 * @param domainNumber  domain number of the PTP instance
 */
void tsn_ptp_stack_init(tsn_ptp_instance_t *pStack, uint8_t *pMacAddr, uint8_t domainNumber);

/**
 * Perform action when a PTP frame is received on a port
 *
 * @param pStack        pointer to PTP stack
 * @param portIndex     port index (0 .. N-1)
 * @param hdrAddr       address of start of PTP header of received message
 * @param length        length of the PTP frame (valid data after hdrAddr)
 * @param pRxTime       pointer to ingressTimestamp = timestamp to reference plane (timestamp corrected by ingress latency)
 */
void tsn_ptp_stack_rxframe(tsn_ptp_instance_t *pStack, uint16_t portIndex, uintptr_t hdrAddr, uint32_t length, ptp_timestamp_t *pRxTime);

/**
 * Call when transmission of a PTP frame is done and a tx timestamp is available.
 * The provided timestamp shall already be corrected by egress latency!
 *
 * @param pPort         pointer to the protocol structure of the port
 * @param msgType       PTP message type of the transmitted frame
 * @param pTxTime       pointer to tx timestamp (ptp_timestamp_t)
 */
void tsn_ptp_stack_txdone(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, uint8_t msgType, ptp_timestamp_t *pTxTime);

/**
 * Port role reselection
 * Must be run after hookReselectRoles has been called by any port
 *
 * @param pStack        pointer to stack
 */
void tsn_ptp_stack_portroleselection(tsn_ptp_instance_t *pStack);

/**
 * Timer Event
 *
 * @param pStack        pointer to PTP stack
 * @param tmrEvent      event enumeration
 * @param id            identifier is provided by hookSetTimer callback
 */
void tsn_ptp_stack_timerevent(tsn_ptp_instance_t *pStack, ptp_timer_event_t tmrEvent, uint32_t id);

/**
 * Enable or Disable Port
 * Enabling calls timer init hooks
 *
 * @param pStack        pointer to PTP stack
 * @param portIndex     index of port (0 .. N-1)
 * @param bEnable       boolean, 0 - disable, otherwise enable
 */
void tsn_ptp_stack_port_enable(tsn_ptp_instance_t *pStack, uint16_t portIndex, uint16_t bEnable);

/**
 * Send PortSyncSync structure
 * called by SiteSyncSync state machine, when Sync was received
 *
 * @param pStack    pointer to PTP stack
 * @param pPSS      pointer to PortSyncSync structure (as received by hookRecvPortSyncSync)
 */
void tsn_ptp_stack_send_pss(tsn_ptp_instance_t *pStack, port_sync_sync_t *pPSS);

uint8_t tsn_ptp_msg(uintptr_t hdrAddr);
uint8_t tsn_ptp_domain(uintptr_t hdrAddr);
uint64_t tsn_ptp_clock_offset(tsn_ptp_instance_t *pStack);
uint64_t tsn_ptp_time_get(tsn_ptp_instance_t *pStack);
void tsn_ptp_stack_enable(tsn_ptp_instance_t *pStack, uint16_t bEnable);

#endif /* TSN_PTP_PROTO_H_ */
