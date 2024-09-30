/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_tsw_drv.h"
#include "hpm_swap.h"

static uint32_t tsw_ns_to_systicks(uint32_t nanoseconds, uint32_t busfreq)
{
    return (uint32_t)((uint64_t)nanoseconds * busfreq / TSW_NS_IN_ONE_SEC);
}

void tsw_get_default_dma_config(tsw_dma_config_t *config)
{
    config->soe = false;
    config->irq = false;
    config->maxlen = 0xff;
}

hpm_stat_t tsw_ep_set_mdio_config(TSW_Type *ptr, uint8_t port, uint8_t clk_div)
{
    ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MDIO_CFG = TSW_TSNPORT_MAC_MAC_MDIO_CFG_ENABLE_MASK | TSW_TSNPORT_MAC_MAC_MDIO_CFG_MDC_CLKDIV_SET(clk_div);

    return status_success;
}

hpm_stat_t tsw_ep_mdio_read(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, uint32_t reg_addr, uint16_t *data)
{
    if (data == NULL) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MDIO_CTRL = TSW_TSNPORT_MAC_MAC_MDIO_CTRL_OP_SET(MAC_MDIO_CTRL_OP_RD)
                                                        | TSW_TSNPORT_MAC_MAC_MDIO_CTRL_PHYAD_SET(phy_addr)
                                                        | TSW_TSNPORT_MAC_MAC_MDIO_CTRL_REGAD_SET(reg_addr)
                                                        | TSW_TSNPORT_MAC_MAC_MDIO_CTRL_INIT_SET(1);

    do {

    } while (TSW_TSNPORT_MAC_MAC_MDIO_CTRL_READY_GET(ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MDIO_CTRL) == 0);

     *data = TSW_TSNPORT_MAC_MAC_MDIO_RD_DATA_RD_DATA_GET(ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MDIO_RD_DATA);

    return status_success;
}

hpm_stat_t tsw_ep_mdio_write(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, uint32_t reg_addr, uint16_t data)
{
    ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MDIO_WR_DATA = data;
    ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MDIO_CTRL = TSW_TSNPORT_MAC_MAC_MDIO_CTRL_OP_SET(MAC_MDIO_CTRL_OP_WR)
                                                        | TSW_TSNPORT_MAC_MAC_MDIO_CTRL_PHYAD_SET(phy_addr)
                                                        | TSW_TSNPORT_MAC_MAC_MDIO_CTRL_REGAD_SET(reg_addr)
                                                        | TSW_TSNPORT_MAC_MAC_MDIO_CTRL_INIT_SET(1);

    do {

    } while (TSW_TSNPORT_MAC_MAC_MDIO_CTRL_READY_GET(ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MDIO_CTRL) == 0);

    return status_success;
}

hpm_stat_t tsw_ep_enable_mac_ctrl(TSW_Type *ptr, uint8_t port, uint8_t mac_type)
{
    uint32_t temp;

    temp = ptr->TSNPORT[port].MAC[mac_type].MAC_MAC_CTRL;
    temp |= TSW_TSNPORT_MAC_MAC_MAC_CTRL_TX_EN_MASK | TSW_TSNPORT_MAC_MAC_MAC_CTRL_RX_EN_MASK;
    ptr->TSNPORT[port].MAC[mac_type].MAC_MAC_CTRL = temp;

    return status_success;
}

hpm_stat_t tsw_ep_disable_mac_ctrl(TSW_Type *ptr, uint8_t port, uint8_t mac_type)
{
    uint32_t temp;

    temp = ptr->TSNPORT[port].MAC[mac_type].MAC_MAC_CTRL;
    temp &= ~(TSW_TSNPORT_MAC_MAC_MAC_CTRL_TX_EN_MASK | TSW_TSNPORT_MAC_MAC_MAC_CTRL_RX_EN_MASK);
    ptr->TSNPORT[port].MAC[mac_type].MAC_MAC_CTRL = temp;

    do {
        temp = ptr->TSNPORT[port].MAC[mac_type].MAC_MAC_CTRL;
    } while (TSW_TSNPORT_MAC_MAC_MAC_CTRL_RX_EN_GET(temp) || TSW_TSNPORT_MAC_MAC_MAC_CTRL_TX_EN_GET(temp));

    return status_success;
}

hpm_stat_t tsw_ep_enable_all_mac_ctrl(TSW_Type *ptr, uint8_t mac_type)
{
    for (uint8_t i = 0; i <= TSW_TSNPORT_PORT3; i++) {
        tsw_ep_enable_mac_ctrl(ptr, i, mac_type);
    }

    return status_success;
}

hpm_stat_t tsw_ep_disable_all_mac_ctrl(TSW_Type *ptr, uint8_t mac_type)
{
    for (uint8_t i = 0; i <= TSW_TSNPORT_PORT3; i++) {
        tsw_ep_disable_mac_ctrl(ptr, i, mac_type);
    }

    return status_success;
}

hpm_stat_t tsw_ep_set_mac_addr(TSW_Type *ptr, uint8_t port, uint8_t *mac_addr, bool promisc)
{
    uint32_t temp;

    if (mac_addr == NULL) {
        return status_invalid_argument;
    }

    /* MAC must be disabled when changing mode */
    temp = ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MAC_CTRL;
    if (TSW_TSNPORT_MAC_MAC_MAC_CTRL_RX_EN_GET(temp) || TSW_TSNPORT_MAC_MAC_MAC_CTRL_TX_EN_GET(temp)) {
        return status_fail;
    }

    ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MACADDR_L = MAC_LO(mac_addr);
    ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MACADDR_H = MAC_HI(mac_addr) | TSW_TSNPORT_MAC_MAC_MACADDR_H_PROMISC_SET(promisc);

    return status_success;
}

hpm_stat_t tsw_ep_set_mac_mode(TSW_Type *ptr, uint8_t port, uint8_t gmii)
{
    uint32_t temp;

    /* MAC must be disabled when changing mode */
    temp = ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MAC_CTRL;
    if (TSW_TSNPORT_MAC_MAC_MAC_CTRL_RX_EN_GET(temp) || TSW_TSNPORT_MAC_MAC_MAC_CTRL_TX_EN_GET(temp)) {
        return status_fail;
    }

    temp &= ~(TSW_TSNPORT_MAC_MAC_MAC_CTRL_CLKSEL_MASK | TSW_TSNPORT_MAC_MAC_MAC_CTRL_PHYSEL_MASK | TSW_TSNPORT_MAC_MAC_MAC_CTRL_GMIIMODE_MASK);
    temp &= ~TSW_TSNPORT_MAC_MAC_MAC_CTRL_RESSTAT_MASK;

    temp |= TSW_TSNPORT_MAC_MAC_MAC_CTRL_CLKSEL_SET(1)
         | TSW_TSNPORT_MAC_MAC_MAC_CTRL_PHYSEL_SET(1)
         | TSW_TSNPORT_MAC_MAC_MAC_CTRL_GMIIMODE_SET(gmii);

    ptr->TSNPORT[port].MAC[TSW_RXFIFO_E1].MAC_MAC_CTRL = temp;

    return status_success;
}

void tsw_init_send(TSW_Type *ptr, tsw_dma_config_t *config)
{
    ptr->MM2S_DMA_CR |= TSW_MM2S_DMA_CR_RESET_MASK;

    do {

    } while (TSW_MM2S_DMA_SR_RSET_GET(ptr->MM2S_DMA_SR));

    ptr->MM2S_DMA_CR &= ~TSW_MM2S_DMA_CR_SOE_MASK;
    ptr->MM2S_DMA_CR |= TSW_MM2S_DMA_CR_SOE_SET(config->soe);

    ptr->MM2S_DMA_CR &= ~TSW_MM2S_DMA_CR_MXLEN_MASK;
    ptr->MM2S_DMA_CR |= TSW_MM2S_DMA_CR_MXLEN_SET(config->maxlen);

    ptr->MM2S_DMA_CR &= ~TSW_MM2S_DMA_CR_IRQEN_MASK;
    ptr->MM2S_DMA_CR |= TSW_MM2S_DMA_CR_MXLEN_SET(config->irq);

    ptr->MM2S_DMA_CR |= TSW_MM2S_DMA_CR_RUN_MASK;
}

hpm_stat_t tsw_send_frame(TSW_Type *ptr, uint8_t *buffer, uint16_t length, uint8_t id)
{
    uint32_t resp;

    /* Set ADDRL */
    ptr->MM2S_ADDRLO = (uint32_t)buffer;

    /* Set Length */
    ptr->MM2S_LENGTH = length;

    /* Set Ctrl */
    ptr->MM2S_CTRL |= TSW_MM2S_CTRL_GO_MASK | TSW_MM2S_CTRL_ID_SET(id);

    /* Wait for DMA to finish transmission */
    do {
        /* TODO: add retry cnt */
    } while (TSW_MM2S_DMA_SR_CBUFF_GET(ptr->MM2S_DMA_SR));

    /* Get response */
    resp = ptr->MM2S_RESP;

    if (TSW_MM2S_RESP_ID_GET(resp) == id) {

        /* Check decode error */
        if (TSW_MM2S_RESP_DECERR_GET(resp)) {
            return status_fail;
        }

        /* Check slave error */
        if (TSW_MM2S_RESP_SLVERR_GET(resp)) {
           return status_fail;
        }
    } else {
        return status_fail;
    }

    return status_success;
}

void tsw_init_recv(TSW_Type *ptr, tsw_dma_config_t *config)
{
    ptr->S2MM_DMA_CR |= TSW_S2MM_DMA_CR_RESET_MASK;

    do {

    } while (TSW_S2MM_DMA_SR_RSET_GET(ptr->S2MM_DMA_SR));

    ptr->S2MM_DMA_CR &= ~TSW_S2MM_DMA_CR_SOE_MASK;
    ptr->S2MM_DMA_CR |= TSW_S2MM_DMA_CR_SOE_SET(config->soe);

    ptr->S2MM_DMA_CR &= ~TSW_S2MM_DMA_CR_MXLEN_MASK;
    ptr->S2MM_DMA_CR |= TSW_S2MM_DMA_CR_MXLEN_SET(config->maxlen);

    ptr->S2MM_DMA_CR &= ~TSW_S2MM_DMA_CR_IRQEN_MASK;
    ptr->S2MM_DMA_CR |= TSW_S2MM_DMA_CR_IRQEN_SET(config->irq);

    ptr->S2MM_DMA_CR |= TSW_S2MM_DMA_CR_RUN_MASK;
}

hpm_stat_t tsw_recv_frame(TSW_Type *ptr, tsw_frame_t *frame)
{
    uint32_t resp;
    hpm_stat_t stat = status_timeout;

    if (TSW_S2MM_DMA_SR_IRQ_GET(ptr->S2MM_DMA_SR)) {

        resp = ptr->S2MM_RESP;

        frame->id = TSW_S2MM_RESP_ID_GET(resp);
        frame->length = TSW_S2MM_RESP_LENGTH_GET(resp);

        if (TSW_S2MM_RESP_DECERR_GET(resp) || TSW_S2MM_RESP_SLVERR_GET(resp)) {
            stat = status_fail;
        } else {
            stat = status_success;
        }

        HPM_TSW->S2MM_DMA_SR = TSW_S2MM_DMA_SR_IRQ_MASK;
    }

    return stat;
}

hpm_stat_t tsw_commit_recv_desc(TSW_Type *ptr, uint8_t *buffer, uint16_t length, uint8_t id)
{
    /* Set ADDRL */
    ptr->S2MM_ADDRLO = (uint32_t)buffer;

    /* Set Length */
    ptr->S2MM_LENGTH = length;

    /* Set Ctrl */
    ptr->S2MM_CTRL &= ~TSW_S2MM_CTRL_ID_MASK;
    ptr->S2MM_CTRL |= TSW_S2MM_CTRL_GO_MASK | TSW_S2MM_CTRL_ID_SET(id);

    return status_success;
}

void tsw_mac_lookup_bypass(TSW_Type *ptr, uint8_t dst_port)
{
    ptr->LU_MAIN_BYPASS &= ~TSW_LU_MAIN_BYPASS_DEST_MASK;
    ptr->LU_MAIN_BYPASS |= TSW_LU_MAIN_BYPASS_HIT_MASK | TSW_LU_MAIN_BYPASS_DEST_SET(dst_port);
    ptr->LU_MAIN_CTRL   |= TSW_LU_MAIN_CTRL_BYP_EN_MASK;
}

void tsw_set_cam_vlan_port(TSW_Type *ptr)
{
    ptr->APB2AXI_CAM_REQDATA_1 = 0x0f;
    ptr->APB2AXI_CAM_REQDATA_0 = (1 << 16)  /* VID = 1 */
                               | (1 << 8)   /* 1: Set one VLAN_PORT entry */
                               | (1 << 0);  /* CAM APB2AXIS channel selection. Always 1 for writing to VLAN_PORT table. */

}

/**
 *  speed: 00 1000mbps, 11 100mbps, 10 10mpbs
 *  itf: 000 MII; 001 RGMII; 100 RMII
 */
void tsw_port_gpr(TSW_Type *ptr, uint8_t port, uint8_t speed, uint8_t itf, uint8_t tx_dly, uint8_t rx_dly)
{
    ptr->TSNPORT[port].GPR_CTRL0 = TSW_TSNPORT_GPR_CTRL0_RXCLK_DLY_SEL_SET(rx_dly) | TSW_TSNPORT_GPR_CTRL0_TXCLK_DLY_SEL_SET(tx_dly);
    ptr->TSNPORT[port].GPR_CTRL2 = TSW_TSNPORT_GPR_CTRL2_MAC_SPEED_SET(speed) | TSW_TSNPORT_GPR_CTRL2_PHY_INTF_SEL_SET(itf);
}

void tsw_set_port_clock_delay(TSW_Type *ptr, uint8_t port, uint8_t tx_dly, uint8_t rx_dly)
{
    ptr->TSNPORT[port].GPR_CTRL0 &= ~(TSW_TSNPORT_GPR_CTRL0_TXCLK_DLY_SEL_MASK | TSW_TSNPORT_GPR_CTRL0_RXCLK_DLY_SEL_MASK);
    ptr->TSNPORT[port].GPR_CTRL0 |= TSW_TSNPORT_GPR_CTRL0_TXCLK_DLY_SEL_SET(tx_dly) | TSW_TSNPORT_GPR_CTRL0_RXCLK_DLY_SEL_SET(rx_dly);
}

void tsw_set_port_interface(TSW_Type *ptr, uint8_t port, uint8_t itf)
{
    ptr->TSNPORT[port].GPR_CTRL2 &= ~TSW_TSNPORT_GPR_CTRL2_PHY_INTF_SEL_MASK;
    ptr->TSNPORT[port].GPR_CTRL2 |= TSW_TSNPORT_GPR_CTRL2_PHY_INTF_SEL_SET(itf);
}

void tsw_set_port_speed(TSW_Type *ptr, uint8_t port, uint8_t speed)
{
    ptr->TSNPORT[port].GPR_CTRL2 &= ~TSW_TSNPORT_GPR_CTRL2_MAC_SPEED_MASK;
    ptr->TSNPORT[port].GPR_CTRL2 |= TSW_TSNPORT_GPR_CTRL2_MAC_SPEED_SET(speed);
}

void tsw_set_lookup_table(TSW_Type *ptr, uint16_t entry_num, uint8_t dest_port, uint64_t dest_mac)
{
    uint64_t dest_mac_temp;

    /* Create a new ALMEM entry. This will specify what will be done with those detected frames */
    if (TSW_APB2AXIS_ALMEM_STS_RDY_GET(ptr->APB2AXIS_ALMEM_STS)) {

        ptr->APB2AXIS_ALMEM_REQDATA_1 = TSW_APB2AXIS_ALMEM_REQDATA_1_WR_NRD_SET(1) | TSW_APB2AXIS_ALMEM_REQDATA_1_ENTRY_NUM_SET(entry_num);

        /* set forward to destination port, use PCP field, UTAG 1 and trigger the interface for sending the data */
        ptr->APB2AXIS_ALMEM_REQDATA_0 = TSW_APB2AXIS_ALMEM_REQDATA_0_UTAG_SET(1) |
                                        TSW_APB2AXIS_ALMEM_REQDATA_0_QSEL_SET(0) |
                                        TSW_APB2AXIS_ALMEM_REQDATA_0_DROP_SET(0) |
                                        TSW_APB2AXIS_ALMEM_REQDATA_0_QUEUE_SET(0) |
                                        TSW_APB2AXIS_ALMEM_REQDATA_0_DEST_SET(dest_port);
    }

    /* Create a new CAM entry */
    dest_mac_temp = __bswapdi2(dest_mac) >> 16;

    ptr->APB2AXI_CAM_REQDATA_2 = TSW_APB2AXI_CAM_REQDATA_2_VID_SET(1) | TSW_APB2AXI_CAM_REQDATA_2_DESTMAC_HI_SET((dest_mac_temp >> 32));
    ptr->APB2AXI_CAM_REQDATA_1 = TSW_APB2AXI_CAM_REQDATA_1_DESTMAC_LO_PORT_VEC_SET(dest_mac_temp);
    ptr->APB2AXI_CAM_REQDATA_0 = TSW_APB2AXI_CAM_REQDATA_0_ENTRY_NUM_SET(entry_num) |
                                 TSW_APB2AXI_CAM_REQDATA_0_TYPE_SET(1) |   /* Set one DEST_MAC/VLAN_ID entry */
                                 TSW_APB2AXI_CAM_REQDATA_0_CH_SET(0);      /* CAM APB2AXIS channel selection. Always 0 for writing to DEST_MAC/VLAN_ID */


    /* Add a new VLAN_PORT entry (VID 1) */
    ptr->APB2AXI_CAM_REQDATA_1 = 0x0f;
    ptr->APB2AXI_CAM_REQDATA_0 = (1 << 16)  /* VID = 1 */
                               | (1 << 8)  /* 1: Set one VLAN_PORT entry */
                               | (1 << 0); /* CAM APB2AXIS channel selection. Always 1 for writing to VLAN_PORT table. */

}

void tsw_set_internal_frame_action(TSW_Type *ptr, uint8_t dest_port)
{
    ptr->LU_MAIN_INTF_ACTION &= ~TSW_LU_MAIN_INTF_ACTION_DEST_MASK;
    ptr->LU_MAIN_INTF_ACTION |= TSW_LU_MAIN_INTF_ACTION_DEST_SET(dest_port);
}

void tsw_set_broadcast_frame_action(TSW_Type *ptr, uint8_t dest_port)
{
    ptr->LU_MAIN_BC_ACTION &= ~TSW_LU_MAIN_BC_ACTION_DEST_MASK;
    ptr->LU_MAIN_BC_ACTION |= TSW_LU_MAIN_BC_ACTION_DEST_SET(dest_port);
}

void tsw_set_unknown_frame_action(TSW_Type *ptr, uint8_t dest_port)
{
    ptr->LU_MAIN_NN_ACTION &= ~TSW_LU_MAIN_NN_ACTION_DEST_MASK;
    ptr->LU_MAIN_NN_ACTION |= TSW_LU_MAIN_NN_ACTION_DEST_SET(dest_port);
}

void tsw_clear_cam(TSW_Type *ptr)
{
    ptr->LU_MAIN_HITMEM &= ~TSW_LU_MAIN_HITMEM_CAMMEMCLR_MASK;
    ptr->LU_MAIN_HITMEM |= TSW_LU_MAIN_HITMEM_CAMMEMCLR_MASK;
}

void tsw_enable_store_forward_mode(TSW_Type *ptr, uint8_t port)
{
    ptr->TSNPORT[port].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_OUT_CONFIG |= TSW_TSNPORT_RXFIFO_SW_CTRL_IGRESS_RX_FDFIFO_E_OUT_CONFIG_MODE_STORE_FW_MASK;
}

void tsw_disable_store_forward_mode(TSW_Type *ptr, uint8_t port)
{
    ptr->TSNPORT[port].RXFIFO[TSW_RXFIFO_E1].SW_CTRL_IGRESS_RX_FDFIFO_E_OUT_CONFIG &= ~TSW_TSNPORT_RXFIFO_SW_CTRL_IGRESS_RX_FDFIFO_E_OUT_CONFIG_MODE_STORE_FW_MASK;
}

hpm_stat_t tsw_get_rtc_time_increment(TSW_Type *ptr, uint32_t *increment)
{
    if (increment == NULL) {
        return status_invalid_argument;
    }

    *increment = ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_TIMER_INCR;

    return status_success;
}

hpm_stat_t tsw_set_rtc_time_increment(TSW_Type *ptr, uint32_t increment)
{
    ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_TIMER_INCR = increment;

    return status_success;
}

hpm_stat_t tsw_get_rtc_current_time(TSW_Type *ptr, uint32_t *sec, uint32_t *nsec)
{
    if (sec == NULL || nsec == NULL) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_NS = 0;
    *sec  = TSW_TSNPORT_RTC_CT_CURTIME_SEC_CT_SEC_GET(ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_SEC);
    *nsec = TSW_TSNPORT_RTC_CT_CURTIME_NS_CT_NS_GET(ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_CT_CURTIME_NS);

    return status_success;
}

hpm_stat_t tsw_get_rtc_offset(TSW_Type *ptr, int64_t *sec, uint32_t *nsec)
{
    if (sec == NULL || nsec == NULL) {
        return status_invalid_argument;
    }

    *nsec = TSW_TSNPORT_RTC_OFS_NS_OFS_NS_GET(ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_NS);
    *sec  = TSW_TSNPORT_RTC_OFS_SL_OFS_SL_GET(ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_SL)
         | ((int64_t)TSW_TSNPORT_RTC_OFS_SH_OFS_SH_GET(ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_SH) << 32LL);

    return status_success;
}

hpm_stat_t tsw_set_rtc_offset(TSW_Type *ptr, int64_t sec, uint32_t nsec)
{
    ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_NS = TSW_TSNPORT_RTC_OFS_NS_OFS_NS_SET(nsec);
    ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_SL = TSW_TSNPORT_RTC_OFS_SL_OFS_SL_SET((uint32_t)sec);

    return status_success;
}

hpm_stat_t tsw_set_rtc_offset_change(TSW_Type *ptr, uint32_t change)
{
    ptr->TSNPORT[TSW_TSNPORT_PORT1].RTC_OFS_CH = TSW_TSNPORT_RTC_OFS_CH_SFNS_SET(change);

    return status_success;
}

hpm_stat_t tsw_set_tsync_timer_hclkdiv(TSW_Type *ptr, uint8_t port, uint32_t host_clkdiv)
{
    ptr->TSNPORT[port].TSYN_HCLKDIV = TSW_TSNPORT_TSYN_HCLKDIV_PERIOD_SET(host_clkdiv);

    return status_success;
}

hpm_stat_t tsw_tsync_timer_control(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t period, uint32_t enable)
{
    if (index >= TSW_SOC_PTP_SYNC_TIMER_COUNT) {
        return status_invalid_argument;
    }

    if (enable) {
        /* set period */
        ptr->TSNPORT[port].TSYNTMR[index] = TSW_TSNPORT_TSYNTMR_PERIOD_SET(period);
        /* reset timer-done flag */
        ptr->TSNPORT[port].TSYN_SR |= (1 << index << TSW_TSNPORT_TSYN_SR_TMR_DN_SHIFT);
        /* disable auto-load */
        ptr->TSNPORT[port].TSYN_CR &= ~(1 << index << TSW_TSNPORT_TSYN_CR_TMR_ALD_SHIFT);
        /* enable timer-en */
        ptr->TSNPORT[port].TSYN_CR |= (1 << index << TSW_TSNPORT_TSYN_CR_TMR_EN_SHIFT);
    } else {
        /* disable timer-en */
        ptr->TSNPORT[port].TSYN_CR &= ~(1 << index << TSW_TSNPORT_TSYN_CR_TMR_EN_SHIFT);
        /* reset SR.tmr_done-flag */
        ptr->TSNPORT[port].TSYN_SR |= (1 << index << TSW_TSNPORT_TSYN_SR_TMR_DN_SHIFT);
    }

    return status_success;
}

hpm_stat_t tsw_tsync_timer_interrupt_enable(TSW_Type *ptr, uint8_t port)
{
    ptr->TSNPORT[port].TSYN_CR |= TSW_TSNPORT_TSYN_CR_TMRIE_MASK;

    return status_success;
}

hpm_stat_t tsw_tsync_update_data(TSW_Type *ptr, uint8_t port, uint32_t bin, uint32_t binofs, uint32_t srcaddr, uint8_t lenbytes)
{
    uint32_t *dst;
    uint32_t *src;
    uint32_t index;
    uint32_t wordcount;

    /* check bin */
    if (bin >= TSW_SOC_PTP_BIN_COUNT) {
        return status_invalid_argument;
    }

    /* check binoffset, must be aligned to 32 bits */
    if (binofs & (TSW_SOC_DATA_BUS_WIDTH - 1)) {
        return status_invalid_argument;
    }

    if (binofs >= TSW_SOC_PTP_BIN_SIZE) {
        return status_invalid_argument;
    }

    /* calc destination pointer, set source pointer */
    dst = (uint32_t *)(ptr->TSNPORT[port].BIN[bin].TXDATA[binofs / TSW_SOC_DATA_BUS_WIDTH]);
    src = (uint32_t *)srcaddr;

    /* copy data */
    wordcount = (lenbytes + (TSW_SOC_DATA_BUS_WIDTH - 1)) / TSW_SOC_DATA_BUS_WIDTH;

    for (index = 0; index < wordcount; index++) {
        dst[index] = src[index];
    }

    return status_success;
}

hpm_stat_t tsw_tsync_update_len(TSW_Type *ptr, uint8_t port, uint32_t bin, uint8_t lenbytes, uint8_t tqueue)
{
    /* check bin */
    if (bin >= TSW_SOC_PTP_BIN_COUNT) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[port].BIN[bin].TSYN_TXBUF_BIN0_TQUE_AND_TX_LEN = TSW_TSNPORT_BIN_TSYN_TXBUF_BIN0_TQUE_AND_TX_LEN_TXBUF_BIN0_TQUE_SET(tqueue)
                                                                | TSW_TSNPORT_BIN_TSYN_TXBUF_BIN0_TQUE_AND_TX_LEN_TXBUF_BIN0_TX_LEN_SET(lenbytes);

    return status_success;
}

hpm_stat_t tsw_tsync_trigger_tx(TSW_Type *ptr, uint8_t port, uint32_t bin)
{
    /* check bin */
    if (bin >= TSW_SOC_PTP_BIN_COUNT) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[port].TSYN_PTP_TX_TRIG |= (1 << bin << TSW_TSNPORT_TSYN_PTP_TX_TRIG_TRIG_SHIFT);

    return status_success;
}

hpm_stat_t tsw_tsync_get_txtimestamp(TSW_Type *ptr, uint8_t port, uint32_t bin, uint32_t *timestamplo, uint32_t *timestamphi)
{
    /* check bin */
    if (bin >= TSW_SOC_PTP_BIN_COUNT) {
        return status_invalid_argument;
    }

    if (timestamplo == NULL || timestamphi == NULL) {
        return status_invalid_argument;
    }

    *timestamplo = ptr->TSNPORT[port].BIN[bin].TSYN_TXBUF_BIN0_TX_TIMESTAMP_L;
    *timestamphi = ptr->TSNPORT[port].BIN[bin].TSYN_TXBUF_BIN0_TX_TIMESTAMP_H;

    return status_success;
}

hpm_stat_t tsw_tsync_get_rxtimestamp(TSW_Type *ptr, uint8_t port, uint32_t *timestamplo, uint32_t *timestamphi)
{
    if (timestamplo == NULL || timestamphi == NULL) {
        return status_invalid_argument;
    }

    *timestamplo = ptr->TSNPORT[port].TSYN_RXBUF_RX_TIME_STAMP_L;
    *timestamphi = ptr->TSNPORT[port].TSYN_RXBUF_RX_TIME_STAMP_H;

    return status_success;
}

hpm_stat_t tsw_tsync_disable_current_events(TSW_Type *ptr, uint8_t port, uint32_t *disabled)
{
    uint32_t rtemp, wtemp;

    if (disabled == NULL)  {
        return status_invalid_argument;
    }

    rtemp = ptr->TSNPORT[port].TSYN_SR;
    wtemp = ptr->TSNPORT[port].TSYN_CR;

    if (TSW_TSNPORT_TSYN_SR_TXIS_GET(rtemp)) {
        wtemp &= ~TSW_TSNPORT_TSYN_CR_TXIE_MASK;
    }

    if (TSW_TSNPORT_TSYN_SR_RXIS_GET(rtemp)) {
        wtemp &= ~TSW_TSNPORT_TSYN_CR_RXIE_MASK;
    }

    if (TSW_TSNPORT_TSYN_SR_TMRIS_GET(rtemp)) {
        wtemp &= ~TSW_TSNPORT_TSYN_CR_TMRIE_MASK;
    }

    ptr->TSNPORT[port].TSYN_CR = wtemp;

    *disabled = TSW_TSNPORT_TSYN_CR_TMRIE_GET(rtemp) | TSW_TSNPORT_TSYN_CR_RXIE_GET(rtemp) | TSW_TSNPORT_TSYN_CR_TXIE_GET(rtemp);

    return status_success;
}

hpm_stat_t tsw_tsync_get_rxstatus(TSW_Type *ptr, uint8_t port, uint8_t *ov, uint8_t *avnxt, uint8_t *rxsel)
{
    uint32_t temp;

    if (ov == NULL || avnxt == NULL || rxsel == NULL) {
        return status_invalid_argument;
    }

    temp = ptr->TSNPORT[port].TSYN_PTP_RX_STS;

    *ov    = TSW_TSNPORT_TSYN_PTP_RX_STS_OV_GET(temp);
    *avnxt = TSW_TSNPORT_TSYN_PTP_RX_STS_AV_NXT_GET(temp);
    *rxsel = TSW_TSNPORT_TSYN_PTP_RX_STS_RX_SEL_GET(temp);

    return status_success;
}

hpm_stat_t tsw_tsync_next_rxbuffer(TSW_Type *ptr, uint8_t port)
{
    ptr->TSNPORT[port].TSYN_PTP_RX_STS |= TSW_TSNPORT_TSYN_PTP_RX_STS_AV_NXT_MASK;

    return status_success;
}

hpm_stat_t tsw_tsync_clear_overflow(TSW_Type *ptr, uint8_t port)
{
    ptr->TSNPORT[port].TSYN_PTP_RX_STS |= TSW_TSNPORT_TSYN_PTP_RX_STS_OV_MASK;

    return status_success;
}

hpm_stat_t tsw_tsync_get_txdone(TSW_Type *ptr, uint8_t port, uint32_t *done)
{
    if (done == NULL) {
        return status_invalid_argument;
    }

    *done = TSW_TSNPORT_TSYN_PTP_TX_DONE_DONE_GET(ptr->TSNPORT[port].TSYN_PTP_TX_DONE);

    return status_success;
}

hpm_stat_t tsw_tsync_clear_txdone(TSW_Type *ptr, uint8_t port, uint32_t done)
{
    ptr->TSNPORT[port].TSYN_PTP_TX_DONE = TSW_TSNPORT_TSYN_PTP_TX_DONE_DONE_SET(done);

    return status_success;
}

hpm_stat_t tsw_tsync_get_tmrdone(TSW_Type *ptr, uint8_t port, uint32_t *done)
{
    if (done == NULL) {
        return status_invalid_argument;
    }

    *done = TSW_TSNPORT_TSYN_SR_TMR_DN_GET(ptr->TSNPORT[port].TSYN_SR);

    return status_success;
}

hpm_stat_t tsw_shap_set_tas_listlen(TSW_Type *ptr, uint8_t port, uint32_t listlen)
{
    if (listlen > TSW_SOC_SHAP_MAX_CL_ENTRIES) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[port].TSN_SHAPER_TAS_LISTLEN = TSW_TSNPORT_TSN_SHAPER_TAS_LISTLEN_ALISTLEN_SET(listlen);

    return status_success;
}

hpm_stat_t tsw_shap_get_tas_listlen(TSW_Type *ptr, uint8_t port, tsw_shap_tas_listlen_t *listlen)
{
    listlen->admin_list_length = TSW_TSNPORT_TSN_SHAPER_TAS_LISTLEN_ALISTLEN_GET(ptr->TSNPORT[port].TSN_SHAPER_TAS_LISTLEN);
    listlen->oper_list_length  = TSW_TSNPORT_TSN_SHAPER_TAS_LISTLEN_OLISTLEN_GET(ptr->TSNPORT[port].TSN_SHAPER_TAS_LISTLEN);

    return status_success;
}

hpm_stat_t tsw_shap_set_tas_cycletime(TSW_Type *ptr, uint8_t port, uint32_t cycle_time)
{
    ptr->TSNPORT[port].TSN_SHAPER_TAS_ACYCLETM  = TSW_TSNPORT_TSN_SHAPER_TAS_ACYCLETM_CTIME_SET(cycle_time);

    return status_success;
}

hpm_stat_t tsw_shap_set_tas_basetime(TSW_Type *ptr, uint8_t port, uint32_t basetime_sec, uint32_t basetime_ns)
{
    ptr->TSNPORT[port].TSN_SHAPER_TAS_ABASETM_L = TSW_TSNPORT_TSN_SHAPER_TAS_ABASETM_L_BASETM_L_SET(basetime_ns);
    ptr->TSNPORT[port].TSN_SHAPER_TAS_ABASETM_H = TSW_TSNPORT_TSN_SHAPER_TAS_ABASETM_H_BASETM_H_SET(basetime_sec);

    return status_success;
}

hpm_stat_t tsw_shap_set_tas_controllist(TSW_Type *ptr, uint8_t port, uint32_t index, tsw_tas_controllist_entry_t *entry)
{
    if (index >= TSW_SOC_SHAP_MAX_CL_ENTRIES) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[port].SHACL[index].TSN_SHAPER_ACLIST_ENTRY0_L = TSW_TSNPORT_SHACL_TSN_SHAPER_ACLIST_ENTRY0_L_OP_SET(entry->op)
                                                               | TSW_TSNPORT_SHACL_TSN_SHAPER_ACLIST_ENTRY0_L_STATE_SET(entry->state);
    ptr->TSNPORT[port].SHACL[index].TSN_SHAPER_ACLIST_ENTRY0_H = TSW_TSNPORT_SHACL_TSN_SHAPER_ACLIST_ENTRY0_H_TIME_SET(entry->interval);

    return status_success;
}

hpm_stat_t tsw_shap_get_tas_controllist(TSW_Type *ptr, uint8_t port, uint32_t index, tsw_tas_controllist_entry_t *entry)
{
    if (index >= TSW_SOC_SHAP_MAX_CL_ENTRIES) {
        return status_invalid_argument;
    }

    entry->op       = TSW_TSNPORT_SHACL_TSN_SHAPER_ACLIST_ENTRY0_L_OP_GET(ptr->TSNPORT[port].SHACL[index].TSN_SHAPER_ACLIST_ENTRY0_L);
    entry->state    = TSW_TSNPORT_SHACL_TSN_SHAPER_ACLIST_ENTRY0_L_STATE_GET(ptr->TSNPORT[port].SHACL[index].TSN_SHAPER_ACLIST_ENTRY0_L);
    entry->interval = TSW_TSNPORT_SHACL_TSN_SHAPER_ACLIST_ENTRY0_H_TIME_GET(ptr->TSNPORT[port].SHACL[index].TSN_SHAPER_ACLIST_ENTRY0_H);

    return status_success;
}

hpm_stat_t tsw_shap_set_tas_max_sdu_ticks(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t ticks)
{
    if (index >= TSW_SOC_SHAP_MAX_QUEUES) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[port].MXTK[index] = TSW_TSNPORT_MXTK_TICK_SET(ticks);

    return status_success;
}

hpm_stat_t tsw_shap_get_tas_max_sdu_ticks(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t *ticks)
{
    if (index >= TSW_SOC_SHAP_MAX_QUEUES) {
        return status_invalid_argument;
    }

    if (ticks == NULL) {
        return status_invalid_argument;
    }

    *ticks = TSW_TSNPORT_MXTK_TICK_GET(ptr->TSNPORT[port].MXTK[index]);

    return status_success;
}

hpm_stat_t tsw_shap_get_tas_crsr(TSW_Type *ptr, uint8_t port, uint32_t *crsr)
{
     *crsr = ptr->TSNPORT[port].TSN_SHAPER_TAS_CRSR;

     return status_success;
}

hpm_stat_t tsw_shap_enable_tas(TSW_Type *ptr, uint8_t port)
{
    ptr->TSNPORT[port].TSN_SHAPER_TAS_CRSR |= TSW_TSNPORT_TSN_SHAPER_TAS_CRSR_EN_MASK;

    return status_success;
}

hpm_stat_t tsw_shap_disable_tas(TSW_Type *ptr, uint8_t port)
{
    ptr->TSNPORT[port].TSN_SHAPER_TAS_CRSR &= ~TSW_TSNPORT_TSN_SHAPER_TAS_CRSR_EN_MASK;

    return status_success;
}

hpm_stat_t tsw_shap_tas_change_config(TSW_Type *ptr, uint8_t port)
{
    ptr->TSNPORT[port].TSN_SHAPER_TAS_CRSR |= TSW_TSNPORT_TSN_SHAPER_TAS_CRSR_CFGCHG_MASK;

    return status_success;
}

hpm_stat_t tsw_set_pps_tod_output(TSW_Type *ptr)
{
    ptr->PTP_EVT_PPS_SEL &= ~TSW_PTP_EVT_PPS_SEL_PPS0_SEL_MASK;
    ptr->PTP_EVT_PPS_SEL |= TSW_PTP_EVT_PPS_SEL_PPS0_SEL_SET(0x10);

    return status_success;
}

hpm_stat_t tsw_shap_set_tas(TSW_Type *ptr, uint8_t port, tsw_tas_config_t *config)
{
    tsw_tas_controllist_entry_t entry;

    if (config->entry_count == 0 || config->entry_count > TSW_SOC_SHAP_MAX_CL_ENTRIES) {
        return status_invalid_argument;
    }

    for (uint32_t idx = 0; idx < config->entry_count; idx++, config->entry++) {
        entry.op        = config->entry->op;
        entry.state     = config->entry->state;
        entry.interval  = tsw_ns_to_systicks(config->entry->interval, TSW_BUS_FREQ);
        tsw_shap_set_tas_controllist(ptr, port, idx, &entry);
    }

    tsw_shap_set_tas_listlen(ptr, port, config->entry_count);
    tsw_shap_set_tas_cycletime(ptr, port, config->cycle_time); /* cycle length in nanoseconds */
    tsw_shap_set_tas_basetime(ptr, port, config->base_time_sec, config->base_time_ns);
    tsw_shap_tas_change_config(ptr, port);

    return status_success;
}

hpm_stat_t tsw_shap_get_tas_txov(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t *txov)
{
    *txov = ptr->TSNPORT[port].TXOV[index];

    return status_success;
}

hpm_stat_t tsw_shap_set_cbs(TSW_Type *ptr, uint8_t port, uint8_t index, tsw_cbs_config_t *config)
{
    if ((config->integer == 0) && (config->fract == 0)) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[port].TXSEL[index] &= ~TSW_TSNPORT_TXSEL_CBS_EN_MASK;
    ptr->TSNPORT[port].IDSEL[index] = TSW_TSNPORT_IDSEL_INT_SET(config->integer) | TSW_TSNPORT_IDSEL_FRACT_SET(config->fract);
    ptr->TSNPORT[port].TXSEL[index] |= TSW_TSNPORT_TXSEL_CBS_EN_MASK;

    return status_success;
}

hpm_stat_t tsw_shap_set_cbs_max_sdu_size(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t size)
{
     if (index >= TSW_SOC_SHAP_MAX_QUEUES) {
        return status_invalid_argument;
    }

    ptr->TSNPORT[port].MXSDU[index] = TSW_TSNPORT_MXSDU_SDU_SET(size);

    return status_success;
}

hpm_stat_t tsw_shap_get_cbs_max_sdu_size(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t *size)
{
     if (index >= TSW_SOC_SHAP_MAX_QUEUES) {
        return status_invalid_argument;
    }

    *size = TSW_TSNPORT_MXSDU_SDU_GET(ptr->TSNPORT[port].MXSDU[index]);

    return status_success;
}

hpm_stat_t tsw_get_txtimestampfifo_used(TSW_Type *ptr, uint8_t port, uint32_t *count)
{
      if (count == NULL) {
          return status_invalid_argument;
      }

      *count = TSW_TSNPORT_TSN_EP_TSF_SR_TSF_USED_GET(ptr->TSNPORT[port].TSN_EP_TSF_SR);

      return status_success;
}

hpm_stat_t tsw_get_txtimestampfifo_entry(TSW_Type *ptr, uint8_t port, tsw_tsf_t *entry)
{
    uint32_t temp;

    entry->tstamplo = ptr->TSNPORT[port].TSN_EP_TSF_D0;
    entry->tstamphi = ptr->TSNPORT[port].TSN_EP_TSF_D1;
    temp = ptr->TSNPORT[port].TSN_EP_TSF_D2;
    entry->tuser = TSW_TSNPORT_TSN_EP_TSF_D2_TSF_USR_GET(temp);
    entry->tqueue = TSW_TSNPORT_TSN_EP_TSF_D2_TSF_TQ_GET(temp);

    return status_success;
}