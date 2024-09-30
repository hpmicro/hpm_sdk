/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_fdfifo.h
 * \author fra11385
 * \date 22.03.2021
 * \brief <insert file brief description>
 *
 */

#ifndef NETDEV_FDFIFO_H_
#define NETDEV_FDFIFO_H_

enum fdfifo_command {
    FDFIFO_SF, //!< store & forward mode
    FDFIFO_PM  //!< port grouping via port mask
};

#endif /* NETDEV_FDFIFO_H_ */
