/* protocol.c */

#include "ptpd.h"

static void handle(PtpClock*);
static void handleSync(PtpClock*, TimeInternal*, Boolean);
static void handleFollowUp(PtpClock*, Boolean);
static void handleDelayReq(PtpClock*, TimeInternal*, Boolean);
static void handleDelayResp(PtpClock*, Boolean);
static void handleManagement(PtpClock*, Boolean);

static void issueSync(PtpClock*);
static void issueFollowup(PtpClock*, TimeInternal*);
static void issueDelayReq(PtpClock*);
static void issueDelayResp(PtpClock*, TimeInternal*);
static void issueManagement(PtpClock*, MsgManagement*);

static Boolean doInit(PtpClock *ptpClock)
{
    DBG("manufacturerIdentity: %s\n", MANUFACTURER_ID);

    /* initialize networking */
    netShutdown(&ptpClock->netPath);

    if (!netInit(&ptpClock->netPath, ptpClock))
    {
        ERROR("doInit: failed to initialize network\n");
        toState(ptpClock, PTP_FAULTY);
        return FALSE;
    }

    /* initialize other stuff */
    initData(ptpClock);

    initTimer();

    initClock(ptpClock);

    m1(ptpClock);

    msgPackHeader(ptpClock, ptpClock->msgObuf);

    DBG("sync message interval: %d\n", PTP_SYNC_INTERVAL_TIMEOUT(ptpClock->defaultDS.sync_interval));

    DBG("clock identifier: %s\n", ptpClock->defaultDS.clock_identifier);

    DBG("256*log2(clock variance): %d\n", ptpClock->defaultDS.clock_variance);

    DBG("clock stratum: %d\n", ptpClock->defaultDS.clock_stratum);

    DBG("clock preferred?: %s\n", ptpClock->defaultDS.preferred ? "yes" : "no");

    DBG("bound interface name: %s\n", ptpClock->rtOpts->ifaceName);

    DBG("communication technology: %d\n", ptpClock->portDS.port.communicationTechnology);

    DBG("uuid: %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
        (unsigned char)ptpClock->portDS.port.uuid[0], (unsigned char)ptpClock->portDS.port.uuid[1],
        (unsigned char)ptpClock->portDS.port.uuid[2], (unsigned char)ptpClock->portDS.port.uuid[3],
        (unsigned char)ptpClock->portDS.port.uuid[4], (unsigned char)ptpClock->portDS.port.uuid[5]);

    DBG("PTP subdomain name: %s\n", ptpClock->defaultDS.subdomain_name);

    DBG("subdomain address: %u.%u.%u.%u\n",
        (unsigned char)ptpClock->portDS.subdomain_address[0], (unsigned char)ptpClock->portDS.subdomain_address[1],
        (unsigned char)ptpClock->portDS.subdomain_address[2], (unsigned char)ptpClock->portDS.subdomain_address[3]);

    DBG("event port address: %hhx %hhx\n",
        ptpClock->portDS.event_port_address[0], ptpClock->portDS.event_port_address[1]);

    DBG("general port address: %hhx %hhx\n",
        ptpClock->portDS.general_port_address[0], ptpClock->portDS.general_port_address[1]);

    toState(ptpClock, PTP_LISTENING);

    return TRUE;
}

/* handle actions and events for 'port_state' */
void doState(PtpClock *ptpClock)
{
    UInteger8 state;

    ptpClock->message_activity = FALSE;

    switch (ptpClock->portDS.port_state)
    {
    case PTP_LISTENING:
    case PTP_UNCALIBRATED:
    case PTP_SLAVE:
    case PTP_PRE_MASTER:
    case PTP_MASTER:
    case PTP_PASSIVE:

        if (getEvent(ptpClock->events, STATE_DECISION_EVENT))
        {
            clearEvent(ptpClock->events, STATE_DECISION_EVENT);
            state = bmc(ptpClock);

            if ((ptpClock->portDS.port_state != PTP_SLAVE) && (state == PTP_SLAVE))
            {
                state = PTP_UNCALIBRATED;
            }

            if (state != ptpClock->portDS.port_state)
                toState(ptpClock, state);
        }

        break;

    default:
        break;
    }

    switch (ptpClock->portDS.port_state)
    {
    case PTP_UNCALIBRATED:

        if (getEvent(ptpClock->events, MASTER_CLOCK_SELECTED))
        {
            DBG("event MASTER_CLOCK_SELECTED\n");
            clearEvent(ptpClock->events, MASTER_CLOCK_SELECTED);
            toState(ptpClock, PTP_SLAVE);
        }

        if (getEvent(ptpClock->events, MASTER_CLOCK_CHANGED))
        {
            DBG("event MASTER_CLOCK_CHANGED\n");
            clearEvent(ptpClock->events, MASTER_CLOCK_CHANGED);
            toState(ptpClock, PTP_UNCALIBRATED);
        }

        break;

    case PTP_SLAVE:

        if (getEvent(ptpClock->events, SYNCHRONIZATION_FAULT))
        {
            DBG("event SYNCHRONIZATION_FAULT\n");
            clearEvent(ptpClock->events, SYNCHRONIZATION_FAULT);
            toState(ptpClock, PTP_UNCALIBRATED);
        }

        if (getEvent(ptpClock->events, MASTER_CLOCK_CHANGED))
        {
            DBG("event MASTER_CLOCK_CHANGED\n");
            clearEvent(ptpClock->events, MASTER_CLOCK_CHANGED);
            toState(ptpClock, PTP_UNCALIBRATED);
        }

        break;

    default:
        break;
    }


    switch (ptpClock->portDS.port_state)
    {
    case PTP_INITIALIZING:
        doInit(ptpClock);
        return;
    case PTP_FAULTY:
        /* imaginary troubleshooting */

        DBG("event FAULT_CLEARED\n");
        toState(ptpClock, PTP_INITIALIZING);
        return;

    case PTP_LISTENING:
    case PTP_PASSIVE:
    case PTP_UNCALIBRATED:
    case PTP_SLAVE:
        handle(ptpClock);

        if (timerExpired(SYNC_RECEIPT_TIMER, ptpClock->itimer))
        {
            DBG("event SYNC_RECEIPT_TIMEOUT_EXPIRES\n");
            ptpClock->defaultDS.number_foreign_records = 0;
            ptpClock->foreignMasterDS.i = 0;

            if (!ptpClock->slaveOnly && ptpClock->defaultDS.clock_stratum != 255)
            {
                m1(ptpClock);
                toState(ptpClock, PTP_MASTER);
            }
            else if (ptpClock->portDS.port_state != PTP_LISTENING)
                toState(ptpClock, PTP_LISTENING);
        }

        break;

    case PTP_MASTER:
        if (timerExpired(SYNC_INTERVAL_TIMER, ptpClock->itimer))
        {
            DBGV("event SYNC_INTERVAL_TIMEOUT_EXPIRES\n");
            issueSync(ptpClock);
        }

        handle(ptpClock);

        if (ptpClock->slaveOnly || ptpClock->defaultDS.clock_stratum == 255)
            toState(ptpClock, PTP_LISTENING);

        break;

    case PTP_DISABLED:
        handle(ptpClock);

        break;

    default:
        DBG("doState: unrecognized state\n");

        break;
    }
}

/* perform actions required when leaving 'port_state' and entering 'state' */
void toState(PtpClock *ptpClock, UInteger8 state)
{
    ptpClock->message_activity = TRUE;

    /* leaving state tasks */

    switch (ptpClock->portDS.port_state)
    {
    case PTP_MASTER:
        timerStop(SYNC_INTERVAL_TIMER, ptpClock->itimer);
        initClock(ptpClock);
        break;

    case PTP_SLAVE:
    case PTP_UNCALIBRATED:
        if(state == PTP_SLAVE || state == PTP_UNCALIBRATED)
            break;
        initClock(ptpClock);
        break;

    default:
        initClock(ptpClock);
        break;
    }

    /* entering state tasks */
    switch (state)
    {
    case PTP_INITIALIZING:
        DBG("state PTP_INITIALIZING\n");
        timerStop(SYNC_RECEIPT_TIMER, ptpClock->itimer);

        ptpClock->portDS.port_state = PTP_INITIALIZING;
        break;

    case PTP_FAULTY:
        DBG("state PTP_FAULTY\n");
        timerStop(SYNC_RECEIPT_TIMER, ptpClock->itimer);

        ptpClock->portDS.port_state = PTP_FAULTY;
        break;

    case PTP_DISABLED:
        DBG("state change to PTP_DISABLED\n");
        timerStop(SYNC_RECEIPT_TIMER, ptpClock->itimer);

        ptpClock->portDS.port_state = PTP_DISABLED;
        break;

    case PTP_LISTENING:
        DBG("state PTP_LISTENING\n");
        timerStart(SYNC_RECEIPT_TIMER, PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->defaultDS.sync_interval), ptpClock->itimer);

        ptpClock->portDS.port_state = PTP_LISTENING;
        break;

    case PTP_MASTER:
        DBG("state PTP_MASTER\n");

        if (ptpClock->portDS.port_state != PTP_PRE_MASTER)
            timerStart(SYNC_INTERVAL_TIMER, PTP_SYNC_INTERVAL_TIMEOUT(ptpClock->defaultDS.sync_interval), ptpClock->itimer);

        timerStop(SYNC_RECEIPT_TIMER, ptpClock->itimer);

        ptpClock->portDS.port_state = PTP_MASTER;

        break;

    case PTP_PASSIVE:
        DBG("state PTP_PASSIVE\n");

        timerStart(SYNC_RECEIPT_TIMER, PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->defaultDS.sync_interval), ptpClock->itimer);

        ptpClock->portDS.port_state = PTP_PASSIVE;

        break;

    case PTP_UNCALIBRATED:
        DBG("state PTP_UNCALIBRATED\n");

        /* R is chosen to allow a few syncs before we first get a one-way delay estimate */
        /* this is to allow the offset filter to fill for an accurate initial clock reset */
        ptpClock->Q = 0;

        ptpClock->R = getRand(&ptpClock->random_seed) % 4 + 4;

        DBG("toState: Q = %d, R = %d\n", ptpClock->Q, ptpClock->R);

        ptpClock->waitingForFollow = FALSE;

        ptpClock->timestamp.delayReqSend.seconds =  ptpClock->timestamp.delayReqSend.nanoseconds = 0;

        ptpClock->timestamp.delayReqRecieve.seconds = ptpClock->timestamp.delayReqRecieve.nanoseconds = 0;

        timerStart(SYNC_RECEIPT_TIMER, PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->defaultDS.sync_interval), ptpClock->itimer);

        ptpClock->portDS.port_state = PTP_UNCALIBRATED;

        break;

    case PTP_SLAVE:
        DBG("state PTP_SLAVE\n");

        timerStart(SYNC_RECEIPT_TIMER, PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->defaultDS.sync_interval), ptpClock->itimer);

        ptpClock->portDS.port_state = PTP_SLAVE;

        break;

    default:
        DBG("toState: unrecognized state\n");

        break;
    }

    if (ptpClock->displayStats)
        displayStats(ptpClock);
}

/* check and handle received messages */
static void handle(PtpClock *ptpClock)
{
    int ret;
    Boolean isFromSelf;
    TimeInternal time = { 0, 0 };

    if (!ptpClock->message_activity)
    {
        ret = netSelect(&ptpClock->netPath, 0);

        if (ret < 0)
        {
            ERROR("handle: failed to poll sockets\n");
            toState(ptpClock, PTP_FAULTY);
            return;
        }
        else if (!ret)
        {
            DBGVV("handle: nothing\n");
            return;
        }

        /* else length > 0 */
    }

    DBGVV("handle: something\n");

    ptpClock->msgIbufLength = netRecvEvent(&ptpClock->netPath, ptpClock->msgIbuf, &time);

    if (ptpClock->msgIbufLength < 0)
    {
        ERROR("handle: failed to receive on the event socket\n");
        toState(ptpClock, PTP_FAULTY);
        return;
    }
    else if (!ptpClock->msgIbufLength)
    {
        ptpClock->msgIbufLength = netRecvGeneral(&ptpClock->netPath, ptpClock->msgIbuf);

        if (ptpClock->msgIbufLength < 0)
        {
            ERROR("handle: failed to receive on the general socket\n");
            toState(ptpClock, PTP_FAULTY);
            return;
        }
        else if (!ptpClock->msgIbufLength)
            return;
    }

    ptpClock->message_activity = TRUE;

    if (!msgPeek(ptpClock->msgIbuf, ptpClock->msgIbufLength))
        return;

    if (ptpClock->msgIbufLength < HEADER_LENGTH)
    {
        ERROR("handle: message shorter than header length\n");
        toState(ptpClock, PTP_FAULTY);
        return;
    }

    msgUnpackHeader(ptpClock->msgIbuf, &ptpClock->msgTmpHeader);

    DBGV("handle: event Receipt of Message\n"
         "   version %d\n"
         "   type %d\n"
         "   uuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n"
         "   sequence %d\n"
         "   time %us %dns\n",
         ptpClock->msgTmpHeader.versionPTP,
         ptpClock->msgTmpHeader.control,
         ptpClock->msgTmpHeader.source.uuid[0], ptpClock->msgTmpHeader.source.uuid[1],
         ptpClock->msgTmpHeader.source.uuid[2], ptpClock->msgTmpHeader.source.uuid[3],
         ptpClock->msgTmpHeader.source.uuid[4], ptpClock->msgTmpHeader.source.uuid[5],
         ptpClock->msgTmpHeader.sequenceId,
         time.seconds, time.nanoseconds);

    if (ptpClock->msgTmpHeader.versionPTP != VERSION_PTP)
    {
        DBGV("handle: ignore version %d message\n", ptpClock->msgTmpHeader.versionPTP);
        return;
    }

    if (memcmp(ptpClock->msgTmpHeader.subdomain, ptpClock->defaultDS.subdomain_name,
               PTP_SUBDOMAIN_NAME_LENGTH))
    {
        DBGV("handle: ignore message from subdomain %s\n", ptpClock->msgTmpHeader.subdomain);
        return;
    }

    if (!(ptpClock->msgTmpHeader.source.communicationTechnology == ptpClock->defaultDS.clock.communicationTechnology
            || ptpClock->msgTmpHeader.source.communicationTechnology == PTP_DEFAULT
            || ptpClock->defaultDS.clock.communicationTechnology == PTP_DEFAULT))
    {
        ERROR("handle: unsupported communication technology\n");
        return;
    }


    isFromSelf = (Boolean)(0 == comparePortIdentity(&ptpClock->msgTmpHeader.source, &ptpClock->portDS.port));

    /* subtract the inbound latency adjustment if it is not a loop back and the
       time stamp seems reasonable */

    if (!isFromSelf && time.seconds > 0)
        subTime(&time, &time, &ptpClock->inboundLatency);

    switch (ptpClock->msgTmpHeader.control)
    {
    case PTP_SYNC_MESSAGE:
        handleSync(ptpClock, &time, isFromSelf);
        break;

    case PTP_FOLLOWUP_MESSAGE:
        handleFollowUp(ptpClock, isFromSelf);
        break;

    case PTP_DELAY_REQ_MESSAGE:
        handleDelayReq(ptpClock, &time, isFromSelf);
        break;

    case PTP_DELAY_RESP_MESSAGE:
        handleDelayResp(ptpClock, isFromSelf);
        break;

    case PTP_MANAGEMENT_MESSAGE:
        handleManagement(ptpClock, isFromSelf);
        break;

    default:
        DBG("handle: unrecognized message\n");
        break;
    }
}

static void handleSync(PtpClock *ptpClock, TimeInternal *time, Boolean isFromSelf)
{
    TimeInternal originTimestamp;
    MsgHeader *header = &ptpClock->msgTmpHeader;
    MsgSync *sync = &ptpClock->msgTmp.sync;

    if (ptpClock->msgIbufLength < SYNC_PACKET_LENGTH)
    {
        ERROR("handleSync: short message\n");
        toState(ptpClock, PTP_FAULTY);
        return;
    }

    switch (ptpClock->portDS.port_state)
    {
    case PTP_FAULTY:
    case PTP_INITIALIZING:
    case PTP_DISABLED:
        DBGV("handleSync: disreguard\n");
        return;

    case PTP_UNCALIBRATED:
    case PTP_SLAVE:

        if (isFromSelf)
        {
            DBGV("handleSync: ignore from self\n");
            return;
        }

        if (getFlag(header->flags, PTP_SYNC_BURST) && !ptpClock->portDS.burst_enabled)
            return;

        DBGV("handleSync: looking for uuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
             ptpClock->parentDS.parent.uuid[0], ptpClock->parentDS.parent.uuid[1],
             ptpClock->parentDS.parent.uuid[2], ptpClock->parentDS.parent.uuid[3],
             ptpClock->parentDS.parent.uuid[4], ptpClock->parentDS.parent.uuid[5]);

        if (header->sequenceId > ptpClock->parentDS.parent_last_sync_sequence_number
                && (0 == comparePortIdentity(&header->source, &ptpClock->parentDS.parent)))
        {
            setEvent(ptpClock->events, STATE_DECISION_EVENT);
            msgUnpackSync(ptpClock->msgIbuf, sync);
            addForeign(ptpClock, header, sync);

            if (sync->syncInterval != ptpClock->defaultDS.sync_interval)
            {
                DBGV("handleSync: message's sync interval is %d, but clock's is %d\n", sync->syncInterval, ptpClock->defaultDS.sync_interval);
                /* spec recommends handling a sync interval discrepancy as a fault */
            }

            ptpClock->timestamp.syncRecieve = *time;

            if (!getFlag(header->flags, PTP_ASSIST))
            {
                ptpClock->waitingForFollow = FALSE;

                toInternalTime(&originTimestamp, &sync->originTimestamp, &ptpClock->halfEpoch);
                updateOffset(ptpClock, &originTimestamp, &ptpClock->timestamp.syncRecieve);
                updateClock(ptpClock);
            }
            else
            {
                ptpClock->waitingForFollow = TRUE;
            }

            s1(ptpClock, header, sync);

            if (!(--ptpClock->R))
            {
                if (ptpClock->portDS.port_state == PTP_SLAVE)
                {
                    issueDelayReq(ptpClock);
                }

                ptpClock->Q = 0;

                ptpClock->R = getRand(&ptpClock->random_seed) % (PTP_DELAY_REQ_INTERVAL - 2) + 2;
                DBG("handleSync: Q = %d, R = %d\n", ptpClock->Q, ptpClock->R);
            }

            DBGV("SYNC_RECEIPT_TIMER reset\n");

            timerStart(SYNC_RECEIPT_TIMER, PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->defaultDS.sync_interval), ptpClock->itimer);
        }
        else
        {
            DBGV("handleSync: unwanted\n");
        }

    case PTP_LISTENING:

    case PTP_PASSIVE:
        DBGV("SYNC_RECEIPT_TIMER reset\n");
        timerStart(SYNC_RECEIPT_TIMER, PTP_SYNC_RECEIPT_TIMEOUT(ptpClock->defaultDS.sync_interval), ptpClock->itimer);

        if (!isFromSelf)
        {
            setEvent(ptpClock->events, STATE_DECISION_EVENT);
            msgUnpackSync(ptpClock->msgIbuf, sync);
            addForeign(ptpClock, header, sync);
        }

        break;

    case PTP_MASTER:
    default:

        if (!isFromSelf)
        {
            setEvent(ptpClock->events, STATE_DECISION_EVENT);
            msgUnpackSync(ptpClock->msgIbuf, sync);
            addForeign(ptpClock, header, sync);
        }

/*
                    else if (ptpClock->portDS.port_state == PTP_MASTER && ptpClock->defaultDS.clock_followup_capable)
                    {
                          addTime(time, time, &ptpClock->rtOpts->outboundLatency);
                          issueFollowup(time, ptpClock);
                    }
*/

        break;
    }
}

static void handleFollowUp(PtpClock *ptpClock, Boolean isFromSelf)
{
    MsgFollowUp *follow;
    TimeInternal preciseOriginTimestamp;
    MsgHeader *header = &ptpClock->msgTmpHeader;

    if (ptpClock->msgIbufLength < FOLLOW_UP_PACKET_LENGTH)
    {
        ERROR("handleFollowUp: short message\n");
        toState(ptpClock, PTP_FAULTY);
        return;
    }

    switch (ptpClock->portDS.port_state)
    {
    case PTP_UNCALIBRATED: /* non spec */
    case PTP_SLAVE:

        if (isFromSelf)
        {
            DBG("handleFollowUp: ignore from self\n");
            return;
        }

        if (getFlag(header->flags, PTP_SYNC_BURST) && !ptpClock->portDS.burst_enabled)
            return;

        DBGV("handleFollowUp: looking for uuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
             ptpClock->parentDS.parent.uuid[0], ptpClock->parentDS.parent.uuid[1],
             ptpClock->parentDS.parent.uuid[2], ptpClock->parentDS.parent.uuid[3],
             ptpClock->parentDS.parent.uuid[4], ptpClock->parentDS.parent.uuid[5]);

        follow = &ptpClock->msgTmp.follow;

        msgUnpackFollowUp(ptpClock->msgIbuf, follow);

        if (ptpClock->waitingForFollow
                && follow->associatedSequenceId == ptpClock->parentDS.parent_last_sync_sequence_number
                && (0 == comparePortIdentity(&header->source, &ptpClock->parentDS.parent)))
        {
            ptpClock->waitingForFollow = FALSE;

            toInternalTime(&preciseOriginTimestamp, &follow->preciseOriginTimestamp, &ptpClock->halfEpoch);

            DBGV("handleFollowUp: preciseOriginTimestamp %us %dns\n",
                 preciseOriginTimestamp.seconds, preciseOriginTimestamp.nanoseconds);

            updateOffset(ptpClock, &preciseOriginTimestamp, &ptpClock->timestamp.syncRecieve);
            updateClock(ptpClock);
        }
        else
        {
            DBGV("handleFollowUp: unwanted\n");
        }

        break;

    default:
        DBGV("handleFollowUp: disreguard\n");
        return;
    }
}

static void handleDelayReq(PtpClock *ptpClock, TimeInternal *time, Boolean isFromSelf)
{
    if (ptpClock->msgIbufLength < DELAY_REQ_PACKET_LENGTH)
    {
        ERROR("handleDelayReq: short message\n");
        toState(ptpClock, PTP_FAULTY);
        return;
    }

    switch (ptpClock->portDS.port_state)
    {
    case PTP_MASTER:

        if (isFromSelf)
        {
            DBG("handleDelayReq: ignore from self\n");
            return;
        }

        issueDelayResp(ptpClock, time);

        break;

    case PTP_SLAVE:

        if (isFromSelf)
        {
            DBG("handleDelayReq: ignore from self\n");

            /*
                ptpClock->delay_req_send_time.seconds = time->seconds;
                ptpClock->delay_req_send_time.nanoseconds = time->nanoseconds;

                addTime(&ptpClock->delay_req_send_time, &ptpClock->delay_req_send_time, &ptpClock->outboundLatency);

                if (ptpClock->delay_req_receive_time.seconds)
                {
                    updateDelay(&ptpClock->delay_req_send_time, &ptpClock->delay_req_receive_time,
                                &ptpClock->owd_filt, ptpClock);

                    ptpClock->delay_req_send_time.seconds = 0;
                    ptpClock->delay_req_send_time.nanoseconds = 0;
                    ptpClock->delay_req_receive_time.seconds = 0;
                    ptpClock->delay_req_receive_time.nanoseconds = 0;
                }
            */
        }

        break;

    default:
        DBGV("handleDelayReq: disreguard\n");
        return;
    }
}

static void handleDelayResp(PtpClock *ptpClock, Boolean isFromSelf)
{
    MsgDelayResp *resp;
    MsgHeader *header = &ptpClock->msgTmpHeader;

    if (ptpClock->msgIbufLength < DELAY_RESP_PACKET_LENGTH)
    {
        ERROR("handleDelayResp: short message\n");
        toState(ptpClock, PTP_FAULTY);
        return;
    }

    switch (ptpClock->portDS.port_state)
    {
    case PTP_SLAVE:

        if (isFromSelf)
        {
            DBG("handleDelayResp: ignore from self\n");
            return;
        }

        DBGV("handleDelayResp: looking for uuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",

             ptpClock->parentDS.parent.uuid[0], ptpClock->parentDS.parent.uuid[1],
             ptpClock->parentDS.parent.uuid[2], ptpClock->parentDS.parent.uuid[3],
             ptpClock->parentDS.parent.uuid[4], ptpClock->parentDS.parent.uuid[5]);

        resp = &ptpClock->msgTmp.resp;
        msgUnpackDelayResp(ptpClock->msgIbuf, resp);

        if (ptpClock->sentDelayReq
                && resp->requestingSourceSequenceId == ptpClock->sentDelayReqSequenceId
                && (0 == comparePortIdentity(&resp->requestingSource, &ptpClock->portDS.port))
                && (0 == comparePortIdentity(&header->source, &ptpClock->parentDS.parent)))
        {
            ptpClock->sentDelayReq = FALSE;

            toInternalTime(&ptpClock->timestamp.delayReqRecieve, &resp->delayReceiptTimestamp, &ptpClock->halfEpoch);

            if (ptpClock->timestamp.delayReqSend.seconds)
            {
                updateDelay(ptpClock, &ptpClock->timestamp.delayReqSend, &ptpClock->timestamp.delayReqRecieve);

                ptpClock->timestamp.delayReqRecieve.seconds = ptpClock->timestamp.delayReqRecieve.nanoseconds = 0;
                ptpClock->timestamp.delayReqSend.seconds = ptpClock->timestamp.delayReqSend.nanoseconds = 0;
            }
        }
        else
        {
            DBGV("handleDelayResp: unwanted\n");
        }

        break;

    default:
        DBGV("handleDelayResp: disreguard\n");
        return;
    }
}

static void handleManagement(PtpClock *ptpClock, Boolean isFromSelf)
{
    MsgManagement *manage;

    UInteger8 state;

    if (ptpClock->portDS.port_state == PTP_INITIALIZING)
        return;

    manage = &ptpClock->msgTmp.manage;

    msgUnpackManagement(ptpClock->msgIbuf, manage);

    if (0 == comparePortIdentity(&manage->target, &ptpClock->defaultDS.clock)
            || ((manage->target.communicationTechnology == PTP_DEFAULT
                 || manage->target.communicationTechnology == ptpClock->defaultDS.clock.communicationTechnology)
                && !sum(manage->target.uuid, PTP_UUID_LENGTH)))
    {
        switch (manage->managementMessageKey)
        {
        case PTP_MM_OBTAIN_IDENTITY:
        case PTP_MM_GET_DEFAULT_DATA_SET:
        case PTP_MM_GET_CURRENT_DATA_SET:
        case PTP_MM_GET_PARENT_DATA_SET:
        case PTP_MM_GET_PORT_DATA_SET:
        case PTP_MM_GET_GLOBAL_TIME_DATA_SET:
        case PTP_MM_GET_FOREIGN_DATA_SET:
            issueManagement(ptpClock, manage);
            break;

        default:
            /* TODO: check spec */
            setEvent(ptpClock->events, STATE_DECISION_EVENT);
            state = msgUnloadManagement(ptpClock, ptpClock->msgIbuf, manage);

            if (state != ptpClock->portDS.port_state)
                toState(ptpClock, state);

            break;
        }
    }
    else
    {
        DBG("handleManagement: unwanted\n");
    }
}

/* pack and send various messages */
static void issueSync(PtpClock *ptpClock)
{
    TimeInternal internalTime;
    TimeRepresentation originTimestamp;

    ++ptpClock->portDS.last_sync_event_sequence_number;
    ptpClock->parentDS.grandmaster_sequence_number = ptpClock->portDS.last_sync_event_sequence_number;

    /* try to predict outgoing time stamp */
    getTime(&internalTime);
    fromInternalTime(&internalTime, &originTimestamp, ptpClock->halfEpoch);
    msgPackSync(ptpClock, ptpClock->msgObuf, FALSE, &originTimestamp);

    if (!netSendEvent(&ptpClock->netPath, ptpClock->msgObuf, SYNC_PACKET_LENGTH, &internalTime))
    {
        toState(ptpClock, PTP_FAULTY);
    }
    else
    {
        DBGV("issueSync\n");

        if (ptpClock->defaultDS.clock_followup_capable)
        {
            /* compensate with configurable latency, then tell client real time stamp */
            addTime(&internalTime, &internalTime, &ptpClock->outboundLatency);
            issueFollowup(ptpClock, &internalTime);
        }
    }
}

static void issueFollowup(PtpClock *ptpClock, TimeInternal *time)
{
    TimeRepresentation preciseOriginTimestamp;

    ++ptpClock->portDS.last_general_event_sequence_number;

    fromInternalTime(time, &preciseOriginTimestamp, ptpClock->halfEpoch);
    msgPackFollowUp(ptpClock, ptpClock->msgObuf, ptpClock->portDS.last_sync_event_sequence_number, &preciseOriginTimestamp);

    if (!netSendGeneral(&ptpClock->netPath, ptpClock->msgObuf, FOLLOW_UP_PACKET_LENGTH))
    {
        toState(ptpClock, PTP_FAULTY);
    }
    else
    {
        DBGV("issueFollowup\n");
    }
}

static void issueDelayReq(PtpClock *ptpClock)
{
    TimeInternal internalTime;
    TimeRepresentation originTimestamp;

    ptpClock->sentDelayReq = TRUE;
    ptpClock->sentDelayReqSequenceId = ++ptpClock->portDS.last_sync_event_sequence_number;

    getTime(&internalTime);
    fromInternalTime(&internalTime, &originTimestamp, ptpClock->halfEpoch);
    msgPackDelayReq(ptpClock, ptpClock->msgObuf, FALSE, &originTimestamp);

    if (!netSendEvent(&ptpClock->netPath, ptpClock->msgObuf, DELAY_REQ_PACKET_LENGTH, &internalTime))
    {
        toState(ptpClock, PTP_FAULTY);
    }
    else
    {
        DBGV("issueDelayReq\n");

        addTime(&internalTime, &internalTime, &ptpClock->outboundLatency);
        ptpClock->timestamp.delayReqSend = internalTime;
    }
}

static void issueDelayResp(PtpClock *ptpClock, TimeInternal *time)
{
    TimeRepresentation delayReceiptTimestamp;

    ++ptpClock->portDS.last_general_event_sequence_number;

    fromInternalTime(time, &delayReceiptTimestamp, ptpClock->halfEpoch);
    msgPackDelayResp(ptpClock, ptpClock->msgObuf, &ptpClock->msgTmpHeader, &delayReceiptTimestamp);

    if (!netSendGeneral(&ptpClock->netPath, ptpClock->msgObuf, DELAY_RESP_PACKET_LENGTH))
    {
        toState(ptpClock, PTP_FAULTY);
    }
    else
    {
        DBGV("issueDelayResp\n");
    }
}

static void issueManagement(PtpClock *ptpClock, MsgManagement *manage)
{
    UInteger16 length;

    ++ptpClock->portDS.last_general_event_sequence_number;

    length = msgPackManagementResponse(ptpClock, ptpClock->msgObuf, &ptpClock->msgTmpHeader, manage);

    if (!length)
        return;

    if (!netSendGeneral(&ptpClock->netPath, ptpClock->msgObuf, length))
    {
        toState(ptpClock, PTP_FAULTY);
    }
    else
    {
        DBGV("issueManagement\n");
    }
}
