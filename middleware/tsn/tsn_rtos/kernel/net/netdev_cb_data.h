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
 * \date 23.04.2021
 * \brief <insert file brief description>
 *
 */

#ifndef NETDEV_CB_DATA_H_
#define NETDEV_CB_DATA_H_

#include <kernel/base/klibc.h>

enum cb_command {
    STMID,

    //ERSTMAL mit '_' am Ende, da Namenskollision
    FRER_CTRL_,
    FRER_SIDFNC_,
    FRER_FCTRL_
};

typedef struct {
    uint8_t dir;
    uint8_t entry;
    uint8_t action;
    uint8_t mode;
    uint8_t lookup;
    uint8_t actctl;
    uint8_t seqgen;
    uint8_t sid;
    int32_t seqnum;
    int64_t match;
    uint8_t *mac;
    uint8_t vid;
    uint8_t *amac;
    uint8_t avid;
    uint8_t apcp;
} netdev_cb_stmid_data_t;

typedef struct {
    list_data_t *list;
    uint8_t dir;
} netdev_cb_listdata_t;

typedef struct {
    uint8_t dir;
    uint8_t later;
    uint8_t rtenc;
} netdev_cb_frer_ctrl_data_t;

typedef struct {
    uint8_t dir;
    uint8_t sid;
    uint8_t irfen;
    uint8_t irfidx;
    uint8_t srfen;
    uint8_t srfidx;
} netdev_cb_frer_sidfnc_data_t;

//ERSTMAL
typedef struct {
    uint8_t dir;
    uint8_t fidx;
    uint8_t frset;
    uint8_t paths;
    uint8_t hlen;
    uint8_t algo;
    uint8_t laten;
    uint8_t ind;
    uint8_t tns;

    uint32_t fsrms;
    uint32_t flatr;
    uint32_t flatt;
    uint32_t fdiff;
    uint32_t laterr;

    uint32_t cnt[8];
} netdev_cb_frer_fctrl_data_t;

#endif /* NETDEV_CB_DATA_H_ */
