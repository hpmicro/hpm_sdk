/* constants_dep.h */

#ifndef CONSTANTS_DEP_H
#define CONSTANTS_DEP_H

/**
 *\file
 * \brief Plateform-dependent constants definition
 *
 * This header defines all includes and constants which are plateform-dependent
 *
 */

/* platform dependent */

#define IF_NAMESIZE             2
#define INET_ADDRSTRLEN         16


 /* #define PTPD_DBGVV */
 /* #define PTPD_DBGV */
 #define PTPD_DBG 
 #define PTPD_ERR 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "lwip/mem.h"
#include "lwip/udp.h"
#include "lwip/igmp.h"
#include "lwip/arch.h"
#include "lwip/inet.h"
#include "ethernetif.h"
#include "lwip.h"
#include "hpm_enet_drv.h"

#ifndef stdout
#define stdout 1
#endif

#define IFACE_NAME_LENGTH         IF_NAMESIZE
#define NET_ADDRESS_LENGTH        INET_ADDRSTRLEN

#define IFCONF_LENGTH 10

#if BYTE_ORDER == LITTLE_ENDIAN
#define PTPD_LSBF
#elif BYTE_ORDER == BIG_ENDIAN
#define PTPD_MSBF
#endif

/* pow2ms(a) = round(pow(2,a)*1000) */

#define pow2ms(a) (((a)>0) ? (1000 << (a)) : (1000 >>(-(a))))

#define ADJ_FREQ_MAX  1024000

/* UDP/IPv4 dependent */

#define SUBDOMAIN_ADDRESS_LENGTH  4
#define PORT_ADDRESS_LENGTH       2
#define PTP_UUID_LENGTH     NETIF_MAX_HWADDR_LEN
#define CLOCK_IDENTITY_LENGTH   8
#define FLAG_FIELD_LENGTH    2

#define PACKET_SIZE  300 /* ptpdv1 value kept because of use of TLV... */

#define PTP_EVENT_PORT    319
#define PTP_GENERAL_PORT  320

#define DEFAULT_PTP_DOMAIN_ADDRESS     "224.0.1.129"
#define PEER_PTP_DOMAIN_ADDRESS        "224.0.0.107"

#define MM_STARTING_BOUNDARY_HOPS  0x7fff

#define PBUF_QUEUE_SIZE 4

/* others */

#define SCREEN_BUFSZ  128
#define SCREEN_MAXSZ  80

#endif /*CONSTANTS_DEP_H_*/

