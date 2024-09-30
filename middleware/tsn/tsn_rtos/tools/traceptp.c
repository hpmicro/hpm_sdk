/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   traceptp.c
 * \author zimmerli
 * \date   2020-01-30
 * \brief  PTP trace tool
 *
 */

#include <kernel/base/klibc.h>
#include "traceptp.h"
#include "tsn_ptp_stack.h"

#include <string.h>

// ----------------------------------------------------------------
#define SZ_PER_ENTRY 32

#define CMD_LOG_TX      1
#define CMD_LOG_TX_DONE 2
#define CMD_LOG_RX      3
#define CMD_LOG_TMR     4
#define CMD_LOG_ROLE    5

typedef struct {
    int64_t localtime;
    uint32_t stamp_sec;
    uint32_t stamp_nsec;
    uint16_t cmd;
    uint8_t msgtype;
    uint8_t timerid;
    uint16_t seqid;
} trace_entry_t;

typedef struct {
    uint32_t baseaddr;
    uint32_t ports;
    uint32_t entries;
    struct rtc_s *rtc;
    uint32_t port_wp[TSN_PTP_CFG_PORTNUM]; // write pointer per port
    uint8_t disable;                       // disable for printing
} trace_ptp_t;

trace_ptp_t _ptp_trace = { 0 };

static trace_entry_t *_getNextEntry(uint16_t portnum);
static const char *_strTmr(uint8_t timerid);
static const char *_strMsgType(uint8_t msgtype);

//ERSTMAL
static const char *PTPTRC_CONTEXT = "PTPTRC";

// ----------------------------------------------------------------
void traceptp_init(addr_t baseaddr, uint32_t ports, uint32_t entries, struct rtc_s *rtc)
{
    // each entry will require 32 byte
    // 1 ringbuffer per port
    // ports limited to TSN_PTP_CFG_PORTNUM

    uint32_t portidx;
    uint32_t size;

    if (ports > TSN_PTP_CFG_PORTNUM)
        ports = TSN_PTP_CFG_PORTNUM;

    _ptp_trace.baseaddr = baseaddr;
    _ptp_trace.ports = ports;
    _ptp_trace.entries = entries;
    _ptp_trace.rtc = rtc;
    _ptp_trace.disable = 0;
    for (portidx = 0; portidx < TSN_PTP_CFG_PORTNUM; ++portidx) {
        _ptp_trace.port_wp[portidx] = 0;
    }

    kassert(SZ_PER_ENTRY >= sizeof(trace_entry_t));
    size = ports * entries * SZ_PER_ENTRY;
    memset((void *)baseaddr, 0, size);
    logk(LOG_ALWAYS, PTPTRC_CONTEXT, "PTP trace initialized (baseaddr=0x%08X size=%d)\n", baseaddr, size);
}

void traceptp_print(uint16_t portnum)
{
    uint32_t idx;
    addr_t addr;
    trace_entry_t *p;
    int64_t localtime;
    int32_t sec, usec;

    if (!_ptp_trace.baseaddr)
        return; // not initialized
    if (!portnum || (portnum > _ptp_trace.ports))
        return;

    _ptp_trace.disable = 1;

    logk(LOG_ALWAYS, PTPTRC_CONTEXT, "Trace port %d (wp=%d, entries=%d)\n", portnum, _ptp_trace.port_wp[portnum - 1], _ptp_trace.entries);

    addr = _ptp_trace.baseaddr + _ptp_trace.entries * SZ_PER_ENTRY * (portnum - 1);
    for (idx = 0; idx < _ptp_trace.entries; ++idx) {
        p = (trace_entry_t *)addr;

        sec = (int32_t)(p->localtime / NSEC_PER_SEC);
        localtime = (int64_t)(sec) * (int64_t)NSEC_PER_SEC;
        localtime = (p->localtime - localtime) / 1000LL;
        usec = (int32_t)localtime;

        // print entry
        if (p->cmd != 0)
            logs(LOG_ALWAYS, PTPTRC_CONTEXT, "[%03d] %2d.%06d ", idx, sec, usec);

        if (p->cmd == CMD_LOG_TX)
            loge(LOG_ALWAYS, "tx ---> %s (s=%5d)\n", _strMsgType(p->msgtype), p->seqid);
        else if (p->cmd == CMD_LOG_TX_DONE)
            loge(LOG_ALWAYS, "tx done %s %d.%06d\n", _strMsgType(p->msgtype), p->stamp_sec, p->stamp_nsec / 1000);
        else if (p->cmd == CMD_LOG_RX)
            loge(LOG_ALWAYS, "rx <--- %s (s=%5d) %d.%06d\n", _strMsgType(p->msgtype), p->seqid, p->stamp_sec, p->stamp_nsec / 1000);
        else if (p->cmd == CMD_LOG_TMR)
            loge(LOG_ALWAYS, "Tmr %s\n", _strTmr(p->timerid));
        else if (p->cmd == CMD_LOG_ROLE) {
            uint8_t *pGmid = (uint8_t *)(addr + 8); //TODO magic number
            //uint8_t *pGmid = (uint8_t*)(&p->stamp_nsec + 2); //???

            logc(LOG_ALWAYS, "RoleSel GM");
            for (uint8_t gmbyte = 0; gmbyte < sizeof(clock_id); ++gmbyte)
                logc(LOG_ALWAYS, " %02X", pGmid[gmbyte]);
            //for (gmbyte=0; gmbyte<8; ++gmbyte) logc(LOG_ALWAYS, " %02X", MEM8(addr + 8 + gmbyte));
            loge(LOG_ALWAYS, "\n");
        }

        addr += SZ_PER_ENTRY;
    }
    _ptp_trace.disable = 0;
}

void traceptp_tx(uint16_t portnum, uint8_t msgtype, uint16_t seqid)
{
    trace_entry_t *p = _getNextEntry(portnum);
    if (p == NULL)
        return;
    p->cmd = CMD_LOG_TX;
    p->msgtype = msgtype;
    p->seqid = seqid;
}

void traceptp_tx_done(uint16_t portnum, uint8_t msgtype, uint32_t sec, uint32_t nsec)
{
    trace_entry_t *p = _getNextEntry(portnum);
    if (p == NULL)
        return;
    p->cmd = CMD_LOG_TX_DONE;
    p->msgtype = msgtype;
    p->stamp_sec = sec;
    p->stamp_nsec = nsec;
}

void traceptp_rx(uint16_t portnum, addr_t addr, uint32_t len, uint32_t sec, uint32_t nsec)
{
    (void)len;

    uint8_t msgtype;
    uint16_t seqid;
    trace_entry_t *p = _getNextEntry(portnum);

    if (p == NULL)
        return;
    msgtype = (*(uint8_t *)(addr)); // 1st byte
    seqid = klibc_ntohs((*(uint16_t *)(addr + 30)));
    p->cmd = CMD_LOG_RX;
    p->msgtype = msgtype;
    p->seqid = seqid;
    p->stamp_sec = sec;
    p->stamp_nsec = nsec;
}

void traceptp_tmr(uint16_t portnum, uint8_t timer_id)
{
    trace_entry_t *p = _getNextEntry(portnum);
    if (p == NULL)
        return;
    p->cmd = CMD_LOG_TMR;
    p->timerid = timer_id;
}

void traceptp_reselect(uint64_t gmid)
{
    uint32_t portnum;
    trace_entry_t *p;

    for (portnum = 1; portnum <= _ptp_trace.ports; ++portnum) {
        p = _getNextEntry(portnum);
        if (!p)
            continue;
        p->cmd = CMD_LOG_ROLE;
        p->stamp_sec = gmid & 0xFFFFFFFF;
        p->stamp_nsec = gmid >> 32;
    }
}

// ----------------------------------------------------------------
static trace_entry_t *_getNextEntry(uint16_t portnum)
{
    trace_entry_t *p;
    uint32_t wp;
    addr_t addr;
    struct timespec64 localtime;
    int portindex;

    if (!_ptp_trace.baseaddr)
        return NULL; // not initialized
    if (_ptp_trace.disable)
        return NULL; // currently disabled

    // check portnum
    if ((portnum <= 0) || (portnum > _ptp_trace.ports))
        return NULL;
    portindex = portnum - 1;

    // calc addr && inc ptr
    portENTER_CRITICAL();
    wp = _ptp_trace.port_wp[portindex];
    _ptp_trace.port_wp[portindex]++;
    if (_ptp_trace.port_wp[portindex] >= _ptp_trace.entries) {
        _ptp_trace.port_wp[portindex] = 0;
    }
    portEXIT_CRITICAL();
    addr = _ptp_trace.baseaddr;
    addr += portindex * _ptp_trace.entries * SZ_PER_ENTRY;
    addr += wp * SZ_PER_ENTRY;
    p = (trace_entry_t *)addr;

    // prepare localtime of log
    rtc_get_local_time64(_ptp_trace.rtc, &localtime);
    p->localtime = timespec64_to_ns(&localtime);

    return p;
}

static char *_strTmrText[5] = { "tx-pd", "tx-annc", "tx-syn", "tmout-annc", "tmout-syn" };

static const char *_strTmr(uint8_t timerid)
{
    if (timerid >= 5)
        return "unknown";
    return _strTmrText[timerid];
}

static const char *_strMsgType(uint8_t msgtype)
{
    msgtype &= PTP_MSGTYPE_MASK;

    switch (msgtype) {
    case PTP_MSGTYPE_SYNC:
        return "sync ";
    case PTP_MSGTYPE_PDELAY_REQ:
        return "pdreq";
    case PTP_MSGTYPE_PDELAY_RESP:
        return "pdrsp";
    case PTP_MSGTYPE_FOLLOWUP:
        return "fup  ";
    case PTP_MSGTYPE_PDELAY_FUP:
        return "pdfup";
    case PTP_MSGTYPE_ANNOUNCE:
        return "annc ";
    case PTP_MSGTYPE_SIGNALING:
        return "sign ";
    }

    return "unknown";
}
