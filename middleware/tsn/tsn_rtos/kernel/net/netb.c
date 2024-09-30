/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   netb.c
 * \author zimmerli
 * \date   2020-01-20
 * \brief  Network buffer
 *
 */

#include "netb.h"
#include <kernel/base/kernel.h>
#include <kernel/net/ether.h>

/**
 * \brief Init network buffer
 *
 * @param netb      pointer to network buffer
 */
void netb_set_empty(struct netb_s *netb)
{
    netb->data = netb->head;
    netb->tail = netb->head;
    netb->netdev = (void *)0;
    netb->tstamp_id = 0;
    netb->priority = ETH_PRIO_BE; //default: best effort
}

/**
 * \brief Reserve space in empty netb in front of data
 *
 * @param netb  pointer to netb
 * @param len   length to reserve
 */
void netb_reserve(struct netb_s *netb, uint32_t len)
{
    netb->data += len;
    netb->tail += len;
}

/**
 * \brief Add data to start of buffer
 *
 * @param netb  pointer to netb
 * @param len   length in bytes
 * @return      pointer to data
 */
uint8_t *netb_push(struct netb_s *netb, uint32_t len)
{
    netb->data -= len;
    kassert(netb->data >= netb->head);

    return (uint8_t *)netb->data;
}

/**
 * \brief Remove data from start of buffer
 *
 * @param netb  pointer to netb
 * @param len   length in bytes
 * @return      pointer to data
 */
uint8_t *netb_pull(struct netb_s *netb, uint32_t len)
{
    netb->data += len;
    kassert(netb->data <= netb->tail);

    return (uint8_t *)netb->data;
}

/**
 * \brief Add data to buffer
 *
 * @param netb  pointer to netb
 * @param len   length in bytes
 * @return      pointer to extra amount of data
 */
uint8_t *netb_put(struct netb_s *netb, uint32_t len)
{
    uint8_t *ptail;

    ptail = (uint8_t *)netb->tail;
    netb->tail += len;
    kassert(netb->tail <= netb->end);

    return ptail;
}
