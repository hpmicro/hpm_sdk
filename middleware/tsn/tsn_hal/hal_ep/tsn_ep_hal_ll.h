/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file
 * \author fra11385
 * \date 06.07.2021
 * \brief <insert file brief description>
 *
 */

#ifndef TSN_EP_HAL_LL_H_
#define TSN_EP_HAL_LL_H_

#include <stdint.h>

#define TSN_EP_OFS_EMAC  0x0000 //!< address offset of eMAC registers
#define TSN_EP_OFS_PMAC  0x0200 //!< address offset of pMAC registers
#define TSN_EP_OFS_RTC   0x0800 //!< address offset of RTC registers
#define TSN_EP_OFS_TSYNC 0x1000 //!< address offset of TSYNC registers
#define TSN_EP_OFS_PTPRX 0x1600 //!< address offset of ptp rx buffer
#define TSN_EP_OFS_PTPTX 0x1800 //!< address offset of ptp tx buffer
#define TSN_EP_OFS_SHAP  0x2000 //!< address offset of traffic shaper registers
#define TSN_EP_OFS_CORE  0xF000 //!< address offset of core registers

#define TSN_EP_OFS_SHAP_TAS 0x1000 //!< address offset of tas register

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Module
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int32_t tsn_ep_get_module_reg(uint32_t baseaddr, void *data);
int32_t tsn_ep_set_module_reg(uint32_t baseaddr, void *data);
int32_t tsn_ep_module_reg(uint32_t baseaddr, void *data);

//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// [E/P] MAC
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//int32_t tsn_ep_get_version(uint32_t baseaddr, uint16_t *major, uint16_t *minor);

int32_t tsn_ep_get_mac_ctrl(uint32_t baseaddr, void *macctrl);
int32_t tsn_ep_set_mac_ctrl(uint32_t baseaddr, void *macctrl);

int32_t tsn_ep_set_mac_ctrl_enable(uint32_t baseaddr, uint8_t enable);

//int32_t tsn_ep_get_mac_addr(uint32_t baseaddr, uint8_t *macaddr, uint8_t *promisc);
int32_t tsn_ep_set_mac_addr(uint32_t baseaddr, uint8_t *macaddr, uint8_t promisc);

int32_t tsn_ep_get_mac_stats(uint32_t baseaddr, uint32_t *tx_frames, uint32_t *rx_frames, uint32_t *tx_octets, uint32_t *rx_octets);
int32_t tsn_ep_mac_reset(uint32_t baseaddr);

int32_t tsn_ep_set_mac_mode(uint32_t baseaddr, uint8_t gmii, uint8_t clksel);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// MDIO
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int32_t tsn_ep_set_mdio_cfg(uint32_t baseaddr, uint8_t mdc_clkdiv);
int32_t tsn_ep_mdio_rd(uint32_t baseaddr, uint32_t phyad, uint32_t regad, uint16_t *value);
int32_t tsn_ep_mdio_wr(uint32_t baseaddr, uint32_t phyad, uint32_t regad, uint16_t value);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// RTC
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int32_t tsn_ep_get_rtc_time_increment(uint32_t baseaddr, uint32_t *lti);
int32_t tsn_ep_set_rtc_time_increment(uint32_t baseaddr, uint32_t lti);
int32_t tsn_ep_get_rtc_current_time(uint32_t baseaddr, int64_t *sec, uint32_t *nsec);
int32_t tsn_ep_get_rtc_offset(uint32_t baseaddr, int64_t *sec, uint32_t *nsec);
int32_t tsn_ep_set_rtc_offset(uint32_t baseaddr, int64_t sec, uint32_t nsec);
int32_t tsn_ep_set_rtc_offset_change(uint32_t baseaddr, uint32_t change);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// TimeSync
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define TSYNC_TMR_COUNT 5 //!< implemented number of timers in TimeSync submodule

int32_t tsn_ep_set_tsync_timer_hclkdiv(uint32_t baseaddr, uint32_t host_clkdiv);
int32_t tsn_ep_tsync_timer_control(uint32_t baseaddr, uint8_t idx, uint32_t period, uint32_t enable);
int32_t tsn_ep_tsync_timer_interrupt_enable(uint32_t baseaddr);
int32_t tsn_ep_tsync_update_data(uint32_t baseaddr, uint32_t bin, uint32_t binofs, uint32_t srcaddr, uint8_t lenbytes);
int32_t tsn_ep_tsync_update_len(uint32_t baseaddr, uint32_t bin, uint8_t lenbytes, uint8_t tqueue);
int32_t tsn_ep_tsync_trigger_tx(uint32_t baseaddr, uint32_t bin);
int32_t tsn_ep_tsync_get_txtimestamp(uint32_t baseaddr, uint32_t bin, uint32_t *pTimestampLo, uint32_t *pTimestampHi);
int32_t tsn_ep_tsync_get_rxtimestamp(uint32_t baseaddr, uint32_t *pTimestampLo, uint32_t *pTimestampHi);

int32_t tsn_ep_tsync_disable_current_events(uint32_t baseaddr, uint32_t *any_disabled);
int32_t tsn_ep_tsync_get_rxstatus(uint32_t baseaddr, uint8_t *ov, uint8_t *avnxt, uint8_t *rxsel);
int32_t tsn_ep_tsync_next_rxbuffer(uint32_t baseaddr);
int32_t tsn_ep_tsync_clear_overflow(uint32_t baseaddr);

int32_t tsn_ep_tsync_get_txdone(uint32_t baseaddr, uint32_t *done);
int32_t tsn_ep_tsync_set_txdone(uint32_t baseaddr, uint32_t done);
int32_t tsn_ep_tsync_get_tmrdone(uint32_t baseaddr, uint32_t *done);

//int32_t tsn_ep_tsync_clear_txtimestamp(uint32_t baseaddr, uint32_t bin);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Traffic Shaper
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int32_t tsn_ep_get_shaper_hwcfg(uint32_t baseaddr, uint16_t *lwidth, uint8_t *tqc, uint8_t *tqd);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Core
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int32_t tsn_ep_get_core_version(uint32_t baseaddr, uint8_t *major, uint8_t *minor, uint8_t *rev);
int32_t tsn_ep_get_core_ipcfg(uint32_t baseaddr, uint8_t *fpe, uint8_t *shap, uint8_t *rtc);
int32_t tsn_ep_get_core_txuf(uint32_t baseaddr, uint32_t *txuf);

#endif /* TSN_EP_HAL_LL_H_ */
