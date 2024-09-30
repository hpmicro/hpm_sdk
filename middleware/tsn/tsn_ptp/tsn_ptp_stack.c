/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file    tsn_ptp_stack.c
 * \author  zimmerli
 * \date    2019-01-17
 * \brief   IEEE 802.1as software protocol stack
 *
 */

#include "tsn_ptp_stack.h"

#define SWAP16(value) (((value & 0xFF) << 8 | (value >> 8)) & 0xFFFF)

#define PTPHDR_OFS_MSGID    0
#define PTPHDR_OFS_MSGLEN   2
#define PTPHDR_OFS_DOMAIN   4
#define PTPHDR_OFS_FLAGS    6
#define PTPHDR_OFS_CORFIELD 8
#define PTPHDR_OFS_SPID     20
#define PTPHDR_OFS_SEQID    30
#define PTPHDR_OFS_LOGIVAL  33

#define PTPHDR_OFS_FUP_TLV       (34 + 10)
#define PTPHDR_OFS_ANNC_STEP_REM (34 + 27)

#define NS_PER_SEC (1000000000LL)

// PDelay-Request State-Machine events
// do calculation of Pdelay, neighbor rate ratio only on complete event list
#define PDREQ_FLAG_TXDONE   (1 << 0)
#define PDREQ_FLAG_RXRESP   (1 << 1)
#define PDREQ_FLAG_RXRSPFUP (1 << 2)
#define PDREQ_MASK_COMPLETE (0x7)

// Delay mechanism defines IEEE 802.1as-2020 14.8.5
#define PTP_DELAY_MECH_P2P    2
#define PTP_DELAY_MECH_COMP2P 3

enum announce_info_is_e {
    INFO_IS_DISABLED = 0,
    INFO_IS_MINE = 1,
    INFO_IS_RECEIVED = 2,
    INFO_IS_AGED = 3
};

// ----------------------------------------------------------------
// declarations
static struct ptp_header_s _ptp_default_header = {
    // set default header information
    .majorSdoId = PTP_MAJOR_SDO_ID,
    .minorSdoId = PTP_MINOR_SDO_ID,
    .domainNumber = 0,
    .minorVersionPTP = 1,
    .version = PTP_VERSION,
    .correctionField = 0,
    .sequenceId = 0,
    .sourcePortId = { 0, 0 },
    .msgType = 0,
    .msgLen = PTP_HEADER_SZ,
    .flags = PTP_FLAG_PTP_TIMESCALE,
    .control = PTP_CONTROL_OTHER,
    .logMsgInterval = 0x7F
};

// ----------------------------------------------------------------
// function declarations
static uint16_t ptp_get_seqid(uintptr_t hdraddr);
static void ptp_get_corfield(uintptr_t hdraddr, int64_t *pCorrectionField);

// state machine functions
static void mdpdelayresp_rcvdPdelayReq(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, uintptr_t hdrAddr, ptp_timestamp_t *pRxTime);
static void mdpdelayresp_rcvdMDTimestampReceive(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, ptp_timestamp_t *pTxTime);
static void mdpdelayreq_rcvdPdelayResp(tsn_ptp_port_t *pProtocol, uintptr_t hdrAddr, ptp_timestamp_t *pRxTime);
static void mdpdelayreq_rcvdPdelayRespFup(tsn_ptp_port_t *pProtocol, uintptr_t hdrAddr);
static void mdpdelayreq_calculate(tsn_ptp_port_t *pProtocol);
static void mdpdelayreq_rcvdMDTimestampReceive(tsn_ptp_port_t *pProtocol, ptp_timestamp_t *pTxTime);
static void mdpdelayreq_pdelayIntervalTimer(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort);
static void mdsyncreceive_rcvdSync(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pProtocol, uintptr_t hdrAddr, ptp_timestamp_t *pRxTime);
static void mdsyncreceive_rcvdFollowup(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pProtocol, uintptr_t hdrAddr);
static void portsyncsyncrecv_rcvdMdSync(tsn_ptp_instance_t *pStack, port_sync_sync_t *pPSS);
static void portannouncerecv(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, uintptr_t hdrAddr);
static void portannouncerecv_timeout(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort);
static void portannouncesend(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort);
static void portsyncsyncsend(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, port_sync_sync_t *pPSS);
static void mdsyncsend_rcvdMDTimestampReceive(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, ptp_timestamp_t *pTxTime);

static void siteSyncIntervalTimer(tsn_ptp_instance_t *pStack);
static void siteSyncPssHandle(tsn_ptp_instance_t *pStack, port_sync_sync_t *pPSS);
static void siteSyncPssReceive(tsn_ptp_instance_t *pStack, port_sync_sync_t *pPSS);

static uint64_t tsn_ptp_clock_get_offset(tsn_ptp_clock_t *domain_clk, uint64_t localtime);
static uint64_t ptp_timestamp_to_ns(ptp_timestamp_t *ts);

// static void signalingrecv(tsn_ptp_port_t *pPort, uintptr_t hdrAddr);

// ----------------------------------------------------------------
void tsn_ptp_stack_init(tsn_ptp_instance_t *pStack, uint8_t *pMacAddr, uint8_t domainNumber)
{
    uint32_t idx;
    tsn_ptp_port_t *port;
    uint16_t pnum; // port number

    // set clock identity
    tsn_ptp_mac_to_clockid(pMacAddr, (uint8_t *)(&pStack->thisClock));

    // init hook defaults
    pStack->cbParam = 0;
    pStack->hookSetTimer = 0;
    pStack->hookGetLocalTime = 0;
    pStack->hookGetLocalOffset = 0;
    pStack->hookStatusEvent = 0;
    pStack->hookTxFrame = 0;

    // init system -> default values
    pStack->systemId.prio1 = 254;
    pStack->systemId.prio2 = 255;
    pStack->systemId.clockQuality.clockClass = 248;
    pStack->systemId.clockQuality.clockAccuracy = 254;
    pStack->systemId.clockQuality.offsetScaledLogVariance = 17258;
    pStack->systemId.clockIdentity = pStack->thisClock;

    pStack->gmPriority.rootSystemId = pStack->systemId;
    pStack->gmPriority.stepsRemoved = 0;
    pStack->gmPriority.sourcePortId.clockIdentity = pStack->thisClock;
    pStack->gmPriority.sourcePortId.portNumber = 0;
    pStack->gmPriority.portNumber = 0;
    pStack->masterStepsRemoved = 0;
    pStack->gmChanges = 0;

    pStack->pathTrace[0] = pStack->thisClock;
    pStack->pathTraceCount = 1;
    pStack->selectedRole0 = PTP_PORT_ROLE_SLAVE; // default: system clock is slave

    // system flags and fields - defaults
    pStack->sysInfo.curUtcOfs = 37; // latest known value - 2017-01-01
    pStack->sysInfo.timeSource = PTP_SRC_OTHER;
    pStack->sysInfo.flags = PTP_FLAG_TIME_TRACEABLE | PTP_FLAG_FREQ_TRACEABLE; // leap59/61 unknown, utc valid unknown
    pStack->announceInfo = pStack->sysInfo;

    pStack->domainNumber = domainNumber;
    pStack->enableInvoke = 0;

    // SiteSync initialization
    pStack->siteSync.gmRateOfs = 0;
    pStack->siteSync.hasSavedSync = 0;
    pStack->siteSync.hasSyncRcvd = 0;
    pStack->siteSync.clkSrcPhOfs.fns = 0;
    pStack->siteSync.clkSrcPhOfs.ns_msb = 0;
    pStack->siteSync.clkSrcPhOfs.ns_lsb = 0;
    pStack->siteSync.clkTimeBaseInd = 0;
    pStack->siteSync.clkFrqOfsScaled = 0;
    pStack->siteSync.gmOffset = 0;
    pStack->siteSync.gmOffsetSum = 0;
    pStack->siteSync.logSyncIval = PTP_INIT_INTVAL_SYNC;

    // LocalClock initialization
    pStack->localClock.offset_ns = 0;
    pStack->localClock.rateOfs = 0;
    pStack->localClock.fractOfs = 0;

    // init ports
    for (idx = 0; idx < TSN_PTP_CFG_PORTNUM; ++idx) {
        port = &(pStack->port[idx]);
        pnum = idx + 1; // Port number: 1 .. N

        // port
        port->port.thisPort = pnum; // Port number: 1 .. N
        port->port.thisPortId.clockIdentity = pStack->thisClock;
        port->port.thisPortId.portNumber = pnum;
        port->port.asCapable = 1;
        port->port.enabled = 0;
        port->port.curLogSyncInterval = PTP_INIT_INTVAL_SYNC;
        port->port.neighborRateOfs32 = 0;
        port->port.portIndex = idx;

        port->port.meanLinkDelay = 0;
        port->port.computeNeighborRateRatio = 1;
        port->port.computeMeanLinkDelay = 1;
        port->port.selectedRole = PTP_PORT_ROLE_DISABLED;
        port->port.delayMechanism = PTP_DELAY_MECH_P2P;
        port->port.rcvdPortStateInd = 0;
        port->port.rcvdPortState = PTP_PORT_ROLE_DISABLED;

        // md
        port->md.curLogPdelayReqInterval = PTP_INIT_INTVAL_PDELAY;
        port->md.isMeasuringDelay = 0;
        port->md.meanLinkDelayTresh = 10 * 1000; // init as 10us fixed
        port->md.syncSeqId = 0x1234;             // random
        port->md.asCapableAcrossDomains = 0;

        // init mdPdReqSm
        port->mdPdReq.seqid = 0x2345; // random
        port->mdPdReq.measIsRunning = 0;
        port->mdPdReq.lostResponses = 0;
        port->mdPdReq.neighborRateRatioValid = 0;
        port->mdPdReq.saved_local_rxresp = 0;
        port->mdPdReq.saved_peer_txresp = 0;
        port->mdPdReq.eventflags = 0;
        // init mdPdRespSm
        port->mdPdResp.seqid = 0;
        // init mdSyncRecv
        port->mdSyncRecv.seqid = 0;
        port->mdSyncRecv.waitFup = 0;
        // init mdSyncSend
        port->mdSyncSend.seqid = 0x6789; // random

        // init anncInfo (see IEEE 802.1as 10.3.4)
        port->anncInfo.newInfo = 0;
        port->anncInfo.infoIs = INFO_IS_DISABLED;
        port->anncInfo.txSeqId = 0x4567; // random
        port->anncInfo.curLogInterval = PTP_INIT_INTVAL_ANNC;
    }
}

void tsn_ptp_stack_enable(tsn_ptp_instance_t *pStack, uint16_t bEnable)
{
    if (bEnable) {
        // timer hooks - enable, defaults
        if (pStack->hookSetTimer != 0) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXSYNC, 0, pStack->siteSync.logSyncIval, 1);
        }
    } else {
        // timer hooks - disable (127)
        if (pStack->hookSetTimer != 0) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXSYNC, 0, 127, 1);
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_RXSYNC, 0, 127, 1);
        }
    }
}

void tsn_ptp_stack_rxframe(tsn_ptp_instance_t *pStack, uint16_t portIndex, uintptr_t hdrAddr, uint32_t length, ptp_timestamp_t *pRxTime)
{
    (void)length;
    (void)pStack;

    uint8_t msgType;
    tsn_ptp_port_t *pPort;

    if (portIndex >= TSN_PTP_CFG_PORTNUM)
        return;
    pPort = &(pStack->port[portIndex]);
    if (!pPort->port.enabled)
        return;

    // get msgType and sequence id
    msgType = (*(uint8_t *)(hdrAddr + PTPHDR_OFS_MSGID));
    msgType &= PTP_MSGTYPE_MASK;

    if (msgType == PTP_MSGTYPE_SYNC) {
        // MDSyncReceive state machine: IEEE 802.1as 11.2.13
        mdsyncreceive_rcvdSync(pStack, pPort, hdrAddr, pRxTime);
    } else if (msgType == PTP_MSGTYPE_PDELAY_REQ) {
        // MDPdelayResp state machine: IEEE 802.1as 11.2.16
        mdpdelayresp_rcvdPdelayReq(pStack, pPort, hdrAddr, pRxTime);
    } else if (msgType == PTP_MSGTYPE_PDELAY_RESP) {
        // MDPdelayResp state machine: IEEE 802.1as 11.2.15
        mdpdelayreq_rcvdPdelayResp(pPort, hdrAddr, pRxTime);
    } else if (msgType == PTP_MSGTYPE_FOLLOWUP) {
        // MDSyncReceive state machine: IEEE 802.1as 11.2.13
        mdsyncreceive_rcvdFollowup(pStack, pPort, hdrAddr);
    } else if (msgType == PTP_MSGTYPE_PDELAY_FUP) {
        // MDPdelayResp state machine: IEEE 802.1as 11.2.15
        mdpdelayreq_rcvdPdelayRespFup(pPort, hdrAddr);

        //TODO: optimize according IEEE 802.1as-2020 11.2.2
        if (pPort->md.asCapableAcrossDomains) // && pStack->domainNumber == 0)
            pPort->port.asCapable = 1;

    } else if (msgType == PTP_MSGTYPE_ANNOUNCE) {
        // PortAnnounceReceive:     IEEE 802.1as 10.3.10
        // PortAnnounceInformation: IEEE 802.1as 10.3.11
        portannouncerecv(pStack, pPort, hdrAddr);
    } else if (msgType == PTP_MSGTYPE_SIGNALING) {
        // TODO: LinkDelayIntervalSetting : IEEE 802.1as 11.2.17
        // TODO: AnnounceIntervalSetting  : IEEE 802.1as 10.3.14
        // signalingrecv(pPort, hdrAddr);
    }
}

void tsn_ptp_stack_txdone(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, uint8_t msgType, ptp_timestamp_t *pTxTime)
{
    if (msgType == PTP_MSGTYPE_SYNC) {
        // MDSyncSend 11.2.14
        mdsyncsend_rcvdMDTimestampReceive(pStack, pPort, pTxTime);
    } else if (msgType == PTP_MSGTYPE_PDELAY_RESP) {
        // MDPdelayResp state machine: IEEE 802.1as 11.2.16
        mdpdelayresp_rcvdMDTimestampReceive(pStack, pPort, pTxTime);
    } else if (msgType == PTP_MSGTYPE_PDELAY_REQ) {
        // MDPdelayReq state machine: IEEE 802.1as 11.2.15
        mdpdelayreq_rcvdMDTimestampReceive(pPort, pTxTime);
    }
}

void tsn_ptp_stack_portroleselection(tsn_ptp_instance_t *pStack)
{
    uint32_t idx;
    int32_t cmp;
    ptp_prio_vect_t gmPriorityVector;
    ptp_prio_vect_t prioVect; // used as gmPathPriorityVector and masterPriorityVector
    tsn_ptp_port_t *port;
    time_info_t tmInfo;
    uint16_t portNum;
    uint8_t updtInfo; // used as bool

    // clearReselectTree(); not needed
    // setSelectedTree(); not needed - as port update is done within this function

    // updtRolesTree(); IEEE802.1as: 10.3.12.1.4
    // a) gmPathPrioVect for every port

    // SEARCH BEST GM from set <system, port 1 .. N>
    // only use ports, where infoIs received
    // init: use system as GM
    portNum = 0;
    gmPriorityVector.rootSystemId = pStack->systemId;
    gmPriorityVector.stepsRemoved = 0;
    gmPriorityVector.sourcePortId.clockIdentity = pStack->thisClock;
    gmPriorityVector.sourcePortId.portNumber = 1; //0;
    gmPriorityVector.portNumber = 0;
    tmInfo = pStack->sysInfo;
    for (idx = 0; idx < TSN_PTP_CFG_PORTNUM; ++idx) {
        // port pointer
        port = &(pStack->port[idx]);

        // check info is received; but not my own clockId
        if ((port->anncInfo.infoIs == INFO_IS_RECEIVED) && (port->anncInfo.portPriority.sourcePortId.clockIdentity != pStack->thisClock)) {
            // gmPathPrioVect (is prioVect)
            prioVect = port->anncInfo.portPriority;
            prioVect.stepsRemoved += 1; // increment stepsRemoved, see 10.3.5

            // compare gmPathPrio and current gmPriorityVector
            cmp = tsn_ptp_cmp_priovect(&prioVect, &gmPriorityVector);
            if (cmp < 0) {
                // received priority vector from port is better than actual vector
                // copy to gmPriorityVector, update portNum, copy time information (flags, ...)
                gmPriorityVector = prioVect;
                portNum = port->port.thisPort;
                tmInfo = port->anncInfo.annInfo;
            }

        } // if
    } // for

    // check for GM change --> increment counter
    if (pStack->gmPriority.rootSystemId.clockIdentity != gmPriorityVector.rootSystemId.clockIdentity) {
        pStack->gmChanges++;
    }

    // b) gmPriority = gmPriorityVector
    pStack->gmPriority = gmPriorityVector;

    // c) set global vars&fields for announce (from Slave-Port or System)
    pStack->announceInfo = tmInfo;

    // d) compute masterPrioVect for each port
    // masterPrioVect: is prioVect
    prioVect.rootSystemId = gmPriorityVector.rootSystemId;
    prioVect.stepsRemoved = gmPriorityVector.stepsRemoved;
    prioVect.sourcePortId.clockIdentity = pStack->thisClock;

    // e) compute masterStepsRemoved (portStepsRem + 1 | 0)
    // same as gmPriorityVector.stepsRemoved
    pStack->masterStepsRemoved = gmPriorityVector.stepsRemoved;

    // f) update roles
    for (idx = 0; idx < TSN_PTP_CFG_PORTNUM; ++idx) {
        // port pointer
        port = &(pStack->port[idx]);
        updtInfo = 0;

        // IEEE802.1as-2020: 10.3.15
        if (port->port.rcvdPortStateInd) {
            port->port.selectedRole = port->port.rcvdPortState;
            continue;
        }

        // d) compute masterPrioVect for each port
        // masterPrioVect: is prioVect
        prioVect.portNumber = port->port.thisPort;
        prioVect.sourcePortId.portNumber = port->port.thisPort;

        if (port->anncInfo.infoIs == INFO_IS_DISABLED) {
            // f3) infois==disabled --> role = disabledport
            port->port.selectedRole = PTP_PORT_ROLE_DISABLED;
        } else if (port->anncInfo.infoIs == INFO_IS_AGED) {
            // f4) infois==aged --> role = master + updtInfo
            port->port.selectedRole = PTP_PORT_ROLE_MASTER;
            updtInfo = 1;
        } else if (port->anncInfo.infoIs == INFO_IS_MINE) {
            // f5) infois==mine --> role = master + updtInfo
            port->port.selectedRole = PTP_PORT_ROLE_MASTER;
            updtInfo = 1;
        } else if (port->anncInfo.infoIs == INFO_IS_RECEIVED) {
            // f6-9) infois==received

            if (port->port.thisPort == portNum) {
                // f6) gmPrio derived from port --> slave
                port->port.selectedRole = PTP_PORT_ROLE_SLAVE;
                updtInfo = 0;
            } else {
                if (tsn_ptp_cmp_priovect(&prioVect, &(port->anncInfo.portPriority)) < 0) {
                    // f9) masterPrio better then portPrio --> master
                    port->port.selectedRole = PTP_PORT_ROLE_MASTER;
                    updtInfo = 1;
                } else {
                    // f7-8) else --> passive
                    port->port.selectedRole = PTP_PORT_ROLE_PASSIVE;
                }
            }
        }

        if (updtInfo) {
            // update info per port (when updtInfo==TRUE)
            // portPrio = masterPrio, portStepsRem = masterStepRem, infoIs=mine, newInfo=true
            port->anncInfo.portPriority = prioVect;
            port->anncInfo.portStepsRemoved = pStack->masterStepsRemoved;
            port->anncInfo.infoIs = INFO_IS_MINE;
            port->anncInfo.newInfo = 1;
        }
    }
    // g) SKIPPED gmPresent true if gmPrio1 < 255
    // h) role port0: any slave-port --> role = passive; else role = slave
    if (portNum > 0) {
        pStack->selectedRole0 = PTP_PORT_ROLE_PASSIVE;
    } else {
        pStack->selectedRole0 = PTP_PORT_ROLE_SLAVE;
    }
    // i) set pathtrace if grandmaster (gmPrio.sys.clkid == thisClock)
    if (gmPriorityVector.rootSystemId.clockIdentity == pStack->thisClock) {
        pStack->pathTrace[0] = pStack->thisClock;
        pStack->pathTraceCount = 1;
    }

    // all master-ports -> set tx timer announce (immediately)
    for (idx = 0; idx < TSN_PTP_CFG_PORTNUM; ++idx) {
        // port pointer
        port = &(pStack->port[idx]);

        if (pStack->hookSetTimer != 0 && port->port.enabled) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXANNC, idx, port->anncInfo.curLogInterval, 0);
        }
    }
}

//TODO: split timerevent to disable portIndex for site timer event
void tsn_ptp_stack_timerevent(tsn_ptp_instance_t *pStack, ptp_timer_event_t tmrEvent, uint32_t id)
{
    tsn_ptp_port_t *pPort;

    if (tmrEvent == PTP_TMR_EVENT_TXSYNC) {
        if (pStack->hookSetTimer != 0) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXSYNC, 0, pStack->siteSync.logSyncIval, 1);
        }
        siteSyncIntervalTimer(pStack);
        return;
    }

    if (tmrEvent == PTP_TMR_EVENT_RXSYNC) {
        // Will only be counted
        // no action - see 11.2.13
        if (pStack->hookStatusEvent != 0) {
            pStack->hookStatusEvent(pStack->cbParam, PTP_STS_EVENT_SYNCLOST, 0);
            pStack->siteSync.hasSyncRcvd = 0;
        }
    }

    // check id and get port pointer for the following events
    if (id >= TSN_PTP_CFG_PORTNUM)
        return;
    pPort = &(pStack->port[id]);

    if (tmrEvent == PTP_TMR_EVENT_TXPDREQ) {
        // reset timer
        if (pStack->hookSetTimer != 0) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXPDREQ, id, pPort->md.curLogPdelayReqInterval, 1);
        }
        // MDPdelayReq state machine: IEEE 802.1as 11.2.15
        mdpdelayreq_pdelayIntervalTimer(pStack, pPort);
    }

    if (tmrEvent == PTP_TMR_EVENT_RXANNC) {
        // PortAnnounceInformation: IEEE 802.1as 10.3.11
        portannouncerecv_timeout(pStack, pPort);
    }

    if (tmrEvent == PTP_TMR_EVENT_TXANNC) {
        // PortAnnounceTransmit: IEEE 802.1as 10.3.13
        portannouncesend(pStack, pPort);
        // reset timer
        if (pPort->port.selectedRole == PTP_PORT_ROLE_MASTER) {
            // reset timer
            if (pStack->hookSetTimer != 0) {
                pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXANNC, id, pPort->anncInfo.curLogInterval, 1);
            }
        }
    }

    if (tmrEvent == PTP_TMR_EVENT_TXSYNC) {
        siteSyncIntervalTimer(pStack);
    }
}

void tsn_ptp_stack_port_enable(tsn_ptp_instance_t *pStack, uint16_t portIndex, uint16_t bEnable)
{
    tsn_ptp_port_t *port;

    // check index
    if (portIndex >= TSN_PTP_CFG_PORTNUM)
        return;
    port = (tsn_ptp_port_t *)(&pStack->port[portIndex]);

    if (bEnable) {
        // enable

        // set default intervals
        port->md.curLogPdelayReqInterval = PTP_INIT_INTVAL_PDELAY;
        port->anncInfo.curLogInterval = PTP_INIT_INTVAL_ANNC;

        // announce info: copy master prio
        port->anncInfo.portPriority = pStack->gmPriority;
        port->anncInfo.portPriority.portNumber = port->port.thisPort;
        port->anncInfo.portStepsRemoved = pStack->masterStepsRemoved;
        port->anncInfo.infoIs = INFO_IS_MINE;
        port->anncInfo.newInfo = 1;
        port->port.selectedRole = PTP_PORT_ROLE_MASTER; // 10.3.12.1.4 (f2)
        port->port.rcvdPortStateInd = 0;

        // timer hooks - enable, defaults
        if (pStack->hookSetTimer != 0) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXPDREQ, portIndex, port->md.curLogPdelayReqInterval, 1);
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXANNC, portIndex, port->anncInfo.curLogInterval, 1);
            // enabling of RX timer not required
        }
    } else {
        // disable port
        port->anncInfo.infoIs = INFO_IS_DISABLED;
        port->port.selectedRole = PTP_PORT_ROLE_DISABLED;

        // timer hooks - disable (127)
        if (pStack->hookSetTimer != 0) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXPDREQ, portIndex, 127, 1);
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_TXANNC, portIndex, 127, 1);
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_RXANNC, portIndex, 127, 1);
        }

        tsn_ptp_stack_portroleselection(pStack);
    }

    port->port.enabled = (uint8_t)bEnable; // set enabled flag
}

void tsn_ptp_stack_send_pss(tsn_ptp_instance_t *pStack, port_sync_sync_t *pPSS)
{
    uint32_t idx;
    tsn_ptp_port_t *port;

    // SiteSyncSync: 10.2.6

    for (idx = 0; idx < TSN_PTP_CFG_PORTNUM; ++idx) {
        port = &(pStack->port[idx]);
        if ((port->port.thisPort != pPSS->localPortNum) && (port->port.selectedRole == PTP_PORT_ROLE_MASTER)) {
            portsyncsyncsend(pStack, port, pPSS);
        }
    }
}

uint8_t tsn_ptp_msg(uintptr_t hdrAddr)
{
    uint8_t tmp;

    // info: aligned access, hdraddr expected to be aligned to u16
    tmp = (*(uint8_t *)(hdrAddr + PTPHDR_OFS_MSGID));
    return tmp & PTP_MSGTYPE_MASK;
}

uint8_t tsn_ptp_domain(uintptr_t hdrAddr)
{
    uint8_t tmp;

    // info: aligned access, hdraddr expected to be aligned to u16
    tmp = (*(uint8_t *)(hdrAddr + PTPHDR_OFS_DOMAIN));
    return tmp;
}

uint64_t tsn_ptp_clock_offset(tsn_ptp_instance_t *pStack)
{
    ptp_timestamp_t localtime;
    uint64_t localtime_ns;

    if (!pStack->hookGetLocalTime)
        return 0;
    pStack->hookGetLocalTime(pStack->cbParam, &localtime);
    localtime_ns = ptp_timestamp_to_ns(&localtime);

    return tsn_ptp_clock_get_offset(&pStack->localClock, localtime_ns);
}

// ----------------------------------------------------------------
/**
 * Get sequenceId from PTP header
 *
 * @param hdraddr   address of start of PTP header
 * @return          sequenceId
 */
static uint16_t ptp_get_seqid(uintptr_t hdraddr)
{
    uint16_t tmp;

    // info: aligned access, hdraddr expected to be aligned to u16
    tmp = (*(uint16_t *)(hdraddr + PTPHDR_OFS_SEQID));
    return SWAP16(tmp);
}

/**
 * Get correction field from PTP header
 *
 * @param hdraddr           address of start of PTP header
 * @param pCorrectionField  pointer to correctionField
 */
static void ptp_get_corfield(uintptr_t hdraddr, int64_t *pCorrectionField)
{
    uint8_t *pDst8;
    uint8_t *pSrc8;
    uint32_t idx;

    pSrc8 = (uint8_t *)(hdraddr + PTPHDR_OFS_CORFIELD);
    pDst8 = (uint8_t *)pCorrectionField;
    for (idx = 0; idx < 8; idx++) {
        pDst8[idx] = pSrc8[7 - idx]; // swap endianess
    }
}

// ----------------------------------------------------------------
// state machine functions

/* ---------------- */
/* MDPdelayResp     */
/* ---------------- */
static void mdpdelayresp_rcvdPdelayReq(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, uintptr_t hdrAddr, ptp_timestamp_t *pRxTime)
{
    // Part of:
    // MDPdelayResp state machine: IEEE 802.1as 11.2.16
    // notes: correction field is currently ignored (= 0)

    struct ptp_header_s hdr;
    uint32_t payload[5] = { 0, 0, 0, 0, 0 };
    uint16_t sequid;
    sequid = ptp_get_seqid(hdrAddr);

    pPort->mdPdResp.seqid = sequid; // copy sequence id
    tsn_ptp_get_portid(hdrAddr + PTPHDR_OFS_SPID, &pPort->mdPdResp.srcPortId);

    // set header PdelayResponse
    hdr = _ptp_default_header;
    hdr.sequenceId = sequid;
    hdr.sourcePortId = pPort->port.thisPortId;
    hdr.msgType = PTP_MSGTYPE_PDELAY_RESP;
    hdr.msgLen = PTP_HEADER_SZ + PTP_PAYLOAD_SZ_PDELAY;
    hdr.flags |= PTP_FLAG_TWO_STEP;
    hdr.control = PTP_CONTROL_OTHER;
    hdr.logMsgInterval = 0x7F;
    hdr.domainNumber = pStack->domainNumber;

    // set payload
    tsn_ptp_set_pdelay_resp((uintptr_t)(&payload), pRxTime, &pPort->mdPdResp.srcPortId);

    // call TX PdelayResponse
    if (pStack->hookTxFrame != 0) {
        pStack->hookTxFrame(pStack->cbParam, pPort->port.portIndex, &hdr, (uint8_t *)&payload, PTP_PAYLOAD_SZ_PDELAY);
    }
}

static void mdpdelayresp_rcvdMDTimestampReceive(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, ptp_timestamp_t *pTxTime)
{
    // Part of:
    // MDPdelayResp state machine: IEEE 802.1as 11.2.16
    // notes: correction field is currently ignored (= 0)

    struct ptp_header_s hdr;
    uint32_t payload[5] = { 0, 0, 0, 0, 0 };

    // set header PdelayRespFup
    hdr = _ptp_default_header;
    hdr.sequenceId = pPort->mdPdResp.seqid;
    hdr.sourcePortId = pPort->port.thisPortId;
    hdr.msgType = PTP_MSGTYPE_PDELAY_FUP;
    hdr.msgLen = PTP_HEADER_SZ + PTP_PAYLOAD_SZ_PDELAY;
    (void)hdr.flags; // no change, default: PTP_FLAG_PTP_TIMESCALE
    hdr.control = PTP_CONTROL_OTHER;
    hdr.logMsgInterval = 0x7F;
    hdr.domainNumber = pStack->domainNumber;

    // set payload
    tsn_ptp_set_pdelay_resp((uintptr_t)(&payload), pTxTime, &pPort->mdPdResp.srcPortId);

    // call TX PdelayResponseFup
    if (pStack->hookTxFrame != 0) {
        pStack->hookTxFrame(pStack->cbParam, pPort->port.portIndex, &hdr, (uint8_t *)&payload, PTP_PAYLOAD_SZ_PDELAY);
    }
}

/* ---------------- */
/* MDPdelayReq      */
/* ---------------- */
static void mdpdelayreq_rcvdPdelayResp(tsn_ptp_port_t *pProtocol, uintptr_t hdrAddr, ptp_timestamp_t *pRxTime)
{
    // Part of:
    // MDPdelayReq state machine: IEEE 802.1as 11.2.15

    int64_t corfield;
    ptp_timestamp_t tstamp;
    uint16_t seqid;

    // get frame infos
    seqid = ptp_get_seqid(hdrAddr);

    // check validity
    // TODO: check reqportid == thisclock/thisport
    if ((seqid == pProtocol->mdPdReq.seqid)) {
        // save peer rx timestamp
        // timestamp from payload + correctionField
        tsn_ptp_get_tstamp(hdrAddr + PTP_HEADER_SZ, &tstamp);
        ptp_get_corfield(hdrAddr, &corfield);
        pProtocol->mdPdReq.time_peer_rxreq = tstamp.seconds * NS_PER_SEC;
        pProtocol->mdPdReq.time_peer_rxreq += tstamp.nanoseconds;
        pProtocol->mdPdReq.time_peer_rxreq += (corfield >> 16);

        // save local rx timestamp
        pProtocol->mdPdReq.time_local_rxresp = pRxTime->seconds * NS_PER_SEC;
        pProtocol->mdPdReq.time_local_rxresp += pRxTime->nanoseconds;

        // set event flag
        pProtocol->mdPdReq.eventflags |= PDREQ_FLAG_RXRESP;
    }

    // no else branch:
    // in case of invalid response we rely on timer and do reset,
    // because measIsRunning==TRUE
}

static void mdpdelayreq_rcvdPdelayRespFup(tsn_ptp_port_t *pProtocol, uintptr_t hdrAddr)
{
    // Part of:
    // MDPdelayReq state machine: IEEE 802.1as 11.2.15

    int64_t corfield;
    ptp_timestamp_t tstamp;
    uint16_t seqid;

    // get frame infos
    seqid = ptp_get_seqid(hdrAddr);

    // check validity
    // TODO: check sourceportid == responseSrcPortId
    if ((seqid == pProtocol->mdPdReq.seqid)) {
        // save peer tx timestamp
        tsn_ptp_get_tstamp(hdrAddr + PTP_HEADER_SZ, &tstamp);
        ptp_get_corfield(hdrAddr, &corfield);
        pProtocol->mdPdReq.time_peer_txresp = tstamp.seconds * NS_PER_SEC;
        pProtocol->mdPdReq.time_peer_txresp += tstamp.nanoseconds;
        pProtocol->mdPdReq.time_peer_txresp += (corfield >> 16);

        // set event flag
        pProtocol->mdPdReq.eventflags |= PDREQ_FLAG_RXRSPFUP;
    }

    // calculate PDelay if all events are complete
    if (pProtocol->mdPdReq.eventflags == PDREQ_MASK_COMPLETE) {
        mdpdelayreq_calculate(pProtocol);
    }
}

static int32_t calc_nrr(int64_t diffPeer, int64_t diffLocal)
{
    // ratio        = (double)(diffPeer) / ((double)diffLocal);
    // rateOfs      = (ratio-1.0) * 2^32

    int64_t tmp64;

    // difference
    tmp64 = (diffPeer - diffLocal);
    tmp64 <<= 32;
    tmp64 /= diffLocal;

    // high word expected 0 or -1
    // return lower 32 bit
    return (int32_t)tmp64;
}

static uint32_t calc_pdelay(int64_t diffPeer, int64_t diffLocal, int32_t rateOfs)
{
    // pdelay = ((NRR*diffLocal) - diffPeer) / 2.0

    int64_t diff;

    diff = diffLocal * rateOfs;
    diff += (diffLocal << 32);
    diff >>= 32;
    diff -= diffPeer;
    diff /= 2;
    if (diff < 0)
        diff = 0;

    return (uint32_t)diff;
}

static void mdpdelayreq_calculate(tsn_ptp_port_t *pProtocol)
{
    // Part of:
    // MDPdelayReq state machine: IEEE 802.1as 11.2.15

    int64_t diffPeer;
    int64_t diffLocal;

    // computeNeighborRateRatio
    if (pProtocol->port.computeNeighborRateRatio) {
        diffPeer = pProtocol->mdPdReq.time_peer_txresp - pProtocol->mdPdReq.saved_peer_txresp;
        diffLocal = pProtocol->mdPdReq.time_local_rxresp - pProtocol->mdPdReq.saved_local_rxresp;
        if (diffPeer < 10LL * NS_PER_SEC) {
            pProtocol->port.neighborRateOfs32 = calc_nrr(diffPeer, diffLocal);
            pProtocol->mdPdReq.neighborRateRatioValid = 1;
        } else {
            pProtocol->port.neighborRateOfs32 = 0;
            pProtocol->mdPdReq.neighborRateRatioValid = 0;
        }
    }

    pProtocol->mdPdReq.saved_local_rxresp = pProtocol->mdPdReq.time_local_rxresp;
    pProtocol->mdPdReq.saved_peer_txresp = pProtocol->mdPdReq.time_peer_txresp;

    // computeNeighborPropTime
    if (pProtocol->port.computeMeanLinkDelay) {
        diffLocal = pProtocol->mdPdReq.time_local_rxresp - pProtocol->mdPdReq.time_local_txreq;
        diffPeer = pProtocol->mdPdReq.time_peer_txresp - pProtocol->mdPdReq.time_peer_rxreq;
        pProtocol->port.meanLinkDelay = calc_pdelay(diffPeer, diffLocal, pProtocol->port.neighborRateOfs32);
    }

    //
    pProtocol->mdPdReq.lostResponses = 0;
    pProtocol->md.isMeasuringDelay = 1;

    if ((pProtocol->port.meanLinkDelay < pProtocol->md.meanLinkDelayTresh) && (pProtocol->mdPdReq.neighborRateRatioValid)) {
        pProtocol->md.asCapableAcrossDomains = 1;
        pProtocol->md.detectedFaults = 0;
    } else {
        if (pProtocol->md.detectedFaults <= PTP_ALLOW_PREQ_FAULTS) {
            pProtocol->md.detectedFaults++;
        } else {
            pProtocol->md.detectedFaults = 0;
            pProtocol->md.asCapableAcrossDomains = 0;
        }
    }

    // measuring done
    pProtocol->mdPdReq.measIsRunning = 0;
    // clear event flags
    pProtocol->mdPdReq.eventflags = 0;
}

static void mdpdelayreq_rcvdMDTimestampReceive(tsn_ptp_port_t *pProtocol, ptp_timestamp_t *pTxTime)
{
    // Part of:
    // MDPdelayReq state machine: IEEE 802.1as 11.2.15

    // save timestamp (t1)
    pProtocol->mdPdReq.time_local_txreq = pTxTime->seconds * NS_PER_SEC;
    pProtocol->mdPdReq.time_local_txreq += pTxTime->nanoseconds;

    // set event flag
    pProtocol->mdPdReq.eventflags |= PDREQ_FLAG_TXDONE;

    // if for any reason event processing order is inaccurate the TX timestamp may be last event;
    // the pdelay calculation must then be done here
    if (pProtocol->mdPdReq.eventflags == PDREQ_MASK_COMPLETE) {
        mdpdelayreq_calculate(pProtocol);
    }
}

static void mdpdelayreq_pdelayIntervalTimer(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort)
{
    // Part of:
    // MDPdelayReq state machine: IEEE 802.1as 11.2.15

    struct ptp_header_s hdr;
    uint32_t payload[5] = { 0, 0, 0, 0, 0 };

    // check measuring not completely done
    // action: do state RESET
    if (pPort->mdPdReq.measIsRunning) {
        if (pPort->mdPdReq.lostResponses > PTP_ALLOW_LOST_RESP) {
            pPort->md.isMeasuringDelay = 0;
            pPort->md.asCapableAcrossDomains = 0;
        } else {
            pPort->mdPdReq.lostResponses++;
        }
    }
    pPort->mdPdReq.measIsRunning = 0;

    pPort->mdPdReq.seqid++;        // increase Sequence ID, do before copying to hdr
    pPort->mdPdReq.eventflags = 0; // clear any pending flags

    // set header PdelayRequest
    hdr = _ptp_default_header;
    hdr.sequenceId = pPort->mdPdReq.seqid;
    hdr.sourcePortId = pPort->port.thisPortId;
    hdr.msgType = PTP_MSGTYPE_PDELAY_REQ;
    hdr.msgLen = PTP_HEADER_SZ + PTP_PAYLOAD_SZ_PDELAY;
    (void)hdr.flags; // no change, default: PTP_FLAG_PTP_TIMESCALE
    hdr.control = PTP_CONTROL_OTHER;
    hdr.logMsgInterval = pPort->md.curLogPdelayReqInterval;
    hdr.domainNumber = pStack->domainNumber;

    // Send Pdelay Request
    if (pStack->hookTxFrame != 0) {
        pPort->mdPdReq.measIsRunning = 1;
        pStack->hookTxFrame(pStack->cbParam, pPort->port.portIndex, &hdr, (uint8_t *)&payload, PTP_PAYLOAD_SZ_PDELAY);
    }
}

/* ---------------- */
/* MDSyncReceive    */
/* ---------------- */
static void mdsyncreceive_rcvdSync(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pProtocol, uintptr_t hdrAddr, ptp_timestamp_t *pRxTime)
{
    // Part of:
    // MDSyncReceive state machine: IEEE 802.1as 11.2.13
    (void)pStack;
    uint16_t seqid;
    uint32_t propDelay;

    // check asCapable
    if (!(pProtocol->port.asCapable)) {
        pProtocol->mdSyncRecv.waitFup = 0;
        return;
    }

    // get frame infos
    seqid = ptp_get_seqid(hdrAddr);

    pProtocol->mdSyncRecv.waitFup = 1;
    pProtocol->mdSyncRecv.seqid = seqid;

    tsn_ptp_get_portid(hdrAddr + PTPHDR_OFS_SPID, &pProtocol->mdSyncRecv.pss.srcPortIdentity);         // 11.2.13.2.1 b
    pProtocol->mdSyncRecv.pss.logMsgInterval = *((int8_t *)hdrAddr + PTPHDR_OFS_LOGIVAL);              // 11.2.13.2.1 c
    pProtocol->mdSyncRecv.pss.upstreamTxTimeNs = pRxTime->seconds * NS_PER_SEC + pRxTime->nanoseconds; // 11.2.13.2.1 f
    if (pProtocol->port.meanLinkDelay < INT32_MAX) {
        // propDelay = (uint32_t)((double)pProtocol->port.neighborPropDelayNs / pProtocol->port.neighborRateRatio);
        // simplified: (pdelay / neighborRateRation) ~~ pdelay
        // expected values: pdelay (0 .. 5000), nrr (0.9996 .. 1.0004) --> +- 2ns
        propDelay = pProtocol->port.meanLinkDelay;
        pProtocol->mdSyncRecv.pss.upstreamTxTimeNs -= propDelay;
    }
}

static void mdsyncreceive_rcvdFollowup(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pProtocol, uintptr_t hdrAddr)
{
    // Part of:
    // MDSyncReceive state machine: IEEE 802.1as 11.2.13

    uint16_t seqid;
    struct ptp_followup_s followupData;

    // check waiting for followup
    if (!(pProtocol->mdSyncRecv.waitFup))
        return;

    // TODO: check timeout
    // currentTime >= followupReceiptTimeout

    // check correct sequenceId
    seqid = ptp_get_seqid(hdrAddr);
    if (seqid != pProtocol->mdSyncRecv.seqid)
        return;

    // read followup data
    if (tsn_ptp_get_followup(hdrAddr + PTP_HEADER_SZ, &followupData)) {
        // error reading followup
        return;
    }

    // setMDSyncReceive
    ptp_get_corfield(hdrAddr, &pProtocol->mdSyncRecv.pss.followUpCorrectionField);          // 11.2.13.2.1 a
    pProtocol->mdSyncRecv.pss.preciseOriginTimestamp = followupData.preciseOriginTimestamp; // 11.2.13.2.1 d
    pProtocol->mdSyncRecv.pss.scaledRateRatio = followupData.cumScaledRateOfs;              // 11.2.13.2.1 e
    pProtocol->mdSyncRecv.pss.gmTimeBaseIndicator = followupData.gmTimeBaseInd;             // 11.2.13.2.1 g
    pProtocol->mdSyncRecv.pss.lastGmPhaseChange = followupData.lastGmPhaseCh;               // 11.2.13.2.1 h
    pProtocol->mdSyncRecv.pss.scaledLastGmFreqChange = followupData.scaledLastGmFreqCh;     // 11.2.13.2.1 i

    // txMDSyncReceive: transmit PSS to portSyncSyncRecv
    // code contains parts of PortSyncSyncRecv (10.2.7) due to optimization
    pProtocol->mdSyncRecv.pss.localPortNum = pProtocol->port.thisPort; // 10.2.7.1 a
    // pProtocol->mdSyncRecv.pss.rateRatio += (pProtocol->port.neighborRateRatio - 1.0); // 10.2.7.1 e
    pProtocol->mdSyncRecv.pss.scaledRateRatio += (pProtocol->port.neighborRateOfs32 << 9); // 10.2.7.1 e
    portsyncsyncrecv_rcvdMdSync(pStack, &(pProtocol->mdSyncRecv.pss));

    // wait for next sync
    pProtocol->mdSyncRecv.waitFup = 0;
}

/* ---------------- */
/* PortSyncSyncRecv */
/* ---------------- */
static void portsyncsyncrecv_rcvdMdSync(tsn_ptp_instance_t *pStack, port_sync_sync_t *pPSS)
{
    // Part of:
    // PortSyncSyncRecv state machine: IEEE 802.1as 10.2.7

    // check field gmPriority.srcPortId
    if (pStack->gmPriority.sourcePortId.clockIdentity != pPSS->srcPortIdentity.clockIdentity)
        return;
    if (pStack->gmPriority.sourcePortId.portNumber != pPSS->srcPortIdentity.portNumber)
        return;

    // 10.2.7.1 a localPortNum, already adjusted before call
    // 10.2.7.1 b,c no need for copying, using pPSS structure
    // 10.2.7.1 d TODO: pPSS->syncReceiptTimeout = ?;
    // 10.2.7.1 e rateRatio, already adjusted before call

    // reset timer
    if (pStack->hookSetTimer != 0) {
        pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_RXSYNC, 0, pStack->siteSync.logSyncIval, 3);
    }

    if (pStack->hookStatusEvent != 0 && pStack->siteSync.hasSyncRcvd == 0) {
        pStack->hookStatusEvent(pStack->cbParam, PTP_STS_EVENT_SYNCRECV, 0);
    }

    siteSyncPssHandle(pStack, pPSS);
    pStack->siteSync.hasSyncRcvd = 1;
}

/* ------------------- */
/* PortAnnounceReceive */
/* ------------------- */
static clock_id rdClockId(uint8_t *ptr)
{
    uint32_t idx;
    clock_id id;
    uint8_t *pDst;

    pDst = (uint8_t *)(&id);
    for (idx = 0; idx < 8; ++idx) {
        pDst[idx] = ptr[idx];
    }

    return id;
}

static void portannouncerecv(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, uintptr_t hdrAddr)
{
    uint32_t idx;
    port_id_t srcportid;
    clock_id pathTraceElem;
    struct ptp_announce_s announceInfo;
    ptp_prio_vect_t msgPrioVect;
    int32_t rcvdInfo;
    uint16_t anncFlags;
    int8_t anncLogInterval;

    //
    // qualify announce (10.3.10.2.1)
    //
    if (!(pPort->port.asCapable))
        return; // (state machine)
    // check thisClock != srcPortId.clockId (a)
    tsn_ptp_get_portid(hdrAddr + PTPHDR_OFS_SPID, &srcportid);
    if (srcportid.clockIdentity == pStack->thisClock)
        return;
    // get announce payload info
    if (tsn_ptp_get_announce(hdrAddr + PTP_HEADER_SZ, &announceInfo) < 0)
        return;
    // check stepsremoved < 255 (b)
    if (announceInfo.stepsRemoved >= 255)
        return;
    // check path trace tlv (if present) (c)
    for (idx = 0; idx < announceInfo.pathCount; ++idx) {
        pathTraceElem = rdClockId(announceInfo.pPathSequence + (idx * PTP_PATHTRACE_SZ));
        if (pathTraceElem == pStack->thisClock)
            return;
    }

    // copy path trace tlv when slave port
    if (pPort->port.selectedRole == PTP_PORT_ROLE_SLAVE) {
        // copy all elements from path trace
        for (idx = 0; idx < announceInfo.pathCount; ++idx) {
            pathTraceElem = rdClockId(announceInfo.pPathSequence + (idx * PTP_PATHTRACE_SZ));
            if (idx < TSN_PTP_CFG_PATHTRACE_MAX) {
                pStack->pathTrace[idx] = pathTraceElem;
            }
        }
        // append thisClock (idx == announceInfo.pathCount)
        if (idx < TSN_PTP_CFG_PATHTRACE_MAX) {
            pStack->pathTrace[idx] = pStack->thisClock;
        }
        pStack->pathTraceCount = announceInfo.pathCount + 1;
    }

    // buid messagePriorityVector (see 10.3.5)
    msgPrioVect.rootSystemId.prio1 = announceInfo.gmPrio1;
    msgPrioVect.rootSystemId.prio2 = announceInfo.gmPrio2;
    msgPrioVect.rootSystemId.clockIdentity = announceInfo.gmIdentity;
    msgPrioVect.rootSystemId.clockQuality = announceInfo.clockQuality;
    msgPrioVect.stepsRemoved = announceInfo.stepsRemoved;
    tsn_ptp_get_portid(hdrAddr + PTPHDR_OFS_SPID, &msgPrioVect.sourcePortId);
    msgPrioVect.portNumber = pPort->port.thisPort;

    // rcvdInfo = rcvInfo(); IEEE802.1as 10.13.11.2.1
    rcvdInfo = tsn_ptp_cmp_priovect(&msgPrioVect, &pPort->anncInfo.portPriority);
    anncFlags = tsn_ptp_get_flags(hdrAddr + PTPHDR_OFS_FLAGS);
    anncFlags &= (PTP_FLAG_LEAP61 | PTP_FLAG_LEAP59 | PTP_FLAG_CURUTCO_VALID | PTP_FLAG_FREQ_TRACEABLE | PTP_FLAG_TIME_TRACEABLE);
    anncLogInterval = *((uint8_t *)(hdrAddr + PTPHDR_OFS_LOGIVAL));
    if (rcvdInfo == 0) {
        // REPEATED MASTER INFO

        // check here for update of flags&fields?

        // reset timer
        if (pStack->hookSetTimer != 0) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_RXANNC, pPort->port.portIndex, anncLogInterval, 3);
        }

    } else {
        // SUPERIOR MASTER INFO +
        // INFERIOR MASTER INFO - do port role selection

        // copy information
        pPort->anncInfo.portPriority = msgPrioVect;
        pPort->anncInfo.portStepsRemoved = announceInfo.stepsRemoved;
        pPort->anncInfo.infoIs = INFO_IS_RECEIVED;
        // recordOtherAnnounceInfo(); // 10.3.11.2.2
        // leap61, leap59, curUtcOfsValid, timeTraceable, freqTraceable; curUtcOfs; timeSrc
        pPort->anncInfo.annInfo.flags = anncFlags;
        pPort->anncInfo.annInfo.curUtcOfs = announceInfo.currentUtcOfs;
        pPort->anncInfo.annInfo.timeSource = announceInfo.timeSource;

        // PortRoleSelect: reselect
        tsn_ptp_stack_portroleselection(pStack);

        // reset timer
        if (pStack->hookSetTimer != 0) {
            pStack->hookSetTimer(pStack->cbParam, PTP_TMR_EVENT_RXANNC, pPort->port.portIndex, anncLogInterval, 3);
        }
    }
}

static void portannouncerecv_timeout(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort)
{
    // PortAnnounceInformation: IEEE802.1as 10.3.13

    // set info is aged
    pPort->anncInfo.infoIs = INFO_IS_AGED;
    // PortRoleSelect: reselect
    tsn_ptp_stack_portroleselection(pStack);
}

static void portannouncesend(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort)
{
    // PortAnnounceTransmit: IEEE 802.1as 10.3.13

    struct ptp_header_s hdr;
    struct ptp_announce_s announce;
    uint32_t payloadLen;
    uint8_t *ptrPayload;

    // only send, when master
    if (pPort->port.selectedRole != PTP_PORT_ROLE_MASTER)
        return;

    ptrPayload = (uint8_t *)&(pPort->anncInfo.payloadBuf);

    // build payload
    payloadLen = PTP_PAYLOAD_SZ_ANNOUNCE;
    announce.currentUtcOfs = pPort->anncInfo.annInfo.curUtcOfs;
    announce.gmPrio1 = pStack->gmPriority.rootSystemId.prio1;
    announce.gmPrio2 = pStack->gmPriority.rootSystemId.prio2;
    announce.clockQuality = pStack->gmPriority.rootSystemId.clockQuality;
    announce.gmIdentity = pStack->gmPriority.rootSystemId.clockIdentity;
    announce.stepsRemoved = pStack->masterStepsRemoved;
    announce.timeSource = pPort->anncInfo.annInfo.timeSource;
    announce.pPathSequence = (uint8_t *)&pStack->pathTrace;
    announce.pathCount = pStack->pathTraceCount;
    if (announce.pathCount > TSN_PTP_CFG_PATHTRACE_MAX) {
        announce.pathCount = 0; // -> do not send pathTrace TLV
    }

    payloadLen = tsn_ptp_set_announce((uintptr_t)ptrPayload, &announce);

    // set header announce-frame
    hdr = _ptp_default_header;
    hdr.sequenceId = pPort->anncInfo.txSeqId;
    hdr.sourcePortId = pPort->port.thisPortId;
    hdr.msgType = PTP_MSGTYPE_ANNOUNCE;
    hdr.msgLen = PTP_HEADER_SZ + payloadLen;
    hdr.flags |= pPort->anncInfo.annInfo.flags;
    hdr.control = PTP_CONTROL_OTHER;
    hdr.logMsgInterval = pPort->anncInfo.curLogInterval;
    hdr.domainNumber = pStack->domainNumber;

    if (pStack->hookTxFrame != 0) {
        pStack->hookTxFrame(pStack->cbParam, pPort->port.portIndex, &hdr, ptrPayload, payloadLen);
        pPort->anncInfo.txSeqId++;
    }
}

/* ------------------- */
/* PortSyncSyncSend    */
/* ------------------- */
static void portsyncsyncsend(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, port_sync_sync_t *pPSS)
{
    struct ptp_header_s hdr;
    uint32_t payloadBuffer[3] = { 0, 0, 0 };

    //
    // PortSyncSyncSend: 10.2.11
    //

    // check enabled and asCapable
    if (!(pPort->port.enabled))
        return;
    if (!(pPort->port.asCapable))
        return;

    pPort->mdSyncSend.pss = *pPSS;                                         // copy PSS
    pPort->mdSyncSend.pss.srcPortIdentity = pPort->port.thisPortId;        // 10.2.11.2.1 a
    pPort->mdSyncSend.pss.logMsgInterval = pPort->port.curLogSyncInterval; // 10.2.11.2.1 b
    // nothing to be done: 10.2.11.2.1 c-f

    // inc sequence ID
    pPort->mdSyncSend.seqid++;

    //
    // MDSyncSend (11.2.14)
    //
    // set header sync-frame
    hdr = _ptp_default_header;
    hdr.sequenceId = pPort->mdSyncSend.seqid;
    hdr.sourcePortId = pPort->mdSyncSend.pss.srcPortIdentity;
    hdr.msgType = PTP_MSGTYPE_SYNC;
    hdr.msgLen = PTP_HEADER_SZ + PTP_PAYLOAD_SZ_SYNC;
    hdr.flags |= PTP_FLAG_TWO_STEP;
    hdr.control = PTP_CONTROL_SYNC;
    hdr.logMsgInterval = pPort->port.curLogSyncInterval;
    hdr.domainNumber = pStack->domainNumber;

    // call hookTx Sync
    if (pStack->hookTxFrame != 0) {
        // call hook
        pStack->hookTxFrame(pStack->cbParam, pPort->port.portIndex, &hdr, (uint8_t *)(&payloadBuffer), PTP_PAYLOAD_SZ_SYNC);
    }
}

static int64_t _scale_residence_time(int64_t residenceTime, int32_t scaledRateRatio)
{
    // residenceTime  = pPort->mdSyncSend.pss.rateRatio * residenceTime;
    // scaledRateRatio: scaled to 2^41

    int64_t tmp;
    // scale to 2^32
    scaledRateRatio >>= 9;
    tmp = residenceTime;
    tmp *= scaledRateRatio;
    tmp += (residenceTime << 32);
    tmp >>= 32;
    return tmp;
}

static void mdsyncsend_rcvdMDTimestampReceive(tsn_ptp_instance_t *pStack, tsn_ptp_port_t *pPort, ptp_timestamp_t *pTxTime)
{
    struct ptp_header_s hdr;
    struct ptp_followup_s fupData;
    int64_t residenceTime;
    uint32_t payloadBuffer[12]; // must be large enough for PTP_PAYLOAD_SZ_FOLLOWUP data

    //
    // MDSyncSend (11.2.14)
    //

    // setFollowUp() (11.2.14.2.3)
    // a) followUpCorrectionField
    residenceTime = pTxTime->seconds * NS_PER_SEC;
    residenceTime += pTxTime->nanoseconds;
    residenceTime -= pPort->mdSyncSend.pss.upstreamTxTimeNs;
    // here: residenceTime relative to LocalClock
    // next: scale to GM time
    residenceTime = _scale_residence_time(residenceTime, pPort->mdSyncSend.pss.scaledRateRatio);

    // set header follow-up frame
    hdr = _ptp_default_header;
    hdr.correctionField = pPort->mdSyncSend.pss.followUpCorrectionField + (residenceTime << 16);
    hdr.sequenceId = pPort->mdSyncSend.seqid;
    hdr.sourcePortId = pPort->mdSyncSend.pss.srcPortIdentity;
    hdr.msgType = PTP_MSGTYPE_FOLLOWUP;
    hdr.msgLen = PTP_HEADER_SZ + PTP_PAYLOAD_SZ_FOLLOWUP;
    (void)hdr.flags; // no change, default: PTP_FLAG_PTP_TIMESCALE
    hdr.control = PTP_CONTROL_FOLLOWUP;
    hdr.logMsgInterval = pPort->port.curLogSyncInterval;
    hdr.domainNumber = pStack->domainNumber;

    // set payload
    fupData.preciseOriginTimestamp = pPort->mdSyncSend.pss.preciseOriginTimestamp;
    fupData.cumScaledRateOfs = pPort->mdSyncSend.pss.scaledRateRatio;
    fupData.gmTimeBaseInd = pPort->mdSyncSend.pss.gmTimeBaseIndicator;
    fupData.lastGmPhaseCh = pPort->mdSyncSend.pss.lastGmPhaseChange;
    fupData.scaledLastGmFreqCh = pPort->mdSyncSend.pss.scaledLastGmFreqChange;
    tsn_ptp_set_followup((uintptr_t)(&payloadBuffer), &fupData); // fupData -> payloadBuffer

    // txFollowUp
    if (pStack->hookTxFrame != 0) {
        // call hook
        pStack->hookTxFrame(pStack->cbParam, pPort->port.portIndex, &hdr, (uint8_t *)(&payloadBuffer), PTP_PAYLOAD_SZ_FOLLOWUP);
    }
}

/**
 * \brief: Convert timespec to nanoseconds
 *
 * @param ts    pointer to timespec
 * @return      scalar nanoseconds
 */
static uint64_t ptp_timestamp_to_ns(ptp_timestamp_t *ts)
{
    return ((uint64_t)ts->seconds * NS_PER_SEC) + ts->nanoseconds;
}

static uint64_t tsn_ptp_clock_get_offset(tsn_ptp_clock_t *domain_clk, uint64_t localtime)
{
    int64_t timediff;

    timediff = localtime - domain_clk->last_localtime;
    if (domain_clk->rateOfs) {
        timediff *= domain_clk->rateOfs;
        timediff += domain_clk->fractOfs;
        domain_clk->fractOfs = (timediff & 0xFFFFFFFF); // low 32 bits
        timediff >>= 32;
        domain_clk->offset_ns += timediff;
    }

    domain_clk->last_localtime = localtime;
    return domain_clk->offset_ns;
}

void tsn_ptp_clock_set_offset(tsn_ptp_clock_t *domain_clk, uint64_t localtime, uint64_t offset)
{
    domain_clk->offset_ns = offset;
    domain_clk->fractOfs = 0;
    domain_clk->last_localtime = localtime;
}

uint64_t tsn_ptp_time_get(tsn_ptp_instance_t *pStack)
{
    ptp_timestamp_t localtime;
    uint64_t localtime_ns, local_ofs;

    if (!pStack->hookGetLocalTime)
        return 0;
    pStack->hookGetLocalTime(pStack->cbParam, &localtime);
    localtime_ns = ptp_timestamp_to_ns(&localtime);
    local_ofs = tsn_ptp_clock_get_offset(&pStack->localClock, localtime_ns);
    return localtime_ns + local_ofs;
}

/* ------------------- */
/* SiteSync            */
/* ------------------- */
static void siteSyncIntervalTimer(tsn_ptp_instance_t *pStack)
{
    port_sync_sync_t portSync;
    ptp_timestamp_t localtime;
    ptp_timestamp_t offset;
    uint64_t localtime_ns;
    uint64_t rtctime;

    //
    // ClockMasterSyncSend 10.2.8; sends when no sync is received by any port
    //
    if (pStack->siteSync.hasSyncRcvd)
        return;

    // get local time from upper layer
    if (!pStack->hookGetLocalTime)
        return;
    pStack->hookGetLocalTime(pStack->cbParam, &localtime);

    if (!pStack->hookGetLocalOffset)
        return;
    pStack->hookGetLocalOffset(pStack->cbParam, &offset);

    localtime_ns = ptp_timestamp_to_ns(&localtime);
    pStack->localClock.offset_ns = ptp_timestamp_to_ns(&offset);

    // set rtc time
    // rtctime = localtime + localpriv->domainclock.offset_ns;
    rtctime = localtime_ns + pStack->localClock.offset_ns;

    // build port sync structure
    portSync.localPortNum = 0;                                          // a
    portSync.preciseOriginTimestamp.nanoseconds = rtctime % NS_PER_SEC; // b
    portSync.preciseOriginTimestamp.seconds = rtctime / NS_PER_SEC;     // b
    portSync.followUpCorrectionField = 0;                               // c
    portSync.srcPortIdentity.clockIdentity = pStack->thisClock;         // d
    portSync.srcPortIdentity.portNumber = 0;                            // e
    portSync.logMsgInterval = pStack->siteSync.logSyncIval;             // f
    portSync.upstreamTxTimeNs = localtime_ns;                           // g
    portSync.syncReceiptTimeout = 0xFFFFFFFFFFFFFFFFLL;                 // h
    // portSync.rateRatio                          = site->clock_master.gmRateRatio; // i
    portSync.scaledRateRatio = pStack->siteSync.gmRateOfs << 9;         // i
    portSync.gmTimeBaseIndicator = pStack->siteSync.clkTimeBaseInd;     // j
    portSync.lastGmPhaseChange = pStack->siteSync.clkSrcPhOfs;          // k
    portSync.scaledLastGmFreqChange = pStack->siteSync.clkFrqOfsScaled; // l

    // call GM received PSS
    siteSyncPssHandle(pStack, &portSync);
}

static void siteSyncPssHandle(tsn_ptp_instance_t *pStack, port_sync_sync_t *pPSS)
{
    uint8_t receivingPortRole;

    // reset sync timeout
    pStack->siteSync.hasSyncRcvd = 0;

    // default: port 0
    receivingPortRole = pStack->selectedRole0;
    if ((pPSS->localPortNum > 0) && (pPSS->localPortNum <= TSN_PTP_CFG_PORTNUM)) {
        receivingPortRole = pStack->port[pPSS->localPortNum - 1].port.selectedRole;
    }

    // check gmpresent: gmPrio.priority1 < 255
    // check role == slavePort
    if ((pStack->gmPriority.rootSystemId.prio1 < 255) && (receivingPortRole == PTP_PORT_ROLE_SLAVE)) {
        // slave clock
        siteSyncPssReceive(pStack, pPSS);

        // send PortSyncSync / (== MDSync) structure to all Master ports
        tsn_ptp_stack_send_pss(pStack, pPSS);
    }
}

// GM abstraction
static int32_t _scale_lc_offset_change(int64_t diff, int32_t rateOfs)
{
    diff *= (int64_t)rateOfs;
    diff >>= 32;
    return (int32_t)diff;
}

static void siteSyncPssReceive(tsn_ptp_instance_t *pStack, port_sync_sync_t *pPSS)
{
    int64_t syncReceiptTime;
    int64_t syncReceiptLocalTime;
    int64_t lc_ofs_ns;
    int32_t lc_ofs_ch;
    int64_t gmTimeDiff;
    int64_t localTimeDiff;
    uint64_t lc_offset;
    uint64_t localtime_ns;
    uint64_t lc_offset_old;
    ptp_timestamp_t localtime;

    // created by this time aware system? -> do nothing
    if (pPSS->localPortNum == 0)
        return;

    // calc syncReceiptTime, syncReceiptLocalTime
    syncReceiptTime = pPSS->preciseOriginTimestamp.seconds * NS_PER_SEC + pPSS->preciseOriginTimestamp.nanoseconds;
    syncReceiptTime += (pPSS->followUpCorrectionField >> 16);
    syncReceiptLocalTime = pPSS->upstreamTxTimeNs;

    // get local time from upper layer
    if (!pStack->hookGetLocalTime)
        return;
    pStack->hookGetLocalTime(pStack->cbParam, &localtime);
    localtime_ns = ptp_timestamp_to_ns(&localtime);

    // set RTC offset to current offset
    // calc RTC-OFFSET
    lc_ofs_ns = syncReceiptTime - syncReceiptLocalTime;
    // take account of gmRateRatio
    lc_ofs_ch = _scale_lc_offset_change(localtime_ns - syncReceiptLocalTime, pStack->siteSync.gmRateOfs);
    lc_ofs_ns -= lc_ofs_ch;

    // calc parts: sec, ns
    lc_offset = (uint64_t)lc_ofs_ns;

    // get old RTC offset, calc gmDifference
    lc_offset_old = tsn_ptp_clock_get_offset(&pStack->localClock, localtime_ns);
    pStack->siteSync.gmOffset = lc_offset - lc_offset_old;
    pStack->siteSync.gmOffsetSum += pStack->siteSync.gmOffset;

    // set new RTC-OFFSET
    tsn_ptp_clock_set_offset(&pStack->localClock, localtime_ns, lc_offset);

    if (pStack->hookStatusEvent != 0 && pStack->enableInvoke) {
        pStack->hookStatusEvent(pStack->cbParam, PTP_STS_EVENT_PHASE_CHANGE, (void *)&lc_offset);
    }

    // calc: gmRateRatio --> ofs change
    gmTimeDiff = syncReceiptTime - pStack->siteSync.savedSyncReceiptTime;
    // skip negative and very large jumps
    if ((gmTimeDiff < 0) || (gmTimeDiff > (32 * PTP_SYNC_ADJ_FREQ_IVAL))) {
        pStack->siteSync.hasSavedSync = 0;
        pStack->siteSync.gmOffsetSum = 0;
    }
    if ((pStack->siteSync.hasSavedSync) && (gmTimeDiff > PTP_SYNC_ADJ_FREQ_IVAL)) {
        // recalc gmRateRatio at around ADJ_FREQ_IVAL
        localTimeDiff = syncReceiptLocalTime - pStack->siteSync.savedSyncReceiptLocalTime;

        // calc gmRateRatio
        // site->clock_master.gmRateRatio = (double)(gmTimeDiff) / (double)localTimeDiff;
        int64_t diff;
        diff = gmTimeDiff - localTimeDiff;
        diff <<= 32;
        diff /= localTimeDiff;
        pStack->siteSync.gmRateOfs = (int32_t)diff;

        // calc rtc_ofs_ch
        // sanity check
        if ((pStack->siteSync.gmRateOfs > -PTP_SYNC_RATE_OFS_TH) && (pStack->siteSync.gmRateOfs < PTP_SYNC_RATE_OFS_TH)) {
            // rtc_adjfine(site->rtc, localpriv->clock_master.gmRateOfs);
            pStack->localClock.rateOfs = pStack->siteSync.gmRateOfs;

            if (pStack->hookStatusEvent != 0 && pStack->enableInvoke) {
                pStack->hookStatusEvent(pStack->cbParam, PTP_STS_EVENT_FREQ_CHANGE, (void *)&pStack->localClock.rateOfs);
            }
        }

        // trigger saving of current timestamp
        pStack->siteSync.hasSavedSync = 0;
        pStack->siteSync.gmOffsetSum = 0;
    }

    // save current receipt timestamps
    if (!(pStack->siteSync.hasSavedSync)) {
        pStack->siteSync.savedSyncReceiptTime = syncReceiptTime;
        pStack->siteSync.savedSyncReceiptLocalTime = syncReceiptLocalTime;
        pStack->siteSync.hasSavedSync = 1;
    }
}
