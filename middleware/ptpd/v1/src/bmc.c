/* bmc.c */

#include "ptpd.h"

/* 6.2.4.1 */
Integer32 comparePortIdentity(const PortIdentity * A, const PortIdentity * B)
{
    Integer32 cmpres;

    if (A->communicationTechnology < B->communicationTechnology)
        return -1;
    else if (A->communicationTechnology > B->communicationTechnology)
        return 1;

    cmpres = memcmp(A->uuid, B->uuid, PTP_UUID_LENGTH);

    if (cmpres < 0)
        return -1;
    else if (cmpres > 0)
        return 1;

    if (A->portId < B->portId)
        return -1;
    else if (A->portId > B->portId)
        return 1;

    return 0;
}

void copyPortIdentity(PortIdentity * R, const PortIdentity * A)
{
    R->communicationTechnology = A->communicationTechnology;
    R->portId = A->portId;
    memcpy(R->uuid, A->uuid, PTP_UUID_LENGTH);
}


void initData(PtpClock *ptpClock)
{
    RunTimeOpts * rtOpts;

    DBG("initData\n");

    rtOpts = ptpClock->rtOpts;

    /* Port configuration data set */
    ptpClock->portDS.last_sync_event_sequence_number = 0;
    ptpClock->portDS.last_general_event_sequence_number = 0;
    ptpClock->portDS.port.portId = 1;
    ptpClock->portDS.burst_enabled = BURST_ENABLED;

    /* Default data set */
    copyPortIdentity(&ptpClock->defaultDS.clock, &ptpClock->portDS.port);
    ptpClock->defaultDS.clock.portId = 0;
    ptpClock->defaultDS.clock_stratum = rtOpts->clockStratum;
    memcpy(ptpClock->defaultDS.clock_identifier, rtOpts->clockIdentifier, PTP_CODE_STRING_LENGTH);
    ptpClock->defaultDS.sync_interval = rtOpts->syncInterval;
    ptpClock->defaultDS.clock_variance = rtOpts->clockVariance;  /* see spec 7.7 */
    ptpClock->defaultDS.clock_followup_capable = CLOCK_FOLLOWUP;
    ptpClock->defaultDS.preferred = rtOpts->clockPreferred;
    ptpClock->defaultDS.initializable = INITIALIZABLE;
    ptpClock->defaultDS.external_timing = EXTERNAL_TIMING;
    ptpClock->defaultDS.is_boundary_clock = BOUNDARY_CLOCK;
    memcpy(ptpClock->defaultDS.subdomain_name, rtOpts->subdomainName, PTP_SUBDOMAIN_NAME_LENGTH);
    ptpClock->defaultDS.number_ports = NUMBER_PORTS;
    ptpClock->defaultDS.number_foreign_records = 0;

    ptpClock->foreignMasterDS.capacity = rtOpts->max_foreign_records;

    /* Global time properties data set */
    ptpClock->timeDS.current_utc_offset = rtOpts->currentUtcOffset;
    ptpClock->timeDS.epoch_number = rtOpts->epochNumber;

    /* other stuff */
    ptpClock->random_seed = ptpClock->portDS.port.uuid[PTP_UUID_LENGTH-1];

    ptpClock->slaveOnly = rtOpts->slaveOnly;
    ptpClock->displayStats = rtOpts->displayStats;
    ptpClock->servo.noAdjust = rtOpts->noAdjust;
    ptpClock->servo.noResetClock = rtOpts->noResetClock;
    ptpClock->servo.ap = rtOpts->ap;
    ptpClock->servo.ai = rtOpts->ai;
    ptpClock->servo.sDelay = rtOpts->sDelay;
    ptpClock->servo.sOffset = rtOpts->sOffset;
    ptpClock->inboundLatency = rtOpts->inboundLatency;
    ptpClock->outboundLatency = rtOpts->outboundLatency;
}

/* add or update an entry in the foreign master data set */
void addForeign(PtpClock *ptpClock, MsgHeader *header, MsgSync *sync)
{
    int i, j;
    Boolean found = FALSE;

    DBGV("addForeign\n");

    j = ptpClock->foreignMasterDS.best;

    for (i = 0; i < ptpClock->defaultDS.number_foreign_records; ++i)
    {
        if (0 == comparePortIdentity(&header->source, &ptpClock->foreignMasterDS.records[j].foreignMaster))
        {
            ++ptpClock->foreignMasterDS.records[j].foreignMasterSyncs;
            found = TRUE;
            DBGV("addForeign: update record %d\n", j);
            break;
        }

        j = (j + 1) % ptpClock->defaultDS.number_foreign_records;
    }

    if (!found)
    {
        if (ptpClock->defaultDS.number_foreign_records < ptpClock->foreignMasterDS.capacity)
            ++ptpClock->defaultDS.number_foreign_records;

        j = ptpClock->foreignMasterDS.i;

        copyPortIdentity(&ptpClock->foreignMasterDS.records[j].foreignMaster, &header->source);

        DBG("addForeign: new record (%d,%d) %d %d %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
            ptpClock->foreignMasterDS.i, ptpClock->defaultDS.number_foreign_records,
            ptpClock->foreignMasterDS.records[j].foreignMaster.communicationTechnology,
            ptpClock->foreignMasterDS.records[j].foreignMaster.portId,
            ptpClock->foreignMasterDS.records[j].foreignMaster.uuid[0], ptpClock->foreignMasterDS.records[j].foreignMaster.uuid[1],
            ptpClock->foreignMasterDS.records[j].foreignMaster.uuid[2], ptpClock->foreignMasterDS.records[j].foreignMaster.uuid[3],
            ptpClock->foreignMasterDS.records[j].foreignMaster.uuid[4], ptpClock->foreignMasterDS.records[j].foreignMaster.uuid[5]);

        ptpClock->foreignMasterDS.i = (ptpClock->foreignMasterDS.i + 1) % ptpClock->foreignMasterDS.capacity;
    }

    ptpClock->foreignMasterDS.records[j].header = *header;

    ptpClock->foreignMasterDS.records[j].sync = *sync;
}

#define m2 m1
/* see spec table 18 */
void m1(PtpClock *ptpClock)
{
    DBGV("bmc: m1 or m2\n");
    /* Default data set */
    ptpClock->currentDS.steps_removed = 0;
    ptpClock->currentDS.offset_from_master.seconds = ptpClock->currentDS.offset_from_master.nanoseconds = 0;
    ptpClock->currentDS.one_way_delay.seconds = ptpClock->currentDS.one_way_delay.nanoseconds = 0;

    /* Parent data set */
    copyPortIdentity(&ptpClock->parentDS.parent, &ptpClock->defaultDS.clock);
    ptpClock->parentDS.parent_last_sync_sequence_number = 0;
    ptpClock->parentDS.parent_followup_capable = ptpClock->defaultDS.clock_followup_capable;
    ptpClock->parentDS.parent_external_timing = ptpClock->defaultDS.external_timing;
    ptpClock->parentDS.parent_variance = ptpClock->defaultDS.clock_variance;
    copyPortIdentity(&ptpClock->parentDS.grandmaster, &ptpClock->defaultDS.clock);
    ptpClock->parentDS.grandmaster_stratum = ptpClock->defaultDS.clock_stratum;
    memcpy(ptpClock->parentDS.grandmaster_identifier, ptpClock->defaultDS.clock_identifier, PTP_CODE_STRING_LENGTH);
    ptpClock->parentDS.grandmaster_variance = ptpClock->defaultDS.clock_variance;
    ptpClock->parentDS.grandmaster_preferred  = ptpClock->defaultDS.preferred;
    ptpClock->parentDS.grandmaster_is_boundary_clock  = ptpClock->defaultDS.is_boundary_clock;
    ptpClock->parentDS.grandmaster_sequence_number = ptpClock->portDS.last_sync_event_sequence_number;
}

void p1(PtpClock *ptpClock)
{
    DBGV("bmc: p1\n");
}

/* see spec table 21 */
void s1(PtpClock *ptpClock, MsgHeader *header, MsgSync *sync)
{

    DBGV("bmc: s1\n");

    if (0 != comparePortIdentity(&ptpClock->parentDS.parent, &header->source))
    {
        setEvent(ptpClock->events, MASTER_CLOCK_CHANGED);
    }



    /* Current data set */
    ptpClock->currentDS.steps_removed = sync->localStepsRemoved + 1;

    /* Parent data set */
    copyPortIdentity(&ptpClock->parentDS.parent, &header->source);

    ptpClock->parentDS.parent_last_sync_sequence_number = header->sequenceId;

    ptpClock->parentDS.parent_followup_capable = getFlag(header->flags, PTP_ASSIST);

    ptpClock->parentDS.parent_external_timing = getFlag(header->flags, PTP_EXT_SYNC);

    ptpClock->parentDS.parent_variance = sync->localClockVariance;

    copyPortIdentity(&ptpClock->parentDS.grandmaster, &sync->grandmaster);

    ptpClock->parentDS.grandmaster_stratum = sync->grandmasterClockStratum;

    memcpy(ptpClock->parentDS.grandmaster_identifier, sync->grandmasterClockIdentifier, PTP_CODE_STRING_LENGTH);

    ptpClock->parentDS.grandmaster_variance = sync->grandmasterClockVariance;

    ptpClock->parentDS.grandmaster_preferred = sync->grandmasterPreferred;

    ptpClock->parentDS.grandmaster_is_boundary_clock = sync->grandmasterIsBoundaryClock;

    ptpClock->parentDS.grandmaster_sequence_number = sync->grandmasterSequenceId;

    /* Global time properties data set */
    ptpClock->timeDS.current_utc_offset = sync->currentUTCOffset;

    ptpClock->timeDS.leap_59 =  getFlag(header->flags, PTP_LI_59);

    ptpClock->timeDS.leap_61 =  getFlag(header->flags, PTP_LI_61);

    ptpClock->timeDS.epoch_number = sync->epochNumber;
}

/* 7.6.4 - Table 16 */
static void copyD0(PtpClock *ptpClock, MsgHeader *header, MsgSync *sync)
{
    /* GMUUID */
    copyPortIdentity(&sync->grandmaster, &ptpClock->defaultDS.clock);

    /* GMStratum */
    sync->grandmasterClockStratum = ptpClock->defaultDS.clock_stratum;

    /* GMIdentifier */
    memcpy(sync->grandmasterClockIdentifier, ptpClock->defaultDS.clock_identifier, PTP_CODE_STRING_LENGTH);

    /* GMVariance */
    sync->grandmasterClockVariance = ptpClock->defaultDS.clock_variance;

    /* GM is boundary clock*/
    sync->grandmasterIsBoundaryClock = ptpClock->defaultDS.is_boundary_clock;

    /* GM is preffered clock */
    sync->grandmasterPreferred = ptpClock->defaultDS.preferred;

    /* Steps Removed */
    sync->localStepsRemoved = ptpClock->currentDS.steps_removed;

    /* UUID of sender */
    copyPortIdentity(&header->source, &ptpClock->defaultDS.clock);

    /* Port Number Recieving */
    /* port_id_field = ptpClock->defaultDS.clock.portId; */

    /* GM sequenceId */
    sync->grandmasterSequenceId = ptpClock->parentDS.grandmaster_sequence_number;

    /* sequenceId */
    header->sequenceId = ptpClock->parentDS.grandmaster_sequence_number;

    /* UUID of reciever */
    /*  copyPortIdentity(reciever, ptpClock->defaultDS.clock); */

    /* Port Number Sending */
    /* header->source.portId = ptpClock->defaultDS.clock.portId; */
}

/* 7.6.6 - Figure 21 */
static int getIdentifierOrder(Octet identifier[PTP_CODE_STRING_LENGTH])
{
    if (!memcmp(identifier, IDENTIFIER_ATOM, PTP_CODE_STRING_LENGTH))
        return 1;
    else if (!memcmp(identifier, IDENTIFIER_GPS, PTP_CODE_STRING_LENGTH))
        return 1;
    else if (!memcmp(identifier, IDENTIFIER_NTP, PTP_CODE_STRING_LENGTH))
        return 2;
    else if (!memcmp(identifier, IDENTIFIER_HAND, PTP_CODE_STRING_LENGTH))
        return 3;
    else if (!memcmp(identifier, IDENTIFIER_INIT, PTP_CODE_STRING_LENGTH))
        return 4;
    else if (!memcmp(identifier, IDENTIFIER_DFLT, PTP_CODE_STRING_LENGTH))
        return 5;

    return 6;
}

/* return similar to memcmp()s
   note: communicationTechnology can be ignored because
   if they differed they would not have made it here */
static Integer8 bmcDataSetComparison(PtpClock *ptpClock,
                                     MsgHeader *headerA, MsgSync *syncA, MsgHeader *headerB, MsgSync *syncB)
{
    int identA, identB, cmpres;

    /* 7.6.4 - Figure 17 */
    DBGV("bmcDataSetComparison\n");

    if (0 == comparePortIdentity(&syncA->grandmaster, &syncB->grandmaster))
    {
        goto path_X;
    }

    if (syncA->grandmasterClockStratum < syncB->grandmasterClockStratum)
        goto path_A;
    else if (syncA->grandmasterClockStratum > syncB->grandmasterClockStratum)
        goto path_B;

    /* grandmasterClockStratums same */

    /* Order Identifiers */
    identA = getIdentifierOrder(syncA->grandmasterClockIdentifier);

    identB = getIdentifierOrder(syncB->grandmasterClockIdentifier);

    if (identA < identB)
        goto path_A;
    else if (identA > identB)
        goto path_B;

    /* grandmasterClockIdentifiers same */
    if (!(syncA->grandmasterClockStratum > 2))
    {
        if (syncA->grandmasterPreferred || syncB->grandmasterPreferred)
        {
            if (syncA->grandmasterPreferred)
                goto path_A1;
            else
                goto path_B1;
        }
        else
        {
            goto path_X;
        }
    }

    /* 7.6.6 - Figure 22 */
    if (syncA->grandmasterClockVariance > syncB->grandmasterClockVariance + PTP_LOG_VARIANCE_THRESHOLD
            || syncA->grandmasterClockVariance < syncB->grandmasterClockVariance - PTP_LOG_VARIANCE_THRESHOLD)
    {
        /* grandmasterClockVariances are not similar */
        if (syncA->grandmasterClockVariance < syncB->grandmasterClockVariance)
            goto path_A;
        else
            goto path_B;
    }

    /* grandmasterClockVariances are similar */
    if (syncA->grandmasterIsBoundaryClock || syncB->grandmasterIsBoundaryClock)
    {
        if (syncA->grandmasterIsBoundaryClock)
            goto path_A;
        else
            goto path_B;
    }

    /* neither is grandmasterIsBoundaryClock */
    if (comparePortIdentity(&syncA->grandmaster, &syncB->grandmaster) < 0)
        goto path_A;
    else
        goto path_B;


path_A:
    DBGV("bmcDataSetComparison: path A\n");

    if (syncB->grandmasterPreferred && !syncA->grandmasterPreferred)
        goto path_B1;
    else
        goto path_A1;

path_B:
    DBGV("bmcDataSetComparison: path B\n");

    if (syncA->grandmasterPreferred && !syncB->grandmasterPreferred)
        goto path_A1;
    else
        goto path_B1;

    /* 7.6.4 - Figure 18 */
path_X:
    DBGV("bmcDataSetComparison: path X\n");

    if (syncA->localStepsRemoved > syncB->localStepsRemoved + 1
            || syncA->localStepsRemoved < syncB->localStepsRemoved - 1)
    {
        /* localStepsRemoved not within 1 */
        if (syncA->localStepsRemoved < syncB->localStepsRemoved)
            goto path_A1;
        else
            goto path_B1;
    }

    /* localStepsRemoved within 1 */
    if (syncA->localStepsRemoved < syncB->localStepsRemoved)
        goto path_A3;
    else if (syncA->localStepsRemoved > syncB->localStepsRemoved)
        goto path_B3;

    /* localStepsRemoved same */
    cmpres = comparePortIdentity(&headerA->source, &headerB->source);

    if (0 == cmpres)
        goto path_Z;
    else if (cmpres > 0)
        goto path_A2;
    else
        goto path_B2;

    /* 7.6.4 - Figure 19 */
path_Z:
    DBGV("bmcDataSetComparison: path Z\n");

    /* sourceUuid same */

    /* port number is the same */
    /* ptpClock->portDS.port.portId == ptpClock->portDS.port.portId */

    if (syncA->grandmasterSequenceId > syncB->grandmasterSequenceId)
        goto path_A4;
    else if (syncA->grandmasterSequenceId < syncB->grandmasterSequenceId)
        goto path_B4;

    /* grandmasterSequenceId same */
    if (headerA->sequenceId > headerB->sequenceId)
        goto path_A4;
    else if (headerA->sequenceId < headerB->sequenceId)
        goto path_B4;

    /* sequenceId same */
    goto path_SAME;  /* same */

    /* 7.6.4 - Figure 20 */
path_A3:
    DBGV("bmcDataSetComparison: path A3\n");

    cmpres = comparePortIdentity(&ptpClock->portDS.port, &headerB->source);

    if (cmpres < 0)
        goto path_A1;
    else if (cmpres > 0)
        goto path_A2;


    /* this port_uuid_field same as headerB->sourceUuid */
    if (ptpClock->portDS.port.portId < headerB->source.portId)
        goto path_A1;
    else if (ptpClock->portDS.port.portId > headerB->source.portId)
        goto path_A2;

    /* this port_id_field same as headerB->sourcePortId */
    goto path_SAME;

path_B3:
    DBGV("bmcDataSetComparison: path B3\n");

    cmpres = memcmp(ptpClock->portDS.port.uuid, headerA->source.uuid, PTP_UUID_LENGTH);

    if (cmpres < 0)
        goto path_B1;
    else if (cmpres > 0)
        goto path_B2;

    /* this port_uuid_field same as headerA->sourceUuid */
    if (ptpClock->portDS.port.portId < headerA->source.portId)
        goto path_B1;
    else if (ptpClock->portDS.port.portId > headerA->source.portId)
        goto path_B2;

    /* this port_id_field same as headerA->sourcePortId */
    goto path_SAME;

path_A1:
path_A2:
path_A4:
    DBGV("bmcDataSetComparison: A better then B\n");

    return 1; //A_better_then_B;

path_B1:
path_B2:
path_B4:
    DBGV("bmcDataSetComparison: B better then A\n");

    return -1; //B_better_then_A;

path_SAME:
    DBGV("bmcDataSetComparison: A = B\n");

    return 0;
}

/* 7.6.4 - Figure 16*/
static UInteger8 bmcStateDecision(PtpClock *ptpClock, MsgHeader *header, MsgSync *sync)
{
    /* non spec */
    if (ptpClock->slaveOnly)
    {
        s1(ptpClock, header, sync);
        return PTP_SLAVE;
    }

    copyD0(ptpClock, &ptpClock->msgTmpHeader, &ptpClock->msgTmp.sync);

    if (ptpClock->defaultDS.clock_stratum < 3)
    {
        if (bmcDataSetComparison(ptpClock, &ptpClock->msgTmpHeader, &ptpClock->msgTmp.sync, header, sync) > 0)
        {
            m1(ptpClock);
            return PTP_MASTER;
        }
        else
        {
            p1(ptpClock);
            return PTP_PASSIVE;
        }
    }
    else
    {
        if (bmcDataSetComparison(ptpClock, &ptpClock->msgTmpHeader, &ptpClock->msgTmp.sync, header, sync) > 0)
        {
            m2(ptpClock);
            return PTP_MASTER;
        }
        else
        {
            s1(ptpClock, header, sync);
            return PTP_SLAVE;
        }
    }
}

UInteger8 bmc(PtpClock *ptpClock)
{
    Integer16 i, best;
    ForeignMasterRecord *foreign = ptpClock->foreignMasterDS.records;

    if (!ptpClock->defaultDS.number_foreign_records)
    {
        if (ptpClock->portDS.port_state == PTP_MASTER)
            m1(ptpClock);

        return ptpClock->portDS.port_state;  /* no change */
    }

    for (i = 1, best = 0; i < ptpClock->defaultDS.number_foreign_records; ++i)
    {
        if (bmcDataSetComparison(ptpClock, &foreign[i].header, &foreign[i].sync,
                                 &foreign[best].header, &foreign[best].sync) > 0)
            best = i;
    }

    DBGV("bmc: best record %d\n", best);

    ptpClock->foreignMasterDS.best = best;

    return bmcStateDecision(ptpClock, &foreign[best].header, &foreign[best].sync);
}
