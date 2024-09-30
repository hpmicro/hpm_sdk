/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file    tsn_ctrl_ll.h
 * \author  zimmerli
 * \date    2019-01-11
 * \brief   TSN_CTRL Low Level Driver (Header)
 *
 */
#ifndef TSN_CTRL_LL_H_
#define TSN_CTRL_LL_H_

#include <stdint.h>
#include "tsn_hal_port.h"

// ----------------------------------------------------------------
// general
#define TSN_LL_OK   0 //!< low-level function return value: ok
#define TSN_LL_FAIL 1 //!< low-level function return value: general error

#define TSN_OFS_EMAC  0x0000 //!< address offset of eMAC registers
#define TSN_OFS_PMAC  0x0200 //!< address offset of pMAC registers
#define TSN_OFS_RTC   0x0800 //!< address offset of RTC registers
#define TSN_OFS_TSYNC 0x1000 //!< address offset of TSYNC registers
#define TSN_OFS_PTPRX 0x1600 //!< address offset of ptp rx buffer
#define TSN_OFS_PTPTX 0x1800 //!< address offset of ptp tx buffer
#define TSN_OFS_SHAP  0x2000 //!< address offset of traffic shaper registers
#define TSN_OFS_CORE  0xF000 //!< address offset of core registers

// TSN header, size in Bytes
#define TSN_HDR_SIZE         16
#define TSN_HDR_TSTAMP_SEQNO 4
#define TSN_HDR_TSTAMP_HI    8
#define TSN_HDR_TSTAMP_LO    12

// ----------------------------------------------------------------
// mac
/**
 * \brief MAC register addresses, relative to TSN_OFS_MAC
 */
enum tsn_mac_register {
    MAC_VER = 0x00,
    MAC_ADDR_L = 0x04,
    MAC_ADDR_H = 0x08,
    MAC_CTRL = 0x0C,
    MAC_TXFRAMES = 0x10,
    MAC_RXFRAMES = 0x14,
    MAC_TXOCTETS = 0x18,
    MAC_RXOCTETS = 0x1C,
    MAC_MDIO_CFG = 0x20,
    MAC_MDIO_CTRL = 0x24,
    MAC_MDIO_RD = 0x28,
    MAC_MDIO_WR = 0x2C,
    MAC_INT = 0x30
};

//#define OFS_MAC_CTRL_CLKSEL     8   //!< bit position in register MAC_CTRL
#define CLKSEL_MII       0
#define CLKSEL_REFCLK    1
#define CLKSEL_REF_BY_5  2
#define CLKSEL_REF_BY_10 3
#define CLKSEL_REF_BY_50 4

/**
 * \brief bit masks in register MAC_MDIO_CFG
 */
enum tsn_mac_mdio_cfg {
    MAC_MDIO_CFG_CLKDIV = (0xFF << 0),
    MAC_MDIO_CFG_EN = (1 << 8),
    MAC_MDIO_CFG_NPRE = (1 << 15)
};

#define OFS_MAC_MDIO_CTRL_REGAD 16 //!< bit position in register MAC_MDIO_CTRL
#define OFS_MAC_MDIO_CTRL_PHYAD 24 //!< bit position in register MAC_MDIO_CTRL

/**
 * \brief bit masks in register MAC_MDIO_CTRL
 */
enum tsn_mac_mdio_ctrl {
    MAC_MDIO_CTRL_RDY = (1 << 0),
    MAC_MDIO_CTRL_INIT = (1 << 8),
    MAC_MDIO_CTRL_OP_WR = (1 << 30),
    MAC_MDIO_CTRL_OP_RD = (2 << 30)
};

// ----------------------------------------------------------------
// TimeSync
/**
 * \brief TSYNC register addresses, relative to TSN_OFS_TSYNC
 */
enum tsn_tsync_register {
    TSYNC_VER = 0x00,
    TSYNC_CR = 0x04,
    TSYNC_SR = 0x08,
    TSYNC_TX_STS = 0x10,
    TSYNC_TX_DONE = 0x14,
    TSYNC_TX_TRIG = 0x18,
    TSYNC_RX_STS = 0x1C,
    TSYNC_TMR0 = 0x20,
    TSYNC_HCLKDIV = 0x3C
};

#define TSYNC_TMR_COUNT 5                    //!< implemented number of timers in TSYNC submodule
#define TSYNC_TMR_MASK  (0x1F)               //!< TSYNC timer bit mask
#define TSYNC_TMR(i)    (TSYNC_TMR0 + 4 * i) //!< TSYNC timer N register address macro

/**
 * \brief bit masks in register TSYNC_CR
 */
enum tsn_tsync_cr {
    TSYNC_CR_TXIE = (1 << 0),
    TSYNC_CR_RXIE = (1 << 1),
    TSYNC_CR_TMRIE = (1 << 2)
};

#define TSYNC_CR_TMR_EN_OFS  8  //!< register TSYNC_CR, bit offset
#define TSYNC_CR_TMR_ALD_OFS 16 //!< register TSYNC_CR, bit offset

/**
 * \brief bit masks in register TSYNC_SR
 */
enum tsn_tsync_sr {
    TSYNC_SR_TXIS = (1 << 0),
    TSYNC_SR_RXIS = (1 << 1),
    TSYNC_SR_TMRIS = (1 << 2)
};

#define TSYNC_SR_TMR_DN_OFS 8 //!< register TSYNC_SR, bit offset

/**
 * \brief bit masks in register TSYNC_PTP_RX_STS
 */
enum tsn_tsync_rx_sts {
    TSYNC_RXSTS_RXSEL_MSK = (7 << 0),
    TSYNC_RXSTS_AVNXT = (1 << 30),
    TSYNC_RXSTS_OV = (1 << 31)
};

#define PTP_TX_OFS_LEN_TQ    0xF0 //!< memory offset in PTP buffer, length of frame and traffic queue select
#define PTP_TX_OFS_TSTAMP_LO 0xF8 //!< memory offset in PTP buffer, timestamp (lo part)
#define PTP_TX_OFS_TSTAMP_HI 0xFC //!< memory offset in PTP buffer, timestamp (hi part)

#define TSN_PTP_BIN_SZ  0x100 //!< memory offset in PTP buffer, buffer size of single buffer
#define TSN_PTP_BIN_CNT 8     //!< memory offset in PTP buffer, buffer count

#define PTP_RX_OFS_LEN       0x00 //!< memory offset in PTP RX buffer, length of received frame
#define PTP_RX_OFS_TSTAMP_LO 0x08 //!< memory offset in PTP RX buffer, timestamp (lo part)
#define PTP_RX_OFS_TSTAMP_HI 0x0C //!< memory offset in PTP RX buffer, timestamp (hi part)
#define PTP_RX_OFS_DATA0     0x10 //!< memory offset in PTP RX buffer, start of frame data

// ----------------------------------------------------------------
// TrafficShaper

// shaper cbs porttxrate
// this is not the actual port transmit rate but the maximum transmit rate of the 32 bit axi stream,
// port width is constant (32 bit)
#define SHAP_CBS_PORTTXRATE (32 << 16) //!< implicit maximum porttransmitrate, sendslope = idleslope - porttransmitrate

#define SHAP_TAS_OFS        0x1000 //!< register address offset
#define SHAP_MAX_QUEUES     8      //!< maximum number of traffic queues
#define SHAP_MAX_CL_ENTRIES 256    //!< maximum number of control list entries

/**
 * \brief Traffic Shaper register addresses, relative to TSN_OFS_SHAP
 */
enum tsn_shap_register {
    SHAP_REG_HWCFG1 = 0x0004,
    SHAP_REG_TQAV = 0x000C,
    SHAP_REG_TQEM = 0x0010,
    SHAP_REG_FPST = 0x0014,
    SHAP_REG_MM_CTL = 0x0018,
    SHAP_REG_HOLDADV = 0x001C,
    SHAP_REG_MXSDU0 = 0x0100,
    SHAP_REG_TXSEL0 = 0x0120,
    SHAP_REG_IDSLP0 = 0x0140,
    SHAP_REG_TAS_CRSR = (SHAP_TAS_OFS + 0x0000),
    SHAP_REG_TAS_ACYCLETM = (SHAP_TAS_OFS + 0x0004),
    SHAP_REG_TAS_ABASETM_L = (SHAP_TAS_OFS + 0x0008),
    SHAP_REG_TAS_ABASETM_H = (SHAP_TAS_OFS + 0x000C),
    SHAP_REG_TAS_LISTLEN = (SHAP_TAS_OFS + 0x0010),
    SHAP_REG_TAS_OCYCLETM = (SHAP_TAS_OFS + 0x0014),
    SHAP_REG_TAS_OBASETM_L = (SHAP_TAS_OFS + 0x0018),
    SHAP_REG_TAS_OBASETM_H = (SHAP_TAS_OFS + 0x001C),
    SHAP_REG_TAS_MXTK0 = (SHAP_TAS_OFS + 0x0020),
    SHAP_REG_TAS_TXOV0 = (SHAP_TAS_OFS + 0x0040),
    SHAP_REG_TAS_ACLIST = (SHAP_TAS_OFS + 0x0800)
};

/**
 * \brief bit positions in register SHAP_REG_HWCFG1
 */
enum tsn_shap_register_hwcfg1 {
    SHAP_REG_HWCFG1_POS_LWIDTH = 24,
    SHAP_REG_HWCFG1_POS_TQC = 16,
    SHAP_REG_HWCFG1_POS_TQD = 8,
    SHAP_REG_HWCFG1_POS_DW = 0
};

/**
 * \brief bit masks in register SHAP_REG_TXSELn
 */
enum tsn_shap_register_txsel {
    SHAP_REG_TXSEL_BIT_CBS_EN = (1 << 0)
};

/**
 * \brief bit masks in register SHAP_REG_TAS_CRSR
 */
enum tsn_shap_register_tas_crsr {
    SHAP_REG_TAS_CRSR_BIT_EN = (1 << 0),
    SHAP_REG_TAS_CRSR_BIT_CFGCHG = (1 << 1),
    SHAP_REG_TAS_CRSR_BIT_CFGERR = (1 << 2),
    SHAP_REG_TAS_CRSR_BIT_CFGPEND = (1 << 3)
};

#define SHAP_REG_TAS_CRSR_OPERGS_MSK  0xFF
#define SHAP_REG_TAS_CRSR_OPERGS_SHI  16
#define SHAP_REG_TAS_CRSR_ADMINGS_MSK 0xFF
#define SHAP_REG_TAS_CRSR_ADMINGS_SHI 24

/**
 * \brief Time-aware-scheduler control list entry
 */
struct tas_controllist_entry_s {
    uint8_t gatestate; //!< gate state vector
    uint8_t cmd;       //!< command (set, set-and-hold, set-and-release)
    uint32_t interval; //!< interval time in <sys_clk> ticks
};

void tsn_shap_config_traffic_queue(addr_t baseaddr, uint8_t trafficqueue, uint16_t sduwords, uint8_t cbs_en, uint32_t idleslope);
uint32_t tsn_shap_config_tas_listlen(addr_t baseaddr);
void tsn_shap_config_tas_cycle(addr_t baseaddr, uint32_t cycletime, uint32_t controllistlength, uint32_t bt_sec, uint32_t bt_ns);
void tsn_shap_config_tas_controllist(addr_t baseaddr, uint32_t index, struct tas_controllist_entry_s *entry);
void tsn_shap_config_tas_controllist_get(addr_t baseaddr, uint32_t index, struct tas_controllist_entry_s *entry);
void tsn_shap_config_tas_max_sdu_ticks(addr_t baseaddr, uint8_t index, uint32_t ticks);
void tsn_shap_get_tas_max_sdu_ticks(addr_t baseaddr, uint8_t index, uint32_t *ticks);

// ----------------------------------------------------------------
// core
/**
 * \brief TSN_CTRL core register addresses, relative to TSN_OFS_CORE
 */
enum tsn_core_register {
    CORE_VER = 0x00,
    CORE_CTRL = 0x04,
    CORE_TXUF = 0x10,
    CORE_IPCFG = 0x14,
    CORE_TSF_D0 = 0x20,
    CORE_TSF_D1 = 0x24,
    CORE_TSF_D2 = 0x28,
    CORE_TSF_SR = 0x2C,
    CORE_MM_CTRL = 0x30,
    CORE_MM_STS = 0x34,
    CORE_MM_VTIME = 0x38,
    CORE_MM_STAT = 0x3C
};

/**
 * \brief Tx-Timestamp-Fifo Entry, see registers TSF_D0 - TSF_D2
 */
typedef struct {
    uint32_t tstamplo;
    uint32_t tstamphi;
    uint8_t tqueue;
    uint8_t tuser;
} tsn_ctrl_tsf_t;

#define CORE_CTRL_IE_TSF    (1 << 0)
#define CORE_CTRL_RXFILTDIS (1 << 31)

#define CORE_IPCFG_INCL_RTC    (1 << 31) //!< register CORE_IPCFG, bit mask INCL_RTC
#define CORE_IPCFG_INCL_SHAPER (1 << 30) //!< register CORE_IPCFG, bit mask INCL_SHAPER
#define CORE_IPCFG_INCL_FPE    (1 << 29) //!< register CORE_IPCFG, bit mask INCL_FPE

#define CORE_TSFSR_MASK_USED (0xFF) //!< register CORE_TSF_SR, bit mask TSF_USED

enum tsn_core_mm_ctrl {
    CORE_MM_CTRL_EN = (1 << 0),
    CORE_MM_CTRL_LINK = (1 << 1),
    CORE_MM_CTRL_DISV = (1 << 2),
    CORE_MM_CTRL_FRAGSZ_MSK = (3 << 3),
    CORE_MM_CTRL_STATSEL_MSK = (7 << 5),
};

enum tsn_core_mm_sts {
    CORE_MM_STS_HOLD = (1 << 0),
    CORE_MM_STS_VOK = (1 << 1),
    CORE_MM_STS_VFAIL = (1 << 2),
};

uint32_t tsn_ctrl_txtimestampfifo_used(uint32_t baseaddr);
void tsn_ctrl_txtimestampfifo_read(uint32_t baseaddr, tsn_ctrl_tsf_t *pEntry);

#endif /* TSN_CTRL_LL_H_ */
