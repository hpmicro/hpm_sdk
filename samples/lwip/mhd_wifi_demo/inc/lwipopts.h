/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LWIPOPTS_H
#define LWIPOPTS_H

#if defined(__GNUC__) && (__GNUC__ >= 14)
#include "sys/select.h"
#endif

#define LWIP_DNS						1
#define LWIP_NETIF_HOSTNAME				1
#define LWIP_NETIF_STATUS_CALLBACK		1
#define LWIP_IGMP						1
#define LWIP_RAND                       rand

/* MHD WiFi definitions */
#define MHD_MAX_BUS_HEADER_LENGTH      (12)
#define MHD_MAX_SDPCM_HEADER_LENGTH    (30)
#define MHD_ETHERNET_SIZE              (14)
#define MHD_PHYSICAL_HEADER            (MHD_MAX_BUS_HEADER_LENGTH + MHD_MAX_SDPCM_HEADER_LENGTH + MHD_ETHERNET_SIZE)
#define PBUF_LINK_HLEN                 (MHD_PHYSICAL_HEADER)

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT           1
#define ETHARP_TRUST_IP_MAC            0
#define IP_REASSEMBLY                  0
#define IP_FRAG                        0
#define ARP_QUEUEING                   0
#define LWIP_RAW                       1
#define LWIP_IPV4                      1
#define LWIP_TIMERS                    1
#define LWIP_NETIF_TX_SINGLE_PBUF      1


#define DEFAULT_RAW_RECVMBOX_SIZE      1
/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                         0

/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2.
*/
#define MEM_ALIGNMENT                  4

/* MEM_SIZE: the size of the heap memory. If the application will send
   a lot of data that needs to be copied, this should be set high.
*/
#ifndef MEM_SIZE
#define MEM_SIZE                       (512*1024)
#endif

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high.
*/
#define MEMP_OVERFLOW_CHECK            1
#ifndef MEMP_NUM_PBUF
#define MEMP_NUM_PBUF                  256
#endif
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One per active UDP "connection". */
#define MEMP_NUM_UDP_PCB               4
/* MEMP_NUM_TCP_PCB: the number of simultaneously active TCP connections  */
#define MEMP_NUM_TCP_PCB               4
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections. */
#define MEMP_NUM_TCP_PCB_LISTEN        5
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments. */
#ifndef MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG               TCP_SND_QUEUELEN
#endif
/* MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active timeouts. */
#define MEMP_NUM_SYS_TIMEOUT           10
#define MEMP_NUM_NETBUF                512

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE                 (256)
#endif

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE              1600
#endif

/* ---------- TCP options ---------- */
#define LWIP_TCP                       1
#define TCP_TTL                        255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory.
*/
#define TCP_QUEUE_OOSEQ                1

/* TCP Maximum segment size. */
#define TCP_MSS                        (1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */
#define LWIP_WND_SCALE                 1
#define TCP_RCV_SCALE                  10

/* TCP sender buffer space (bytes). */
#ifndef TCP_SND_BUF
#define TCP_SND_BUF                    ((32768)/TCP_MSS*TCP_MSS)
#endif
#define TCP_TMR_INTERVAL               200

/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work.
*/

#define TCP_SND_QUEUELEN               (4* TCP_SND_BUF/TCP_MSS)

/* TCP receive window. */
#ifndef TCP_WND
#define TCP_WND                        ((32768)/TCP_MSS*TCP_MSS)
#endif

#define TCP_SNDLOWAT                   (TCP_MSS*4)
#define TCP_SNDQUEUELOWAT              TCP_SND_QUEUELEN/2
/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX                     12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX                  4


/* ---------- ICMP options ---------- */
#define LWIP_ICMP                      1
#define ICMP_TTL                       64

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work.
*/
#ifndef LWIP_DHCP
#define LWIP_DHCP                      0
#endif

/* ---------- UDP options ---------- */
#define LWIP_UDP                       1
#define UDP_TTL                        255


/* ---------- Statistics options ---------- */
#define LWIP_STATS                     1
#define LWIP_PROVIDE_ERRNO             1

/* ---------- link callback options ---------- */
/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK       1

#ifndef MEMP_NUM_TCPIP_MSG_INPKT
#define MEMP_NUM_TCPIP_MSG_INPKT       16
#endif

/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/

/**
 * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */
#define LWIP_NETIF_API                    1

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

/*
Some MCUs allow computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the checksum.
*/

#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               0
#else
/* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
#define CHECKSUM_GEN_IP                 1
/* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP                1
/* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP                1
/* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
#define CHECKSUM_CHECK_IP               1
/* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP              1
/* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP              1
/* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
#define CHECKSUM_GEN_ICMP               1
#endif


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    1

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                    1
#define LWIP_SOCKET_SELECT             1
#define LWIP_SOCKET_POLL               1

/*
   -----------------------------------
   ---------- DEBUG options ----------
   -----------------------------------
*/
#define MEMP_STATS                      1

#define LWIP_DEBUG                     1

#ifdef LWIP_DEBUG

#define LWIP_DBG_MIN_LEVEL             0
#define PPP_DEBUG                  LWIP_DBG_OFF
#define MEM_DEBUG                  LWIP_DBG_OFF
#define MEMP_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                 LWIP_DBG_OFF
#define API_LIB_DEBUG              LWIP_DBG_OFF
#define API_MSG_DEBUG              LWIP_DBG_OFF
#define TCPIP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                LWIP_DBG_OFF
#define SOCKETS_DEBUG              LWIP_DBG_OFF
#define DNS_DEBUG                  LWIP_DBG_OFF
#define AUTOIP_DEBUG               LWIP_DBG_OFF
#define DHCP_DEBUG                 LWIP_DBG_OFF
#define IP_DEBUG                   LWIP_DBG_OFF
#define IP_REASS_DEBUG             LWIP_DBG_OFF
#define ICMP_DEBUG                 LWIP_DBG_OFF
#define IGMP_DEBUG                 LWIP_DBG_OFF
#define UDP_DEBUG                  LWIP_DBG_OFF
#define TCP_DEBUG                  LWIP_DBG_OFF
#define TCP_INPUT_DEBUG            LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG           LWIP_DBG_OFF
#define TCP_RTO_DEBUG              LWIP_DBG_OFF
#define TCP_CWND_DEBUG             LWIP_DBG_OFF
#define TCP_WND_DEBUG              LWIP_DBG_OFF
#define TCP_FR_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG             LWIP_DBG_OFF
#define TCP_RST_DEBUG              LWIP_DBG_OFF
#define ETHARP_DEBUG               LWIP_DBG_OFF
#endif


/*
   ---------------------------------
   ---------- OS options ----------
   ---------------------------------
*/
#include "FreeRTOSConfig.h"
#define TCPIP_THREAD_NAME              "TCP/IP"
#define TCPIP_THREAD_STACKSIZE          8192
#define TCPIP_MBOX_SIZE                 64
#define DEFAULT_UDP_RECVMBOX_SIZE       1024
#define DEFAULT_TCP_RECVMBOX_SIZE       1024
#define DEFAULT_ACCEPTMBOX_SIZE         10
#define DEFAULT_THREAD_STACKSIZE        4096
#define TCPIP_THREAD_PRIO               (configMAX_PRIORITIES - 4)
#define LWIP_SINGLE_NETIF               0
#define LWIP_COMPAT_MUTEX               1
#define LWIP_COMPAT_MUTEX_ALLOWED       1
#define LWIP_SO_RCVTIMEO                1
#define LWIP_SO_SNDTIMEO                1
#define xLWIP_TCP_KEEPALIVE             1

#endif /* __LWIPOPTS_H__ */
