/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   traceptp.h
 * \author zimmerli
 * \date   2020-01-30
 * \brief  PTP trace tool
 *
 */

#ifndef TRACEPTP_H_
#define TRACEPTP_H_

#include <kernel/base/kernel.h>
#include <kernel/base/rtc.h>

void traceptp_init(addr_t baseaddr, uint32_t ports, uint32_t entries, struct rtc_s *rtc);
void traceptp_print(uint16_t portnum);

void traceptp_tx(uint16_t portnum, uint8_t msgtype, uint16_t seqid);
void traceptp_tx_done(uint16_t portnum, uint8_t msgtype, uint32_t sec, uint32_t nsec);
void traceptp_rx(uint16_t portnum, addr_t addr, uint32_t len, uint32_t sec, uint32_t nsec);
void traceptp_tmr(uint16_t portnum, uint8_t timer_id);
void traceptp_reselect(uint64_t gmid);

#endif /* TRACEPTP_H_ */
