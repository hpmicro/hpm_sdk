/* net.c */

#include "../ptpd.h"

/* Network Buffer Queue Functions. */
static void netQInit(BufQueue *pQ)
{
    pQ->get = 0;
    pQ->put = 0;
    pQ->count = 0;
}

static Integer32 netQPut(BufQueue *pQ, void *pbuf)
{
    if (pQ->count >= PBUF_QUEUE_SIZE)
        return FALSE;

    pQ->pbuf[pQ->put] = pbuf;

    pQ->put = (pQ->put + 1) % PBUF_QUEUE_SIZE;

    pQ->count++;

    return TRUE;
}

static void *netQGet(BufQueue *pQ)
{
    void *pbuf;

    if (!pQ->count)
        return NULL;

    pbuf = pQ->pbuf[pQ->get];

    pQ->get = (pQ->get + 1) % PBUF_QUEUE_SIZE;

    pQ->count--;

    return pbuf;
}

static void netQEmpty(BufQueue * pQ)
{

    struct pbuf * p;
    int cnt = pQ->count;

    for (;cnt > 0; cnt--)
    {
        p = (struct pbuf*)netQGet(pQ);

        if (p) pbuf_free(p);
    }
}


static Integer32 netQCheck(BufQueue *pQ)
{
    if (!pQ->count)
        return FALSE;

    return TRUE;
}

/* shut down the UDP stuff */
Boolean netShutdown(NetPath *netPath)
{
    ip_addr_t multicastAaddr;

    DBG("netShutdown\n");

    /* leave multicast group */
    multicastAaddr.addr = netPath->multicastAddr;
    if (multicastAaddr.addr != 0)
    {
        igmp_leavegroup(IP_ADDR_ANY, &multicastAaddr);
    }

    /* Disconnect and close the Event UDP interface */

    if (netPath->eventPcb)
    {
        udp_disconnect(netPath->eventPcb);
        udp_remove(netPath->eventPcb);
        netPath->eventPcb = NULL;
    }

    /* Disconnect and close the General UDP interface */
    if (netPath->generalPcb)
    {
        udp_disconnect(netPath->generalPcb);
        udp_remove(netPath->generalPcb);
        netPath->generalPcb = NULL;
    }

    /* Clear the network addresses. */
    netPath->multicastAddr = 0;

    netPath->unicastAddr = 0;

    /* Return a success code. */
    return TRUE;
}

static Boolean lookupSubdomainAddress(Octet *subdomainName, Octet *subdomainAddress)
{
    UInteger32 h;

    /* set multicast group address based on subdomainName */

    if (!memcmp(subdomainName, DEFAULT_PTP_DOMAIN_NAME, PTP_SUBDOMAIN_NAME_LENGTH))
        memcpy(subdomainAddress, DEFAULT_PTP_DOMAIN_ADDRESS, min(sizeof(DEFAULT_PTP_DOMAIN_ADDRESS), NET_ADDRESS_LENGTH));
    else if (!memcmp(subdomainName, ALTERNATE_PTP_DOMAIN1_NAME, PTP_SUBDOMAIN_NAME_LENGTH))
        memcpy(subdomainAddress, ALTERNATE_PTP_DOMAIN1_ADDRESS, min(sizeof(ALTERNATE_PTP_DOMAIN1_ADDRESS), NET_ADDRESS_LENGTH));
    else if (!memcmp(subdomainName, ALTERNATE_PTP_DOMAIN2_NAME, PTP_SUBDOMAIN_NAME_LENGTH))
        memcpy(subdomainAddress, ALTERNATE_PTP_DOMAIN2_ADDRESS, min(sizeof(ALTERNATE_PTP_DOMAIN2_ADDRESS), NET_ADDRESS_LENGTH));
    else if (!memcmp(subdomainName, ALTERNATE_PTP_DOMAIN3_NAME, PTP_SUBDOMAIN_NAME_LENGTH))
        memcpy(subdomainAddress, ALTERNATE_PTP_DOMAIN3_ADDRESS, min(sizeof(ALTERNATE_PTP_DOMAIN3_ADDRESS), NET_ADDRESS_LENGTH));
    else
    {
        h = crc_algorithm(subdomainName, PTP_SUBDOMAIN_NAME_LENGTH) % 3;

        switch (h)
        {
        case 0:
            memcpy(subdomainAddress, ALTERNATE_PTP_DOMAIN1_ADDRESS, min(sizeof(ALTERNATE_PTP_DOMAIN1_ADDRESS), NET_ADDRESS_LENGTH));
            break;
        case 1:
            memcpy(subdomainAddress, ALTERNATE_PTP_DOMAIN2_ADDRESS, min(sizeof(ALTERNATE_PTP_DOMAIN2_ADDRESS), NET_ADDRESS_LENGTH));
            break;
        case 2:
            memcpy(subdomainAddress, ALTERNATE_PTP_DOMAIN3_ADDRESS, min(sizeof(ALTERNATE_PTP_DOMAIN3_ADDRESS), NET_ADDRESS_LENGTH));
            break;
        default:
            ERROR("lookupSubdomainAddress: handle out of range for '%s'!\n", subdomainName);
            return FALSE;
        }
    }

    return TRUE;
}

static Integer32 findIface(NetPath *netPath, const Octet *ifaceName, UInteger8 *communicationTechnology,
                           Octet *uuid)
{

    struct netif * iface;
    iface = netif_default;
    *communicationTechnology = PTP_ETHER;
    memcpy(uuid, iface->hwaddr, PTP_UUID_LENGTH);
    return iface->ip_addr.addr;
}

/* Processing an incoming message on the Event port. */
static void netRecvEventCallback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                                 const ip_addr_t *addr, u16_t port)
{
    NetPath *netPath = (NetPath *)arg;

    /* Place the incoming message on the Event Port QUEUE. */

    if (!netQPut(&netPath->eventQ, p))
    {
        pbuf_free(p);
        p = NULL;
        ERROR("netRecvEventCallback: queue full\n");
        return;
    }
}

/* Processing an incoming message on the General port. */
static void netRecvGeneralCallback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                                  const ip_addr_t *addr, u16_t port)
{
    NetPath *netPath = (NetPath *)arg;

    /* Place the incoming message on the Event Port QUEUE. */

    if (!netQPut(&netPath->generalQ, p))
    {
        pbuf_free(p);
        p = NULL;
        ERROR("netRecvGeneralCallback: queue full\n");
        return;
    }
}

/* start all of the UDP stuff */
/* must specify 'subdomainName', optionally 'ifaceName', if not then pass ifaceName == "" */
/* returns other args */
/* on socket options, see the 'socket(7)' and 'ip' man pages */
Boolean netInit(NetPath *netPath, PtpClock *ptpClock)
{
    int i;

    ip_addr_t interfaceAddr;

    struct in_addr netAddr;
    char addrStr[NET_ADDRESS_LENGTH];

    DBG("netInit\n");

    /* find a network interface */
    interfaceAddr.addr = findIface(netPath, ptpClock->rtOpts->ifaceName, &ptpClock->portDS.port.communicationTechnology,
                                   ptpClock->portDS.port.uuid);

    if (!(interfaceAddr.addr))
    {

        goto fail01;
    }

    /* Open lwIP raw udp interfaces for the event port. */
    netPath->eventPcb = udp_new();

    if (NULL == netPath->eventPcb)
    {
        ERROR("netInit: Failed to open Event UDP PCB\n");
        goto fail03;
    }

    /* Open lwIP raw udp interfaces for the general port. */
    netPath->generalPcb = udp_new();

    if (NULL == netPath->generalPcb)
    {
        ERROR("netInit: Failed to open General UDP PCB\n");
        goto fail04;
    }

    /* Initialize the buffer queues. */
    netQInit(&netPath->eventQ);

    netQInit(&netPath->generalQ);

    /* Configure network (broadcast/unicast) addresses. */
    netPath->unicastAddr = 0; /* disable unicast */

    if (!lookupSubdomainAddress(ptpClock->rtOpts->subdomainName, addrStr))
    {
        ERROR("netInit: invalid subdomain name: %s\n", ptpClock->rtOpts->subdomainName);
        goto fail05;
    }

    if (!inet_aton(addrStr, &netAddr))
    {
        ERROR("netInit: failed to encode multi-cast address: %s\n", addrStr);
        goto fail05;
    }

    /* Setup subdomain address string. */

    for (i = 0; i < SUBDOMAIN_ADDRESS_LENGTH; ++i)
    {
        ptpClock->portDS.subdomain_address[i] = (netAddr.s_addr >> (i * 8)) & 0xff;
    }

    netPath->multicastAddr = netAddr.s_addr;

    /* multicast send only on specified interface */
    netPath->eventPcb->mcast_ip4.addr = netPath->multicastAddr;
    netPath->generalPcb->mcast_ip4.addr = netPath->multicastAddr;

    /* Establish the appropriate UDP bindings/connections for events. */

    udp_recv(netPath->eventPcb, netRecvEventCallback, netPath);

    udp_bind(netPath->eventPcb, IP_ADDR_ANY, PTP_EVENT_PORT);

    /* udp_connect(netPath->eventPcb, &netAddr, PTP_EVENT_PORT); */
    *(Integer16*)ptpClock->portDS.event_port_address = PTP_EVENT_PORT;

    /* Establish the appropriate UDP bindings/connections for general. */
    udp_recv(netPath->generalPcb, netRecvGeneralCallback, netPath);

    udp_bind(netPath->generalPcb, IP_ADDR_ANY, PTP_GENERAL_PORT);

    /* udp_connect(netPath->generalPcb, &netAddr, PTP_GENERAL_PORT); */
    *(Integer16*)ptpClock->portDS.general_port_address = PTP_GENERAL_PORT;

    /* join multicast group (for receiving) on specified interface */
    igmp_joingroup(&interfaceAddr, (ip_addr_t *)&netAddr);

    /* Return a success code. */
    return TRUE;

fail05:
    udp_disconnect(netPath->eventPcb);
    udp_disconnect(netPath->generalPcb);
    udp_remove(netPath->generalPcb);

fail04:
    udp_remove(netPath->eventPcb);

fail03:
fail01:
    return FALSE;
}

/* Wait for a packet to come in on either port.  For now, there is no wait.
 * Simply check to see if a packet is available on either port and return 1,
 * otherwise return 0. */
int netSelect(NetPath *netPath, const TimeInternal *timeout)
{
    /* Check the packet queues.  If there is data, return TRUE. */
    if (netQCheck(&netPath->eventQ) || netQCheck(&netPath->generalQ))
        return 1;

    return 0;
}

void netEmptyEventQ(NetPath *netPath)
{
    netQEmpty(&netPath->eventQ);
}

static ssize_t netRecv(Octet *buf, TimeInternal *time, BufQueue * msgQueue)
{
    ssize_t length;
    int i, j;

    /* get actual buffer */

    struct pbuf * p, *pcopy;
    p = (struct pbuf*)netQGet(msgQueue);

    if (!p)
    {
        return 0;
    }

    pcopy = p;

    /* Here, p points to a valid PBUF structure.  Verify that we have
     * enough space to store the contents. */

    if (p->tot_len > PACKET_SIZE)
    {
        ERROR("netRecv: received truncated message\n");
        return 0;
    }

    if (NULL != time)
    {
#if LWIP_PTP
        time->seconds = p->time_sec;

        time->nanoseconds = p->time_nsec;
#else
        getTime(time);
#endif
        DBGV("netRecv: %ds %dns\n", time->seconds, time->nanoseconds);
    }
    else
    {
        DBGV("netRecv\n");
    }

    /* Copy the PBUF payload into the buffer. */
    j = 0;

    length = p->tot_len;

    for (i = 0; i < length; i++)
    {
        buf[i] = ((u8_t *)pcopy->payload)[j++];

        if (j == pcopy->len)
        {
            pcopy = pcopy->next;
            j = 0;
        }
    }

    /* Free up the pbuf (chain). */
    pbuf_free(p);

    return length;
}

ssize_t netRecvEvent(NetPath *netPath, Octet *buf, TimeInternal *time)
{
    return netRecv(buf, time, &netPath->eventQ);
}

ssize_t netRecvGeneral(NetPath *netPath, Octet *buf)
{
    return netRecv(buf, NULL, &netPath->generalQ);
}

static ssize_t netSend(const Octet *buf, UInteger16 length, TimeInternal *time, const Integer32 * addr, struct udp_pcb * pcb)
{
    err_t result;

    struct pbuf * p;

    /* Allocate the tx pbuf based on the current size. */
    p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);

    if (NULL == p)
    {
        ERROR("netSend: Failed to allocate Tx Buffer\n");
        goto fail01;
    }

    /* Copy the incoming data into the pbuf payload. */
    result = pbuf_take(p, buf, length);

    if (ERR_OK != result)
    {
        ERROR("netSend: Failed to copy data to Pbuf (%d)\n", result);
        goto fail02;
    }

    /* send the buffer. */
    result = udp_sendto(pcb, p, (void *)addr, pcb->local_port);

    if (ERR_OK != result)
    {
        ERROR("netSend: Failed to send data (%d)\n", result);
        goto fail02;
    }

    if (NULL != time)
    {
#if LWIP_PTP
        time->seconds = p->time_sec;

        time->nanoseconds = p->time_nsec;
#else
        /* TODO: use of loopback mode */
        /*
        time->seconds = 0;
        time->nanoseconds = 0;
        */
        getTime(time);
#endif
        DBGV("netSend: %ds %dns\n", time->seconds, time->nanoseconds);
    }
    else
    {
        DBGV("netSend\n");
    }

fail02:

    pbuf_free(p);

fail01:
    return length;

    /*  return (0 == result) ? length : 0; */
}

ssize_t netSendEvent(NetPath *netPath, const Octet *buf, UInteger16 length, TimeInternal *time)
{
    return netSend(buf, length, time, &netPath->multicastAddr, netPath->eventPcb);
}

ssize_t netSendGeneral(NetPath *netPath, const Octet *buf, UInteger16 length)
{
    return netSend(buf, length, NULL, &netPath->multicastAddr, netPath->generalPcb);
}
