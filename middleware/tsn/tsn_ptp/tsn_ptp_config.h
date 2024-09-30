/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file    tsn_ptp_config.h
 * \author  zimmerli
 * \date    2019-03-08
 * \brief   IEEE 802.1as software protocol stack configuration
 *
 */
#ifndef TSN_PTP_CONFIG_H_
#define TSN_PTP_CONFIG_H_

/**
 * DEFINE TSN_PTP_CFG_PORTNUM
 *
 * Static maximum number of ports used by the PTP stack
 */
#define TSN_PTP_CFG_PORTNUM 4

/**
 * DEFINE TSN_PTP_CFG_INSTANCENUM
 *
 * Static maximum number of PTP instances used by the PTP stack
 */
#define TSN_PTP_CFG_INSTANCENUM 2

/**
 * DEFINE TSN_PTP_CFG_FOLLOWUP_TLV_ORG_CHECK
 *
 * check OrganizationId of the TLV in FollowUp frame
 * uncomment to skip this check
 *
 * introduced due to Spirent 802.1as Bug (2019/03)
 */
#define TSN_PTP_CFG_FOLLOWUP_TLV_ORG_CHECK

/**
 * DEFINE TSN_PTP_CFG_PATHTRACE_MAX
 *
 * maximum number of pathtrace elements to be used
 * each element consumes 8 Bytes of memory
 */
#define TSN_PTP_CFG_PATHTRACE_MAX 16

#endif /* TSN_PTP_CONFIG_H_ */
