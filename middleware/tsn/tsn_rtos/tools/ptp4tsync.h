/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   ptp4tsync.h
 * \author zimmerli
 * \date   2020-01-17
 * \brief  PTP (802.1as) for TSN-EP (TSYNC)
 *
 */

#ifndef PTP4TSYNC_H_
#define PTP4TSYNC_H_

#include <kernel/base/rtc.h>
#include <kernel/driver/tsn_ipms/ep/tsn_ctrl_drv.h>

enum ptp4sync_sync_e {
    PTP4SYNC_PRIMARY,
    PTP4SYNC_FALLBACK,
    PTP4SYNC_NONE
};

enum ptp4sync_port_role_e {
    PTP4SYNC_ROLE_DYNAMIC,
    PTP4SYNC_ROLE_MASTER,
    PTP4SYNC_ROLE_SLAVE,
    PTP4SYNC_ROLE_PASSIVE,
    PTP4SYNC_ROLE_DISABLED
};

void ptp4sync_start(struct rtc_s *rtc, uint8_t *mac_addr);

void ptp4sync_add_domain(uint8_t domainNumber, uint8_t gmPrio1, uint32_t port_num, enum ptp4sync_sync_e type);
void ptp4sync_rm_domain(uint8_t domainNumber);
void ptp4sync_add_sync(uint8_t sourceDomainNumber, uint8_t targetDomainNumber);
void ptp4sync_rm_sync(uint8_t sourceDomainNumber);
void ptp4sync_add_port(uint8_t domainNumber, uint16_t portIndex, struct netdev_s *device);
void ptp4sync_rm_port(uint8_t domainNumber, uint16_t portIndex);

void ptp4sync_set_traffic_prio(uint8_t domainNumber, uint8_t prio);
void ptp4sync_set_gm_prio1(uint8_t domainNumber, uint8_t prio1);
void ptp4sync_set_rtcsync_mode(uint8_t domainNumber, enum ptp4sync_sync_e syncMode);
void ptp4sync_set_ext_port_state(uint8_t domainNumber, uint32_t index, enum ptp4sync_port_role_e role);

void ptp4sync_get_state(uint8_t domainNumber, uint64_t *gmid, uint16_t *gmchanges, int32_t *gmoffset);
void ptp4sync_get_port_state(uint8_t domainNumber, uint32_t index, uint8_t *ascapable, uint8_t *portstate, uint16_t *pdelay);

void ptp4sync_list(void);
void ptp4sync_status(uint8_t domainNumber);

#endif /* PTP4TSYNC_H_ */
