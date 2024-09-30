/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file netdev_ethtool.h
 * \author fra11385
 * \date 16.04.2021
 * \brief <insert file brief description>
 *
 */

#ifndef NETDEV_ETHTOOL_H_
#define NETDEV_ETHTOOL_H_

enum ethtool_command {
    ETHTOOL_RESET, //!< reset mac/pmac counter
    ETHTOOL_STATS, //!< mac/pmac statistics
};

typedef struct {
    uint32_t emac_tx_frames;
    uint32_t emac_tx_octets;
    uint32_t emac_rx_frames;
    uint32_t emac_rx_octets;
    uint32_t pmac_tx_frames;
    uint32_t pmac_tx_octets;
    uint32_t pmac_rx_frames;
    uint32_t pmac_rx_octets;
    uint32_t txuf;
    //	uint32_t mms_sts;
} mac_stat_t;

#endif /* NETDEV_ETHTOOL_H_ */
