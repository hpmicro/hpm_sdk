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

#include <kernel/net/netdev_psfp.h>

#include <kernel/net/netdev.h>

int32_t netdev_psfp(struct netdev_s *netdev, enum netdev_access access, enum psfp_command cmd, void *data)
{
    if (!netdev || !netdev->netdev_psfp)
        return NETDEV_ERR_GENERAL;
    return netdev->netdev_psfp(netdev, access, cmd, data);
}
