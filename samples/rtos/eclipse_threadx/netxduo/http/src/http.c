/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "nx_api.h"
#include "tx_api.h"
#include <stdio.h>
#include "hpmicro_netx_driver.h"
/* If not using FileX, define this option and define the file writing services
   declared in filex_stub.h.
#define      NX_HTTP_NO_FILEX
*/
#ifndef NX_HTTP_NO_FILEX
#include "fx_api.h"
#else
#include "filex_stub.h"
#endif
#include "nxd_http_server.h"

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
#include "nxd_dhcp_client.h"
#endif

/* For NetX Duo applications, determine which IP version to use. For IPv6,
   set IP_TYPE to 6; for IPv4 set to 4. Note that for IPv6, you must enable
   USE_DUO so the application 'knows' to enabled IPv6 services on the IP task.  */

#ifdef NX_DISABLE_IPV4
#define IP_TYPE 6
#else
#define IP_TYPE 4
#endif /* NX_DISABLE_IPV4 */

#define DEMO_STACK_SIZE 4096

/* Set up FileX and file memory resources. */
UCHAR ram_disk_memory[32000];
FX_MEDIA ram_disk;
UCHAR media_memory[512];

/* Define device drivers.  */
extern VOID _fx_ram_driver(FX_MEDIA *media_ptr);

UINT authentication_check(NX_HTTP_SERVER *server_ptr, UINT request_type, CHAR *resource, CHAR **name, CHAR **password, CHAR **realm);

/* Set up the HTTP server global variables */

NX_HTTP_SERVER my_server;
NX_PACKET_POOL server_pool;
TX_THREAD server_thread;
NX_IP server_ip;
NXD_ADDRESS server_ip_address;
#define SERVER_PACKET_SIZE (NX_HTTP_SERVER_MIN_PACKET_SIZE * 2)

/* Define packet pool.  */
#define PACKET_COUNT 30
#define SERVER_PACKET_POOL_SIZE ((SERVER_PACKET_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)

ULONG server_thread_stack[DEMO_STACK_SIZE >> 2];
ULONG server_pool_area[SERVER_PACKET_POOL_SIZE >> 2];
ULONG server_ip_stack[4096 >> 2];
ULONG server_arp_area[1024 >> 2];
ULONG server_stack_area[2048 >> 2];

VOID thread_server_entry(ULONG thread_input);
/* Define sample IP address.  */
#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
#define SAMPLE_IPV4_ADDRESS 0.0.0.0
#define SAMPLE_IPV4_MASK 0.0.0.0

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(64) NX_DHCP dhcp_client;
UCHAR ip_address[4];
UCHAR network_mask[4];
#else

#ifndef SAMPLE_IPV4_ADDRESS
#define SAMPLE_IPV4_ADDRESS 192.168.1.223
#endif
#ifndef SAMPLE_IPV4_MASK
#define SAMPLE_IPV4_MASK 255.255.255.0
#endif
#endif

/* Define the application's authentication check.  This is called by
   the HTTP server whenever a new request is received.  */
UINT authentication_check(NX_HTTP_SERVER *server_ptr, UINT request_type, CHAR *resource, CHAR **name, CHAR **password, CHAR **realm)
{
    NX_PARAMETER_NOT_USED(server_ptr);
    NX_PARAMETER_NOT_USED(request_type);
    NX_PARAMETER_NOT_USED(resource);

    /* Just use a simple name, password, and realm for all
       requests and resources.  */
    *name = "name";
    *password = "password";
    *realm = "NetX Duo HTTP demo";

    /* Request basic authentication.  */
    return NX_HTTP_BASIC_AUTHENTICATE;
}

/* Define main entry point.  */

INT main(VOID)
{
    board_init();

    /* Initialize GPIOs */
    board_init_enet_pins(ENET);

    /* Reset an enet PHY */
    board_reset_enet_phy(ENET);

/* Set RGMII clock delay */
#if defined(RGMII) && RGMII
    board_init_enet_rgmii_clock_delay(ENET);
#elif defined(RMII) && RMII
    /* Set RMII reference clock */
    board_init_enet_rmii_reference_clock(ENET, BOARD_ENET_RMII_INT_REF_CLK);
    printf("Reference Clock: %s\n", BOARD_ENET_RMII_INT_REF_CLK ? "Internal Clock" : "External Clock");
#endif

    /* Start a board timer */
    board_timer_create(2000, sys_timer_callback);

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}


/* Define what the initial system looks like.  */
VOID tx_application_define(VOID *first_unused_memory)
{
    UINT ip, mask;
    UINT status;

    TX_PARAMETER_NOT_USED(first_unused_memory);
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SAMPLE_IPV4_ADDRESS), &ip)) {
        printf("SAMPLE_IPV4_ADDRESS(%s) is not correct\n", HPM_STRINGIFY(SAMPLE_IPV4_ADDRESS));
        while (1) {
        }
    }
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SAMPLE_IPV4_MASK), &mask)) {
        printf("SAMPLE_IPV4_MASK(%s) is not correct\n", HPM_STRINGIFY(SAMPLE_IPV4_MASK));
        while (1) {
        }
    }
    /* Create a helper thread for the server. */
    tx_thread_create(&server_thread, "HTTP Server thread", thread_server_entry, 0, server_thread_stack, DEMO_STACK_SIZE, 4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create the server packet pool.  */
    status = nx_packet_pool_create(&server_pool, "HTTP Server Packet Pool", SERVER_PACKET_SIZE, server_pool_area, sizeof(server_pool_area));

    /* Check for pool creation error.  */
    if (status) {

        return;
    }

    /* Create an IP instance.  */
    status = nx_ip_create(&server_ip, "HTTP Server IP", ip, mask, &server_pool, _nx_driver_hpm, server_ip_stack, 4096, 1);

    /* Check for IP create errors.  */
    if (status) {
        return;
    }

#ifndef NX_DISABLE_IPV4
    /* Enable ARP and supply ARP cache memory for the server IP instance.  */
    status = nx_arp_enable(&server_ip, (VOID *)server_arp_area, 1024);

    /* Check for ARP enable errors.  */
    if (status) {
        return;
    }

    /* Enable ARP and supply ARP cache memory for the server IP instance.  */
    status = nx_icmp_enable(&server_ip);

    /* Check for ARP enable errors.  */
    if (status) {
        return;
    }

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    /* Enable UDP traffic.  */
    status = nx_udp_enable(&server_ip);

    /* Check for UDP enable errors.  */
    if (status) {
        return;
    }
#endif
#endif /* NX_DISABLE_IPV4  */

    /* Enable TCP traffic.  */
    status = nx_tcp_enable(&server_ip);

    if (status) {
        return;
    }
#if (IP_TYPE == 6)

    /* Set up the server's IPv6 address here. */
    server_ip_address.nxd_ip_address.v6[3] = 0x105;
    server_ip_address.nxd_ip_address.v6[2] = 0x0;
    server_ip_address.nxd_ip_address.v6[1] = 0x0000f101;
    server_ip_address.nxd_ip_address.v6[0] = 0x20010db8;
    server_ip_address.nxd_ip_version = NX_IP_VERSION_V6;
#else

    /* Set up the server's IPv4 address here. */
    server_ip_address.nxd_ip_address.v4 = ip;
    server_ip_address.nxd_ip_version = NX_IP_VERSION_V4;

#endif /* (IP_TYPE==6) */

    /* Create the HTTP Server.  */
    status = nx_http_server_create(&my_server, "My HTTP Server", &server_ip, &ram_disk, server_stack_area, 2048, &server_pool, authentication_check, NX_NULL);
    if (status) {
        return;
    }
}

/* Define the helper HTTP server thread.  */
VOID thread_server_entry(ULONG thread_input)
{

    UINT status;
#if (IP_TYPE == 6)
    UINT iface_index;
    UINT address_index;
#endif

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    ULONG actual_status;
    ULONG temp;

    /* Create the DHCP instance.  */
    printf("DHCP In Progress...\r\n");

    nx_dhcp_create(&dhcp_client, &server_ip, "dhcp_client");

    /* Start the DHCP Client.  */
    nx_dhcp_start(&dhcp_client);

    /* Wait util address is solved. */
    status = nx_ip_status_check(&server_ip, NX_IP_ADDRESS_RESOLVED, &actual_status, 8000);

    if (status) {
        /* DHCP Failed...  no IP address! */
        printf("Can't resolve address\r\n");
    } else {
        /* Get IP address. */
        nx_ip_address_get(&server_ip, (ULONG *)&ip_address[0], (ULONG *)&network_mask[0]);

        /* Convert IP address & network mask from little endian.  */
        temp = *((ULONG *)&ip_address[0]);
        NX_CHANGE_ULONG_ENDIAN(temp);
        *((ULONG *)&ip_address[0]) = temp;

        temp = *((ULONG *)&network_mask[0]);
        NX_CHANGE_ULONG_ENDIAN(temp);
        *((ULONG *)&network_mask[0]) = temp;

        /* Output IP address. */
        printf("IP address: %d.%d.%d.%d\r\nMask: %d.%d.%d.%d\r\n", (UINT)(ip_address[0]), (UINT)(ip_address[1]),
               (UINT)(ip_address[2]), (UINT)(ip_address[3]), (UINT)(network_mask[0]), (UINT)(network_mask[1]),
               (UINT)(network_mask[2]), (UINT)(network_mask[3]));
    }
#else
    /* Output IP address and network mask.  */
    printf("NetXDuo is running\r\n");
    printf("IP address: %s\r\n", HPM_STRINGIFY(SAMPLE_IPV4_ADDRESS));
    printf("Mask: %s\r\n", HPM_STRINGIFY(SAMPLE_IPV4_MASK));

#endif
    NX_PARAMETER_NOT_USED(thread_input);

    /* Give NetX a chance to initialize the system. */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);

#if (IP_TYPE == 6)

    /* Here's where we make the HTTP server IPv6 enabled. */

    /* Enable ICMPv6 services for the Server. */
    status = nxd_icmp_enable(&server_ip);
    if (status != NX_SUCCESS) {
        return;
    }

    /* Enable IPv6 services for the Server. */
    status = nxd_ipv6_enable(&server_ip);
    if (status != NX_SUCCESS) {
        return;
    }

    /* Register the Server link local and global IPv6 address with NetX Duo. */

    /* This assumes we are using the Server primary interface. See the NetX Duo
       User Guide for more information on address configuration. */

    iface_index = 0;
    status = nxd_ipv6_address_set(&server_ip, iface_index, NX_NULL, 10, &address_index);
    status += nxd_ipv6_address_set(&server_ip, iface_index, &server_ip_address, 64, &address_index);

    if (status != NX_SUCCESS) {
        return;
    }

    /* Wait for DAD to validate Server address. */
    tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);

#endif /* (IP_TYPE == 6) */

    /* OK to start the HTTP Server.   */
    status = nx_http_server_start(&my_server);

    if (status != NX_SUCCESS) {
        return;
    }

    /* HTTP server ready to take requests! */

    /* Let the IP thread execute.    */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);

    return;
}