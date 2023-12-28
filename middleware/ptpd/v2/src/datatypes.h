/* datatypes.h */
#ifndef DATATYPES_H_
#define DATATYPES_H_

/**
 *\file
 * \brief 5.3 Derived data type specifications
 *
 * This header file defines structures defined by the spec,
 * main program data structure, and all messages structures
 */


/**
 * \brief 5.3.2 The TimeInterval type represents time intervals
 * in scaled nanoseconds where scaledNanoseconds = time[ns] * 2^16
 */

typedef struct
{
    Integer64 scaledNanoseconds;
} TimeInterval;

/**
 * \brief 5.3.3 The Timestamp type represents a positive time with respect to the epoch
 */

typedef struct
{
    UInteger48 secondsField;
    UInteger32 nanosecondsField;
} Timestamp;

/**
 * \brief 5.3.4 The ClockIdentity type identifies a clock
 */
typedef Octet ClockIdentity[CLOCK_IDENTITY_LENGTH];

/**
 * \brief 5.3.5 The PortIdentity identifies a PTP port.
 */

typedef struct
{
    ClockIdentity clockIdentity;
    UInteger16 portNumber;
} PortIdentity;

/**
 * \brief 5.3.6 The PortAdress type represents the protocol address of a PTP port
 */

 typedef struct
{
    Enumeration16 networkProtocol;
    UInteger16 adressLength;
    Octet* adressField;
} PortAdress;

/**
* \brief 5.3.7 The ClockQuality represents the quality of a clock
 */

typedef struct
{
    UInteger8 clockClass;
    Enumeration8 clockAccuracy;
    UInteger16 offsetScaledLogVariance;
} ClockQuality;

/**
 * \brief 5.3.8 The TLV type represents TLV extension fields
 */

typedef struct
{
    Enumeration16 tlvType;
    UInteger16 lengthField;
    Octet* valueField;
} TLV;

/**
 * \brief 5.3.9 The PTPText data type is used to represent textual material in PTP messages
 * textField - UTF-8 encoding
 */

typedef struct
{
    UInteger8 lengthField;
    Octet* textField;
} PTPText;

/**
* \brief 5.3.10 The FaultRecord type is used to construct fault logs
 */

typedef struct
{
    UInteger16 faultRecordLength;
    Timestamp faultTime;
    Enumeration8 severityCode;
    PTPText faultName;
    PTPText faultValue;
    PTPText faultDescription;
} FaultRecord;


/**
 * \brief The common header for all PTP messages (Table 18 of the spec)
 */

typedef struct
{
    Nibble transportSpecific;
    Enumeration4 messageType;
    UInteger4 versionPTP;
    UInteger16 messageLength;
    UInteger8 domainNumber;
    Octet flagField[2];
    Integer64 correctionfield;
    PortIdentity sourcePortIdentity;
    UInteger16 sequenceId;
    UInteger8 controlField;
    Integer8 logMessageInterval;
} MsgHeader;


/**
 * \brief Announce message fields (Table 25 of the spec)
 */

typedef struct
{
    Timestamp originTimestamp;
    Integer16 currentUtcOffset;
    UInteger8 grandmasterPriority1;
    ClockQuality grandmasterClockQuality;
    UInteger8 grandmasterPriority2;
    ClockIdentity grandmasterIdentity;
    UInteger16 stepsRemoved;
    Enumeration8 timeSource;
}MsgAnnounce;


/**
 * \brief Sync message fields (Table 26 of the spec)
 */

typedef struct
{
    Timestamp originTimestamp;
}MsgSync;

/**
 * \brief DelayReq message fields (Table 26 of the spec)
 */

typedef struct
{
    Timestamp originTimestamp;
}MsgDelayReq;

/**
 * \brief DelayResp message fields (Table 30 of the spec)
 */

typedef struct
{
    Timestamp receiveTimestamp;
    PortIdentity requestingPortIdentity;
}MsgDelayResp;

/**
 * \brief FollowUp message fields (Table 27 of the spec)
 */

typedef struct
{
    Timestamp preciseOriginTimestamp;
}MsgFollowUp;

/**
 * \brief PDelayReq message fields (Table 29 of the spec)
 */

typedef struct
{
    Timestamp originTimestamp;

}MsgPDelayReq;

/**
 * \brief PDelayResp message fields (Table 30 of the spec)
 */

typedef struct
{
    Timestamp requestReceiptTimestamp;
    PortIdentity requestingPortIdentity;
}MsgPDelayResp;

/**
 * \brief PDelayRespFollowUp message fields (Table 31 of the spec)
 */

typedef struct
{
    Timestamp responseOriginTimestamp;
    PortIdentity requestingPortIdentity;
}MsgPDelayRespFollowUp;

/**
* \brief Signaling message fields (Table 33 of the spec)
 */

typedef struct
{
    PortIdentity targetPortIdentity;
    char* tlv;
}MsgSignaling;

/**
* \brief Management message fields (Table 37 of the spec)
 */

typedef struct
{
    PortIdentity targetPortIdentity;
    UInteger8 startingBoundaryHops;
    UInteger8 boundaryHops;
    Enumeration4 actionField;
    char* tlv;
}MsgManagement;


/**
* \brief Time structure to handle Linux time information
 */

typedef struct
{
    Integer32 seconds;
    Integer32 nanoseconds;
} TimeInternal;

/**
* \brief Structure used as a timer
 */

typedef struct
{
    Integer32  interval;
    Integer32  left;
    Boolean expire;
} IntervalTimer;


/**
* \brief ForeignMasterRecord is used to manage foreign masters
 */

typedef struct
{
    PortIdentity foreignMasterPortIdentity;
    UInteger16 foreignMasterAnnounceMessages;

    /* This one is not in the spec */
    MsgAnnounce  announce;
    MsgHeader    header;

} ForeignMasterRecord;

/**
 * \struct DefaultDS
 * \brief spec 8.2.1 default data set
 * spec 7.6.2, spec 7.6.3 PTP device attributes
 */

typedef struct
{
    Boolean twoStepFlag;
    ClockIdentity clockIdentity; /**< spec 7.6.2.1 */
    UInteger16 numberPorts; /**< spec 7.6.2.7 */
    ClockQuality clockQuality; /**< spec 7.6.2.4, 7.6.3.4 and 7.6.3*/
    UInteger8 priority1; /**< spec 7.6.2.2 */
    UInteger8 priority2; /**< spec 7.6.2.3 */
    UInteger8 domainNumber;
    Boolean slaveOnly;
} DefaultDS;


/**
 * \struct CurrentDS
 * \brief spec 8.2.2 current data set
 */

typedef struct
{
    UInteger16 stepsRemoved;
    TimeInternal offsetFromMaster;
    TimeInternal meanPathDelay;
} CurrentDS;


/**
 * \struct ParentDS
 * \brief spec 8.2.3 parent data set
 */

typedef struct
{
    PortIdentity parentPortIdentity;
    /* 7.6.4 Parent clock statistics - parentDS*/
    Boolean parentStats; /**< spec 7.6.4.2 */
    Integer16 observedParentOffsetScaledLogVariance; /**< spec 7.6.4.3 */
    Integer32 observedParentClockPhaseChangeRate; /**< spec 7.6.4.4 */

    ClockIdentity grandmasterIdentity;
    ClockQuality grandmasterClockQuality;
    UInteger8 grandmasterPriority1;
    UInteger8 grandmasterPriority2;
} ParentDS;

/**
 * \struct TimePropertiesDS
 * \brief spec 8.2.4 time properties data set
 */

 typedef struct
{
    Integer16 currentUtcOffset;
    Boolean currentUtcOffsetValid;
    Boolean leap59;
    Boolean leap61;
    Boolean timeTraceable;
    Boolean frequencyTraceable;
    Boolean ptpTimescale;
    Enumeration8 timeSource; /**< spec 7.6.2.6 */
} TimePropertiesDS;


/**
 * \struct PortDS
 * \brief spec 8.2.5 port data set
 */

 typedef struct
{
    PortIdentity portIdentity;
    Enumeration8 portState;
    Integer8 logMinDelayReqInterval; /**< spec 7.7.2.4 */
    TimeInternal peerMeanPathDelay;
    Integer8 logAnnounceInterval; /**< spec 7.7.2.2 */
    UInteger8 announceReceiptTimeout; /**< spec 7.7.3.1 */
    Integer8 logSyncInterval; /**< spec 7.7.2.3 */
    Enumeration8 delayMechanism;
    Integer8 logMinPdelayReqInterval; /**< spec 7.7.2.5 */
    UInteger4 versionNumber;
} PortDS;


/**
 * \struct ForeignMasterDS
 * \brief Foreign master data set
 */

typedef struct
{
    ForeignMasterRecord *records;

    /* Other things we need for the protocol */
    UInteger16 count;
    Integer16  capacity;
    Integer16  i;
    Integer16  best;
} ForeignMasterDS;

/**
 * \struct Servo
 * \brief Clock servo filters and PI regulator values
 */

typedef struct
{
    Boolean noResetClock;
    Boolean noAdjust;
    Integer16 ap, ai;
    Integer16 sDelay;
    Integer16 sOffset;
} Servo;
	
/**
 * \struct RunTimeOpts
 * \brief Program options set at run-time
 */

typedef struct
{
    Integer8  announceInterval;
    Integer8  syncInterval;
    ClockQuality clockQuality;
    UInteger8  priority1;
    UInteger8  priority2;
    UInteger8  domainNumber;
    Boolean  slaveOnly;
    Integer16  currentUtcOffset;
    Octet   ifaceName[IFACE_NAME_LENGTH];
    Enumeration8 stats;
    Octet   unicastAddress[NET_ADDRESS_LENGTH];
    TimeInternal  inboundLatency, outboundLatency;
    Integer16   maxForeignRecords;
    Enumeration8  delayMechanism;
	Servo servo;
} RunTimeOpts;

/**
 * \struct PtpClock
 * \brief Main program data structure
 */
/* main program data structure */

typedef struct
{

	DefaultDS defaultDS; /**< default data set */
	CurrentDS currentDS; /**< current data set */
	ParentDS parentDS; /**< parent data set */
	TimePropertiesDS timePropertiesDS; /**< time properties data set */
	PortDS portDS; /**< port data set */
	ForeignMasterDS foreignMasterDS; /**< foreign master data set */

    MsgHeader msgTmpHeader; /**< buffer for incomming message header */

    union
    {
        MsgSync  sync;
        MsgFollowUp  follow;
        MsgDelayReq  req;
        MsgDelayResp resp;
        MsgPDelayReq  preq;
        MsgPDelayResp  presp;
        MsgPDelayRespFollowUp  prespfollow;
        MsgManagement  manage;
        MsgAnnounce  announce;
        MsgSignaling signaling;
    } msgTmp; /**< buffer for incomming message body */


    Octet msgObuf[PACKET_SIZE]; /**< buffer for outgoing message */
    Octet msgIbuf[PACKET_SIZE]; /** <buffer for incomming message */
    ssize_t msgIbufLength; /**< length of incomming message */

    TimeInternal Tms; /**< Time Master -> Slave */
    TimeInternal Tsm; /**< Time Slave -> Master */

	TimeInternal pdelay_t1; /**< peer delay time t1 */
	TimeInternal pdelay_t2; /**< peer delay time t2 */
	TimeInternal pdelay_t3; /**< peer delay time t3 */
	TimeInternal pdelay_t4; /**< peer delay time t4 */

    TimeInternal timestamp_syncRecieve; /**< timestamp of Sync message */
    TimeInternal timestamp_delayReqSend; /**< timestamp of delay request message */
    TimeInternal timestamp_delayReqRecieve; /**< timestamp of delay request message */

    TimeInternal correctionField_sync; /**< correction field of Sync and FollowUp messages */
    TimeInternal correctionField_pDelayResp; /**< correction fieald of peedr delay response */

    /* MsgHeader  PdelayReqHeader; */ /**< last recieved peer delay reques header */

    UInteger16  sentPDelayReqSequenceId;
    UInteger16  sentDelayReqSequenceId;
    UInteger16  sentSyncSequenceId;
    UInteger16  sentAnnounceSequenceId;

    UInteger16  recvPDelayReqSequenceId;
    UInteger16  recvSyncSequenceId;

    Boolean  waitingForFollowUp; /**< true if sync message was recieved and 2step flag is set */
	Boolean  waitingForPDelayRespFollowUp; /**< true if PDelayResp message was recieved and 2step flag is set */

    Filter  ofm_filt; /**< filter offset from master */
    Filter  owd_filt; /**< filter one way delay */
	Filter  slv_filt; /**< filter scaled log variance */
	UInteger16 offsetHistory[2];
    Integer32  observedDrift;

    Boolean messageActivity;

    IntervalTimer  itimer[TIMER_ARRAY_SIZE];

    NetPath netPath;

    Enumeration8 recommendedState;

    Octet portUuidField[PTP_UUID_LENGTH]; /**< Usefull to init network stuff*/

    TimeInternal  inboundLatency, outboundLatency;

	Servo servo;

    Integer32  events;

    Enumeration8  stats;

    RunTimeOpts * rtOpts;

} PtpClock;

#endif /*DATATYPES_H_*/
