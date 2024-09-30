/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_template_data.h
 * \author fra11385
 * \date 23.04.2021
 * \brief <insert file brief description>
 *
 */

#ifndef NETDEV_TEMPLATE_DATA_H_
#define NETDEV_TEMPLATE_DATA_H_

#include <kernel/base/klibc.h>

//#######################################################################################
//define of commands
//=======================================================================================

enum template_command {
    TEMPLATE_CMD1,
    TEMPLATE_CMD2
};

//#######################################################################################

//#######################################################################################
//define data structure(s) for netdev_template() call in templatetool.c
//=======================================================================================
typedef struct {
    bool has_keyword;
    uint32_t templ_dec;
    uint32_t templ_hex;
    uint32_t templ_enum;
    char *templ_string;
    uint8_t *templ_mac;
    list_data_t *templ_list;
} netdev_template_data_t;

//#######################################################################################

#endif /* NETDEV_TEMPLATE_DATA_H_ */
