/* msg.c */
/* see spec annex d */

#include "../ptpd.h"

Boolean msgPeek(Octet *buf, ssize_t length)
{
    /* not imlpemented yet */
    return TRUE;
}

void msgUnpackHeader(Octet *buf, MsgHeader *header)
{
    header->versionPTP = flip16(*(UInteger16*)(buf + 0));
    header->versionNetwork = flip16(*(UInteger16*)(buf + 2));
    DBGVV("msgUnpackHeader: versionPTP %d\n", header->versionPTP);
    DBGVV("msgUnpackHeader: versionNetwork %d\n", header->versionNetwork);

    memcpy(header->subdomain, (buf + 4), 16);
    DBGVV("msgUnpackHeader: subdomain %s\n", header->subdomain);

    header->messageType = *(UInteger8*)(buf + 20);
    header->source.communicationTechnology = *(UInteger8*)(buf + 21);
    DBGVV("msgUnpackHeader: messageType %d\n", header->messageType);
    DBGVV("msgUnpackHeader: sourceCommunicationTechnology %d\n", header->source.communicationTechnology);

    memcpy(header->source.uuid, (buf + 22), 6);
    DBGVV("msgUnpackHeader: sourceUuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
          header->source.uuid[0], header->source.uuid[1], header->source.uuid[2],
          header->source.uuid[3], header->source.uuid[4], header->source.uuid[5]);

    header->source.portId = flip16(*(UInteger16*)(buf + 28));
    header->sequenceId = flip16(*(UInteger16*)(buf + 30));
    DBGVV("msgUnpackHeader: sourcePortId %d\n", header->source.portId);
    DBGVV("msgUnpackHeader: sequenceId %d\n", header->sequenceId);

    header->control = *(UInteger8*)(buf + 32);
    DBGVV("msgUnpackHeader: control %d\n", header->control);

    memcpy(header->flags, (buf + 34), 2);
    DBGVV("msgUnpackHeader: flags %02hhx %02hhx\n", header->flags[0], header->flags[1]);
}

void msgUnpackSync(Octet *buf, MsgSync *sync)
{
    sync->originTimestamp.seconds = flip32(*(UInteger32*)(buf + 40));
    DBGVV("msgUnpackSync: originTimestamp.seconds %u\n", sync->originTimestamp.seconds);
    sync->originTimestamp.nanoseconds = flip32(*(Integer32*)(buf + 44));
    DBGVV("msgUnpackSync: originTimestamp.nanoseconds %d\n", sync->originTimestamp.nanoseconds);
    sync->epochNumber = flip16(*(UInteger16*)(buf + 48));
    DBGVV("msgUnpackSync: epochNumber %d\n", sync->epochNumber);
    sync->currentUTCOffset = flip16(*(Integer16*)(buf + 50));
    DBGVV("msgUnpackSync: currentUTCOffset %d\n", sync->currentUTCOffset);
    sync->grandmaster.communicationTechnology = *(UInteger8*)(buf + 53);
    DBGVV("msgUnpackSync: grandmasterCommunicationTechnology %d\n", sync->grandmaster.communicationTechnology);
    memcpy(sync->grandmaster.uuid, (buf + 54), 6);
    DBGVV("msgUnpackSync: grandmasterClockUuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
          sync->grandmaster.uuid[0], sync->grandmaster.uuid[1], sync->grandmaster.uuid[2],
          sync->grandmaster.uuid[3], sync->grandmaster.uuid[4], sync->grandmaster.uuid[5]);
    sync->grandmaster.portId = flip16(*(UInteger16*)(buf + 60));
    DBGVV("msgUnpackSync: grandmasterPortId %d\n", sync->grandmaster.portId);
    sync->grandmasterSequenceId = flip16(*(UInteger16*)(buf + 62));
    DBGVV("msgUnpackSync: grandmasterSequenceId %d\n", sync->grandmasterSequenceId);
    sync->grandmasterClockStratum = *(UInteger8*)(buf + 67);
    DBGVV("msgUnpackSync: grandmasterClockStratum %d\n", sync->grandmasterClockStratum);
    memcpy(sync->grandmasterClockIdentifier, (buf + 68), PTP_CODE_STRING_LENGTH);
    DBGVV("msgUnpackSync: grandmasterClockIdentifier %c%c%c%c\n",
          sync->grandmasterClockIdentifier[0], sync->grandmasterClockIdentifier[1],
          sync->grandmasterClockIdentifier[2], sync->grandmasterClockIdentifier[3]);
    sync->grandmasterClockVariance = flip16(*(Integer16*)(buf + 74));
    DBGVV("msgUnpackSync: grandmasterClockVariance %d\n", sync->grandmasterClockVariance);
    sync->grandmasterPreferred = (Boolean) * (UInteger8*)(buf + 77);
    DBGVV("msgUnpackSync: grandmasterPreferred %d\n", sync->grandmasterPreferred);
    sync->grandmasterIsBoundaryClock = (Boolean) * (UInteger8*)(buf + 79);
    DBGVV("msgUnpackSync: grandmasterIsBoundaryClock %d\n", sync->grandmasterIsBoundaryClock);
    sync->syncInterval = *(Integer8*)(buf + 83);
    DBGVV("msgUnpackSync: syncInterval %d\n", sync->syncInterval);
    sync->localClockVariance = flip16(*(Integer16*)(buf + 86));
    DBGVV("msgUnpackSync: localClockVariance %d\n", sync->localClockVariance);
    sync->localStepsRemoved = flip16(*(UInteger16*)(buf + 90));
    DBGVV("msgUnpackSync: localStepsRemoved %d\n", sync->localStepsRemoved);
    sync->localClockStratum = *(UInteger8*)(buf + 95);
    DBGVV("msgUnpackSync: localClockStratum %d\n", sync->localClockStratum);
    memcpy(sync->localClockIdentifer, (buf + 96), PTP_CODE_STRING_LENGTH);
    DBGVV("msgUnpackSync: localClockIdentifer %c%c%c%c\n",
          sync->localClockIdentifer[0], sync->localClockIdentifer[1],
          sync->localClockIdentifer[2], sync->localClockIdentifer[3]);
    sync->parent.communicationTechnology = *(UInteger8*)(buf + 101);
    DBGVV("msgUnpackSync: parentCommunicationTechnology %d\n", sync->parent.communicationTechnology);
    memcpy(sync->parent.uuid, (buf + 102), PTP_UUID_LENGTH);
    DBGVV("msgUnpackSync: parentUuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
          sync->parent.uuid[0], sync->parent.uuid[1], sync->parent.uuid[2],
          sync->parent.uuid[3], sync->parent.uuid[4], sync->parent.uuid[5]);
    sync->parent.portId = flip16(*(UInteger16*)(buf + 110));
    DBGVV("msgUnpackSync: parentPortField %d\n", sync->parent.portId);
    sync->estimatedMasterVariance = flip16(*(Integer16*)(buf + 114));
    DBGVV("msgUnpackSync: estimatedMasterVariance %d\n", sync->estimatedMasterVariance);
    sync->estimatedMasterDrift = flip32(*(Integer32*)(buf + 116));
    DBGVV("msgUnpackSync: estimatedMasterDrift %d\n", sync->estimatedMasterDrift);
    sync->utcReasonable = (Boolean) * (UInteger8*)(buf + 123);
    DBGVV("msgUnpackSync: utcReasonable %d\n", sync->utcReasonable);
}

void msgUnpackDelayReq(Octet *buf, MsgDelayReq *req) {}

void msgUnpackFollowUp(Octet *buf, MsgFollowUp *follow)
{
    follow->associatedSequenceId = flip16(*(UInteger16*)(buf + 42));
    DBGVV("msgUnpackFollowUp: associatedSequenceId %u\n", follow->associatedSequenceId);
    follow->preciseOriginTimestamp.seconds = flip32(*(UInteger32*)(buf + 44));
    DBGVV("msgUnpackFollowUp: preciseOriginTimestamp.seconds %u\n", follow->preciseOriginTimestamp.seconds);
    follow->preciseOriginTimestamp.nanoseconds = flip32(*(Integer32*)(buf + 48));
    DBGVV("msgUnpackFollowUp: preciseOriginTimestamp.nanoseconds %d\n", follow->preciseOriginTimestamp.nanoseconds);
}

void msgUnpackDelayResp(Octet *buf, MsgDelayResp *resp)
{
    resp->delayReceiptTimestamp.seconds = flip32(*(UInteger32*)(buf + 40));
    DBGVV("msgUnpackDelayResp: delayReceiptTimestamp.seconds %u\n", resp->delayReceiptTimestamp.seconds);
    resp->delayReceiptTimestamp.nanoseconds = flip32(*(Integer32*)(buf + 44));
    DBGVV("msgUnpackDelayResp: delayReceiptTimestamp.nanoseconds %d\n", resp->delayReceiptTimestamp.nanoseconds);
    resp->requestingSource.communicationTechnology = *(UInteger8*)(buf + 49);
    DBGVV("msgUnpackDelayResp: requestingSourceCommunicationTechnology %d\n", resp->requestingSource.communicationTechnology);
    memcpy(resp->requestingSource.uuid, (buf + 50), 6);
    DBGVV("msgUnpackDelayResp: requestingSourceUuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
         resp->requestingSource.uuid[0], resp->requestingSource.uuid[1], resp->requestingSource.uuid[2],
         resp->requestingSource.uuid[3], resp->requestingSource.uuid[4], resp->requestingSource.uuid[5]);
    resp->requestingSource.portId = flip16(*(UInteger16*)(buf + 56));
    DBGVV("msgUnpackDelayResp: requestingSourcePortId %d\n", resp->requestingSource.portId);
    resp->requestingSourceSequenceId = flip16(*(UInteger16*)(buf + 58));
    DBGVV("msgUnpackDelayResp: requestingSourceSequenceId %d\n", resp->requestingSourceSequenceId);
}

void msgUnpackManagement(Octet *buf, MsgManagement *manage)
{
    manage->target.communicationTechnology = *(UInteger8*)(buf + 41);
    DBGVV("msgUnpackManagement: targetCommunicationTechnology %d\n", manage->target.communicationTechnology);
    memcpy(manage->target.uuid, (buf + 42), 6);
    DBGVV("msgUnpackManagement: targetUuid %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
          manage->target.uuid[0], manage->target.uuid[1], manage->target.uuid[2],
          manage->target.uuid[3], manage->target.uuid[4], manage->target.uuid[5]);
    manage->target.portId = flip16(*(UInteger16*)(buf + 48));
    DBGVV("msgUnpackManagement: targetPortId %d\n", manage->target.portId);
    manage->startingBoundaryHops = flip16(*(Integer16*)(buf + 50));
    DBGVV("msgUnpackManagement: startingBoundaryHops %d\n", manage->startingBoundaryHops);
    manage->boundaryHops = flip16(*(Integer16*)(buf + 52));
    DBGVV("msgUnpackManagement: boundaryHops %d\n", manage->boundaryHops);
    manage->managementMessageKey = *(UInteger8*)(buf + 55);
    DBGVV("msgUnpackManagement: managementMessageKey %d\n", manage->managementMessageKey);
    manage->parameterLength = flip16(*(UInteger16*)(buf + 58));
    DBGVV("msgUnpackManagement: parameterLength %d\n", manage->parameterLength);

    if (manage->managementMessageKey == PTP_MM_GET_FOREIGN_DATA_SET)
        manage->recordKey = flip16(*(UInteger16*)(buf + 62));
}

UInteger8 msgUnloadManagement(PtpClock *ptpClock, Octet *buf, MsgManagement *manage)
{
    TimeInternal internalTime;
    TimeRepresentation externalTime;

    switch (manage->managementMessageKey)
    {
    case PTP_MM_INITIALIZE_CLOCK:

        if (ptpClock->defaultDS.initializable)
            return PTP_INITIALIZING;

        break;

    case PTP_MM_GOTO_FAULTY_STATE:
        DBG("event FAULT_DETECTED (forced by management message)\n");

        return PTP_FAULTY;

        /* break; */

    case PTP_MM_DISABLE_PORT:
        if (manage->target.portId == 1)
        {
            DBG("event DESIGNATED_DISABLED\n");
            return PTP_DISABLED;
        }

        break;

    case PTP_MM_ENABLE_PORT:

        if (manage->target.portId == 1)
        {
            DBG("event DESIGNATED_ENABLED\n");
            return PTP_INITIALIZING;
        }

        break;

    case PTP_MM_CLEAR_DESIGNATED_PREFERRED_MASTER:
        ptpClock->defaultDS.preferred = FALSE;
        break;

    case PTP_MM_SET_DESIGNATED_PREFERRED_MASTER:
        ptpClock->defaultDS.preferred = TRUE;
        break;

    case PTP_MM_DISABLE_BURST:
        break;

    case PTP_MM_ENABLE_BURST:
        break;

    case PTP_MM_SET_SYNC_INTERVAL:
        ptpClock->rtOpts->syncInterval = *(Integer8*)(buf + 63);
        break;

    case PTP_MM_SET_SUBDOMAIN:
        memcpy(ptpClock->rtOpts->subdomainName, buf + 60, 16);
        DBG("set subdomain to %s\n", ptpClock->rtOpts->subdomainName);
        break;

    case PTP_MM_SET_TIME:
        externalTime.seconds = flip32(*(UInteger32*)(buf + 60));
        externalTime.nanoseconds = flip32(*(Integer32*)(buf + 64));
        toInternalTime(&internalTime, &externalTime, &ptpClock->halfEpoch);
        setTime(&internalTime);
        break;

    case PTP_MM_UPDATE_DEFAULT_DATA_SET:

        if (!ptpClock->slaveOnly)
            ptpClock->defaultDS.clock_stratum = *(UInteger8*)(buf + 63);

        memcpy(ptpClock->defaultDS.clock_identifier, buf + 64, PTP_CODE_STRING_LENGTH);

        ptpClock->defaultDS.clock_variance = flip16(*(Integer16*)(buf + 70));

        ptpClock->defaultDS.preferred = (Boolean) * (UInteger8*)(buf + 75);

        ptpClock->rtOpts->syncInterval = *(UInteger8*)(buf + 79);

        memcpy(ptpClock->rtOpts->subdomainName, buf + 80, 16);

        break;

    case PTP_MM_UPDATE_GLOBAL_TIME_PROPERTIES:
        ptpClock->timeDS.current_utc_offset = flip16(*(Integer16*)(buf + 62));

        ptpClock->timeDS.leap_59 = (Boolean) * (UInteger8*)(buf + 67);

        ptpClock->timeDS.leap_61 = (Boolean) * (UInteger8*)(buf + 71);

        ptpClock->timeDS.epoch_number = flip16(*(UInteger16*)(buf + 74));

        break;

    default:
        break;
    }

    return ptpClock->portDS.port_state;
}

void msgUnpackManagementPayload(Octet *buf, MsgManagement *manage)
{
    switch (manage->managementMessageKey)
    {
    case PTP_MM_CLOCK_IDENTITY:
        DBGVV("msgUnloadManagementPayload: managementMessageKey PTP_MM_CLOCK_IDENTITY\n");
        manage->payload.clockIdentity.clock.communicationTechnology = *(UInteger8*)(buf + 63);
        memcpy(manage->payload.clockIdentity.clock.uuid, buf + 64, PTP_UUID_LENGTH);
        manage->payload.clockIdentity.clock.portId = flip16(*(UInteger16*)(buf + 74));
        memcpy(manage->payload.clockIdentity.manufacturerIdentity, buf + 76, MANUFACTURER_ID_LENGTH);
        break;

    case PTP_MM_DEFAULT_DATA_SET:
        DBGVV("msgUnloadManagementPayload: managementMessageKey PTP_MM_DEFAULT_DATA_SET\n");
        manage->payload.defaultData.clock.communicationTechnology = *(UInteger8*)(buf + 63);
        memcpy(manage->payload.defaultData.clock.uuid, buf + 64, PTP_UUID_LENGTH);
        manage->payload.defaultData.clock.portId = flip16(*(UInteger16*)(buf + 74));
        manage->payload.defaultData.clockStratum = *(UInteger16*)(buf + 79);
        memcpy(manage->payload.defaultData.clockIdentifier, buf + 80, PTP_CODE_STRING_LENGTH);
        manage->payload.defaultData.clockVariance = flip16(*(UInteger16*)(buf + 86));
        manage->payload.defaultData.clockFollowupCapable = (Boolean) * (UInteger8*)(buf + 91);
        manage->payload.defaultData.preferred = (Boolean) * (UInteger8*)(buf + 95);
        manage->payload.defaultData.initializable = (Boolean) * (UInteger8*)(buf + 99);
        manage->payload.defaultData.externalTiming = (Boolean) * (UInteger8*)(buf + 103);
        manage->payload.defaultData.isBoundaryClock = (Boolean) * (UInteger8*)(buf + 107);
        manage->payload.defaultData.syncInterval = *(UInteger8*)(buf + 111);
        memcpy(manage->payload.defaultData.subdomainName, buf + 112, PTP_SUBDOMAIN_NAME_LENGTH);
        manage->payload.defaultData.numberPorts = flip16(*(UInteger16*)(buf + 130));
        manage->payload.defaultData.numberForeignRecords = flip16(*(UInteger16*)(buf + 134));
        break;

    case PTP_MM_CURRENT_DATA_SET:
        DBGVV("msgUnloadManagementPayload: managementMessageKey PTP_MM_CURRENT_DATA_SET\n");
        manage->payload.current.stepsRemoved = flip16(*(UInteger16*)(buf + 62));
        manage->payload.current.offsetFromMaster.seconds = flip32(*(UInteger32*)(buf + 64));
        manage->payload.current.offsetFromMaster.nanoseconds = flip32(*(UInteger32*)(buf + 68));
        manage->payload.current.oneWayDelay.seconds = flip32(*(UInteger32*)(buf + 72));
        manage->payload.current.oneWayDelay.nanoseconds = flip32(*(Integer32*)(buf + 76));
        break;

    case PTP_MM_PARENT_DATA_SET:
        DBGVV("msgUnloadManagementPayload: managementMessageKey PTP_MM_PORT_DATA_SET\n");
        manage->payload.parent.parent.communicationTechnology = *(UInteger8*)(buf + 63);
        memcpy(manage->payload.parent.parent.uuid, buf + 64, PTP_UUID_LENGTH);
        manage->payload.parent.parent.portId = flip16(*(UInteger16*)(buf + 74));
        manage->payload.parent.parentLastSyncSequenceNumber = flip16(*(UInteger16*)(buf + 74));
        manage->payload.parent.parentFollowupCapable = (Boolean) * (UInteger8*)(buf + 83);
        manage->payload.parent.parentExternalTiming = (Boolean) * (UInteger8*)(buf + 87);
        manage->payload.parent.parentVariance = flip16(*(UInteger16*)(buf + 90));
        manage->payload.parent.parentStats = (Boolean) * (UInteger8*)(buf + 85);
        manage->payload.parent.observedVariance = flip16(*(Integer16*)(buf + 98));
        manage->payload.parent.observedDrift = flip32(*(Integer32*)(buf + 100));
        manage->payload.parent.utcReasonable = (Boolean) * (UInteger8*)(buf + 107);
        manage->payload.parent.grandmaster.communicationTechnology = *(UInteger8*)(buf + 111);
        memcpy(manage->payload.parent.grandmaster.uuid, buf + 112, PTP_UUID_LENGTH);
        manage->payload.parent.grandmaster.portId = flip16(*(UInteger16*)(buf + 122));
        manage->payload.parent.grandmasterStratum = *(UInteger8*)(buf + 127);
        memcpy(manage->payload.parent.grandmasterIdentifier, buf + 128, PTP_CODE_STRING_LENGTH);
        manage->payload.parent.grandmasterVariance = flip16(*(Integer16*)(buf + 134));
        manage->payload.parent.grandmasterPreferred = (Boolean) * (UInteger8*)(buf + 139);
        manage->payload.parent.grandmasterIsBoundaryClock = (Boolean) * (UInteger8*)(buf + 144);
        manage->payload.parent.grandmasterSequenceNumber = flip16(*(UInteger16*)(buf + 146));
        break;

    case PTP_MM_PORT_DATA_SET:
        DBGVV("msgUnloadManagementPayload: managementMessageKey PTP_MM_FOREIGN_DATA_SET\n");
        manage->payload.port.returnedPortNumber = flip16(*(UInteger16*)(buf + 62));
        manage->payload.port.portState = *(UInteger8*)(buf + 67);
        manage->payload.port.lastSyncEventSequenceNumber = flip16(*(UInteger16*)(buf + 70));
        manage->payload.port.lastGeneralEventSequenceNumber = flip16(*(UInteger16*)(buf + 74));
        manage->payload.port.port.communicationTechnology = *(UInteger8*)(buf + 79);
        memcpy(manage->payload.port.port.uuid, buf + 80, PTP_UUID_LENGTH);
        manage->payload.port.port.portId = flip16(*(UInteger16*)(buf + 90));
        manage->payload.port.burstEnabled = (Boolean) * (UInteger8*)(buf + 95);
        manage->payload.port.subdomainAddressOctets = *(UInteger8*)(buf + 97);
        manage->payload.port.eventPortAddressOctets = *(UInteger8*)(buf + 98);
        manage->payload.port.generalPortAddressOctets = *(UInteger8*)(buf + 99);
        memcpy(manage->payload.port.subdomainAddress, buf + 100, SUBDOMAIN_ADDRESS_LENGTH);
        memcpy(manage->payload.port.eventPortAddress, buf + 106, PORT_ADDRESS_LENGTH);
        memcpy(manage->payload.port.generalPortAddress, buf + 110, PORT_ADDRESS_LENGTH);
        break;

    case PTP_MM_GLOBAL_TIME_DATA_SET:
        DBGVV("msgUnloadManagementPayload: managementMessageKey PTP_MM_GLOBAL_TIME_DATA_SET\n");
        manage->payload.globalTime.localTime.seconds = flip32(*(UInteger32*)(buf + 60));
        manage->payload.globalTime.localTime.nanoseconds = flip32(*(Integer32*)(buf + 64));
        manage->payload.globalTime.currentUtcOffset = flip16(*(Integer16*)(buf + 70));
        manage->payload.globalTime.leap59 = (Boolean) * (UInteger8*)(buf + 75);
        manage->payload.globalTime.leap61 = (Boolean) * (UInteger8*)(buf + 79);
        manage->payload.globalTime.epochNumber = flip16(*(UInteger16*)(buf + 82));
        break;


    case PTP_MM_FOREIGN_DATA_SET:
        DBGVV("msgUnloadManagementPayload: managementMessageKey PTP_MM_FOREIGN_DATA_SET\n");
        manage->payload.foreign.returnedPortNumber = flip16(*(UInteger16*)(buf + 62));
        manage->payload.foreign.returnedRecordNumber = flip16(*(UInteger16*)(buf + 68));
        manage->payload.foreign.foreignMaster.communicationTechnology = *(UInteger8*)(buf + 71);
        memcpy(manage->payload.foreign.foreignMaster.uuid, buf + 72, PTP_UUID_LENGTH);
        manage->payload.foreign.foreignMaster.portId = flip16(*(UInteger16*)(buf + 82));
        manage->payload.foreign.foreignMasterSyncs = flip16(*(UInteger16*)(buf + 66));
        break;

    case PTP_MM_NULL:
        DBGVV("msgUnloadManagementPayload: managementMessageKey NULL\n");
        break;

    default:
        DBGVV("msgUnloadManagementPayload: managementMessageKey ?\n");
        break;
    }

    return;
}

void msgPackHeader(PtpClock *ptpClock, Octet *buf)
{
    *(Integer32*)(buf + 0) = shift16(flip16(VERSION_PTP), 0) | shift16(flip16(VERSION_NETWORK), 1);
    memcpy((buf + 4), ptpClock->defaultDS.subdomain_name, PTP_SUBDOMAIN_NAME_LENGTH);
    *(Integer32*)(buf + 20) = shift8(ptpClock->portDS.port.communicationTechnology, 1);
    memcpy((buf + 22), ptpClock->portDS.port.uuid, PTP_UUID_LENGTH);

    if (ptpClock->defaultDS.external_timing)
        setFlag((buf + 34), PTP_EXT_SYNC);

    if (ptpClock->defaultDS.clock_followup_capable)
        setFlag((buf + 34), PTP_ASSIST);

    if (ptpClock->defaultDS.is_boundary_clock)
        setFlag((buf + 34), PTP_BOUNDARY_CLOCK);
}

void msgPackSync(PtpClock *ptpClock, Octet *buf, Boolean burst, TimeRepresentation *originTimestamp)
{
    *(UInteger8*)(buf + 20) = 1; /* messageType */
    *(Integer32*)(buf + 28) = shift16(flip16(ptpClock->portDS.port.portId), 0) | shift16(flip16(ptpClock->portDS.last_sync_event_sequence_number), 1);
    *(UInteger8*)(buf + 32) = PTP_SYNC_MESSAGE; /* control */

    if (ptpClock->portDS.burst_enabled && burst)
        setFlag((buf + 34), PTP_SYNC_BURST);
    else
        clearFlag((buf + 34), PTP_SYNC_BURST);

    if (ptpClock->parentDS.parent_stats)
        setFlag((buf + 34), PARENT_STATS);
    else
        clearFlag((buf + 34), PARENT_STATS);


    *(Integer32*)(buf + 40) = flip32(originTimestamp->seconds);

    *(Integer32*)(buf + 44) = flip32(originTimestamp->nanoseconds);

    *(Integer32*)(buf + 48) = shift16(flip16(ptpClock->timeDS.epoch_number), 0) | shift16(flip16(ptpClock->timeDS.current_utc_offset), 1);

    *(Integer32*)(buf + 52) = shift8(ptpClock->parentDS.grandmaster.communicationTechnology, 1);

    memcpy((buf + 54), ptpClock->parentDS.grandmaster.uuid, 6);

    *(Integer32*)(buf + 60) = shift16(flip16(ptpClock->parentDS.grandmaster.portId), 0) | shift16(flip16(ptpClock->parentDS.grandmaster_sequence_number), 1);

    *(Integer32*)(buf + 64) = shift8(ptpClock->parentDS.grandmaster_stratum, 3);

    memcpy((buf + 68), ptpClock->parentDS.grandmaster_identifier, PTP_CODE_STRING_LENGTH);

    *(Integer32*)(buf + 72) = shift16(flip16(ptpClock->parentDS.grandmaster_variance), 1);

    *(Integer32*)(buf + 76) = shift16(flip16(ptpClock->parentDS.grandmaster_preferred), 0) | shift16(flip16(ptpClock->parentDS.grandmaster_is_boundary_clock), 1);

    *(Integer32*)(buf + 80) = shift16(flip16(ptpClock->defaultDS.sync_interval), 1);

    *(Integer32*)(buf + 84) = shift16(flip16(ptpClock->defaultDS.clock_variance), 1);

    *(Integer32*)(buf + 88) = shift16(flip16(ptpClock->currentDS.steps_removed), 1);

    *(Integer32*)(buf + 92) = shift8(ptpClock->defaultDS.clock_stratum, 3);

    memcpy((buf + 96), ptpClock->defaultDS.clock_identifier, PTP_CODE_STRING_LENGTH);

    *(Integer32*)(buf + 100) = shift8(ptpClock->parentDS.parent.communicationTechnology, 1);

    memcpy((buf + 102), ptpClock->parentDS.parent.uuid, 6);

    *(Integer32*)(buf + 108) = shift16(flip16(ptpClock->parentDS.parent.portId), 1);

    *(Integer32*)(buf + 112) = shift16(flip16(ptpClock->parentDS.observed_variance), 1);

    *(Integer32*)(buf + 116) = flip32(ptpClock->parentDS.observed_drift);

    *(Integer32*)(buf + 120) = shift8(ptpClock->parentDS.utc_reasonable, 3);
}

void msgPackDelayReq(PtpClock *ptpClock, Octet *buf, Boolean burst, TimeRepresentation *originTimestamp)
{
    *(UInteger8*)(buf + 20) = 1;  /* messageType */
    *(Integer32*)(buf + 28) = shift16(flip16(ptpClock->portDS.port.portId), 0) | shift16(flip16(ptpClock->portDS.last_sync_event_sequence_number), 1);
    *(UInteger8*)(buf + 32) = PTP_DELAY_REQ_MESSAGE;  /* control */

    if (ptpClock->portDS.burst_enabled && burst)
        setFlag((buf + 34), PTP_SYNC_BURST);
    else
        clearFlag((buf + 34), PTP_SYNC_BURST);

    if (ptpClock->parentDS.parent_stats)
        setFlag((buf + 34), PARENT_STATS);
    else
        clearFlag((buf + 34), PARENT_STATS);

    *(Integer32*)(buf + 40) = flip32(originTimestamp->seconds);

    *(Integer32*)(buf + 44) = flip32(originTimestamp->nanoseconds);

    *(Integer32*)(buf + 48) = shift16(flip16(ptpClock->timeDS.epoch_number), 0) | shift16(flip16(ptpClock->timeDS.current_utc_offset), 1);

    *(Integer32*)(buf + 52) = shift8(ptpClock->parentDS.grandmaster.communicationTechnology, 1);

    memcpy((buf + 54), ptpClock->parentDS.grandmaster.uuid, 6);

    *(Integer32*)(buf + 60) = shift16(flip16(ptpClock->parentDS.grandmaster.portId), 0) | shift16(flip16(ptpClock->parentDS.grandmaster_sequence_number), 1);

    *(Integer32*)(buf + 64) = shift8(ptpClock->parentDS.grandmaster_stratum, 3);

    memcpy((buf + 68), ptpClock->parentDS.grandmaster_identifier, PTP_CODE_STRING_LENGTH);

    *(Integer32*)(buf + 72) = shift16(flip16(ptpClock->parentDS.grandmaster_variance), 1);

    *(Integer32*)(buf + 76) = shift16(flip16(ptpClock->parentDS.grandmaster_preferred), 0) | shift16(flip16(ptpClock->parentDS.grandmaster_is_boundary_clock), 1);

    *(Integer32*)(buf + 80) = shift16(flip16(ptpClock->defaultDS.sync_interval), 1);

    *(Integer32*)(buf + 84) = shift16(flip16(ptpClock->defaultDS.clock_variance), 1);

    *(Integer32*)(buf + 88) = shift16(flip16(ptpClock->currentDS.steps_removed), 1);

    *(Integer32*)(buf + 92) = shift8(ptpClock->defaultDS.clock_stratum, 3);

    memcpy((buf + 96), ptpClock->defaultDS.clock_identifier, 4);

    *(Integer32*)(buf + 100) = shift8(ptpClock->parentDS.parent.communicationTechnology, 1);

    memcpy((buf + 102), ptpClock->parentDS.parent.uuid, 6);

    *(Integer32*)(buf + 108) = shift16(flip16(ptpClock->parentDS.parent.portId), 1);

    *(Integer32*)(buf + 112) = shift16(flip16(ptpClock->parentDS.observed_variance), 1);

    *(Integer32*)(buf + 116) = flip32(ptpClock->parentDS.observed_drift);

    *(Integer32*)(buf + 120) = shift8(ptpClock->parentDS.utc_reasonable, 3);
}

void msgPackFollowUp(PtpClock *ptpClock, Octet *buf, UInteger16 associatedSequenceId,
                     TimeRepresentation *preciseOriginTimestamp)
{
    *(UInteger8*)(buf + 20) = 2;  /* messageType */
    *(Integer32*)(buf + 28) = shift16(flip16(ptpClock->portDS.port.portId), 0) | shift16(flip16(ptpClock->portDS.last_general_event_sequence_number), 1);
    *(UInteger8*)(buf + 32) = PTP_FOLLOWUP_MESSAGE;  /* control */
    clearFlag((buf + 34), PTP_SYNC_BURST);
    clearFlag((buf + 34), PARENT_STATS);

    *(Integer32*)(buf + 40) = shift16(flip16(associatedSequenceId), 1);
    *(Integer32*)(buf + 44) = flip32(preciseOriginTimestamp->seconds);
    *(Integer32*)(buf + 48) = flip32(preciseOriginTimestamp->nanoseconds);
}

void msgPackDelayResp(PtpClock *ptpClock, Octet *buf, MsgHeader *header,
                      TimeRepresentation *delayReceiptTimestamp)
{
    *(UInteger8*)(buf + 20) = 2;  /* messageType */
    *(Integer32*)(buf + 28) = shift16(flip16(ptpClock->portDS.port.portId), 0) | shift16(flip16(ptpClock->portDS.last_general_event_sequence_number), 1);
    *(UInteger8*)(buf + 32) = PTP_DELAY_RESP_MESSAGE;  /* control */
    clearFlag((buf + 34), PTP_SYNC_BURST);
    clearFlag((buf + 34), PARENT_STATS);

    *(Integer32*)(buf + 40) = flip32(delayReceiptTimestamp->seconds);
    *(Integer32*)(buf + 44) = flip32(delayReceiptTimestamp->nanoseconds);
    *(Integer32*)(buf + 48) = shift8(header->source.communicationTechnology, 1);
    memcpy(buf + 50, header->source.uuid, 6);
    *(Integer32*)(buf + 56) = shift16(flip16(header->source.portId), 0) | shift16(flip16(header->sequenceId), 1);
}

UInteger16 msgPackManagement(PtpClock *ptpClock, Octet *buf, MsgManagement *manage)
{
    *(UInteger8*)(buf + 20) = 2;  /* messageType */
    *(Integer32*)(buf + 28) = shift16(flip16(ptpClock->portDS.port.portId), 0) | shift16(flip16(ptpClock->portDS.last_general_event_sequence_number), 1);
    *(UInteger8*)(buf + 32) = PTP_MANAGEMENT_MESSAGE;  /* control */
    clearFlag((buf + 34), PTP_SYNC_BURST);
    clearFlag((buf + 34), PARENT_STATS);
    *(Integer32*)(buf + 40) = shift8(manage->target.communicationTechnology, 1);
    memcpy(buf + 42, manage->target.uuid, 6);
    *(Integer32*)(buf + 48) = shift16(flip16(manage->target.portId), 0) | shift16(flip16(MM_STARTING_BOUNDARY_HOPS), 1);
    *(Integer32*)(buf + 52) = shift16(flip16(MM_STARTING_BOUNDARY_HOPS), 0);

    *(UInteger8*)(buf + 55) = manage->managementMessageKey;

    switch (manage->managementMessageKey)
    {
    case PTP_MM_GET_FOREIGN_DATA_SET:
        *(UInteger16*)(buf + 62) = manage->recordKey;
        *(Integer32*)(buf + 56) = shift16(flip16(4), 1);
        return 64;

    default:
        *(Integer32*)(buf + 56) = shift16(flip16(0), 1);
        return 60;
    }
}

UInteger16 msgPackManagementResponse(PtpClock *ptpClock, Octet *buf, MsgHeader *header, MsgManagement *manage)
{
    TimeInternal internalTime;
    TimeRepresentation externalTime;

    *(UInteger8*)(buf + 20) = 2;  /* messageType */
    *(Integer32*)(buf + 28) = shift16(flip16(ptpClock->portDS.port.portId), 0) | shift16(flip16(ptpClock->portDS.last_general_event_sequence_number), 1);
    *(UInteger8*)(buf + 32) = PTP_MANAGEMENT_MESSAGE;  /* control */
    clearFlag((buf + 34), PTP_SYNC_BURST);
    clearFlag((buf + 34), PARENT_STATS);
    *(Integer32*)(buf + 40) = shift8(header->source.communicationTechnology, 1);
    memcpy(buf + 42, header->source.uuid, 6);
    *(Integer32*)(buf + 48) = shift16(flip16(header->source.portId), 0) | shift16(flip16(MM_STARTING_BOUNDARY_HOPS), 1);
    *(Integer32*)(buf + 52) = shift16(flip16(manage->startingBoundaryHops - manage->boundaryHops + 1), 0);

    switch (manage->managementMessageKey)
    {
    case PTP_MM_OBTAIN_IDENTITY:
        *(UInteger8*)(buf + 55) = PTP_MM_CLOCK_IDENTITY;
        *(Integer32*)(buf + 56) = shift16(flip16(64), 1);
        *(Integer32*)(buf + 60) = shift8(ptpClock->defaultDS.clock.communicationTechnology, 3);
        memcpy(buf + 64, ptpClock->defaultDS.clock.uuid, 6);
        *(Integer32*)(buf + 72) = shift16(flip16(ptpClock->defaultDS.clock.portId), 1);
        memcpy((buf + 76), MANUFACTURER_ID, 48);
        return 124;

    case PTP_MM_GET_DEFAULT_DATA_SET:
        *(UInteger8*)(buf + 55) = PTP_MM_DEFAULT_DATA_SET;
        *(Integer32*)(buf + 56) = shift16(flip16(76), 1);
        *(Integer32*)(buf + 60) = shift8(ptpClock->defaultDS.clock.communicationTechnology, 3);
        memcpy(buf + 64, ptpClock->defaultDS.clock.uuid, 6);
        *(Integer32*)(buf + 72) = shift16(flip16(ptpClock->defaultDS.clock.portId), 1);
        *(Integer32*)(buf + 76) = shift8(ptpClock->defaultDS.clock_stratum, 3);
        memcpy(buf + 80, ptpClock->defaultDS.clock_identifier, PTP_CODE_STRING_LENGTH);
        *(Integer32*)(buf + 84) = shift16(flip16(ptpClock->defaultDS.clock_variance), 1);
        *(Integer32*)(buf + 88) = shift8(ptpClock->defaultDS.clock_followup_capable, 3);
        *(Integer32*)(buf + 92) = shift8(ptpClock->defaultDS.preferred, 3);
        *(Integer32*)(buf + 96) = shift8(ptpClock->defaultDS.initializable, 3);
        *(Integer32*)(buf + 100) = shift8(ptpClock->defaultDS.external_timing, 3);
        *(Integer32*)(buf + 104) = shift8(ptpClock->defaultDS.is_boundary_clock, 3);
        *(Integer32*)(buf + 108) = shift8(ptpClock->defaultDS.sync_interval, 3);
        memcpy(buf + 112, ptpClock->defaultDS.subdomain_name, 16);
        *(Integer32*)(buf + 128) = shift16(flip16(ptpClock->defaultDS.number_ports), 1);
        *(Integer32*)(buf + 132) = shift16(flip16(ptpClock->defaultDS.number_foreign_records), 1);
        return 136;

    case PTP_MM_GET_CURRENT_DATA_SET:
        *(UInteger8*)(buf + 55) = PTP_MM_CURRENT_DATA_SET;
        *(Integer32*)(buf + 56) = shift16(flip16(20), 1);
        *(Integer32*)(buf + 60) = shift16(flip16(ptpClock->currentDS.steps_removed), 1);

        fromInternalTime(&ptpClock->currentDS.offset_from_master, &externalTime, FALSE);
        *(Integer32*)(buf + 64) = flip32(externalTime.seconds);
        *(Integer32*)(buf + 68) = flip32(externalTime.nanoseconds);

        fromInternalTime(&ptpClock->currentDS.one_way_delay, &externalTime, FALSE);
        *(Integer32*)(buf + 72) = flip32(externalTime.seconds);
        *(Integer32*)(buf + 76) = flip32(externalTime.nanoseconds);
        return 80;

    case PTP_MM_GET_PARENT_DATA_SET:
        *(UInteger8*)(buf + 55) = PTP_MM_PARENT_DATA_SET;
        *(Integer32*)(buf + 56) = shift16(flip16(90), 1);
        *(Integer32*)(buf + 60) = shift8(ptpClock->parentDS.parent.communicationTechnology, 3);
        memcpy(buf + 64, ptpClock->parentDS.parent.uuid, 6);
        *(Integer32*)(buf + 72) = shift16(flip16(ptpClock->parentDS.parent.portId), 1);
        *(Integer32*)(buf + 76) = shift16(flip16(ptpClock->parentDS.parent_last_sync_sequence_number), 1);
        *(Integer32*)(buf + 80) = shift8(ptpClock->parentDS.parent_followup_capable, 1);
        *(Integer32*)(buf + 84) = shift8(ptpClock->parentDS.parent_external_timing, 3);
        *(Integer32*)(buf + 88) = shift16(flip16(ptpClock->parentDS.parent_variance), 1);
        *(Integer32*)(buf + 92) = shift8(ptpClock->parentDS.parent_stats, 3);
        *(Integer32*)(buf + 96) = shift16(flip16(ptpClock->parentDS.observed_variance), 1);
        *(Integer32*)(buf + 100) = flip32(ptpClock->parentDS.observed_drift);
        *(Integer32*)(buf + 104) = shift8(ptpClock->parentDS.utc_reasonable, 3);
        *(Integer32*)(buf + 108) = shift8(ptpClock->parentDS.grandmaster.communicationTechnology, 3);
        memcpy(buf + 112, ptpClock->parentDS.grandmaster.uuid, 6);
        *(Integer32*)(buf + 120) = shift16(flip16(ptpClock->parentDS.grandmaster.portId), 1);
        *(Integer32*)(buf + 124) = shift8(ptpClock->parentDS.grandmaster_stratum, 3);
        memcpy(buf + 128, ptpClock->parentDS.grandmaster_identifier, PTP_CODE_STRING_LENGTH);
        *(Integer32*)(buf + 132) = shift16(flip16(ptpClock->parentDS.grandmaster_variance), 1);
        *(Integer32*)(buf + 136) = shift8(ptpClock->parentDS.grandmaster_preferred, 3);
        *(Integer32*)(buf + 140) = shift8(ptpClock->parentDS.grandmaster_is_boundary_clock, 3);
        *(Integer32*)(buf + 144) = shift16(flip16(ptpClock->parentDS.grandmaster_sequence_number), 1);
        return 148;

    case PTP_MM_GET_PORT_DATA_SET:

        if (manage->target.portId && manage->target.portId != ptpClock->portDS.port.portId)
        {
            *(UInteger8*)(buf + 55) = PTP_MM_NULL;
            *(Integer32*)(buf + 56) = shift16(flip16(0), 1);
            return 0;
        }

        *(UInteger8*)(buf + 55) = PTP_MM_PORT_DATA_SET;

        *(Integer32*)(buf + 56) = shift16(flip16(52), 1);
        *(Integer32*)(buf + 60) = shift16(flip16(ptpClock->portDS.port.portId), 1);
        *(Integer32*)(buf + 64) = shift8(ptpClock->portDS.port_state, 3);
        *(Integer32*)(buf + 68) = shift16(flip16(ptpClock->portDS.last_sync_event_sequence_number), 1);
        *(Integer32*)(buf + 72) = shift16(flip16(ptpClock->portDS.last_general_event_sequence_number), 1);
        *(Integer32*)(buf + 76) = shift8(ptpClock->portDS.port.communicationTechnology, 3);
        memcpy(buf + 80, ptpClock->portDS.port.uuid, 6);
        *(Integer32*)(buf + 88) = shift16(flip16(ptpClock->portDS.port.portId), 1);
        *(Integer32*)(buf + 92) = shift8(ptpClock->portDS.burst_enabled, 3);
        *(Integer32*)(buf + 96) = shift8(4, 1) | shift8(2, 2) | shift8(2, 3);
        memcpy(buf + 100, ptpClock->portDS.subdomain_address, 4);
        memcpy(buf + 106, ptpClock->portDS.event_port_address, 2);
        memcpy(buf + 110, ptpClock->portDS.general_port_address, 2);
        return 112;

    case PTP_MM_GET_GLOBAL_TIME_DATA_SET:
        *(UInteger8*)(buf + 55) = PTP_MM_GLOBAL_TIME_DATA_SET;
        *(Integer32*)(buf + 56) = shift16(flip16(24), 1);

        getTime(&internalTime);
        fromInternalTime(&internalTime, &externalTime, ptpClock->halfEpoch);
        *(Integer32*)(buf + 60) = flip32(externalTime.seconds);
        *(Integer32*)(buf + 64) = flip32(externalTime.nanoseconds);

        *(Integer32*)(buf + 68) = shift16(flip16(ptpClock->timeDS.current_utc_offset), 1);
        *(Integer32*)(buf + 72) = shift8(ptpClock->timeDS.leap_59, 3);
        *(Integer32*)(buf + 76) = shift8(ptpClock->timeDS.leap_61, 3);
        *(Integer32*)(buf + 80) = shift16(flip16(ptpClock->timeDS.epoch_number), 1);
        return 84;

    case PTP_MM_GET_FOREIGN_DATA_SET:

        if ((manage->target.portId && manage->target.portId != ptpClock->portDS.port.portId)
                || !manage->recordKey || manage->recordKey > ptpClock->defaultDS.number_foreign_records)
        {
            *(UInteger8*)(buf + 55) = PTP_MM_NULL;
            *(Integer32*)(buf + 56) = shift16(flip16(0), 1);
            return 0;
        }

        *(UInteger8*)(buf + 55) = PTP_MM_FOREIGN_DATA_SET;

        *(Integer32*)(buf + 56) = shift16(flip16(28), 1);
        *(Integer32*)(buf + 60) = shift16(flip16(ptpClock->portDS.port.portId), 1);
        *(Integer32*)(buf + 64) = shift16(flip16(manage->recordKey - 1), 1);
        *(Integer32*)(buf + 68) = shift8(ptpClock->foreignMasterDS.records[manage->recordKey - 1].foreignMaster.communicationTechnology, 3);
        memcpy(buf + 72, ptpClock->foreignMasterDS.records[manage->recordKey - 1].foreignMaster.uuid, 6);
        *(Integer32*)(buf + 80) = shift16(flip16(ptpClock->foreignMasterDS.records[manage->recordKey - 1].foreignMaster.portId), 1);
        *(Integer32*)(buf + 84) = shift16(flip16(ptpClock->foreignMasterDS.records[manage->recordKey - 1].foreignMasterSyncs), 1);
        return 88;

    default:
        return 0;
    }
}
