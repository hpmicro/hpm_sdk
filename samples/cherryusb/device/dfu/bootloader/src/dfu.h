/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _DFU_H_
#define _DFU_H_

#include "usbd_core.h"
#include "usbd_dfu.h"

void usbd_event_handler(uint8_t busid, uint8_t event);
void dfu_init(uint8_t busid, uint32_t reg_base);

#endif /* _DFU_H_ */
