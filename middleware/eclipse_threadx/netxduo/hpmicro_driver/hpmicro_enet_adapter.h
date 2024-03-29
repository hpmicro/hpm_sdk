/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _HPMICRO_ENET_ADAPTER_H_
#define _HPMICRO_ENET_ADAPTER_H_

#include "hpm_enet_drv.h"
#include "tx_port.h"
#include "hpmicro_netx_conf.h"

#define IS_UUID_INVALID(UUID) (UUID[0] == 0 && \
                               UUID[1] == 0 && \
                               UUID[2] == 0 && \
                               UUID[3] == 0)

typedef struct {
    UINT buffer;
    USHORT size;
} enet_buff_config_netx_t;

typedef struct {
    enet_tx_desc_t *tx_desc_list_head;
    enet_rx_desc_t *rx_desc_list_head;
    enet_tx_desc_t *tx_desc_list_cur;
    enet_rx_desc_t *rx_desc_list_cur;
    enet_buff_config_t tx_buff_cfg;
#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
    enet_buff_config_netx_t rx_buff_cfg[60];
#else
    enet_buff_config_t rx_buff_cfg;
#endif
    enet_rx_frame_info_t rx_frame_info;
    enet_tx_control_config_t tx_control_config;
} enet_desc_t_netx;

INT enet_controller_netx_init(ENET_Type *ptr, enet_inf_type_t inf_type, enet_desc_t_netx *desc, enet_mac_config_t *config, enet_int_config_t *int_config);
VOID _hardware_get_mac_address(UCHAR *mac);
VOID enet_dma_rx_desc_set_ch_buffer(ENET_Type *ptr, enet_desc_t_netx *desc, UCHAR ch, VOID *buf, size_t bytes);
VOID enet_dma_rx_desc_give_to_hardware(ENET_Type *ptr, enet_desc_t_netx *desc, UCHAR ch);
#endif