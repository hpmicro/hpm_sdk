/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   netdev_tc.h
 * \author zimmerli
 * \date   2020-01-26
 * \brief  Network device traffic control
 *
 */

#ifndef NETDEV_TC_H_
#define NETDEV_TC_H_

/**
 * \brief Traffic control type
 */
enum tc_setup_type {
    TC_TYPE_CBS, //!< credit based shaper
    TC_TYPE_TAS, //!< time aware scheduler
    TC_TYPE_TGB, //!< time aware scheduler (guardband)
    TC_TYPE_MAP  //!< prio2queue remapping
};

struct tc_prio2queue_map {
    uint32_t p0       : 3; //prio/pcp 0
    uint32_t p1       : 3; //prio/pcp 1
    uint32_t p2       : 3; //prio/pcp 2
    uint32_t p3       : 3; //prio/pcp 3
    uint32_t p4       : 3; //prio/pcp 4
    uint32_t p5       : 3; //prio/pcp 5
    uint32_t p6       : 3; //prio/pcp 6
    uint32_t p7       : 3; //prio/pcp 7
    uint32_t reserved : 8; //reserved
};

/**
 * \brief CBS settings
 */
struct tc_cbs_qopt_offload {
    uint8_t enable;    //!< enable flag
    uint8_t queue;     //!< traffic queue
    int32_t credit_hi; //!< credit, maximum
    int32_t credit_lo; //!< credit, minimum
    int32_t idleslope; //!< idle slope [bit/s]
    int32_t sendslope; //!< send slope
};

#define TAPRIO_CMD_SET_GATESTATES       0
#define TAPRIO_CMD_SET_AND_HOLD_MAC     1
#define TAPRIO_CMD_SET_AND_REALEASE_MAC 2

/**
 * \brief TAS scheduler entry
 */
struct tc_taprio_sched_entry {
    uint8_t cmd;       //!< Qbu command, see TAPRIO_CMD_*
    uint8_t gatestate; //!< gate state vector
    uint32_t interval; //!< interval in nanoseconds
};

/**
 * \brief TAS settings
 */
struct tc_taprio_qopt_offload {
    uint8_t enable;                      //!< enable flag
    uint8_t cfgpend;                     //!< configuration pending
    uint8_t cfgerr;                      //!< configuration errror
    uint32_t basetime_ns;                //!< admin base time, nanoseconds
    uint32_t basetime_sec;               //!< admin base time, seconds
    uint32_t cycletime;                  //!< admin cycle time in nanoseconds
    uint32_t num_entries;                //!< number of used entries in array entry
    struct tc_taprio_sched_entry *entry; //!< scheduler entries

    uint8_t opergs;         //!< operational gate states
    uint8_t admings;        //!< admin gate states
    uint32_t obasetime_ns;  //!< operational base time, nanoseconds
    uint32_t obasetime_sec; //!< operational base time, seconds
    uint32_t ocycletime;    //!< operational cycle time in nanoseconds
    uint32_t txov[8];       //!< transmission overrun counter
};

/**
 * \brief TAS guardband
 */
struct tc_tas_gb {
    uint8_t queue;
    uint32_t clktks;
};

#endif /* NETDEV_TC_H_ */
