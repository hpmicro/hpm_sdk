/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   ptp4tsync.c
 * \author zimmerli
 * \date   2020-01-17
 * \brief  PTP (802.1as) for TSN-EP (TSYNC)
 *
 */

#include <kernel/base/kernel.h>

#if (USE_TSN_RTOS_NET_PTP4SYNC == 1)

#include "ptp4tsync.h"
#include <kernel/base/rtc.h>
#include <kernel/base/klibc.h>
#include <kernel/net/netdev.h>
#include <kernel/net/ether.h>

#include <string.h>
#include <stdlib.h>
#include <time.h>

// include PTP Stack
#include "tsn_ptp_stack.h"
#include "traceptp.h"

// ----------------------------------------------------------------
/*
 * PTP Trace:
 * weak definition of trace functions,
 * get overridden if traceptp.c is compiled
 *
 */

void traceptp_print(uint16_t portnum) __attribute__((weak));
void traceptp_tx(uint16_t portnum, uint8_t msgtype, uint16_t seqid) __attribute__((weak));
void traceptp_tx_done(uint16_t portnum, uint8_t msgtype, uint32_t sec, uint32_t nsec) __attribute__((weak));
void traceptp_rx(uint16_t portnum, addr_t addr, uint32_t len, uint32_t sec, uint32_t nsec) __attribute__((weak));
void traceptp_tmr(uint16_t portnum, uint8_t timer_id) __attribute__((weak));
void traceptp_reselect(uint64_t gmid) __attribute__((weak));

/* weak default implementations */
void traceptp_print(uint16_t portnum)
{
    (void)portnum;
}
void traceptp_tx(uint16_t portnum, uint8_t msgtype, uint16_t seqid)
{
    (void)portnum;
    (void)msgtype;
    (void)seqid;
}
void traceptp_tx_done(uint16_t portnum, uint8_t msgtype, uint32_t sec, uint32_t nsec)
{
    (void)portnum;
    (void)msgtype;
    (void)sec, (void)nsec;
}
void traceptp_rx(uint16_t portnum, addr_t addr, uint32_t len, uint32_t sec, uint32_t nsec)
{
    (void)portnum;
    (void)addr;
    (void)len;
    (void)sec, (void)nsec;
}
void traceptp_tmr(uint16_t portnum, uint8_t timer_id)
{
    (void)portnum;
    (void)timer_id;
}
void traceptp_reselect(uint64_t gmid)
{
    (void)gmid;
}

/* trace macros */
#define TRACE_PTP_TX(portnum, msgtype, seqid)        traceptp_tx(portnum, msgtype, seqid)
#define TRACE_PTP_TX_DONE(portnum, msgtype, sec, ns) traceptp_tx_done(portnum, msgtype, sec, ns)
#define TRACE_PTP_RX(portnum, addr, len, sec, ns)    traceptp_rx(portnum, addr, len, sec, ns)
#define TRACE_PTP_TMR(portnum, timerid)              traceptp_tmr(portnum, timerid)
#define TRACE_PTP_RESELECT(gmid)                     traceptp_reselect(gmid)

// ----------------------------------------------------------------
//TODO ........
#define EGRESS_LAT  0
#define INGRESS_LAT 0

#define ADJ_FREQ_IVAL 1000000000LL
#define PORTSYNC_QSZ  8

#define SYNC_DOMAIN 1
#define SYNC_RTC    2

// ----------------------------------------------------------------
#define MSG_NUM_MAX 7

static uint8_t _map_idx_to_msgtype[MSG_NUM_MAX + 1] = { PTP_MSGTYPE_SYNC,       PTP_MSGTYPE_FOLLOWUP, PTP_MSGTYPE_PDELAY_REQ, PTP_MSGTYPE_PDELAY_RESP,
                                                        PTP_MSGTYPE_PDELAY_FUP, PTP_MSGTYPE_ANNOUNCE, PTP_MSGTYPE_SIGNALING,  PTP_MSGTYPE_MASK };

static uint8_t _msgtype2idx(uint8_t msgtype)
{
    uint8_t idx;
    for (idx = 0; idx < MSG_NUM_MAX; ++idx) {
        if (_map_idx_to_msgtype[idx] == msgtype)
            break;
    }
    return idx;
}

static const char *_syncmode2string(enum ptp4sync_sync_e mode)
{
    if (mode == PTP4SYNC_NONE)
        return "none";
    if (mode == PTP4SYNC_FALLBACK)
        return "fallback";
    if (mode == PTP4SYNC_PRIMARY)
        return "primary";
    return "Unknown sync mode!";
}

static const char *_portrole2string(enum ptp4sync_port_role_e role)
{
    if (role == PTP4SYNC_ROLE_DYNAMIC)
        return "dynamic";
    if (role == PTP4SYNC_ROLE_MASTER)
        return "master";
    if (role == PTP4SYNC_ROLE_SLAVE)
        return "slave";
    if (role == PTP4SYNC_ROLE_PASSIVE)
        return "passive";
    if (role == PTP4SYNC_ROLE_DISABLED)
        return "disabled";
    return "Unknown port role!";
}

// ----------------------------------------------------------------
// stats
#define TMRMAX 5

typedef struct {
    uint16_t rx[MSG_NUM_MAX + 1];
    uint16_t tx[MSG_NUM_MAX + 1];
    uint16_t timer[TMRMAX];
} ptp_portstats_t;

typedef struct {
    ptp_portstats_t stats;
    struct netdev_s *device;
    uint32_t tmr[TMRMAX];
} domain_port_handle_t;

typedef struct {
    uint8_t domainNumber;
    uint32_t portNumber;
    tsn_ptp_instance_t stack;
    uint8_t trafficPrio;
    domain_port_handle_t *ports;
    enum ptp4sync_sync_e syncMode;
    uint8_t synced;
    uint8_t syncFlags;

    void *syncDomainSource;
    void *syncDomainTarget;
} domain_handle_t;

typedef struct {
    int enablePrint;
    list_data_t *domainList;
    struct rtc_s *rtc;
    TaskHandle_t hSyncTask;
    TaskHandle_t hPrintTask;
    uint8_t mac_addr[MAC_ADDR_LEN];

    domain_handle_t *syncRTC;
} ptp_site_t;

ptp_site_t _ptp_site;

// task
void task_ptp_sync(void *pvParameters);
void task_ptp_print(void *pvParameters);

static void printTime(domain_handle_t *pDomain);
static domain_handle_t *searchDomain(uint8_t domainNumber);
static void timerDomain(domain_handle_t *pDomain);

// hook functions
static void hookfunc_txFrame(void *cbParam, uint16_t portIndex, struct ptp_header_s *pPtpHeader, uint8_t *pPayload, uint32_t payloadLen);
static void hookfunc_setTimer(void *cbParam, ptp_timer_event_t timer, uint32_t id, int8_t logInterval, uint8_t mult);
static void hookfunc_getLocalClock(void *cbParam, ptp_timestamp_t *localtime);
static void hookfunc_getLocalClockOffset(void *cbParam, ptp_timestamp_t *offset);
static void hookfunc_statusEvent(void *cbParam, ptp_status_event_t event, void *param);

static void hook_tsync_rx_frame(struct netdev_s *netdev, void *rxdata, uint32_t rxlen, uint32_t localtm_ns, uint32_t localtm_sec);
static void hook_tsync_tx_done(struct netdev_s *netdev, void *arg, uint32_t localtm_ns, uint32_t localtm_sec);

static const char *PTP4TSYNC_CONTEXT = "PTP4TSYNC";

/**
 * \brief Start ptp4tsync task
 *
 * @param rtc           RTC to be used
 * @param mac_addr		pointer to the MAC address of the domain
 */
void ptp4sync_start(struct rtc_s *rtc, uint8_t *mac_addr)
{
    // init site struct
    _ptp_site.enablePrint = 1;
    _ptp_site.rtc = rtc;
    _ptp_site.syncRTC = NULL;

    memcpy(_ptp_site.mac_addr, mac_addr, MAC_ADDR_LEN);

    _ptp_site.domainList = list_create();
    if (!_ptp_site.domainList)
        goto fail;

    // create task
    kTaskCreate_a(PTP4TSYNC_CONTEXT, task_ptp_sync, (const char *)"ptp sync", 10 * 1024, (void *)&_ptp_site, KERNEL_PRIO_NORMAL, &_ptp_site.hSyncTask);
    kTaskCreate_a(PTP4TSYNC_CONTEXT, task_ptp_print, (const char *)"ptp print", 10 * 1024, (void *)&_ptp_site, KERNEL_PRIO_NORMAL, &_ptp_site.hPrintTask);

    return;

fail:
    printf("%s: init failed\n", PTP4TSYNC_CONTEXT);

    return;
}

/**
 * \brief Add a new PTP domain
 *
 * @param domainNumber	number of the domain
 * @param gmPrio1		priority of the PTP domain grandmaster
 * @param port_num		number of ports in the PTP domain
 * @param type			sync type for RTC sync
 */
void ptp4sync_add_domain(uint8_t domainNumber, uint8_t gmPrio1, uint32_t port_num, enum ptp4sync_sync_e type)
{
    domain_handle_t *pDomain;

    //reuse resources for domain, ptp4sync_rm_domain will not remove resources
    pDomain = searchDomain(domainNumber);
    if (!pDomain) {
        pDomain = (domain_handle_t *)kallocz(sizeof(domain_handle_t));
        if (!pDomain)
            goto fail;

        pDomain->ports = (domain_port_handle_t *)kallocz(port_num * sizeof(domain_port_handle_t));
        if (!pDomain->ports)
            goto fail;
    }

    printf("%s | add domain %u\n", PTP4TSYNC_CONTEXT, domainNumber);

    pDomain->portNumber = port_num;
    pDomain->domainNumber = domainNumber;
    pDomain->syncMode = type;
    pDomain->synced = 0;
    pDomain->syncDomainSource = NULL;
    pDomain->syncDomainTarget = NULL;
    pDomain->syncFlags = 0;

    // stack init function
    tsn_ptp_stack_init(&pDomain->stack, _ptp_site.mac_addr, domainNumber);

    // init PTP stack hook functions
    pDomain->stack.cbParam = pDomain;
    pDomain->stack.hookSetTimer = hookfunc_setTimer;
    pDomain->stack.hookTxFrame = hookfunc_txFrame;
    pDomain->stack.hookGetLocalTime = hookfunc_getLocalClock;
    pDomain->stack.hookGetLocalOffset = hookfunc_getLocalClockOffset;
    pDomain->stack.hookStatusEvent = hookfunc_statusEvent;

    tsn_ptp_stack_enable(&pDomain->stack, 1);

    pDomain->stack.systemId.prio1 = gmPrio1;
    pDomain->stack.systemId.prio2 = 255;
    tsn_ptp_stack_portroleselection(&pDomain->stack);

    list_add(_ptp_site.domainList, pDomain);

    return;
fail:
    printf("%s | add domain failed\n", PTP4TSYNC_CONTEXT);
    return;
}

/**
 * \brief Add a new PTP domain
 *
 * @param domainNumber	number of the domain
 * @param gmPrio1		priority of the PTP domain grandmaster
 * @param port_num		number of ports in the PTP domain
 * @param mac_addr		pointer to the MAC address of the domain
 */
void ptp4sync_rm_domain(uint8_t domainNumber)
{
    domain_handle_t *pDomain;

    pDomain = searchDomain(domainNumber);
    if (!pDomain) {
        printf("%s | remove domain failed\n", PTP4TSYNC_CONTEXT);
        return;
    }

    //disable all ports
    for (uint16_t portIndex = 0; portIndex < pDomain->portNumber; portIndex++) {
        struct netdev_s *netdev = pDomain->ports[portIndex].device;
        if (netdev) {
            printf("%s | remove %s from domain %u\n", PTP4TSYNC_CONTEXT, netdev->name, domainNumber);
            tsn_ptp_stack_port_enable(&pDomain->stack, portIndex, 0);
        }
    }

    tsn_ptp_stack_enable(&pDomain->stack, 0);
    //free of domain resources need locking of domain access, will be implemented later
    vTaskDelay(1000);
    kfree(pDomain->ports);
    kfree(pDomain);

    list_del(_ptp_site.domainList, pDomain);

    return;
}

void ptp4sync_add_sync(uint8_t sourceDomainNumber, uint8_t targetDomainNumber)
{
    domain_handle_t *pDomainSource;
    domain_handle_t *pDomainTarget;

    pDomainSource = searchDomain(sourceDomainNumber);
    pDomainTarget = searchDomain(targetDomainNumber);
    if (!pDomainSource || !pDomainTarget) {
        printf("%s | add domain sync failed\n", PTP4TSYNC_CONTEXT);
        return;
    }

    printf("%s | add sync from domain %u to domain %u\n", PTP4TSYNC_CONTEXT, sourceDomainNumber, targetDomainNumber);

    pDomainSource->syncDomainTarget = pDomainTarget;
    pDomainTarget->syncDomainSource = pDomainSource;
    pDomainSource->syncFlags |= SYNC_DOMAIN;
    pDomainSource->stack.enableInvoke = 1;
}

void ptp4sync_rm_sync(uint8_t sourceDomainNumber)
{
    domain_handle_t *pDomainSource;
    domain_handle_t *pDomainTarget;

    pDomainSource = searchDomain(sourceDomainNumber);
    if (!pDomainSource) {
        printf("%s | rm domain sync failed\n", PTP4TSYNC_CONTEXT);
        return;
    }

    printf("%s | remove sync from domain %u\n", PTP4TSYNC_CONTEXT, sourceDomainNumber);

    if (pDomainSource->syncDomainTarget) {
        pDomainTarget = pDomainSource->syncDomainTarget;
        pDomainTarget->syncDomainSource = NULL;
    }

    pDomainSource->syncDomainTarget = NULL;

    pDomainSource->syncFlags &= ~SYNC_DOMAIN;
    if (pDomainSource->syncFlags == 0)
        pDomainSource->stack.enableInvoke = 0;
}

/**
 * \brief Add netdev to PTP domain
 *
 * Will use the port->id for assignment to PTP port index
 *
 * @param domainNumber	number of the domain
 * @param portIndex     port index in the PTP domain
 * @param device		pointer to the netdev
 */
void ptp4sync_add_port(uint8_t domainNumber, uint16_t portIndex, struct netdev_s *device)
{
    domain_handle_t *pDomain;

    if (!device)
        goto fail;
    if (portIndex >= TSN_PTP_CFG_PORTNUM)
        goto fail;

    pDomain = searchDomain(domainNumber);
    if (!pDomain)
        goto fail;

    printf("%s | add %s to domain %u\n", PTP4TSYNC_CONTEXT, device->name, domainNumber);
    pDomain->ports[portIndex].device = device;

    device->tsync.cb_rx_frame = hook_tsync_rx_frame;
    device->tsync.cb_tx_done = hook_tsync_tx_done;

    tsn_ptp_stack_port_enable(&pDomain->stack, portIndex, 1);

    return;
fail:
    printf("%s | add port failed\n", PTP4TSYNC_CONTEXT);
    return;
}

/**
 * \brief Remove port from PTP domain
 *
 * @param domainNumber	number of the domain
 * @param portIndex     port index in the PTP domain
 */
void ptp4sync_rm_port(uint8_t domainNumber, uint16_t portIndex)
{
    domain_handle_t *pDomain;
    struct netdev_s *device;

    if (portIndex >= TSN_PTP_CFG_PORTNUM)
        goto fail;

    pDomain = searchDomain(domainNumber);
    if (!pDomain)
        goto fail;

    device = pDomain->ports[portIndex].device;
    if (!device)
        goto fail;
    printf("%s | remove %s from domain %u\n", PTP4TSYNC_CONTEXT, device->name, domainNumber);
    tsn_ptp_stack_port_enable(&pDomain->stack, portIndex, 0);

    return;
fail:
    printf("%s | remove port failed\n", PTP4TSYNC_CONTEXT);
    return;
}

/**
 * \brief Set priority for all PTP traffic
 *
 * @param domainNumber	number of the domain
 * @param prio    		traffic priority number
 */
void ptp4sync_set_traffic_prio(uint8_t domainNumber, uint8_t prio)
{
    domain_handle_t *pDomain;

    pDomain = searchDomain(domainNumber);
    if (!pDomain)
        return;

    pDomain->trafficPrio = prio;
    printf("%s | set ptp traffic priority=%d (%s) for domain %u\n", PTP4TSYNC_CONTEXT, prio, ETH_PRIO_NAMES[prio], domainNumber);
}

/**
 * \brief Set priority for all PTP traffic
 *
 * @param domainNumber	number of the domain
 * @param syncMode    	sync mode selcetion
 */
void ptp4sync_set_rtcsync_mode(uint8_t domainNumber, enum ptp4sync_sync_e syncMode)
{
    domain_handle_t *pDomain;

    pDomain = searchDomain(domainNumber);
    if (!pDomain)
        return;

    pDomain->syncMode = syncMode;
    printf("%s | set RTC sync mode to %s for domain %u\n", PTP4TSYNC_CONTEXT, _syncmode2string(syncMode), domainNumber);
}

void ptp4sync_set_ext_port_state(uint8_t domainNumber, uint32_t index, enum ptp4sync_port_role_e role)
{
    if (index >= TSN_PTP_CFG_PORTNUM)
        return;

    domain_handle_t *pDomain = searchDomain(domainNumber);
    if (!pDomain)
        return;

    printf("%s | set port role to %s for domain %u index %u\n", PTP4TSYNC_CONTEXT, _portrole2string(role), domainNumber, index);

    if (role == PTP4SYNC_ROLE_DYNAMIC) {
        pDomain->stack.port[index].port.rcvdPortStateInd = 0;
    } else {
        pDomain->stack.port[index].port.rcvdPortStateInd = 1;

        switch (role) {
        case PTP4SYNC_ROLE_DISABLED:
            pDomain->stack.port[index].port.rcvdPortState = PTP_PORT_ROLE_DISABLED;
            break;
        case PTP4SYNC_ROLE_MASTER:
            pDomain->stack.port[index].port.rcvdPortState = PTP_PORT_ROLE_MASTER;
            break;
        case PTP4SYNC_ROLE_SLAVE:
            pDomain->stack.port[index].port.rcvdPortState = PTP_PORT_ROLE_SLAVE;
            break;
        case PTP4SYNC_ROLE_PASSIVE:
            pDomain->stack.port[index].port.rcvdPortState = PTP_PORT_ROLE_PASSIVE;
            break;
        default:
            break;
        }
    }

    tsn_ptp_stack_portroleselection(&pDomain->stack);
}

/**
 * \brief Get current PTP status
 *
 * @param gmid          dest pointer to Grandmaster id
 * @param gmchanges     dest pointer to number of GM changes
 * @param gmoffset      dest pointer to offset to GM at last sync
 */
void ptp4sync_get_state(uint8_t domainNumber, uint64_t *gmid, uint16_t *gmchanges, int32_t *gmoffset)
{
    domain_handle_t *pDomain = searchDomain(domainNumber);
    if (!pDomain)
        return;

    if (gmid)
        *gmid = pDomain->stack.gmPriority.rootSystemId.clockIdentity;
    if (gmchanges)
        *gmchanges = pDomain->stack.gmChanges;
    if (gmoffset)
        *gmoffset = pDomain->stack.siteSync.gmOffset;
}

/**
 * \brief Get current PTP status of port
 *
 * @param index         port index (0 .. n)
 * @param ascapable     dest pointer to asCapable flag
 * @param portstate     dest pointer to port role
 * @param pdelay        dest pointer to pdelay (nanoseconds)
 */
void ptp4sync_get_port_state(uint8_t domainNumber, uint32_t index, uint8_t *ascapable, uint8_t *portstate, uint16_t *pdelay)
{
    uint16_t delay;

    if (index >= TSN_PTP_CFG_PORTNUM)
        return;

    domain_handle_t *pDomain = searchDomain(domainNumber);
    if (!pDomain)
        return;

    if (ascapable)
        *ascapable = pDomain->stack.port[index].port.asCapable;
    if (portstate)
        *portstate = pDomain->stack.port[index].port.selectedRole;
    delay = 0;
    if (pDomain->stack.port[index].md.isMeasuringDelay)
        delay = pDomain->stack.port[index].port.meanLinkDelay;
    if (pdelay)
        *pdelay = delay;
}

void ptp4sync_set_gm_prio1(uint8_t domainNumber, uint8_t prio1)
{
    domain_handle_t *pDomain = searchDomain(domainNumber);
    if (!pDomain)
        return;

    pDomain->stack.systemId.prio1 = prio1; // set prio1

    tsn_ptp_stack_portroleselection(&pDomain->stack);
    TRACE_PTP_RESELECT(pDomain->stack.gmPriority.rootSystemId.clockIdentity);
}

// ----------------------------------------------------------------
void task_ptp_sync(void *pvParameters)
{
    ptp_site_t *site;

    KERNEL_TASK_FPU();

    site = (ptp_site_t *)pvParameters;

    list_data_t *entry;
    domain_handle_t *pDomain;

    while (1) {
        vTaskDelay(32);

        entry = site->domainList;
        while (entry != NULL) {
            pDomain = (domain_handle_t *)entry->data;

            timerDomain(pDomain);

            entry = list_next(_ptp_site.domainList, entry);
        }
    }
}

// ----------------------------------------------------------------
void task_ptp_print(void *pvParameters)
{
    ptp_site_t *site;
    domain_handle_t *pDomain;
    list_data_t *entry;

    KERNEL_TASK_FPU();
    site = (ptp_site_t *)pvParameters;

    while (1) {
        vTaskDelay(3000);

        if (site->enablePrint) {
            entry = site->domainList;

            while (entry != NULL) {
                pDomain = (domain_handle_t *)entry->data;
                printTime(pDomain);
                entry = list_next(_ptp_site.domainList, entry);
            }
        }
    }
}

// ----------------------------------------------------------------
static domain_handle_t *searchDomain(uint8_t domainNumber)
{
    list_data_t *entry;
    domain_handle_t *pDomain;

    entry = _ptp_site.domainList;
    if (!entry)
        return NULL;

    while (entry != NULL) {
        pDomain = (domain_handle_t *)entry->data;
        if (pDomain->domainNumber == domainNumber) {
            return pDomain;
        }

        entry = list_next(_ptp_site.domainList, entry);
    }

    return NULL;
}

static void timerDomain(domain_handle_t *pDomain)
{
    uint32_t tmrCnt;

    for (uint8_t i = 0; i < pDomain->portNumber; i++) {
        for (int j = 0; j < TMRMAX; j++) {
            tmrCnt = pDomain->ports[i].tmr[j];

            if (tmrCnt > 0) {
                pDomain->ports[i].tmr[j] = tmrCnt - 1;
                if (tmrCnt == 1) {
                    TRACE_PTP_TMR(i + 1, j);
                    tsn_ptp_stack_timerevent(&pDomain->stack, j, i);
                    pDomain->ports[i].stats.timer[j]++;
                }
            }
        }
    }
}

static void syncRTC(void)
{
    list_data_t *entry;
    domain_handle_t *pDomain;
    domain_handle_t *pDomainSync = NULL;
    enum ptp4sync_sync_e curtype = PTP4SYNC_NONE;

    entry = _ptp_site.domainList;
    if (!entry)
        return;

    while (entry != NULL) {
        pDomain = (domain_handle_t *)entry->data;
        if (pDomain->synced && pDomain->syncMode < curtype) {
            pDomainSync = pDomain;
            curtype = pDomain->syncMode;
        }

        entry = list_next(_ptp_site.domainList, entry);
    }

    if (pDomainSync != _ptp_site.syncRTC) {
        printf("%s | assign sync source to domain ", PTP4TSYNC_CONTEXT);
        if (pDomainSync)
            printf("%d\n", pDomainSync->domainNumber);
        else

            printf("none\n");

        if (_ptp_site.syncRTC) {
            _ptp_site.syncRTC->syncFlags &= ~SYNC_RTC;

            if (_ptp_site.syncRTC->syncFlags == 0)
                _ptp_site.syncRTC->stack.enableInvoke = 0;
        }

        if (pDomainSync) {
            pDomainSync->syncFlags |= SYNC_RTC;
            pDomainSync->stack.enableInvoke = 1;
        }

        _ptp_site.syncRTC = pDomainSync;
    }
}

static void printTime(domain_handle_t *pDomain)
{
    struct timespec64 rtctime;

    uint64_t time_ns = tsn_ptp_time_get(&pDomain->stack);

    rtctime.tv_sec = time_ns / 1000000000;
    rtctime.tv_nsec = time_ns % 1000000000;

    time_t time = rtctime.tv_sec;
    struct tm *utc = NULL;
    utc = gmtime(&time);

    printf("domain %3u: ", pDomain->domainNumber);
    printf("domaintime=%d.%09d ", (uint32_t)rtctime.tv_sec, rtctime.tv_nsec); // sec.nsec
    if (utc != NULL) {
        printf("or %s", asctime(utc));
    } else {
        printf("\n");
    }
}

// ----------------------------------------------------------------

static void hookfunc_txFrame(void *cbParam, uint16_t portIndex, struct ptp_header_s *pPtpHeader, uint8_t *pPayload, uint32_t payloadLen)
{
    addr_t addr;
    uint32_t ofs;
    uint8_t _ptp_tq;
    struct netdev_s *netdev;
    struct netb_s *netb;

    netb = kalloc_netb(10);
    if (!netb)
        return; //TODO implement TX error counter

    domain_handle_t *pDomain = (domain_handle_t *)cbParam;

    netdev = pDomain->ports[portIndex].device;
    netb_reserve(netb, netdev->needed_head);

    _ptp_tq = netdev->prio2tq[pDomain->trafficPrio];

    addr = (netb->data);
    ofs = tsn_ptp_set_eth_header(addr, netdev->dev_addr);
    ofs += tsn_ptp_set_ptp_header(addr + ofs, pPtpHeader);
    memcpy((uint8_t *)addr + ofs, pPayload, payloadLen);
    netb_put(netb, payloadLen + ofs);

    // update data and trigger tx
    TRACE_PTP_TX(portIndex + 1, pPtpHeader->msgType, pPtpHeader->sequenceId);

    if (tsn_ctrl_tsync_tx(netdev, netb, _ptp_tq, netb) != NETDEV_RESULT_OK) {
        kfree_netb(netb);
        return;
    }

    // debug counter
    pDomain->ports[portIndex].stats.tx[_msgtype2idx(pPtpHeader->msgType & PTP_MSGTYPE_MASK)]++;
}

static void hookfunc_setTimer(void *cbParam, ptp_timer_event_t timer, uint32_t id, int8_t logInterval, uint8_t mult)
{
    domain_handle_t *pDomain = (domain_handle_t *)cbParam;

    uint32_t period;

    // disable timer when logInterval > 127
    // 1:1 assignment from ptp_timer_event -> timer index of TSN.TSYNC

    // (logInterval <= -5) || (logInterval >= 5) --> ignore
    // HW should support up to -10 .. 10 (when mult=1)

    if (logInterval == 127) {
        pDomain->ports[id].tmr[timer] = 0;
        return;
    }

    if (logInterval <= -5) {
        return;
    }
    if (logInterval >= 5) {
        return;
    }

    period = (1 << (logInterval + 5)); // 1/32th second tmr tick
    period *= mult;

    if (period == 0) {
        period++; // 1/32th second tick
    }

    pDomain->ports[id].tmr[timer] = period;
}

static void hookfunc_getLocalClock(void *cbParam, ptp_timestamp_t *localtime)
{
    (void)cbParam;

    struct timespec64 rtc;

    rtc_get_local_time64(_ptp_site.rtc, &rtc);

    localtime->seconds = (uint64_t)rtc.tv_sec;
    localtime->nanoseconds = rtc.tv_nsec;
}

static void hookfunc_getLocalClockOffset(void *cbParam, ptp_timestamp_t *offset)
{
    (void)cbParam;

    struct timespec64 ofs;

    rtc_get_rtc_offset(_ptp_site.rtc, &ofs);

    offset->seconds = (uint64_t)ofs.tv_sec;
    offset->nanoseconds = ofs.tv_nsec;
}

static void hookfunc_statusEvent(void *cbParam, ptp_status_event_t event, void *param)
{
    domain_handle_t *pDomain = (domain_handle_t *)cbParam;

    if (event == PTP_STS_EVENT_PHASE_CHANGE) {
        uint64_t *p_ns = (uint64_t *)param;

        if (pDomain->syncFlags & SYNC_DOMAIN) {
            domain_handle_t *pDomainTarget = pDomain->syncDomainTarget;
            pDomainTarget->stack.localClock.offset_ns = *p_ns;
        }

        if (pDomain->syncFlags & SYNC_RTC) {
            struct timespec64 rtcoffset;
            rtcoffset.tv_sec = *p_ns / NSEC_PER_SEC;
            rtcoffset.tv_nsec = *p_ns % NSEC_PER_SEC;
            rtc_set_rtc_offset(_ptp_site.rtc, &rtcoffset);
        }
        return;
    }

    if (event == PTP_STS_EVENT_FREQ_CHANGE) {
        int32_t *p_ppb = (int32_t *)param;
        if (pDomain->syncFlags & SYNC_DOMAIN) {
            domain_handle_t *pDomainTarget = pDomain->syncDomainTarget;
            pDomainTarget->stack.localClock.rateOfs = *p_ppb;
        }

        if (pDomain->syncFlags & SYNC_RTC) {
            rtc_adjfine(_ptp_site.rtc, *p_ppb);
        }

        return;
    }

    if (event == PTP_STS_EVENT_SYNCRECV) {
        pDomain->synced = 1;
    }

    if (event == PTP_STS_EVENT_SYNCLOST) {
        pDomain->synced = 0;
    }

    //reassignment of sync domain
    syncRTC();
}

// ----------------------------------------------------------------
static void hook_tsync_rx_frame(struct netdev_s *netdev, void *rxdata, uint32_t rxlen, uint32_t localtm_ns, uint32_t localtm_sec)
{
    ptp_timestamp_t timestamp;

    if (netdev->tsn_port >= TSN_PTP_CFG_PORTNUM)
        return;

    uint8_t domainNumber = tsn_ptp_domain((addr_t)rxdata + 14);
    uint8_t msgType = tsn_ptp_msg((addr_t)rxdata + 14);

    domain_handle_t *pDomain = searchDomain(domainNumber);
    if (!pDomain)
        return;

    TRACE_PTP_RX(netdev->tsn_port + 1, (addr_t)rxdata + 14, rxlen - 14, localtm_sec, localtm_ns);
    // adjust timestamp and process
    timestamp.nanoseconds = localtm_ns;
    timestamp.seconds = localtm_sec;
    tsn_ptp_adjust_timestamp(&timestamp, -INGRESS_LAT);
    tsn_ptp_stack_rxframe(&pDomain->stack, netdev->tsn_port, (addr_t)rxdata + 14, rxlen - 14, &timestamp);

    // debug counter
    pDomain->ports[netdev->tsn_port].stats.rx[_msgtype2idx(msgType)]++;
}

static void hook_tsync_tx_done(struct netdev_s *netdev, void *arg, uint32_t localtm_ns, uint32_t localtm_sec)
{
    struct netb_s *netb = (struct netb_s *)arg;
    ptp_timestamp_t timestamp;
    tsn_ptp_port_t *ptpport;

    if (netdev->tsn_port >= TSN_PTP_CFG_PORTNUM)
        return;
    if (!netb)
        return;

    uint8_t domainNumber = tsn_ptp_domain(netb->data + 14);
    uint8_t msgType = tsn_ptp_msg(netb->data + 14);

    kfree_netb(netb);

    domain_handle_t *pDomain = searchDomain(domainNumber);
    if (!pDomain)
        return;

    ptpport = &pDomain->stack.port[netdev->tsn_port];

    timestamp.nanoseconds = localtm_ns;
    timestamp.seconds = localtm_sec;
    tsn_ptp_adjust_timestamp(&timestamp, EGRESS_LAT); // adjust TX timestamp

    TRACE_PTP_TX_DONE(ptpport->port.thisPort, msgType, localtm_sec, localtm_ns);
    tsn_ptp_stack_txdone(&pDomain->stack, ptpport, msgType, &timestamp);
}

#endif /* USE_TSN_RTOS_NET_PTP4SYNC */
