/* datatypes.h */

#ifndef DATATYPES_H
#define DATATYPES_H


typedef struct
{
    UInteger32 seconds;
    Integer32 nanoseconds;
} TimeRepresentation;

typedef struct
{
    Integer32 seconds;
    Integer32 nanoseconds;
} TimeInternal;

typedef struct
{
    Integer32  interval;
    Integer32  left;
    Boolean expire;
} IntervalTimer;

/* inspired by v2 */
typedef Octet ClockIdentity[PTP_UUID_LENGTH];

typedef struct
{
    UInteger8  communicationTechnology;
    ClockIdentity uuid;
    UInteger16  portId;
} PortIdentity;

/* Message header */

typedef struct
{
    UInteger16  versionPTP;
    UInteger16  versionNetwork;
    Octet subdomain[PTP_SUBDOMAIN_NAME_LENGTH];
    UInteger8  messageType;
   PortIdentity source;
    UInteger16  sequenceId;
    UInteger8  control;
    Octet  flags[2];

} MsgHeader;

/* Sync or Delay_Req message */

typedef struct
{
    TimeRepresentation  originTimestamp;
    UInteger16  epochNumber;
    Integer16  currentUTCOffset;
    PortIdentity grandmaster;
    UInteger16  grandmasterSequenceId;
    UInteger8  grandmasterClockStratum;
    Octet  grandmasterClockIdentifier[PTP_CODE_STRING_LENGTH];
    Integer16  grandmasterClockVariance;
    Boolean  grandmasterPreferred;
    Boolean  grandmasterIsBoundaryClock;
    Integer8  syncInterval;
    Integer16  localClockVariance;
    UInteger16  localStepsRemoved;
    UInteger8  localClockStratum;
    Octet  localClockIdentifer[PTP_CODE_STRING_LENGTH];
    PortIdentity parent;
    Integer16  estimatedMasterVariance;
    Integer32  estimatedMasterDrift;
    Boolean  utcReasonable;

} MsgSync;

typedef MsgSync MsgDelayReq;

/* Follow_Up message */

typedef struct
{
    UInteger16  associatedSequenceId;
    TimeRepresentation  preciseOriginTimestamp;

} MsgFollowUp;

/* Delay_Resp message */

typedef struct
{
    TimeRepresentation  delayReceiptTimestamp;
   PortIdentity requestingSource;
    UInteger16  requestingSourceSequenceId;

} MsgDelayResp;

/* Management message */
typedef union
{

    struct ClockIdentity
    {
      PortIdentity clock;
        Octet  manufacturerIdentity[MANUFACTURER_ID_LENGTH];
    } clockIdentity;

    struct DefaultData
    {
      PortIdentity clock;
        UInteger8  clockStratum;
        Octet  clockIdentifier[PTP_CODE_STRING_LENGTH];
        Integer16  clockVariance;
        Boolean  clockFollowupCapable;
        Boolean  preferred;
        Boolean  initializable;
        Boolean  externalTiming;
        Boolean  isBoundaryClock;
        Integer8  syncInterval;
        Octet  subdomainName[PTP_SUBDOMAIN_NAME_LENGTH];
        UInteger16  numberPorts;
        UInteger16  numberForeignRecords;
    } defaultData;

    struct Current
    {
        UInteger16  stepsRemoved;
        TimeRepresentation  offsetFromMaster;
        TimeRepresentation  oneWayDelay;
    } current;

    struct Parent
    {
      PortIdentity parent;
        UInteger16  parentLastSyncSequenceNumber;
        Boolean  parentFollowupCapable;
        Boolean  parentExternalTiming;
        Integer16  parentVariance;
        Boolean  parentStats;
        Integer16  observedVariance;
        Integer32  observedDrift;
        Boolean  utcReasonable;
      PortIdentity grandmaster;
        UInteger8  grandmasterStratum;
        Octet  grandmasterIdentifier[PTP_CODE_STRING_LENGTH];
        Integer16  grandmasterVariance;
        Boolean  grandmasterPreferred;
        Boolean  grandmasterIsBoundaryClock;
        UInteger16  grandmasterSequenceNumber;
    } parent;

    struct Port
    {
        UInteger16  returnedPortNumber;
        UInteger8  portState;
        UInteger16  lastSyncEventSequenceNumber;
        UInteger16  lastGeneralEventSequenceNumber;
      PortIdentity port;
        Boolean  burstEnabled;
        UInteger8  subdomainAddressOctets;
        UInteger8  eventPortAddressOctets;
        UInteger8  generalPortAddressOctets;
        Octet  subdomainAddress[SUBDOMAIN_ADDRESS_LENGTH];
        Octet  eventPortAddress[PORT_ADDRESS_LENGTH];
        Octet  generalPortAddress[PORT_ADDRESS_LENGTH];
    } port;

    struct GlobalTime
    {
        TimeRepresentation  localTime;
        Integer16  currentUtcOffset;
        Boolean  leap59;
        Boolean  leap61;
        UInteger16  epochNumber;
    } globalTime;

    struct Foreign
    {
        UInteger16  returnedPortNumber;
        UInteger16  returnedRecordNumber;
      PortIdentity foreignMaster;
        UInteger16  foreignMasterSyncs;
    } foreign;

} MsgManagementPayload;

typedef struct
{
   PortIdentity target;
    Integer16  startingBoundaryHops;
    Integer16  boundaryHops;
    UInteger8  managementMessageKey;
    UInteger16  parameterLength;
    UInteger16  recordKey;

    MsgManagementPayload payload;
} MsgManagement;

typedef struct
{
   PortIdentity foreignMaster;
    UInteger16  foreignMasterSyncs;

    MsgHeader  header;
    MsgSync  sync;
} ForeignMasterRecord;

/* program options set at run-time */

typedef struct
{
    Integer8  syncInterval;
    Octet  subdomainName[PTP_SUBDOMAIN_NAME_LENGTH];
    Octet  clockIdentifier[PTP_CODE_STRING_LENGTH];
    Integer16  clockVariance;
    UInteger8  clockStratum;
    Boolean  clockPreferred;
    Integer16  currentUtcOffset;
    UInteger16  epochNumber;
    Octet  ifaceName[IFACE_NAME_LENGTH];
    Boolean  noResetClock;
    Boolean  noAdjust;
    Integer8  displayStats;
    Octet  unicastAddress[NET_ADDRESS_LENGTH];
    Integer16  ap, ai;
    Integer16  sDelay;
    Integer16  sOffset;
    TimeInternal  inboundLatency, outboundLatency;
    Integer16  max_foreign_records;
    Boolean  slaveOnly;
    Boolean  probe;
    UInteger8  probe_management_key;
    UInteger16  probe_record_key;
    Boolean  halfEpoch;

} RunTimeOpts;


/* main program data structure */

typedef struct
{
    /* Default data set */
    struct {
        PortIdentity clock;
        UInteger8  clock_stratum;
        Octet  clock_identifier[PTP_CODE_STRING_LENGTH];
        Integer16  clock_variance;
        Boolean  clock_followup_capable;
        Boolean  preferred;
        Boolean  initializable;
        Boolean  external_timing;
        Boolean  is_boundary_clock;
        Integer8  sync_interval;
        Octet  subdomain_name[PTP_SUBDOMAIN_NAME_LENGTH];
        UInteger16  number_ports;
        UInteger16  number_foreign_records;
    } defaultDS;

    /* Current data set */
    struct {
        UInteger16  steps_removed;
        TimeInternal  offset_from_master;
        TimeInternal  one_way_delay;
    } currentDS;

    /* Parent data set */
    struct {
      PortIdentity parent;
        UInteger16  parent_last_sync_sequence_number;
        Boolean  parent_followup_capable;
        Boolean  parent_external_timing;
        Integer16  parent_variance;
        Boolean  parent_stats;
        Integer16  observed_variance;
        Integer32  observed_drift;
        Boolean  utc_reasonable;
      PortIdentity grandmaster;
        UInteger8  grandmaster_stratum;
        Octet  grandmaster_identifier[PTP_CODE_STRING_LENGTH];
        Integer16  grandmaster_variance;
        Boolean  grandmaster_preferred;
        Boolean  grandmaster_is_boundary_clock;
        UInteger16  grandmaster_sequence_number;
    } parentDS;

    /* Global time properties data set */
    struct {
        Integer16  current_utc_offset;
        Boolean  leap_59;
        Boolean  leap_61;
        UInteger16  epoch_number;
    } timeDS;

    /* Port configuration data set */
    struct {
        UInteger8  port_state;
        UInteger16  last_sync_event_sequence_number;
        UInteger16  last_general_event_sequence_number;
        Octet  subdomain_address[SUBDOMAIN_ADDRESS_LENGTH];
        Octet  event_port_address[PORT_ADDRESS_LENGTH];
        Octet  general_port_address[PORT_ADDRESS_LENGTH];
        PortIdentity port;
        Boolean  burst_enabled;
    } portDS;

    /* Foreign master data set */
    struct
    {
        ForeignMasterRecord *records;

        /* UInteger16 count; = defaultDS.number_foreign_records */
        Integer16  capacity;
        Integer16  i;
        Integer16  best;
    } foreignMasterDS;


    /* Other things we need for the protocol */
    Boolean halfEpoch;

    UInteger32 random_seed;

    MsgHeader msgTmpHeader;

    union
    {
        MsgSync  sync;
        MsgFollowUp  follow;
        MsgDelayReq  req;
        MsgDelayResp  resp;
        MsgManagement  manage;
    } msgTmp;

    Octet msgObuf[PACKET_SIZE];
    Octet msgIbuf[PACKET_SIZE];
    ssize_t msgIbufLength;

    TimeInternal Tms;

    struct {
        TimeInternal syncRecieve;
        TimeInternal delayReqSend;
        TimeInternal delayReqRecieve;
    } timestamp;

    UInteger16  Q;
    UInteger16  R;

    Boolean  sentDelayReq;
    UInteger16  sentDelayReqSequenceId;
    Boolean  waitingForFollow;

    Boolean message_activity;

    IntervalTimer  itimer[TIMER_ARRAY_SIZE];

    NetPath netPath;

   Boolean  slaveOnly;
    Integer8 displayStats;

    struct {
        Boolean  noResetClock;
        Boolean  noAdjust;

        Integer16 ap, ai;
        Integer16 sDelay;
        Integer16 sOffset;
        Filter ofm_filt;
        Filter owd_filt;
    } servo;

    TimeInternal  inboundLatency, outboundLatency;

    Integer32  events;

    RunTimeOpts *rtOpts;
} PtpClock;

#endif
