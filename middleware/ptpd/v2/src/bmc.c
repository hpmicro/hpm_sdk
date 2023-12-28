/* bmc.c */

#include "ptpd.h"

/**
 * \brief Convert EUI48 format to EUI64
 */
void EUI48toEUI64(const Octet * eui48, Octet * eui64)
{
    eui64[0] = eui48[0];
    eui64[1] = eui48[1];
    eui64[2] = eui48[2];
    eui64[3] = 0xff;
    eui64[4] = 0xfe;
    eui64[5] = eui48[3];
    eui64[6] = eui48[4];
    eui64[7] = eui48[5];
}

/* Init ptpClock with run time values (initialization constants are in constants.h)*/
void initData(PtpClock *ptpClock)
{
    RunTimeOpts * rtOpts;

    DBG("initData\n");

    rtOpts = ptpClock->rtOpts;

    /* Default data set */
    ptpClock->defaultDS.twoStepFlag = DEFAULT_TWO_STEP_FLAG;

    /*init clockIdentity with MAC address and 0xFF and 0xFE. see spec 7.5.2.2.2*/

    if ((CLOCK_IDENTITY_LENGTH == 8) && (PTP_UUID_LENGTH == 6))
    {
        DBGVV("initData: EUI48toEUI64\n");
        EUI48toEUI64(ptpClock->portUuidField, ptpClock->defaultDS.clockIdentity);
    }
    else if (CLOCK_IDENTITY_LENGTH == PTP_UUID_LENGTH)
    {
        memcpy(ptpClock->defaultDS.clockIdentity, ptpClock->portUuidField, CLOCK_IDENTITY_LENGTH);
    }
    else
    {
        ERROR("initData: UUID length is not valid");
    }

    ptpClock->defaultDS.numberPorts = NUMBER_PORTS;

    ptpClock->defaultDS.clockQuality.clockAccuracy = rtOpts->clockQuality.clockAccuracy;
    ptpClock->defaultDS.clockQuality.clockClass = rtOpts->clockQuality.clockClass;
    ptpClock->defaultDS.clockQuality.offsetScaledLogVariance = rtOpts->clockQuality.offsetScaledLogVariance;

    ptpClock->defaultDS.priority1 = rtOpts->priority1;
    ptpClock->defaultDS.priority2 = rtOpts->priority2;

    ptpClock->defaultDS.domainNumber = rtOpts->domainNumber;
    ptpClock->defaultDS.slaveOnly = rtOpts->slaveOnly;

    /*Port configuration data set */

    /*PortIdentity Init (portNumber = 1 for an ardinary clock spec 7.5.2.3)*/
    memcpy(ptpClock->portDS.portIdentity.clockIdentity, ptpClock->defaultDS.clockIdentity, CLOCK_IDENTITY_LENGTH);
    ptpClock->portDS.portIdentity.portNumber = NUMBER_PORTS;
    ptpClock->portDS.logMinDelayReqInterval = DEFAULT_DELAYREQ_INTERVAL;
    ptpClock->portDS.peerMeanPathDelay.seconds = ptpClock->portDS.peerMeanPathDelay.nanoseconds = 0;
    ptpClock->portDS.logAnnounceInterval = rtOpts->announceInterval;
    ptpClock->portDS.announceReceiptTimeout = DEFAULT_ANNOUNCE_RECEIPT_TIMEOUT;
    ptpClock->portDS.logSyncInterval = rtOpts->syncInterval;
    ptpClock->portDS.delayMechanism = rtOpts->delayMechanism;
    ptpClock->portDS.logMinPdelayReqInterval = DEFAULT_PDELAYREQ_INTERVAL;
    ptpClock->portDS.versionNumber = VERSION_PTP;

    /*Init other stuff*/
    ptpClock->foreignMasterDS.count = 0;
    ptpClock->foreignMasterDS.capacity = rtOpts->maxForeignRecords;

    ptpClock->inboundLatency = rtOpts->inboundLatency;
    ptpClock->outboundLatency = rtOpts->outboundLatency;

    ptpClock->servo.sDelay = rtOpts->servo.sDelay;
    ptpClock->servo.sOffset = rtOpts->servo.sOffset;
    ptpClock->servo.ai = rtOpts->servo.ai;
    ptpClock->servo.ap = rtOpts->servo.ap;
    ptpClock->servo.noAdjust = rtOpts->servo.noAdjust;
    ptpClock->servo.noResetClock = rtOpts->servo.noResetClock;

    ptpClock->stats = rtOpts->stats;
}

Boolean isSamePortIdentity(const PortIdentity * A, const PortIdentity * B)
{
    return (Boolean)(
               0 == memcmp(A->clockIdentity, B->clockIdentity, CLOCK_IDENTITY_LENGTH)
               && (A->portNumber == B->portNumber)
           );
}

void addForeign(PtpClock *ptpClock, const MsgHeader *header, const MsgAnnounce * announce)
{
    int i, j;
    Boolean found = FALSE;

    j = ptpClock->foreignMasterDS.best;

    /*Check if Foreign master is already known*/

    for (i = 0;i < ptpClock->foreignMasterDS.count;i++)
    {
        if (isSamePortIdentity(&header->sourcePortIdentity, &ptpClock->foreignMasterDS.records[j].foreignMasterPortIdentity))
        {
            /*Foreign Master is already in Foreignmaster data set*/
            ptpClock->foreignMasterDS.records[j].foreignMasterAnnounceMessages++;
            found = TRUE;
            DBGV("addForeign: AnnounceMessage incremented \n");
            ptpClock->foreignMasterDS.records[j].header = *header;
            ptpClock->foreignMasterDS.records[j].announce = *announce;
            break;
        }

        j = (j + 1) % ptpClock->foreignMasterDS.count;
    }

    /*New Foreign Master*/
    if (!found)
    {
        if (ptpClock->foreignMasterDS.count < ptpClock->foreignMasterDS.capacity)
        {
            ptpClock->foreignMasterDS.count++;
        }

        j = ptpClock->foreignMasterDS.i;

        /*Copy new foreign master data set from Announce message*/
        memcpy(ptpClock->foreignMasterDS.records[j].foreignMasterPortIdentity.clockIdentity, header->sourcePortIdentity.clockIdentity, CLOCK_IDENTITY_LENGTH);
        ptpClock->foreignMasterDS.records[j].foreignMasterPortIdentity.portNumber = header->sourcePortIdentity.portNumber;
        ptpClock->foreignMasterDS.records[j].foreignMasterAnnounceMessages = 0;

        /*header and announce field of each Foreign Master are usefull to run Best Master Clock Algorithm*/
        ptpClock->foreignMasterDS.records[j].header = *header;
        ptpClock->foreignMasterDS.records[j].announce = *announce;
        DBGV("addForeign: New foreign Master added \n");

        ptpClock->foreignMasterDS.i = (ptpClock->foreignMasterDS.i + 1) % ptpClock->foreignMasterDS.capacity;
    }
}

#define m2 m1

/*Local clock is becoming Master. Table 13 (9.3.5) of the spec.*/
void m1(PtpClock *ptpClock)
{
    DBGV("bmc: m1\n");

    /*Current data set update*/
    ptpClock->currentDS.stepsRemoved = 0;
    ptpClock->currentDS.offsetFromMaster.seconds = ptpClock->currentDS.offsetFromMaster.nanoseconds = 0;
    ptpClock->currentDS.meanPathDelay.seconds = ptpClock->currentDS.meanPathDelay.nanoseconds = 0;

    /*Parent data set*/
    memcpy(ptpClock->parentDS.parentPortIdentity.clockIdentity, ptpClock->defaultDS.clockIdentity, CLOCK_IDENTITY_LENGTH);
    ptpClock->parentDS.parentPortIdentity.portNumber = 0;
    memcpy(ptpClock->parentDS.grandmasterIdentity, ptpClock->defaultDS.clockIdentity, CLOCK_IDENTITY_LENGTH);
    ptpClock->parentDS.grandmasterClockQuality.clockAccuracy = ptpClock->defaultDS.clockQuality.clockAccuracy;
    ptpClock->parentDS.grandmasterClockQuality.clockClass = ptpClock->defaultDS.clockQuality.clockClass;
    ptpClock->parentDS.grandmasterClockQuality.offsetScaledLogVariance = ptpClock->defaultDS.clockQuality.offsetScaledLogVariance;
    ptpClock->parentDS.grandmasterPriority1 = ptpClock->defaultDS.priority1;
    ptpClock->parentDS.grandmasterPriority2 = ptpClock->defaultDS.priority2;

    /*Time Properties data set*/
    
    ptpClock->timePropertiesDS.currentUtcOffset = ptpClock->rtOpts->currentUtcOffset;
    ptpClock->timePropertiesDS.currentUtcOffsetValid = DEFAULT_UTC_VALID;
    ptpClock->timePropertiesDS.leap59 = FALSE;
    ptpClock->timePropertiesDS.leap61 = FALSE;
    ptpClock->timePropertiesDS.timeTraceable = DEFAULT_TIME_TRACEABLE;
    ptpClock->timePropertiesDS.frequencyTraceable = DEFAULT_FREQUENCY_TRACEABLE;
    ptpClock->timePropertiesDS.ptpTimescale = (Boolean)(DEFAULT_TIMESCALE == PTP_TIMESCALE);
    ptpClock->timePropertiesDS.timeSource = DEFAULT_TIME_SOURCE;
}

void p1(PtpClock *ptpClock)
{
    DBGV("bmc: p1\n");
}

/*Local clock is synchronized to Ebest Table 16 (9.3.5) of the spec*/
void s1(PtpClock *ptpClock, const MsgHeader *header, const MsgAnnounce *announce)
{
    Boolean isFromCurrentParent;

    DBGV("bmc: s1\n");

    /*Current DS*/
    ptpClock->currentDS.stepsRemoved = announce->stepsRemoved + 1;

    isFromCurrentParent = isSamePortIdentity(&ptpClock->parentDS.parentPortIdentity, &header->sourcePortIdentity);

    if (!isFromCurrentParent)
    {
        setFlag(ptpClock->events, MASTER_CLOCK_CHANGED);
    }

    /*Parent DS*/
    memcpy(ptpClock->parentDS.parentPortIdentity.clockIdentity, header->sourcePortIdentity.clockIdentity, CLOCK_IDENTITY_LENGTH);
    ptpClock->parentDS.parentPortIdentity.portNumber = header->sourcePortIdentity.portNumber;
    memcpy(ptpClock->parentDS.grandmasterIdentity, announce->grandmasterIdentity, CLOCK_IDENTITY_LENGTH);
    ptpClock->parentDS.grandmasterClockQuality.clockAccuracy = announce->grandmasterClockQuality.clockAccuracy;
    ptpClock->parentDS.grandmasterClockQuality.clockClass = announce->grandmasterClockQuality.clockClass;
    ptpClock->parentDS.grandmasterClockQuality.offsetScaledLogVariance = announce->grandmasterClockQuality.offsetScaledLogVariance;
    ptpClock->parentDS.grandmasterPriority1 = announce->grandmasterPriority1;
    ptpClock->parentDS.grandmasterPriority2 = announce->grandmasterPriority2;

    /*Timeproperties DS*/
    ptpClock->timePropertiesDS.currentUtcOffset = announce->currentUtcOffset;
    ptpClock->timePropertiesDS.currentUtcOffsetValid = getFlag(header->flagField[1], FLAG1_UTC_OFFSET_VALID);
    ptpClock->timePropertiesDS.leap59 = getFlag(header->flagField[1], FLAG1_LEAP59);
    ptpClock->timePropertiesDS.leap61 = getFlag(header->flagField[1], FLAG1_LEAP61);
    ptpClock->timePropertiesDS.timeTraceable = getFlag(header->flagField[1], FLAG1_TIME_TRACEABLE);
    ptpClock->timePropertiesDS.frequencyTraceable = getFlag(header->flagField[1], FLAG1_FREQUENCY_TRACEABLE);
    ptpClock->timePropertiesDS.ptpTimescale = getFlag(header->flagField[1], FLAG1_PTP_TIMESCALE);
    ptpClock->timePropertiesDS.timeSource = announce->timeSource;
}

/**
 * \brief Copy local data set into header and announce message. 9.3.4 table 12
 */
void copyD0(MsgHeader *header, MsgAnnounce *announce, PtpClock *ptpClock)
{
    announce->grandmasterPriority1 = ptpClock->defaultDS.priority1;
    memcpy(announce->grandmasterIdentity, ptpClock->defaultDS.clockIdentity, CLOCK_IDENTITY_LENGTH);
    announce->grandmasterClockQuality.clockClass = ptpClock->defaultDS.clockQuality.clockClass;
    announce->grandmasterClockQuality.clockAccuracy = ptpClock->defaultDS.clockQuality.clockAccuracy;
    announce->grandmasterClockQuality.offsetScaledLogVariance = ptpClock->defaultDS.clockQuality.offsetScaledLogVariance;
    announce->grandmasterPriority2 = ptpClock->defaultDS.priority2;
    announce->stepsRemoved = 0;
    memcpy(header->sourcePortIdentity.clockIdentity, ptpClock->defaultDS.clockIdentity, CLOCK_IDENTITY_LENGTH);
}


#define A_better_then_B 1
#define B_better_then_A -1
#define A_better_by_topology_then_B 1
#define B_better_by_topology_then_A -1
#define ERROR_1         0
#define ERROR_2         -0


#define COMPARE_AB_RETURN_BETTER(cond, msg)                             \
    if((announceA->cond) > (announceB->cond)) {                         \
        DBGVV("bmcDataSetComparison: " msg ": B better then A\n");       \
        return B_better_then_A;                                         \
    }                                                                   \
    if((announceB->cond) > (announceA->cond)) {                         \
        DBGVV("bmcDataSetComparison: " msg ": A better then B\n");       \
        return A_better_then_B;                                         \
    }                                                                   \
     
/**
 * \brief Data set comparison bewteen two foreign masters (9.3.4 fig 27)
 * return similar to memcmp()
 */
Integer8 bmcDataSetComparison(MsgHeader *headerA, MsgAnnounce *announceA,
                              MsgHeader *headerB, MsgAnnounce *announceB, PtpClock *ptpClock)
{
    int grandmasterIdentityComp;
    short comp = 0;

    DBGV("bmcDataSetComparison\n");
    /*Identity comparison*/

    /* GM identity of A == GM identity of B */
    /* TODO: zkontrolovat memcmp, co vraci za vysledky !*/
    grandmasterIdentityComp = memcmp(announceA->grandmasterIdentity, announceB->grandmasterIdentity, CLOCK_IDENTITY_LENGTH);

    if (0 != grandmasterIdentityComp)
    {
        /* Algoritgm part 1 - Figure 27 */
        COMPARE_AB_RETURN_BETTER(grandmasterPriority1,"grandmaster.Priority1");
        COMPARE_AB_RETURN_BETTER(grandmasterClockQuality.clockClass,"grandmaster.clockClass");
        COMPARE_AB_RETURN_BETTER(grandmasterClockQuality.clockAccuracy,"grandmaster.clockAccuracy");
        COMPARE_AB_RETURN_BETTER(grandmasterClockQuality.offsetScaledLogVariance,"grandmaster.Variance");
        COMPARE_AB_RETURN_BETTER(grandmasterPriority2,"grandmaster.Priority2");

        if (grandmasterIdentityComp > 0)
        {
            DBGVV("bmcDataSetComparison: grandmaster.Identity: B better then A\n");
            return B_better_then_A;
        }
        else if (grandmasterIdentityComp < 0)
        {
            DBGVV("bmcDataSetComparison: grandmaster.Identity: A better then B\n");
            return A_better_then_B;
        }
    }

    /* Algoritgm part 2 - Figure 28 */
    if ((announceA->stepsRemoved) > (announceB->stepsRemoved + 1)) {
        DBGVV("bmcDataSetComparison: stepsRemoved: B better then A\n");
        return B_better_then_A;
    }

    if ((announceB->stepsRemoved) > (announceA->stepsRemoved + 1)) {
        DBGVV("bmcDataSetComparison: stepsRemoved: A better then B\n");
        return A_better_then_B;
    }

    if ((announceA->stepsRemoved) > (announceB->stepsRemoved))
    {
        comp = memcmp(headerA->sourcePortIdentity.clockIdentity, ptpClock->portDS.portIdentity.clockIdentity, CLOCK_IDENTITY_LENGTH);

        if (comp > 0)
        {
            /* reciever < sender*/
            DBGVV("bmcDataSetComparison: PortIdentity: B better then A\n");
            return B_better_then_A;
        }
        else if (comp < 0)
        {
            /* reciever > sender */
            DBGVV("bmcDataSetComparison: PortIdentity: B better by topology then A\n");
            return B_better_by_topology_then_A;
        }
        else
        {
            DBGVV("bmcDataSetComparison: ERROR 1\n");
            return ERROR_1;
        }
    }
    else if ((announceA->stepsRemoved) < (announceB->stepsRemoved))
    {
        comp = memcmp(headerB->sourcePortIdentity.clockIdentity, ptpClock->portDS.portIdentity.clockIdentity, CLOCK_IDENTITY_LENGTH);

        if (comp > 0)
        {
            /* reciever < sender*/
            DBGVV("bmcDataSetComparison: PortIdentity: A better then B\n");
            return A_better_then_B;
        }
        else if (comp < 0)
        {
            /* reciever > sender */
            DBGVV("bmcDataSetComparison: PortIdentity: A better by topology then B\n");
            return A_better_by_topology_then_B;
        }
        else
        {
            DBGV("bmcDataSetComparison: ERROR 1\n");
            return ERROR_1;
        }
    }

    comp = memcmp(headerA->sourcePortIdentity.clockIdentity, headerB->sourcePortIdentity.clockIdentity, CLOCK_IDENTITY_LENGTH);

    if (comp > 0)
    {
        /* A > B*/
        DBGVV("bmcDataSetComparison: sourcePortIdentity: B better by topology then A\n");
        return B_better_by_topology_then_A;
    }
    else if (comp < 0)
    {
        /* B > A */
        DBGVV("bmcDataSetComparison: sourcePortIdentity: A better by topology then B\n");
        return A_better_by_topology_then_B;
    }

    /* compare port numbers of recievers of A and B - same as we have only one port */
    DBGV("bmcDataSetComparison: ERROR 2\n");
    return ERROR_2;
}

/**
 * \brief State decision algorithm 9.3.3 Fig 26
 */
UInteger8 bmcStateDecision(MsgHeader *header, MsgAnnounce *announce, PtpClock *ptpClock)
{
    int comp;

    if ((!ptpClock->foreignMasterDS.count) && (ptpClock->portDS.portState == PTP_LISTENING))
    {
        return PTP_LISTENING;
    }

    copyD0(&ptpClock->msgTmpHeader, &ptpClock->msgTmp.announce, ptpClock);

    comp = bmcDataSetComparison(&ptpClock->msgTmpHeader, &ptpClock->msgTmp.announce, header, announce, ptpClock);

    DBGV("bmcStateDecision: %d\n", comp);

    if (ptpClock->defaultDS.clockQuality.clockClass < 128)
    {
        if (A_better_then_B == comp)
        {
            m1(ptpClock);  /* M1 */
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
        if (A_better_then_B == comp)
        {
            m2(ptpClock); /* M2 */
            return PTP_MASTER;
        }
        else
        {
            s1(ptpClock, header, announce);
            return PTP_SLAVE;
        }
    }
}



UInteger8 bmc(PtpClock *ptpClock)
{
    Integer16 i, best;

    /* starting from i = 1, not necessery to test record[i = 0] against record[best = 0] -> they are the same*/
    for (i = 1, best = 0; i < ptpClock->foreignMasterDS.count; i++)
    {
        if ((bmcDataSetComparison(&ptpClock->foreignMasterDS.records[i].header, &ptpClock->foreignMasterDS.records[i].announce,
                                  &ptpClock->foreignMasterDS.records[best].header, &ptpClock->foreignMasterDS.records[best].announce, ptpClock)) < 0)
        {
            best = i;
        }
    }

    DBGV("bmc: best record %d\n", best);

    ptpClock->foreignMasterDS.best = best;

    return bmcStateDecision(&ptpClock->foreignMasterDS.records[best].header, &ptpClock->foreignMasterDS.records[best].announce, ptpClock);
}


