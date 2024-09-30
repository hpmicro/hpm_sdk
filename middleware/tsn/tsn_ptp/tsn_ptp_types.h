/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file    tsn_ptp_types.h
 * \author  zimmerli
 * \date    2019-01-30
 * \brief   PTP data types
 *
 */
#ifndef TSN_PTP_TYPES_H_
#define TSN_PTP_TYPES_H_

#include <stdint.h>

/**
 * \brief Scaled Nanoseconds
 *
 * IEEE 802.1as 6.3.3.1: int96_t
 */
typedef struct {
    int16_t ns_msb;  //!< nanoseconds msb part (signed)
    uint64_t ns_lsb; //!< nanoseconds lsb part
    uint16_t fns;    //!< fractional nanoseconds (div by 2**16)
} scaled_ns_t;       // 6.3.3.1

/**
 * \brief Unsigned Scaled Nanoseconds
 *
 * IEEE 802.1as 6.3.3.2: uint96_t
 */
typedef struct {
    uint16_t ns_msb; //!< nanoseconds msb part (unsigned)
    uint64_t ns_lsb; //!< nanoseconds lsb part
    uint16_t fns;    //!< fractional nanoseconds (div by 2**16)
} uscaled_ns_t;

/**
 * \brief PTP Timestamp
 *
 * IEEE 802.1as 6.3.3.4
 */
typedef struct {
    uint64_t seconds;     //!< seconds part, 48 bit are used
    uint32_t nanoseconds; //!< nanoseconds, 0 .. 10^9-1
} ptp_timestamp_t;

/**
 * \brief PTP Extended Timestamp
 *
 * IEEE 802.1as-2020 6.4.3.5
 */
typedef struct {
    uint64_t seconds; //!< seconds part, 48 bit are used
    uint64_t fns;     //!< fractional nanoseconds, 48 bit are used, in units of 2^-16 nanoseconds
} ptp_ext_timestamp_t;

/**
 * \brief Clock Identity
 *
 * 8 octets, see IEEE 802.1as 6.3.3.6
 */
typedef uint64_t clock_id;

/**
 * \brief Port Identity
 *
 * IEEE 802.1as 6.3.3.7
 */
typedef struct {
    clock_id clockIdentity; //!< mapping of EUI-48 to EUI-64 format
    uint16_t portNumber;    //!< see 8.5.2.3, shall be 1 for EndPoint and 1 .. N for Bridge
} port_id_t;

/**
 * \brief Clock Quality
 *
 * IEEE 802.1as 6.3.3.8
 */
typedef struct {
    uint8_t clockClass;               //!< see 8.6.2.2, see 7.6.2.4 of IEEE Std 1588 for details
    uint8_t clockAccuracy;            //!< see 7.6.2.5 of IEEE Std 1588 for details
    uint16_t offsetScaledLogVariance; //!< see 8.6.2.4, scaled offset of estimate of PTP variance
} clock_quality_t;

/**
 * \brief PortSyncSync
 *
 * IEEE 802.1as 10.2.2.3
 * Modification:
 * * using scaledLastGmFreqChange
 * * using uint64_t for upstreamTxTime --> upstreamTxTimeNs
 */
typedef struct {
    uint64_t syncReceiptTimeout;            //!< syncReceiptTimeout, ns
    int64_t followUpCorrectionField;        //!< followUpCorrectionField, scaled ns
    port_id_t srcPortIdentity;              //!< sourcePortIdentity
    ptp_timestamp_t preciseOriginTimestamp; //!< preciseOriginTimestamp
    uint64_t upstreamTxTimeNs;              //!< upstreamTxTimeNs, only ns part
    int32_t scaledRateRatio;                //!< scaledRateRatio: (rateRatio-1.0)<<41
    scaled_ns_t lastGmPhaseChange;          //!< lastGmPhaseChange
    int32_t scaledLastGmFreqChange;         //!< scaledLastGmFreqChange
    uint16_t gmTimeBaseIndicator;           //!< gmTimeBaseIndicator
    uint16_t localPortNum;                  //!< localPortNumber
    int8_t logMsgInterval;                  //!< logMessageInterval
} port_sync_sync_t;

/**
 * \brief systemIdentity
 *
 * IEEE 802.1as 10.3.2: UInteger112
 * lower numerical values to indicate better information;
 */
typedef struct {
    uint8_t prio1;                //!< priority1, see 8.6.2.1 for defaults
    uint8_t prio2;                //!< priority2, see 8.6.2.5
    clock_quality_t clockQuality; //!< clockQuality
    clock_id clockIdentity;       //!< clockIdentity
} ptp_system_id_t;

/**
 * \brief Priority Vector
 *
 * IEEE 802.1as 10.3.5
 * lower numerical values to indicate better information
 */
typedef struct {
    ptp_system_id_t rootSystemId; //!< rootSystemIdentity
    uint16_t stepsRemoved;        //!< stepsRemoved
    port_id_t sourcePortId;       //!< sourcePortIdentity
    uint16_t portNumber;          //!< portNumber
} ptp_prio_vect_t;

#endif /* TSN_PTP_TYPES_H_ */
