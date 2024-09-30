/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_bridge.h
 * \author fra11385
 * \date 22.03.2021
 * \brief <insert file brief description>
 *
 */

#ifndef NETDEV_BRIDGE_H_
#define NETDEV_BRIDGE_H_

enum bridge_command {
    BRIDGE_HITMEM, //!< hit memory
    BRIDGE_FDB,    //!< forwarding database entry
    BRIDGE_VLAN,   //!< VLAN filter list
    BRIDGE_FW      //!< global forward action
};

#endif /* NETDEV_BRIDGE_H_ */
