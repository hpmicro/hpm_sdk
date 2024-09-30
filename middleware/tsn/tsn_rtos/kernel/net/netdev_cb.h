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

#ifndef NETDEV_CB_H_
#define NETDEV_CB_H_

#include <kernel/net/netdev.h>

int32_t netdev_cb(struct netdev_s *netdev, enum netdev_access access, enum cb_command cmd, void *data);

#endif /* NETDEV_CB_H_ */
