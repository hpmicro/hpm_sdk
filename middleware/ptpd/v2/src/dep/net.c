/* net.c */

#include "../ptpd.h"

/**
  * @brief  Initialize network queue
  * @param  pQ the queue to be initialized
  * @retval None
  */
static void netQInit(BufQueue *pQ)
{
    pQ->get = 0;
    pQ->put = 0;
    pQ->count = 0;
}

/**
  * @brief  Put data to the network queue
  * @param  pQ the queue to be used
  * @param  pbuf the packet to be put to the queue
  * @retval Boolean success
  */
static Boolean netQPut(BufQueue *pQ, void *pbuf)
{
    if (pQ->count >= PBUF_QUEUE_SIZE)
        return FALSE;

    pQ->pbuf[pQ->put] = pbuf;

    pQ->put = (pQ->put + 1) % PBUF_QUEUE_SIZE;

    pQ->count++;

    return TRUE;
}

/**
  * @brief  Get data from the network queue
  * @param  pQ the queue to be used
  * @retval void* pointer to pbuf or NULL
  */
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

/**
  * @brief  Free all pbufs in the queue
  * @param  pQ the queue to be used
  * @retval None
  */
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

/**
  * @brief  Check if something is in the queue
  * @param  pQ the queue to be used
  * @retval Boolean TRUE if success
  */
static Boolean netQCheck(BufQueue *pQ)
{
    if (!pQ->count)
        return FALSE;

    return TRUE;
}

/**
  * @brief  shut down the UDP and network stuff
  * @retval TRUE if success
  */
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

    netPath->multicastAddr = 0;

    netPath->unicastAddr = 0;

    /* Clear the network addresses. */
    netPath->multicastAddr = 0;
    netPath->unicastAddr = 0;

    /* Return a success code. */
    return TRUE;
}

/**
  * @brief  Find interface to be used
  * @param  ifaceName name of the required interface (will be filled with appropriate interface name if not set)
  * @param  uuid will be filled with MAC address of the interface
  * @param  netPath network object
  * @retval Integer32 IPv4 address of the interface
  */
static Integer32 findIface(const Octet *ifaceName, Octet *uuid, NetPath *netPath)
{

    struct netif * iface;
    iface = netif_default;
    memcpy(uuid, iface->hwaddr, iface->hwaddr_len);
    return iface->ip_addr.addr;
}

/**
  * @brief  Processing an incoming message on the Event port.
  * @param  arg the user argument
  * @param  pcb the tcp_pcb that has received the data
  * @param  p the packet buffer
  * @param  addr the addres of sender
  * @param  port the port number of sender
  * @retval None
  */
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

/**
  * @brief  Processing an incoming message on the General port.
  * @param  arg the user argument
  * @param  pcb the tcp_pcb that has received the data
  * @param  p the packet buffer
  * @param  addr the addres of sender
  * @param  port the port number of sender
  * @retval None
  */
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

/**
  * @brief  Start all of the UDP stuff
  * @param  netPath network object
  * @param  ptpClock PTP clock object
  * @retval Boolean success
  */
Boolean netInit(NetPath *netPath, PtpClock *ptpClock)
{

    ip_addr_t interfaceAddr;

    struct in_addr netAddr;
    char addrStr[NET_ADDRESS_LENGTH];

    DBG("netInit\n");

    /* find a network interface */
    interfaceAddr.addr = findIface(ptpClock->rtOpts->ifaceName, ptpClock->portUuidField, netPath);

    if (!(interfaceAddr.addr))
    {

        goto fail01;
    }

    /* Open lwIP raw udp interfaces for the event port. */
    netPath->eventPcb = udp_new();

    if (NULL == netPath->eventPcb)
    {
        ERROR("netInit: Failed to open Event UDP PCB\n");
        goto fail02;
    }

    /* Open lwIP raw udp interfaces for the general port. */
    netPath->generalPcb = udp_new();

    if (NULL == netPath->generalPcb)
    {
        ERROR("netInit: Failed to open General UDP PCB\n");
        goto fail03;
    }

    /* Initialize the buffer queues. */
    netQInit(&netPath->eventQ);

    netQInit(&netPath->generalQ);

    /* Configure network (broadcast/unicast) addresses. */
    netPath->unicastAddr = 0; /* disable unicast */

    /*Init General multicast IP address*/
    memcpy(addrStr, DEFAULT_PTP_DOMAIN_ADDRESS, NET_ADDRESS_LENGTH);

    if (!inet_aton(addrStr, &netAddr))
    {
        ERROR("netInit: failed to encode multi-cast address: %s\n", addrStr);
        goto fail04;
    }

    netPath->multicastAddr = netAddr.s_addr;

    /* join multicast group (for receiving) on specified interface */
    igmp_joingroup(&interfaceAddr, (ip_addr_t *)&netAddr);


    /*Init Peer multicast IP address*/
    memcpy(addrStr, PEER_PTP_DOMAIN_ADDRESS, NET_ADDRESS_LENGTH);

    if (!inet_aton(addrStr, &netAddr))
    {
        ERROR("netInit: failed to encode peer multi-cast address: %s\n", addrStr);
        goto fail04;
    }

    netPath->peerMulticastAddr = netAddr.s_addr;

    /* join peer multicast group (for receiving) on specified interface */
    igmp_joingroup(&interfaceAddr, (ip_addr_t *)&netAddr);


    /* Establish the appropriate UDP bindings/connections for events. */
    udp_recv(netPath->eventPcb, netRecvEventCallback, netPath);
    udp_bind(netPath->eventPcb, IP_ADDR_ANY, PTP_EVENT_PORT);
    /*  udp_connect(netPath->eventPcb, &netAddr, PTP_EVENT_PORT); */

    /* Establish the appropriate UDP bindings/connections for general. */
    udp_recv(netPath->generalPcb, netRecvGeneralCallback, netPath);
    udp_bind(netPath->generalPcb, IP_ADDR_ANY, PTP_GENERAL_PORT);
    /*  udp_connect(netPath->generalPcb, &netAddr, PTP_GENERAL_PORT); */

    /* Return a success code. */
    return TRUE;

    /*
    fail05:
        udp_disconnect(netPath->eventPcb);
        udp_disconnect(netPath->generalPcb);
    */
fail04:
    udp_remove(netPath->generalPcb);
fail03:
    udp_remove(netPath->eventPcb);
fail02:
fail01:
    return FALSE;
}

/**
  * @brief  Wait for a packet to come in on either port.  For now, there is no wait.
  * Simply check to see if a packet is available on either port and return 1,
  * otherwise return 0.
  * @param  netPath network object
  * @param  timeout not used
  * @retval Integer32 number > 0 if there are some data
  */
Integer32 netSelect(NetPath *netPath, const TimeInternal *timeout)
{
    /* Check the packet queues.  If there is data, return TRUE. */
    if (netQCheck(&netPath->eventQ) || netQCheck(&netPath->generalQ))
        return 1;

    return 0;
}

/**
  * @brief  Delete all waiting packets in Event queue
  * @param  netPath network object
  * @retval None
  */
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

ssize_t netRecvGeneral(NetPath *netPath, Octet *buf, TimeInternal *time)
{
    return netRecv(buf, time, &netPath->generalQ);
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
    } else {
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

ssize_t netSendPeerGeneral(NetPath *netPath, const Octet *buf, UInteger16 length)
{
    return netSend(buf, length, NULL, &netPath->peerMulticastAddr, netPath->generalPcb);
}

ssize_t netSendPeerEvent(NetPath *netPath, const Octet *buf, UInteger16 length, TimeInternal* time)
{
    return netSend(buf, length, time, &netPath->peerMulticastAddr, netPath->eventPcb);
}
