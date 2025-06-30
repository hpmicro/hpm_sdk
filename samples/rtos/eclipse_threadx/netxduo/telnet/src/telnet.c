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
#include "nxd_telnet_server.h"

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
#include "nxd_dhcp_client.h"
#endif

#define DEMO_STACK_SIZE 4096

/* Define packet pool.  */
#define PACKET_SIZE 1536
#define PACKET_COUNT 30
#define PACKET_POOL_SIZE ((PACKET_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)

/* Define IP stack size.   */
#define IP_STACK_SIZE 2048

/* Define IP thread priority.  */
#define IP_THREAD_PRIORITY 1

/* Define thread stack size */
#define DEMO_STACK_SIZE 4096

/* Define ARP pool.  */
#define ARP_POOL_SIZE 1024
/* Define memory buffers.  */
ULONG pool_area[PACKET_POOL_SIZE >> 2];
ULONG ip_stack[IP_STACK_SIZE >> 2];
ULONG arp_area[ARP_POOL_SIZE >> 2];
ULONG thread_usr_stack[DEMO_STACK_SIZE >> 2];
ULONG telnet_stack[DEMO_STACK_SIZE >> 2];
/* Define the ThreadX and NetX object control blocks...  */

TX_THREAD server_thread;
NX_PACKET_POOL pool_server;
NX_IP ip_server;

/* If the Telnet connection requires IPv6 support, define USE_IPV6.  Note that
   the NetX Duo Telnet Client and Server can communicate over IPv4 regardless
   if IPv6 is enabled in NetX Duo. However to use IPv6 addressing, the
   FEATURE_NX_IPV6 must be defined in nx_user.h.  */
#ifdef NX_DISABLE_IPV4
#define USE_IPV6
#endif /* NX_DISABLE_IPV4 */

#ifdef USE_IPV6
NXD_ADDRESS server_ip_address;
#endif

/* Define TELNET objects.  */

NX_TELNET_SERVER my_server;

/* Define sample IP address.  */
#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
#define SERVER_ADDRESS 0.0.0.0
#define SAMPLE_IPV4_MASK 0.0.0.0

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(64) NX_DHCP dhcp_client;
UCHAR ip_address[4];
UCHAR network_mask[4];
#else
#ifndef SERVER_ADDRESS
#define SERVER_ADDRESS 192.168.1.223
#endif
#ifndef SAMPLE_IPV4_MASK
#define SAMPLE_IPV4_MASK 255.255.255.0
#endif
#endif

/* Define the counters used in the demo application...  */

ULONG error_counter;

/* Define timeout in ticks for connecting and sending/receiving data. */

#define TELNET_TIMEOUT (2 * NX_IP_PERIODIC_RATE)

/* Define function prototypes.  */

VOID thread_server_entry(ULONG thread_input);

/* Define the application's TELNET Server callback routines.  */

VOID telnet_new_connection(NX_TELNET_SERVER *server_ptr, UINT logical_connection);
VOID telnet_receive_data(NX_TELNET_SERVER *server_ptr, UINT logical_connection, NX_PACKET *packet_ptr);
VOID telnet_connection_end(NX_TELNET_SERVER *server_ptr, UINT logical_connection);

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
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SERVER_ADDRESS), &ip)) {
        printf("SERVER_ADDRESS(%s) is not correct\n", HPM_STRINGIFY(SERVER_ADDRESS));
        while (1) {
        }
    }
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SAMPLE_IPV4_MASK), &mask)) {
        printf("SAMPLE_IPV4_MASK(%s) is not correct\n", HPM_STRINGIFY(SAMPLE_IPV4_MASK));
        while (1) {
        }
    }
    TX_PARAMETER_NOT_USED(first_unused_memory);
    /* Create the server thread.  */
    tx_thread_create(&server_thread, "server thread", thread_server_entry, 0,
        (VOID *)thread_usr_stack, sizeof(thread_usr_stack),
        4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create packet pool.  */
    nx_packet_pool_create(&pool_server, "Server NetX Packet Pool", 1700, (VOID *)pool_area, sizeof(pool_area));

    /* Create an IP instance.  */
    nx_ip_create(&ip_server, "Server NetX IP Instance", ip,
        mask, &pool_server, _nx_driver_hpm,
        ip_stack, sizeof(ip_stack), 1);

#ifndef NX_DISABLE_IPV4
    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    nx_arp_enable(&ip_server, (VOID *)arp_area, sizeof(arp_area));
    /* Enable ICMP */
    nx_icmp_enable(&ip_server);

#endif /* NX_DISABLE_IPV4  */

    /* Enable TCP processing for both IP instances.  */
    nx_tcp_enable(&ip_server);

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    /* Enable UDP traffic.  */
    status = nx_udp_enable(&ip_server);

    /* Check for UDP enable errors.  */
    if (status)
        error_counter++;
#endif

#ifdef USE_IPV6
    /* Next set the NetX Duo Telnet Server address. */
    server_ip_address.nxd_ip_address.v6[3] = 0x106;
    server_ip_address.nxd_ip_address.v6[2] = 0x0;
    server_ip_address.nxd_ip_address.v6[1] = 0x0000f101;
    server_ip_address.nxd_ip_address.v6[0] = 0x20010db8;
    server_ip_address.nxd_ip_version = NX_IP_VERSION_V6;
#endif

    /* Create the NetX TELNET Server.  */
    status = nx_telnet_server_create(&my_server, "Telnet Server", &ip_server,
        (VOID *)telnet_stack, sizeof(telnet_stack), telnet_new_connection, telnet_receive_data,
        telnet_connection_end);

    /* Check for errors.  */
    if (status)
        error_counter++;

    return;
}

/* Define the Server thread.  */
VOID thread_server_entry(ULONG thread_input)
{

    UINT status;
#ifdef USE_IPV6
    UINT iface_index, address_index;
#endif

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    ULONG actual_status;
    ULONG temp;

    /* Create the DHCP instance.  */
    printf("DHCP In Progress...\r\n");

    nx_dhcp_create(&dhcp_client, &ip_server, "dhcp_client");

    /* Start the DHCP Client.  */
    nx_dhcp_start(&dhcp_client);

    /* Wait util address is solved. */
    status = nx_ip_status_check(&ip_server, NX_IP_ADDRESS_RESOLVED, &actual_status, 8000);

    if (status) {
        /* DHCP Failed...  no IP address! */
        printf("Can't resolve address\r\n");
    } else {
        /* Get IP address. */
        nx_ip_address_get(&ip_server, (ULONG *)&ip_address[0], (ULONG *)&network_mask[0]);

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
    printf("IP address: %s\r\n", HPM_STRINGIFY(SERVER_ADDRESS));
    printf("Mask: %s\r\n", HPM_STRINGIFY(SAMPLE_IPV4_MASK));

#endif

    NX_PARAMETER_NOT_USED(thread_input);

    /* Allow IP thread task to initialize the system. */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);

#ifdef USE_IPV6
    /* Here's where we make the Telnet Server IPv6 enabled. */
    status = nxd_ipv6_enable(&ip_server);

    /* Check for ipv6 enable error.  */
    if (status) {

        error_counter++;
        return;
    }

    status = nxd_icmp_enable(&ip_server);

    /* Check for icmp6 enable error.  */
    if (status) {

        error_counter++;
        return;
    }

    /* Set the link local address with the host MAC address. */
    /* This assumes we are using the primary network interface (index 0). */
    iface_index = 0;

    status = nxd_ipv6_address_set(&ip_server, iface_index, NX_NULL, 10, &address_index);

    /* Check for link local address set error.  */
    if (status) {

        error_counter++;
        return;
    }

    /* Set the host global IP address. We are assuming a 64
       bit prefix here but this can be any value (< 128). */
    status = nxd_ipv6_address_set(&ip_server, iface_index, &server_ip_address, 64, &address_index);

    /* Check for global address set error.  */
    if (status) {

        error_counter++;
        return;
    }

    /* Wait while NetX Duo validates the link local and global address. */
    tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);
#endif

    /* Start the TELNET Server.  */
    status = nx_telnet_server_start(&my_server);

    /* Check for errors.  */
    if (status != NX_SUCCESS) {

        return;
    }
}

/* This routine is called by the NetX Telnet Server whenever a new Telnet client
   connection is established.  */
VOID telnet_new_connection(NX_TELNET_SERVER *server_ptr, UINT logical_connection)
{

    UINT status;
    NX_PACKET *packet_ptr;

    /* Allocate a packet for client greeting. */
    status = nx_packet_allocate(&pool_server, &packet_ptr, NX_TCP_PACKET, NX_NO_WAIT);

    if (status != NX_SUCCESS) {
        error_counter++;
        return;
    }

    /* Build a banner message and a prompt.  */
    nx_packet_data_append(packet_ptr, "**** Welcome to NetX TELNET Server ****\r\n\r\n\r\n", 45,
        &pool_server, NX_NO_WAIT);

    nx_packet_data_append(packet_ptr, "NETX> ", 6, &pool_server, NX_NO_WAIT);

    /* Send the packet to the client.  */
    status = nx_telnet_server_packet_send(server_ptr, logical_connection, packet_ptr, TELNET_TIMEOUT);

    if (status != NX_SUCCESS) {
        error_counter++;
        nx_packet_release(packet_ptr);
    }

    return;
}

/* This routine is called by the NetX Telnet Server whenever data is present on a Telnet client
   connection.  */
VOID telnet_receive_data(NX_TELNET_SERVER *server_ptr, UINT logical_connection, NX_PACKET *packet_ptr)
{

    UINT status;
    UCHAR alpha;

    /* This demo just echoes the character back and on <cr,lf> sends a new prompt back to the
       client.  A real system would most likely buffer the character(s) received in a buffer
       associated with the supplied logical connection and process according to it.  */

    /* Just throw away carriage returns.  */
    if ((packet_ptr->nx_packet_prepend_ptr[0] == '\r') && (packet_ptr->nx_packet_length == 1)) {

        nx_packet_release(packet_ptr);
        return;
    }

    /* Setup new line on line feed.  */
    if ((packet_ptr->nx_packet_prepend_ptr[0] == '\n') ||
        ((packet_ptr->nx_packet_prepend_ptr[0] == '\r') && (packet_ptr->nx_packet_prepend_ptr[1] == '\n'))) {

        /* Clean up the packet.  */
        packet_ptr->nx_packet_length = 0;
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_data_start + NX_TCP_PACKET;
        packet_ptr->nx_packet_append_ptr = packet_ptr->nx_packet_data_start + NX_TCP_PACKET;

        /* Build the next prompt.  */
        nx_packet_data_append(packet_ptr, "\r\nNETX> ", 8, &pool_server, NX_NO_WAIT);

        /* Send the packet to the client.  */
        status = nx_telnet_server_packet_send(server_ptr, logical_connection, packet_ptr, TELNET_TIMEOUT);

        if (status != NX_SUCCESS) {
            error_counter++;
            nx_packet_release(packet_ptr);
        }

        return;
    }

    /* Pickup first character (usually only one from client).  */
    alpha = packet_ptr->nx_packet_prepend_ptr[0];

    /* Echo character.  */
    status = nx_telnet_server_packet_send(server_ptr, logical_connection, packet_ptr, TELNET_TIMEOUT);

    if (status != NX_SUCCESS) {
        error_counter++;
        nx_packet_release(packet_ptr);
    }

    /* Check for a disconnection.  */
    if (alpha == 'q') {

        /* Initiate server disconnection.  */
        nx_telnet_server_disconnect(server_ptr, logical_connection);
    }
}

/* This routine is called by the NetX Telnet Server whenever the client disconnects.  */
VOID telnet_connection_end(NX_TELNET_SERVER *server_ptr, UINT logical_connection)
{
    NX_PARAMETER_NOT_USED(server_ptr);
    NX_PARAMETER_NOT_USED(logical_connection);

    /* Cleanup any application specific connection or buffer information.  */
    return;
}