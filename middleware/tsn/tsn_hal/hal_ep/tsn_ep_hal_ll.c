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

#include <hal_ep/tsn_ep_hal_ll.h>
#include <hal_common/tsn_hal_reg.h>
#include <hal_common/tsn_hal_utils.h>

static const char *EP_LL_RTC_CONTEXT = "RTC";
static const char *EP_LL_TSYNC_CONTEXT = "TSYNC";

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// RTC
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief RTC register addresses, relative to TSN_OFS_RTC
 */
enum tsn_rtc_register {
    RTC_CR = 0x00,
    RTC_SR = 0x04,
    RTC_CURTIME_NS = 0x10,
    RTC_CURTIME_SEC = 0x14,
    RTC_TIMER_INCR = 0x1C,
    RTC_RTC_OFS_NS = 0x20,
    RTC_RTC_OFS_SL = 0x24,
    RTC_RTC_OFS_SH = 0x28,
    RTC_RTC_OFS_CH = 0x2C,
    RTC_ALARM_NS = 0x30,
    RTC_ALARM_SL = 0x34,
    RTC_ALARM_SH = 0x38,
    RTC_TIMER_A_PER = 0x40
};

/**
 * Get timer increment value.
 *
 * @param baseaddr  base address of IPcore
 * @param lti	    local time increment
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_get_rtc_time_increment(uint32_t baseaddr, uint32_t *lti)
{
    if (lti != NULL)
        rreg(baseaddr + RTC_TIMER_INCR, lti); //, EP_LL_RTC_CONTEXT, "tmr_incr");
    return TSN_LL_RET_OK;
}

/**
 * Set timer increment value.
 *
 * @param baseaddr  base address of IPcore
 * @param lti	    local time increment
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_set_rtc_time_increment(uint32_t baseaddr, uint32_t lti)
{
    wregl(baseaddr + RTC_TIMER_INCR, &lti, EP_LL_RTC_CONTEXT, "tmr_incr");
    return TSN_LL_RET_OK;
}

/**
 * Get current time.
 *
 * @param baseaddr  base address of IPcore
 * @param sec		current time (second part)
 * @param nsec		current time (nanosecond part)
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_get_rtc_current_time(uint32_t baseaddr, int64_t *sec, uint32_t *nsec)
{
    uint32_t upd = 0;

    // trigger update of time
    wreg(baseaddr + RTC_CURTIME_NS, &upd);

    // read curtime
    *sec = 0;

    if (sec != NULL)
        rreg(baseaddr + RTC_CURTIME_SEC, sec); //, EP_LL_RTC_CONTEXT, "ct_sec");
    if (nsec != NULL)
        rreg(baseaddr + RTC_CURTIME_NS, nsec); //, EP_LL_RTC_CONTEXT, "ct_ns");

    return TSN_LL_RET_OK;
}

/**
 * Get real time offset.
 *
 * @param baseaddr  base address of IPcore
 * @param sec		real time offset (second part)
 * @param nsec		real time offset (nanosecond part)
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_get_rtc_offset(uint32_t baseaddr, int64_t *sec, uint32_t *nsec)
{
    if (nsec != NULL)
        rreg(baseaddr + RTC_RTC_OFS_NS, nsec); //, EP_LL_RTC_CONTEXT, "ofs_ns");

    if (sec != NULL) {
        int64_t tmp = 0;

        rreg(baseaddr + RTC_RTC_OFS_SH, &tmp); //, EP_LL_RTC_CONTEXT, "ofs_sh");
        *sec = tmp << 32LL;

        rreg(baseaddr + RTC_RTC_OFS_SL, &tmp); //, EP_LL_RTC_CONTEXT, "ofs_sl");
        *sec += tmp;
    }

    return TSN_LL_RET_OK;
}

/**
 * Set real time offset.
 *
 * @param baseaddr  base address of IPcore
 * @param sec		real time offset (second part)
 * @param nsec		real time offset (nanosecond part)
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_set_rtc_offset(uint32_t baseaddr, int64_t sec, uint32_t nsec)
{
    uint32_t tmp;

    wreg(baseaddr + RTC_RTC_OFS_NS, &nsec); //, EP_LL_RTC_CONTEXT, "ofs_ns");

    tmp = (uint32_t)(sec & 0xFFFFFFFF);
    wreg(baseaddr + RTC_RTC_OFS_SL, &tmp); //, EP_LL_RTC_CONTEXT, "ofs_sl");

    tmp = (uint32_t)(sec >> 32LL);
    wreg(baseaddr + RTC_RTC_OFS_SH, &tmp); //, EP_LL_RTC_CONTEXT, "ofs_sh");

    return TSN_LL_RET_OK;
}

/**
 * Set real time offset change.
 *
 * @param baseaddr  base address of IPcore
 * @param change	real time offset change
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_set_rtc_offset_change(uint32_t baseaddr, uint32_t change)
{
    wreg(baseaddr + RTC_RTC_OFS_CH, &change); //, EP_LL_RTC_CONTEXT, "ofs_ch");
    return TSN_LL_RET_OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// TimeSync
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// TimeSync register map (offsets)
enum tsn_tsync_register {
    TSYNC_VER = 0x00,
    TSYNC_CR = 0x04,
    TSYNC_SR = 0x08,
    TSYNC_TX_STS = 0x10,
    TSYNC_TX_DONE = 0x14,
    TSYNC_TX_TRIG = 0x18,
    TSYNC_RX_STS = 0x1C,
    TSYNC_TMR0 = 0x20,
    TSYNC_HCLKDIV = 0x3C,
    TSYNC_PTPRX = 0x600,
    TSYNC_PTPTX = 0x800
};

// TimeSync register map (bitfields)
typedef struct {
    uint32_t txie      : 1;  //!< Tx Interrupt Enable
    uint32_t rxie      : 1;  //!< Rx Interrupt Enable
    uint32_t tmrie     : 1;  //!< Timer Interrupt Enable
    uint32_t reserved1 : 5;  //!< reserved
    uint32_t tmr_en    : 5;  //!< Timer Enable: every bit corresponds to Timer 0 - 4
    uint32_t reserved2 : 3;  //!< reserved
    uint32_t tmr_ald   : 5;  //!< Timer Auto Load
    uint32_t reserved3 : 11; //!< reserved
} tsync_cr_t;

typedef struct {
    uint32_t txis      : 1;  //!< TX Buffer interrupt status
    uint32_t rxis      : 1;  //!< RX Buffer interrupt status
    uint32_t tmris     : 1;  //!< Timer Interrupt Status
    uint32_t reserved1 : 5;  //!< reserved
    uint32_t tmr_dn    : 5;  //!< Timer done flag (1 when timer reached 0)
    uint32_t reserved2 : 19; //!< reserved
} tsync_sr_t;

typedef struct {
    uint32_t rx_sel   : 3;  //!< RX Buffer selection
    uint32_t reserved : 27; //!< reserved
    uint32_t av_nxt   : 1;  //!< RX available, next buffer
    uint32_t ov       : 1;  //!< RX Buffer overflow
} tsync_ptp_rx_sts;

typedef struct {
    uint32_t txlen     : 8;  //!< TX Buffer length in bytes
    uint32_t reserved1 : 16; //!< reserved
    uint32_t tqueue    : 3;  //!< TX Buffer traffic queue
    uint32_t reserved2 : 5;  //!< reserved
} tsync_txbuf_qlen_t;

#define TSYNC_TMR(i)    (TSYNC_TMR0 + 4 * i) //!< TSYNC timer N register address macro
#define TSN_PTP_BIN_SZ  0x100                //!< memory offset in PTP buffer, buffer size of single buffer
#define TSN_PTP_BIN_CNT 8                    //!< memory offset in PTP buffer, buffer count

#define PTP_TX_OFS_LEN_TQ    0xF0 //!< memory offset in PTP buffer, length of frame and traffic queue select
#define PTP_TX_OFS_TSTAMP_LO 0xF8 //!< memory offset in PTP buffer, timestamp (lo part)
#define PTP_TX_OFS_TSTAMP_HI 0xFC //!< memory offset in PTP buffer, timestamp (hi part)
#define PTP_RX_OFS_LEN       0x00 //!< memory offset in PTP RX buffer, length of received frame
#define PTP_RX_OFS_TSTAMP_LO 0x08 //!< memory offset in PTP RX buffer, timestamp (lo part)
#define PTP_RX_OFS_TSTAMP_HI 0x0C //!< memory offset in PTP RX buffer, timestamp (hi part)
#define PTP_RX_OFS_DATA0     0x10 //!< memory offset in PTP RX buffer, start of frame data

//static /*const*/ char* TMR_REG_NAME[5] = {"TMR0", "TMR1", "TMR2", "TMR3", "TMR4"};

/**
 * Set PTP Host-Clock-Divider
 *
 * @param baseaddr  	base address of IPcore
 * @param host_clkdiv	clock divider in host-clock ticks (20 bit)
 * @return          	TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_set_tsync_timer_hclkdiv(uint32_t baseaddr, uint32_t host_clkdiv)
{
    host_clkdiv &= 0xFFFFF;
    wregl(baseaddr + TSYNC_HCLKDIV, &host_clkdiv, EP_LL_TSYNC_CONTEXT, "hclkdiv");
    return TSN_LL_RET_OK;
}

/**
 * Control PTP Timer
 *
 * @param baseaddr  base address of IPcore
 * @param idx       timer index (0 .. TSYNC_TMR_COUNT-1)
 * @param period    period in ticks (20 bit)
 * @param enable    enable flag (0: disable, otherwise enable)
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_timer_control(uint32_t baseaddr, uint8_t idx, uint32_t period, uint32_t enable)
{
    // check timer index
    if (idx >= TSYNC_TMR_COUNT)
        return TSN_LL_RET_FAIL;

    tsync_sr_t _sr = { 0 };
    tsync_cr_t _cr = { 0 };
    uint32_t _tmr = 1 << idx;

    if (enable) {
        wreg(baseaddr + TSYNC_TMR(idx), &period); //, EP_LL_TSYNC_CONTEXT, TMR_REG_NAME[idx]); 	// set default period

        _sr.tmr_dn = _tmr;               // reset SR.tmr_done-flag
        wreg(baseaddr + TSYNC_SR, &_sr); //, EP_LL_TSYNC_CONTEXT, "sr"); 						// wr SR

        rreg(baseaddr + TSYNC_CR, &_cr); //, EP_LL_TSYNC_CONTEXT, "cr");						// get CR

        _cr.tmr_ald &= ~_tmr;            // disable auto-load
        _cr.tmr_en |= _tmr;              // enable timer-en
        wreg(baseaddr + TSYNC_CR, &_cr); //, EP_LL_TSYNC_CONTEXT, "cr");						// wr CR
    } else {
        period = 1024;
        wreg(baseaddr + TSYNC_TMR(idx), &period); //, EP_LL_TSYNC_CONTEXT, TMR_REG_NAME[idx]);	// set default period

        rreg(baseaddr + TSYNC_CR, &_cr); //, EP_LL_TSYNC_CONTEXT, "cr");						// get CR

        _cr.tmr_en &= ~_tmr;             // disable timer-en
        wreg(baseaddr + TSYNC_CR, &_cr); //, EP_LL_TSYNC_CONTEXT, "cr");						// wr CR

        _sr.tmr_dn = _tmr;               // reset SR.tmr_done-flag
        wreg(baseaddr + TSYNC_SR, &_sr); //, EP_LL_TSYNC_CONTEXT, "sr"); 						// wr SR
    }

    return TSN_LL_RET_OK;
}

/**
 * Enable timer interrupts
 *
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_timer_interrupt_enable(uint32_t baseaddr)
{
    tsync_cr_t _cr = { 0 };

    rreg(baseaddr + TSYNC_CR, &_cr); //, EP_LL_TSYNC_CONTEXT, "cr"); // get CR
    _cr.txie = _cr.rxie = _cr.tmrie = 1;
    wreg(baseaddr + TSYNC_CR, &_cr); //, EP_LL_TSYNC_CONTEXT, "cr"); // wr CR

    return TSN_LL_RET_OK;
}

/**
 * Copies data to PTP TX buffer
 *
 * @param baseaddr  base address of IPcore
 * @param bin       select PTP TX buffer bin; (0 .. TSN_PTP_BIN_CNT-1)
 * @param binofs    offset in bin
 * @param srcaddr   source memory address
 * @param lenbytes  length in bytes
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_update_data(uint32_t baseaddr, uint32_t bin, uint32_t binofs, uint32_t srcaddr, uint8_t lenbytes)
{
    uint32_t *pDst;
    uint32_t *pSrc;
    uint32_t idx;
    uint32_t wordcount;

    // check bin
    if (bin >= TSN_PTP_BIN_CNT)
        return TSN_LL_RET_FAIL;

    // check binoffset, must be aligned to 32-bit
    if (binofs & 0x03)
        return TSN_LL_RET_FAIL;
    if (binofs >= TSN_PTP_BIN_SZ)
        return TSN_LL_RET_FAIL;

    // calc destination pointer, set source pointer
    pDst = (uint32_t *)(baseaddr + TSYNC_PTPTX + bin * TSN_PTP_BIN_SZ + binofs);
    pSrc = (uint32_t *)(srcaddr);

    // copy data
    wordcount = lenbytes;
    wordcount = (wordcount + 3) / 4;

    for (idx = 0; idx < wordcount; ++idx) {
        pDst[idx] = pSrc[idx];
    }

    return TSN_LL_RET_OK;
}

/**
 * Set length in bytes for PTP tx buffer
 *
 * @param baseaddr  base address of IPcore
 * @param bin       select PTP TX buffer bin; (0 .. TSN_PTP_BIN_CNT-1)
 * @param lenbytes  length in bytes
 * @param tqueue    selected tx traffic queue
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_update_len(uint32_t baseaddr, uint32_t bin, uint8_t lenbytes, uint8_t tqueue)
{
    tsync_txbuf_qlen_t _qlen = { 0 };
    _qlen.tqueue = tqueue;
    _qlen.txlen = lenbytes;

    wreg(baseaddr + TSYNC_PTPTX + bin * TSN_PTP_BIN_SZ + PTP_TX_OFS_LEN_TQ, &_qlen); //, EP_LL_TSYNC_CONTEXT, "txbuf (tqlen)");

    return TSN_LL_RET_OK;
}

/**
 * Trigger transmission of ptp buffer
 *
 * @param baseaddr  base address of IPcore
 * @param bin       select PTP TX buffer bin; (0 .. TSN_PTP_BIN_CNT-1)
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_trigger_tx(uint32_t baseaddr, uint32_t bin)
{
    // check bin
    if (bin >= TSN_PTP_BIN_CNT)
        return TSN_LL_RET_FAIL;

    uint32_t _bin;

    _bin = 1 << bin;

    wreg(baseaddr + TSYNC_TX_TRIG, &_bin); //, EP_LL_TSYNC_CONTEXT, "ptp_tx_trig");

    return TSN_LL_RET_OK;
}

/**
 * Get PTP TX buffer timestamp
 *
 * @param baseaddr      base address of IPcore
 * @param bin           select PTP TX buffer bin; (0 .. TSN_PTP_BIN_CNT-1)
 * @param pTimestampLo  pointer to variable holding low part of timestamp (usually nanoseconds part)
 * @param pTimestampHi  pointer to variable holding high part of timestamp (usually seconds part)
 * @return          	TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_get_txtimestamp(uint32_t baseaddr, uint32_t bin, uint32_t *pTimestampLo, uint32_t *pTimestampHi)
{
    // check bin
    if (bin >= TSN_PTP_BIN_CNT)
        return TSN_LL_RET_FAIL;

    if (pTimestampLo != NULL)
        rreg(baseaddr + TSYNC_PTPTX + bin * TSN_PTP_BIN_SZ + PTP_TX_OFS_TSTAMP_LO, pTimestampLo); //, EP_LL_TSYNC_CONTEXT, "txbuf (ts lo)");
    if (pTimestampHi != NULL)
        rreg(baseaddr + TSYNC_PTPTX + bin * TSN_PTP_BIN_SZ + PTP_TX_OFS_TSTAMP_HI, pTimestampHi); //, EP_LL_TSYNC_CONTEXT, "txbuf (ts hi)");

    return TSN_LL_RET_OK;
}

/**
 * Get PTP RX buffer timestamp
 *
 * @param baseaddr      base address of IPcore
 * @param pTimestampLo  pointer to variable holding low part of timestamp (usually nanoseconds part)
 * @param pTimestampHi  pointer to variable holding high part of timestamp (usually seconds part)
 * @return          	TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_get_rxtimestamp(uint32_t baseaddr, uint32_t *pTimestampLo, uint32_t *pTimestampHi)
{
    if (pTimestampLo != NULL)
        rreg(baseaddr + TSYNC_PTPRX + PTP_RX_OFS_TSTAMP_LO, pTimestampLo); //, EP_LL_TSYNC_CONTEXT, "rxbuf (ts lo)");
    if (pTimestampHi != NULL)
        rreg(baseaddr + TSYNC_PTPRX + PTP_RX_OFS_TSTAMP_HI, pTimestampHi); //, EP_LL_TSYNC_CONTEXT, "rxbuf (ts hi)");

    return TSN_LL_RET_OK;
}

/**
 * Disable current interrupt events
 *
 * @param baseaddr      base address of IPcore
 * @param any_disabled	returns 1 if TX Done interrupt or RX interrupt or timer Interrupt is disabled
 * @return          	TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_disable_current_events(uint32_t baseaddr, uint32_t *any_disabled)
{
    tsync_sr_t _sr = { 0 };
    tsync_cr_t _cr = { 0 };

    //TODO do not use [rw]regl, because this function is called from interrupt !?

    rreg(baseaddr + TSYNC_SR, &_sr); //, EP_LL_TSYNC_CONTEXT, "sr");
    rreg(baseaddr + TSYNC_CR, &_cr); //, EP_LL_TSYNC_CONTEXT, "cr");

    if (_sr.txis) {
        _cr.txie = 0;
    }
    if (_sr.rxis) {
        _cr.rxie = 0;
    }
    if (_sr.tmris) {
        _cr.tmrie = 0;
    }

    wreg(baseaddr + TSYNC_CR, &_cr); //, EP_LL_TSYNC_CONTEXT, "cr");

    if (any_disabled != NULL)
        *any_disabled = _sr.txis | _sr.rxis | _sr.tmris;

    return TSN_LL_RET_OK;
}

/**
 * Get PTP RX status
 *
 * @param baseaddr  base address of IPcore
 * @param ov		returns 1 if fifo overflow flag is set
 * @param avnxt		returns 1 if buffer data are available
 * @param rxsel		returns current selected RX buffer for reading (0-7)
 * @return          TSN_LL_RET_OK if successful; TSN_LL_RET_FAIL if failed
 */
int32_t tsn_ep_tsync_get_rxstatus(uint32_t baseaddr, uint8_t *ov, uint8_t *avnxt, uint8_t *rxsel)
{
    tsync_ptp_rx_sts _rx_sts = { 0 };

    rreg(baseaddr + TSYNC_RX_STS, &_rx_sts); //, EP_LL_TSYNC_CONTEXT, "ptp_rx_sts");

    if (ov != NULL)
        *ov = _rx_sts.ov;
    if (avnxt != NULL)
        *avnxt = _rx_sts.av_nxt;
    if (rxsel != NULL)
        *rxsel = _rx_sts.rx_sel;

    return TSN_LL_RET_OK;
}

int32_t tsn_ep_tsync_next_rxbuffer(uint32_t baseaddr)
{
    tsync_ptp_rx_sts _rx_sts = { 0 };
    _rx_sts.av_nxt = 1;

    wreg(baseaddr + TSYNC_RX_STS, &_rx_sts); //, EP_LL_TSYNC_CONTEXT, "ptp_rx_sts");

    return TSN_LL_RET_OK;
}

int32_t tsn_ep_tsync_clear_overflow(uint32_t baseaddr)
{
    tsync_ptp_rx_sts _rx_sts = { 0 };

    rreg(baseaddr + TSYNC_RX_STS, &_rx_sts); //, EP_LL_TSYNC_CONTEXT, "ptp_rx_sts");

    if (_rx_sts.ov) {
        TO_UINT32(_rx_sts) = 0;
        _rx_sts.ov = 1;
    }

    wreg(baseaddr + TSYNC_RX_STS, &_rx_sts); //, EP_LL_TSYNC_CONTEXT, "ptp_rx_sts");

    return TSN_LL_RET_OK;
}

int32_t tsn_ep_tsync_get_txdone(uint32_t baseaddr, uint32_t *done)
{
    if (done != NULL)
        rreg(baseaddr + TSYNC_TX_DONE, done); //, EP_LL_TSYNC_CONTEXT, "ptp_tx_done");

    return TSN_LL_RET_OK;
}

int32_t tsn_ep_tsync_set_txdone(uint32_t baseaddr, uint32_t done)
{
    wreg(baseaddr + TSYNC_TX_DONE, &done); //, EP_LL_TSYNC_CONTEXT, "ptp_tx_done");

    return TSN_LL_RET_OK;
}

int32_t tsn_ep_tsync_get_tmrdone(uint32_t baseaddr, uint32_t *done)
{
    if (done != NULL) {
        tsync_sr_t _sr = { 0 };

        rreg(baseaddr + TSYNC_SR, &_sr); //, EP_LL_TSYNC_CONTEXT, "sr");

        *done = _sr.tmr_dn;
    }

    return TSN_LL_RET_OK;
}
