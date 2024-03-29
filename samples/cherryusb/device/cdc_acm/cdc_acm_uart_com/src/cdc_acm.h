/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _CDC_ACM_H
#define _CDC_ACM_H

#define USB_BUS_ID 0

/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

uint8_t *usbd_get_write_buffer_ptr(void);
uint8_t *usbd_get_read_buffer_ptr(void);
bool usbd_is_tx_busy(void);
void usbd_set_tx_busy(void);
void cdc_acm_init(uint8_t busid, uint32_t reg_base);
void usbd_init_line_coding(struct cdc_line_coding *line_coding);

#endif
