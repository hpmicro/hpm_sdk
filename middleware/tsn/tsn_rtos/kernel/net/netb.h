/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   netb.h
 * \author zimmerli
 * \date   2020-01-20
 * \brief  Network buffer
 *
 */

#ifndef NETB_H_
#define NETB_H_

#include <stdint.h>

#include <kernel/base/kernel.h>

/**
 * \brief network buffer
 *
 * head, data, tail, end: compatible to linux kernel sk_buff
 * netdev: set by netdev layer
 * tstamp_id: 0 for no timestamping, otherwise registered netdev tstamp_id
 */
struct netb_s {
    addr_t head;       //!< start address of buffer
    addr_t data;       //!< start address of used data
    addr_t tail;       //!< end address of used data
    addr_t end;        //!< end address of buffer
    int64_t timestamp; //!< rx timestamp in nanoseconds (local time)
    void *netdev;      //!< network device
    uint8_t tstamp_id; //!< tx timestamping id, 0 for no timestamping
    uint8_t priority;  //!< priority code, see ether.h ETH_PRIO_*
};

/**
 * \brief Get length of used data
 * @param netb      pointer to network buffer
 */
#define netb_length(netb)          (netb->tail - netb->data)
#define netb_remaininglength(netb) (netb->end - netb->data)

// object functions
void netb_set_empty(struct netb_s *netb);

void netb_reserve(struct netb_s *netb, uint32_t len);
uint8_t *netb_push(struct netb_s *netb, uint32_t len);
uint8_t *netb_pull(struct netb_s *netb, uint32_t len);
uint8_t *netb_put(struct netb_s *netb, uint32_t len);

#endif /* NETB_H_ */
