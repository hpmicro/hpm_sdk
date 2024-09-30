/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_template.c
 * \author fra11385
 * \date 23.04.2021
 * \brief <insert file brief description>
 *
 */

#include <kernel/net/netdev_template.h>

#include <kernel/net/netdev.h>

int32_t netdev_template(struct netdev_s *netdev, enum netdev_access access, enum template_command cmd, void *data)
{
    if (!netdev || !netdev->netdev_template)
        return NETDEV_ERR_GENERAL;
    return netdev->netdev_template(netdev, access, cmd, data);
}
