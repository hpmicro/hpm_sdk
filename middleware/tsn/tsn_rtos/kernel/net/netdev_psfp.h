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

#ifndef NETDEV_PSFP_H_
#define NETDEV_PSFP_H_

#include <kernel/net/netdev.h>

int32_t netdev_psfp(struct netdev_s *netdev, enum netdev_access access, enum psfp_command cmd, void *data);

#endif /* NETDEV_PSFP_H_ */
