/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file
 * \author fra11385
 * \date 06.12.2022
 * \brief <insert file brief description>
 *
 */

#ifndef NETDEV_PSFP_DATA_H_
#define NETDEV_PSFP_DATA_H_

#include <kernel/base/klibc.h>

enum psfp_command {
    PSFP,
    PSFP_FS,
    PSFP_GS,
    PSFP_FM
};

typedef enum {
    PSFP_CNT_MATCHED = 0,       //!< Frames that matched filter
    PSFP_CNT_PASSED_GATE = 1,   //!< Frames that passed gate
    PSFP_CNT_NOT_PASS_GATE = 2, //!< Frames that did not pass gate
    PSFP_CNT_PASSED_MXSZ = 3,   //!< Frames that passed Maximum-SDU size check
    PSFP_CNT_NOT_PASS_MXSZ = 4, //!< Frames that did not pass size check
    PSFP_CNT_DISC_FM = 5,       //!< Frames discarded by flow meter operation
} psfp_filter_counter;

typedef struct {
    int16_t filtersel;
    uint8_t ftd;
    uint8_t gtd;
    uint8_t fmd;
    uint32_t counter[6];
} netdev_psfp_data_t;

typedef struct {
    list_data_t *list;
} netdev_psfp_listdata_t;

typedef struct {
    uint8_t filtersel;
    uint8_t enblk;
    uint8_t enfsz;
    uint8_t enfid;
    uint8_t ensid;
    uint8_t enpcp;
    uint8_t sid;
    uint8_t pcp;
    uint8_t gid;
    uint8_t fmd;
    uint8_t blk;
    bool blkreset;
    uint16_t mxsz;
} netdev_psfp_fs_data_t;

typedef struct {
    uint8_t metersel;
    uint8_t encf;
    uint8_t cmode;
    uint8_t endoy;
    uint8_t enmafr;
    uint32_t ebs;
    uint32_t eir;
    uint32_t cbs;
    uint32_t cir;
    uint8_t mafr;
    bool meterreset;
} netdev_psfp_fm_data_t;

typedef struct {
    //	uint8_t		idx;	//!<
    uint8_t state;
    uint8_t ipv;
    uint32_t oct;
    uint32_t ival;
} netdev_psfp_gs_alist_t;

typedef struct {
    uint8_t gatesel;
    uint8_t engate;
    uint8_t encdir; //!< ClosedDueToInvalidRxEnable
    uint8_t encdoe; //!< ClosedDueToOctetsExceededEnable
    uint8_t cdir;   //!< ClosedDueToInvalidRx
    uint8_t cdoe;   //!< ClosedDueToOctetsExceeded
    uint8_t astate; //!< administrative stream gate state
    uint8_t aipv;   //!< administrative internal priority value specification
    uint8_t ostate; //!< Operational stream gate state
    uint8_t oipv;   //!< Operational internal priority value specification
    uint8_t cfgp;   //!< configuration change pending
    uint8_t cfgerr; //!< configuration change error

    uint8_t olen;  //!< operational list length
    uint8_t alen;  //!< administrative list length
    uint32_t act;  //!< administrative cycle time length(?), nanoseconds
    uint32_t abtl; //!< administrative base time; Nanoseconds and seconds part. Cycle starts after becoming operational when time is reached
    uint32_t abth; //!<
    uint32_t oct;  //!< operational cycle time length, nanoseconds
    uint32_t obtl; //!< operational base time; Nanoseconds and seconds part. Cycle starts after becoming operational when time is reached
    uint32_t obth; //!<

    netdev_psfp_gs_alist_t aentry[16]; //admin list
} netdev_psfp_gs_data_t;

#endif /* NETDEV_PSFP_DATA_H_ */
