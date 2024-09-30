/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   netdev_fpe.h
 * \author zimmerli
 * \date   2020-01-28
 * \brief  Network device frame preemption
 *
 */

#ifndef NETDEV_FPE_H_
#define NETDEV_FPE_H_

/**
 * \brief Frame preemption settings
 */
struct netdev_fpe {
    uint8_t enable;     //!< enable flag
    uint8_t dis_verify; //!< disable verification
    uint8_t fragsz;     //!< fragment size
    uint32_t vtime_ns;  //!< verification timeout, nanoseconds
    uint8_t fpst;       //!< frame preemption status table
    uint16_t holdadv;

    uint8_t hold;        //!< hold state
    uint8_t verify_ok;   //!< verification state ok
    uint8_t verify_fail; //!< verification state failure
    uint8_t link;        //!< link error

    uint32_t counter[6]; //!< statistic counter //ERSTMAL
};

#endif /* NETDEV_FPE_H_ */
