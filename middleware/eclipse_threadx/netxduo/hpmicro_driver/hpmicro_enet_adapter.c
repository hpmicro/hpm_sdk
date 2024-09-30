/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpmicro_netx_conf.h"
#include "hpmicro_enet_adapter.h"
#include "hpm_common.h"
#include "hpm_enet_drv.h"
#include "hpm_enet_phy.h"
#include "hpm_otp_drv.h"

/* MAC ADDRESS */
#define MAC_ADDR0 0x98
#define MAC_ADDR1 0x2C
#define MAC_ADDR2 0xBC
#define MAC_ADDR3 0xB1
#define MAC_ADDR4 0x9F
#define MAC_ADDR5 0x17

ATTR_WEAK VOID _hardware_get_mac_address(UCHAR *mac)
{
    UINT uuid[OTP_SOC_UUID_LEN / sizeof(UINT)];

    for (UINT i = 0; i < ARRAY_SIZE(uuid); i++) {
        uuid[i] = otp_read_from_shadow(OTP_SOC_UUID_IDX + i);
    }

    if (!IS_UUID_INVALID(uuid)) {
        uuid[0] &= 0xfc;
        memcpy(mac, &uuid, ENET_MAC);
    } else {
        mac[0] = MAC_ADDR0;
        mac[1] = MAC_ADDR1;
        mac[2] = MAC_ADDR2;
        mac[3] = MAC_ADDR3;
        mac[4] = MAC_ADDR4;
        mac[5] = MAC_ADDR5;
    }
}

static INT enet_mac_init(ENET_Type *ptr, enet_mac_config_t *config, enet_inf_type_t inf_type)
{
    for (INT i = 0; i < config->valid_max_count; i++) {
        if (i == 0) {
            ptr->MAC_ADDR_0_HIGH &= ~ENET_MAC_ADDR_0_HIGH_ADDRHI_MASK;
            ptr->MAC_ADDR_0_LOW &= ~ENET_MAC_ADDR_0_LOW_ADDRLO_MASK;
            ptr->MAC_ADDR_0_HIGH |= ENET_MAC_ADDR_0_HIGH_ADDRHI_SET(config->mac_addr_high[i]);
            ptr->MAC_ADDR_0_LOW |= ENET_MAC_ADDR_0_LOW_ADDRLO_SET(config->mac_addr_low[i]);
        } else {
            ptr->MAC_ADDR[i - 1].HIGH &= ~ENET_MAC_ADDR_HIGH_ADDRHI_MASK;
            ptr->MAC_ADDR[i - 1].LOW &= ~ENET_MAC_ADDR_LOW_ADDRLO_MASK;
            ptr->MAC_ADDR[i - 1].HIGH |= ENET_MAC_ADDR_HIGH_AE_MASK | ENET_MAC_ADDR_HIGH_ADDRHI_SET(config->mac_addr_high[i]);
            ptr->MAC_ADDR[i - 1].LOW |= ENET_MAC_ADDR_LOW_ADDRLO_SET(config->mac_addr_low[i]);
        }
    }

    /* set the appropriate filters for the incoming frames */
    ptr->MACFF |= ENET_MACFF_RA_SET(1); /* receive all */

    /* replace the content of the mac address 0 in the sa field of all transmitted frames */
    ptr->MACCFG &= ~ENET_MACCFG_SARC_MASK;
    ptr->MACCFG |= ENET_MACCFG_SARC_SET(config->sarc);

    ptr->MACCFG |= ENET_MACCFG_PS_MASK | ENET_MACCFG_FES_MASK;

    if (inf_type == enet_inf_rgmii) {
        ptr->MACCFG &= ~ENET_MACCFG_PS_MASK;
    } else if (inf_type == enet_inf_rmii) {
        ptr->MACCFG |= ENET_MACCFG_PS_MASK | ENET_MACCFG_FES_MASK;
    } else {
        return status_invalid_argument;
    }

    ptr->MACCFG |= ENET_MACCFG_DM_MASK;

    if (ENET_MACCFG_DM_GET(ptr->MACCFG) == 0) {
        ptr->MACCFG |= ENET_MACCFG_IFG_SET(4);
    } else {
        ptr->MACCFG |= ENET_MACCFG_IFG_SET(2);
    }

    /* enable transmitter enable and receiver */
    ptr->MACCFG |= ENET_MACCFG_TE_MASK | ENET_MACCFG_RE_MASK;

    return true;
}

static void enet_mode_init(ENET_Type *ptr, UINT intr)
{
    /* receive and transmit store and forward */
    ptr->DMA_OP_MODE |= ENET_DMA_OP_MODE_RSF_MASK | ENET_DMA_OP_MODE_TSF_MASK;

    /* enalbe hardware flow control */
    ptr->DMA_OP_MODE |= ENET_DMA_OP_MODE_EFC_MASK;

    /* enable error frame and undersized good frame forwarding */
    ptr->DMA_OP_MODE |= ENET_DMA_OP_MODE_FEF_MASK;

    /* disable osf mode */
    ptr->DMA_OP_MODE &= ~ENET_DMA_OP_MODE_OSF_MASK;

    ptr->DMA_INTR_EN |= intr;

    while (ENET_DMA_BUS_STATUS_AXIRDSTS_GET(ptr->DMA_BUS_STATUS) || ENET_DMA_BUS_STATUS_AXWHSTS_GET(ptr->DMA_BUS_STATUS)) {
    }

    /* start the receive and transmit dma */
    ptr->DMA_OP_MODE |= ENET_DMA_OP_MODE_ST_MASK | ENET_DMA_OP_MODE_SR_MASK;
}

static void enet_dma_tx_desc_chain_init_netx(ENET_Type *ptr, enet_desc_t_netx *desc)
{
    UINT i = 0;
    enet_tx_desc_t *dma_tx_desc;

    /* set the tx_desc_list_cur pointer with the first one of the dma_tx_desc_tab list */
    desc->tx_desc_list_cur = desc->tx_desc_list_head;

    /* fill each dma_tx_desc descriptor with the right values */
    for (i = 0; i < desc->tx_buff_cfg.count; i++) {

        /* get the pointer on the ith member of the Tx desc list */
        dma_tx_desc = desc->tx_desc_list_head + i;

        /* set second address chained bit */
        dma_tx_desc->tdes0_bm.tch = 1;
#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_COPY_CPU)
        /* set buffer 1 address pointer */
        dma_tx_desc->tdes2_bm.buffer1 = (uint32_t)(&((uint8_t *)desc->tx_buff_cfg.buffer)[i * desc->tx_buff_cfg.size]);
#endif
        /* link all Tx descriptors */
        if (i < desc->tx_buff_cfg.count - 1) {
            /* set next descriptor address register with the next descriptor base address */
            dma_tx_desc->tdes3_bm.next_desc = (UINT)(desc->tx_desc_list_head + i + 1);
        } else {
            /* for last descriptor, set next descriptor address register equal to the first descriptor base address */
            dma_tx_desc->tdes3_bm.next_desc = (UINT)desc->tx_desc_list_head;
        }
    }

    /* set transmit descriptor list address register */
    ptr->DMA_TX_DESC_LIST_ADDR = (UINT)desc->tx_desc_list_head;
}

static void enet_dma_rx_desc_chain_netx_init(ENET_Type *ptr, enet_desc_t_netx *desc)
{
    UINT i = 0;
    enet_rx_desc_t *dma_rx_desc;

    /* set the rx_desc_list_cur pointer with the first one of the dma_rx_desc_tab list */
    desc->rx_desc_list_cur = desc->rx_desc_list_head;
    /* fill each dma_rx_desc descriptor with the right values */
    for (i = 0; i < ENET_RX_BUFF_COUNT; i++) {
        /* get the pointer on the ith member of the Rx desc list */
        dma_rx_desc = desc->rx_desc_list_head + i;
        /* set own bit of the rx descriptor status */
        dma_rx_desc->rdes0_bm.own = 1;

        /* set buffer 1 size and second address chained bit */
        dma_rx_desc->rdes1_bm.rch = 1;
#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
        dma_rx_desc->rdes1_bm.rbs1 = desc->rx_buff_cfg[i].size;

        /* set buffer 1 address pointer */
        dma_rx_desc->rdes2_bm.buffer1 = (UINT)desc->rx_buff_cfg[i].buffer;
#else
        dma_rx_desc->rdes1_bm.rbs1 = desc->rx_buff_cfg.size;

        /* set buffer 1 address pointer */
        dma_rx_desc->rdes2_bm.buffer1 = (uint32_t)(&((uint8_t *)desc->rx_buff_cfg.buffer)[i * desc->rx_buff_cfg.size]);
#endif
        /* link all Rx descriptors */
        if (i < ENET_RX_BUFF_COUNT - 1) {
            /* set next descriptor address register with next descriptor base address */
            dma_rx_desc->rdes3_bm.next_desc = (UINT)(desc->rx_desc_list_head + i + 1);
        } else {
            /* for last descriptor, set next descriptor address register equal to the first descriptor base address */
            dma_rx_desc->rdes3_bm.next_desc = (UINT)desc->rx_desc_list_head;
        }
    }
    /* set receive descriptor list address register */

    ptr->DMA_RX_DESC_LIST_ADDR = (UINT)desc->rx_desc_list_head;
}

static INT enet_dma_netx_init(ENET_Type *ptr, enet_desc_t_netx *desc, UINT intr, UCHAR pbl)
{
    /* generate software reset */
    ptr->DMA_BUS_MODE |= ENET_DMA_BUS_MODE_SWR_MASK;

    /* wait for the completion of reset process */
    while (ENET_DMA_BUS_MODE_SWR_GET(ptr->DMA_BUS_MODE)) {
    }

    /* initialize bus mode register */
    ptr->DMA_BUS_MODE |= ENET_DMA_BUS_MODE_AAL_MASK;

    ptr->DMA_BUS_MODE &= ~ENET_DMA_BUS_MODE_FB_MASK;

    /* enable pblx8 mode */
    ptr->DMA_BUS_MODE |= ENET_DMA_BUS_MODE_PBLX8_MASK;

    /* set programmable burst length */
    ptr->DMA_BUS_MODE &= ~ENET_DMA_BUS_MODE_PBL_SHIFT;
    ptr->DMA_BUS_MODE |= ENET_DMA_BUS_MODE_PBL_SET(pbl);

    /* disable separate pbl */
    ptr->DMA_BUS_MODE &= ~ENET_DMA_BUS_MODE_USP_MASK;

    /* descriptor length */
#if ENET_SOC_ALT_EHD_DES_LEN == ENET_SOC_ALT_EHD_DES_MIN_LEN
    ptr->DMA_BUS_MODE &= ~ENET_DMA_BUS_MODE_ATDS_MASK;
#elif ENET_SOC_ALT_EHD_DES_LEN == ENET_SOC_ALT_EHD_DES_MAX_LEN
    ptr->DMA_BUS_MODE |= ENET_DMA_BUS_MODE_ATDS_MASK;
#endif

    /* set the maximum enabled burst length */
    if (ENET_DMA_BUS_MODE_FB_GET(ptr->DMA_BUS_MODE) == 0) {
        ptr->DMA_AXI_MODE |= ENET_DMA_AXI_MODE_BLEN4_MASK | ENET_DMA_AXI_MODE_BLEN8_MASK | ENET_DMA_AXI_MODE_BLEN16_MASK;
    } else {
        /* TODO: set BLENX_MASK */
    }

    /* initialize Tx descriptors list: chain mode */

    enet_dma_tx_desc_chain_init_netx(ptr, desc);

    /* initialize Rx descriptors list: Chain Mode  */
    enet_dma_rx_desc_chain_netx_init(ptr, desc);

    enet_mode_init(ptr, intr);

    enet_dma_flush(ptr);

    return true;
}

INT enet_controller_netx_init(ENET_Type *ptr, enet_inf_type_t inf_type, enet_desc_t_netx *desc, enet_mac_config_t *config, enet_int_config_t *int_config)
{
    /* select an interface */
    enet_intf_selection(ptr, inf_type);

    /* initialize DMA */
    enet_dma_netx_init(ptr, desc, int_config->int_enable, config->dma_pbl);

    /* Initialize MAC */
    enet_mac_init(ptr, config, inf_type);

    /* Mask the specified interrupts */
    enet_mask_interrupt_event(ptr, int_config->int_mask);

    return true;
}

ATTR_RAMFUNC VOID enet_dma_rx_desc_set_ch_buffer(ENET_Type *ptr, enet_desc_t_netx *desc, UCHAR ch, VOID *buf, size_t bytes)
{
    enet_rx_desc_t *dma_rx_desc;
    (void)ptr;

    dma_rx_desc = desc->rx_desc_list_head + ch;
    dma_rx_desc->rdes2_bm.buffer1 = (UINT)buf;
    dma_rx_desc->rdes1_bm.rbs1 = bytes;
}

ATTR_RAMFUNC VOID enet_dma_rx_desc_give_to_hardware(ENET_Type *ptr, enet_desc_t_netx *desc, UCHAR ch)
{
    enet_rx_desc_t *dma_rx_desc;
    (void)ptr;

    dma_rx_desc = desc->rx_desc_list_head + ch;
    dma_rx_desc->rdes0_bm.own = 1;
}