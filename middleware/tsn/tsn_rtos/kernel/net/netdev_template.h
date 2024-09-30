/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_template.h
 * \author fra11385
 * \date 23.04.2021
 * \brief <insert file brief description>
 *
 */

#ifndef NETDEV_TEMPLATE_H_
#define NETDEV_TEMPLATE_H_

#include <kernel/net/netdev.h>

//#######################################################################################
//declare netdev access function for use in tools
//=======================================================================================

int32_t netdev_template(struct netdev_s *netdev, enum netdev_access access, enum template_command cmd, void *data);

//#######################################################################################

#endif /* NETDEV_TEMPLATE_H_ */
