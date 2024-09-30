/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file
 * \author zimmerli
 * \date   2020-01-15
 * \brief  TSN-EP (TSN_CTRL) driver
 *
 */

#include <kernel/base/kernel.h>

#if (USE_TSN_RTOS_DRIVER_IPMS_TSN == 1)

#include "tsn_ctrl_drv.h"
#include <kernel/base/klibc.h>

#include "tsn_ctrl_ll.h"

#include <hal_ep/tsn_ep_hal_ll.h>

//module driver
#include <kernel/net/netdev.h>
#include <kernel/net/ether.h>

void _tsn_irq_handler(struct tsn_ctrl_dev_s *tsn_dev);

void _rtc_init(uint32_t baseaddr, uint32_t busfreq);
void _rtc_get_local_time64(struct rtc_s *rtc, struct timespec64 *localtime);
void _rtc_get_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset);
void _rtc_set_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset);
void _rtc_adjfine(struct rtc_s *rtc, int64_t ppb);

void _shaper_init(uint32_t baseaddr);
void _tsync_init(uint32_t baseaddr, uint32_t busfreq);
void _tsync_interrupt_enable(uint32_t baseaddr);
uint32_t _tsync_cpy_ptprx(uint32_t baseaddr, uint32_t dstaddr);

int32_t _getCurrentTxDelay(struct tsn_ctrl_dev_s *tsn_dev);
int32_t _getCurrentRxDelay(struct tsn_ctrl_dev_s *tsn_dev);

void task_tsn_rx(void *pvParameters);
void task_tsn_tsync(void *pvParameters);

/**
 * \brief Tsync send netb
 *
 * @param netdev       	pointer to net device
 * @param netb          net buffer
 * @param tq            traffic queue number to be used
 * @param arg			argument for tx done callback
 * @return NETDEV_RESULT_OK or NETDEV_ERR
 */
int32_t tsn_ctrl_tsync_tx(struct netdev_s *netdev, struct netb_s *netb, uint8_t tq, void *arg)
{
    if (!netdev)
        return NETDEV_ERR_GENERAL;

    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)netdev->prv;

    uint8_t bin, len;
    len = netb_length(netb);

    if (xQueueReceive(prv->qTxPtp, &bin, 100) == pdFALSE) {
        return NETDEV_ERR_QFULL;
    }

    prv->ptpArg[bin] = arg;

    // set data, len
    tsn_ep_tsync_update_data(prv->baseaddr + TSN_OFS_TSYNC, bin, 0, (uint32_t)netb->data, len);
    tsn_ep_tsync_update_len(prv->baseaddr + TSN_OFS_TSYNC, bin, len, tq);
    tsn_ep_tsync_trigger_tx(prv->baseaddr + TSN_OFS_TSYNC, bin);

    return NETDEV_RESULT_OK;
}

/**
 * \brief Set ingress/egress latency for selected speed
 *
 * @param tsn_dev       pointer to device driver
 * @param speed         speed (100, 1000)
 * @param delay_tx      egress delay in nanoseconds
 * @param delay_rx      ingress delay in nanoseconds
 */
void tsn_ctrl_set_delay(struct tsn_ctrl_dev_s *tsn_dev, int speed, int32_t delay_tx, int32_t delay_rx)
{
    if (!tsn_dev)
        return;
    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)tsn_dev->prv;

    if (speed == 100) {
        prv->delay100.tx = delay_tx;
        prv->delay100.rx = delay_rx;
    } else if (speed == 1000) {
        prv->delay1000.tx = delay_tx;
        prv->delay1000.rx = delay_rx;
    }
}

/* IEEE 802.1Q: S. 199 - Table 8-5�Recommended priority to traffic class mappings
   Number of available traffic classes
PCP/   1 2 3 4 5 6 7 8
Prio
BE 0   0 0 0 0 0 1 1 1
BG 1   0 0 0 0 0 0 0 0
EE 2   0 0 0 1 1 2 2 2
CA 3   0 0 0 1 1 2 3 3
VI 4   0 1 1 2 2 3 4 4
VO 5   0 1 1 2 2 3 4 5
IC 6   0 1 2 3 3 4 5 6
NC 7   0 1 2 3 4 5 6 7
 */

//default_prio_pcp2queue_map[Number of available traffic classes(1-8) - 1][PCP/PRIO]
static uint8_t default_prio_pcp2queue_map[8][8] = {
    /*0, 1, 2, 3, 4, 5, 6, 7*/
    { 0, 0, 0, 0, 0, 0, 0, 0 }, /*1*/
    { 0, 0, 0, 0, 1, 1, 1, 1 }, /*2*/
    { 0, 0, 0, 0, 1, 1, 2, 2 }, /*3*/
    { 0, 0, 1, 1, 2, 2, 3, 3 }, /*4*/
    { 0, 0, 1, 1, 2, 2, 3, 4 }, /*5*/
    { 1, 0, 2, 2, 3, 3, 4, 5 }, /*6*/
    { 1, 0, 2, 3, 4, 4, 5, 6 }, /*7*/
    { 1, 0, 2, 3, 4, 5, 6, 7 }, /*8*/
};

void tsn_ctrl_set_prio2q_map(struct netdev_s *netdev)
{
    //OLD
    /*
     * TQ mapping
     * BE    -> queue=1
     * BG    -> queue=max queues - 1 (lowest prio)
     * other -> queue=0              (highest prio)
     */
    //    netdev->prio2tq[0 /*ETH_PRIO_BE*/] = 1;
    //    netdev->prio2tq[1 /*ETH_PRIO_BG*/] = netdev->hw_queues - 1;
    //    for (uint8_t tmp=2; tmp<8; ++tmp) netdev->prio2tq[tmp] = 0;
    uint8_t tqc = netdev->hw_queues;

    if (tqc == 0) /*error*/
        return;
    if (tqc > 8) /*hinweis*/
        tqc = 8;

    uint8_t lpq = tqc - 1; //first index of index default_prio_pcp2queue_map AND lowest priority queue

    //NOTE!!! the IP Core queue '0' is has the highest priority --> so we have to invert the table
    for (uint8_t prio = ETH_PRIO_BE; prio <= ETH_PRIO_NC; prio++) {
        netdev->prio2tq[prio] = lpq - default_prio_pcp2queue_map[lpq][prio];
    }
}

// ----------------------------------------------------------------
void _tsn_irq_handler(struct tsn_ctrl_dev_s *tsn_dev)
{
    uint32_t sr;
    uint32_t tsyncCr;
    uint32_t tsyncSr;
    BaseType_t higherPrioTaskWoken = pdFALSE;
    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)tsn_dev->prv;

    /* eMAC irq currently not checked */
    /* pMAC irq currently not checked */
    /* RTC irq currently not checked */

    /* TSYNC irq */
    sr = 0;
    tsyncSr = MEM32(prv->baseaddr + TSN_OFS_TSYNC + TSYNC_SR);
    tsyncCr = MEM32(prv->baseaddr + TSN_OFS_TSYNC + TSYNC_CR);
    if (tsyncSr & TSYNC_SR_TXIS) {
        sr |= 1;                   // set sr bit
        tsyncCr &= ~TSYNC_CR_TXIE; // disable int
    }
    if (tsyncSr & TSYNC_SR_RXIS) {
        sr |= 1;                   // set sr bit
        tsyncCr &= ~TSYNC_CR_RXIE; // disable int
    }
    if (tsyncSr & TSYNC_SR_TMRIS) {
        sr |= 1;                    // set sr bit
        tsyncCr &= ~TSYNC_CR_TMRIE; // disable int
    }
    MEM32(prv->baseaddr + TSN_OFS_TSYNC + TSYNC_CR) = tsyncCr;
    if (prv->hTaskTsync && sr) {
        xTaskNotifyFromISR(prv->hTaskTsync, sr, eSetBits, &higherPrioTaskWoken);
    }

    /* TSF irq currently not used - polling */

    // end of interrupt function
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

// ----------------------------------------------------------------
void _rtc_init(uint32_t baseaddr, uint32_t busfreq)
{
    uint32_t tmp;
    double nsPerClock;

    // set timer increment value
    nsPerClock = (1000000000.0 / ((double)busfreq));
    tmp = (uint32_t)(nsPerClock * 16777216.0);
    tsn_ep_set_rtc_time_increment(baseaddr + TSN_EP_OFS_RTC, tmp);
}

void _rtc_get_local_time64(struct rtc_s *rtc, struct timespec64 *localtime)
{
    addr_t baseaddr;
    baseaddr = (addr_t)(rtc->prv);
    tsn_ep_get_rtc_current_time(baseaddr + TSN_EP_OFS_RTC, &(localtime->tv_sec), &(localtime->tv_nsec));
}

void _rtc_get_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset)
{
    addr_t baseaddr;
    baseaddr = (addr_t)(rtc->prv);
    tsn_ep_get_rtc_offset(baseaddr + TSN_EP_OFS_RTC, &(offset->tv_sec), &(offset->tv_nsec));
}

void _rtc_set_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset)
{
    addr_t baseaddr;
    baseaddr = (addr_t)(rtc->prv);
    tsn_ep_set_rtc_offset(baseaddr + TSN_EP_OFS_RTC, offset->tv_sec, offset->tv_nsec);
}

void _rtc_adjfine(struct rtc_s *rtc, int64_t ppb)
{
    addr_t baseaddr;
    int64_t fnsPerClock;
    uint32_t tmp;

    baseaddr = (addr_t)(rtc->prv);
    tsn_ep_get_rtc_time_increment(baseaddr + TSN_OFS_RTC, &tmp);
    fnsPerClock = (int64_t)tmp;

    tmp = (int32_t)(fnsPerClock * ppb / 1000000000);
    tsn_ep_set_rtc_offset_change(baseaddr + TSN_EP_OFS_RTC, tmp);
}

void _tsync_init(uint32_t baseaddr, uint32_t busfreq)
{
    uint32_t clkdiv;
    uint32_t idx;

    // Tsync HostClockDivider --> config to 1/1024th second
    clkdiv = busfreq / 1024;
    tsn_ep_set_tsync_timer_hclkdiv(baseaddr + TSN_EP_OFS_TSYNC, clkdiv);

    // Tsync Timer (disabled)
    for (idx = 0; idx < TSYNC_TMR_COUNT; ++idx) {
        tsn_ep_tsync_timer_control(baseaddr + TSN_EP_OFS_TSYNC, idx, 1024, 0); // disable timer
    }
}

void _tsync_interrupt_enable(uint32_t baseaddr)
{
    uint32_t tmp;

    portENTER_CRITICAL();
    tmp = MEM32(baseaddr + TSN_OFS_TSYNC + TSYNC_CR);
    tmp |= (TSYNC_CR_TXIE | TSYNC_CR_RXIE | TSYNC_CR_TMRIE);
    MEM32(baseaddr + TSN_OFS_TSYNC + TSYNC_CR) = tmp;
    portEXIT_CRITICAL();
}

uint32_t _tsync_cpy_ptprx(uint32_t baseaddr, uint32_t dstaddr)
{
    uint32_t idx;
    uint32_t lenbytes;
    uint32_t words;

    if (!dstaddr)
        return 0;
    lenbytes = MEM32(baseaddr + TSN_OFS_PTPRX + PTP_RX_OFS_LEN) & 0xFFF;
    words = ((lenbytes + 3) / 4); // data length (includes header)
    if (words > (TSN_PTP_BIN_SZ / 4))
        words = TSN_PTP_BIN_SZ / 4;
    // copy all: hdr and data bytes
    for (idx = 0; idx < words; ++idx) {
        MEM32(dstaddr + idx * 4) = MEM32(baseaddr + TSN_OFS_PTPRX + idx * 4);
    }
    return lenbytes;
}

// ----------------------------------------------------------------
int32_t _getCurrentTxDelay(struct tsn_ctrl_dev_s *tsn_dev)
{
    int32_t delay = 0;
    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)tsn_dev->prv;

    if (tsn_dev->netdev->speed == 100)
        delay = prv->delay100.tx;
    else if (tsn_dev->netdev->speed == 1000)
        delay = prv->delay1000.tx;

    return delay;
}

int32_t _getCurrentRxDelay(struct tsn_ctrl_dev_s *tsn_dev)
{
    int32_t delay = 0;
    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)tsn_dev->prv;

    if (tsn_dev->netdev->speed == 100)
        delay = prv->delay100.rx;
    else if (tsn_dev->netdev->speed == 1000)
        delay = prv->delay1000.rx;

    return delay;
}

void _tsn_tsync_txdone(struct tsn_ctrl_dev_s *tsn, uint32_t baseaddr)
{
    uint32_t idx;
    uint32_t done;
    uint32_t localtm_lo, localtm_hi;
    int32_t delay;
    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)tsn->prv;

    // Get tx done
    done = MEM32(baseaddr + TSN_OFS_TSYNC + TSYNC_TX_DONE);
    if (!done)
        return;

    // process tx done
    for (idx = 0; idx < TSN_PTP_BIN_CNT; ++idx) {
        if ((done & (1 << idx)) && (tsn->netdev->tsync.cb_tx_done)) {
            tsn_ep_tsync_get_txtimestamp(baseaddr + TSN_OFS_TSYNC, idx, &localtm_lo, &localtm_hi);
            // delay compensation
            delay = _getCurrentTxDelay(tsn);
            localtm_lo += delay;
            while (localtm_lo >= NSEC_PER_SEC) {
                localtm_lo -= NSEC_PER_SEC;
                localtm_hi++;
            }

            tsn->netdev->tsync.cb_tx_done(tsn->netdev, prv->ptpArg[idx], localtm_lo, localtm_hi);
            xQueueSend(prv->qTxPtp, &idx, 0);
        }
    }
    MEM32(baseaddr + TSN_OFS_TSYNC + TSYNC_TX_DONE) = done; // reset processed tx done events
}

void _tsn_tsync_tmrdone(struct tsn_ctrl_dev_s *tsn, uint32_t baseaddr)
{
    uint32_t idx;
    uint32_t done;

    done = MEM32(baseaddr + TSN_OFS_TSYNC + TSYNC_SR); // read sr
    done &= (TSYNC_TMR_MASK << TSYNC_SR_TMR_DN_OFS);   // mask only tmr_dn bits
    done >>= TSYNC_SR_TMR_DN_OFS;

    // process timer events
    for (idx = 0; idx < TSYNC_TMR_COUNT; ++idx) {
        // process 5 timer events
        // 1:1 assignment from ptp_timer_event -> timer index of TSN.TSYNC
        if (done & (1 << idx)) {
            tsn_ep_tsync_timer_control(baseaddr + TSN_OFS_TSYNC, idx, 1024, 0); // disable current processed timer
            if (tsn->netdev->tsync.cb_timer_done)
                tsn->netdev->tsync.cb_timer_done(tsn->netdev, idx);
        }
    }
}

void _tsn_tsync_rxframe(struct tsn_ctrl_dev_s *tsn, addr_t baseaddr, void *rxbuffer)
{
    uint32_t localtm_lo, localtm_hi;
    uint32_t rxlen;
    int32_t delay;

    // PTP data available
    // copy to reserved buffer
    rxlen = _tsync_cpy_ptprx(baseaddr, (addr_t)rxbuffer);

    // get and adjust timestamp
    tsn_ep_tsync_get_rxtimestamp(baseaddr + TSN_OFS_TSYNC, &localtm_lo, &localtm_hi);
    delay = _getCurrentRxDelay(tsn);
    localtm_lo -= delay;
    if (localtm_lo > NSEC_PER_SEC) {
        // overflow
        localtm_lo += NSEC_PER_SEC;
        localtm_hi--;
    }
    // process
    if (tsn->netdev->tsync.cb_rx_frame)
        tsn->netdev->tsync.cb_rx_frame(tsn->netdev, (void *)((addr_t)rxbuffer + RX_HDR_SIZE), (rxlen - RX_HDR_SIZE), localtm_lo, localtm_hi);
}

void task_tsn_tsync(void *pvParameters)
{
    uint32_t tmp, sr;
    uint32_t rxsts, rxsel;
    void *rxbuffer;
    uint32_t ov;

    KERNEL_TASK_FPU();

    struct tsn_ctrl_dev_s *tsn = (struct tsn_ctrl_dev_s *)pvParameters;
    tsn_ep_prv_t *prv = (tsn_ep_prv_t *)tsn->prv;
    ov = 0;

    rxbuffer = kallocz(TSN_PTP_BIN_SZ);

    while (1) {
        /* TSYNC interrupt enable */
        _tsync_interrupt_enable(prv->baseaddr);

        /* process on notification */
        tmp = xTaskNotifyWait(0, 0xFFFFFFFF, &sr, 100);
        if (tmp == pdPASS) {
            /* process TSYNC status */

            // Process TX done
            _tsn_tsync_txdone(tsn, prv->baseaddr);

            // Process all RX frames
            rxsts = MEM32(prv->baseaddr + TSN_OFS_TSYNC + TSYNC_RX_STS);
            while (rxsts & TSYNC_RXSTS_AVNXT) {
                // tx done shall be 0 before any received frame is processed
                // avoid race condition of already received response and request-done not yet processed
                _tsn_tsync_txdone(tsn, prv->baseaddr);

                // process rx frame
                _tsn_tsync_rxframe(tsn, prv->baseaddr, rxbuffer);

                //
                // switch to next PTP data
                //
                rxsel = rxsts & TSYNC_RXSTS_RXSEL_MSK;
                // toggle next buffer
                MEM32(prv->baseaddr + TSN_OFS_TSYNC + TSYNC_RX_STS) = TSYNC_RXSTS_AVNXT;
                do {
                    rxsts = MEM32(prv->baseaddr + TSN_OFS_TSYNC + TSYNC_RX_STS); // handshake
                } while ((rxsel == (rxsts & TSYNC_RXSTS_RXSEL_MSK)));
            }

            // clear overflow flag if set
            rxsts = MEM32(prv->baseaddr + TSN_OFS_TSYNC + TSYNC_RX_STS);
            if (rxsts & TSYNC_RXSTS_OV) {
                MEM32(prv->baseaddr + TSN_OFS_TSYNC + TSYNC_RX_STS) = TSYNC_RXSTS_OV;
                ov++;
            }

            // Process Tmr Done
            _tsn_tsync_tmrdone(tsn, prv->baseaddr);
        }
    }
}

#endif /* USE_TSN_RTOS_DRIVER_IPMS_TSN */
