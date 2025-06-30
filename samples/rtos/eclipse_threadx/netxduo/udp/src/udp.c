/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpmicro_netx_driver.h"
#include "nx_api.h"
#include "tx_api.h"
#include <stdio.h>

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
#include "nxd_dhcp_client.h"
#endif

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

/* Define ECHO server address and port.  */
#ifndef ECHO_SERVER_ADDRESS
#define ECHO_SERVER_ADDRESS 192.168.1.2
#endif
#ifndef ECHO_SERVER_PORT
#define ECHO_SERVER_PORT 7777
#endif
#define ECHO_DATA "ABCDEFGHIJKLMNOPQRSTUVWXYZ "
#define ECHO_RECEIVE_TIMEOUT NX_IP_PERIODIC_RATE
#ifndef ECHO_SEND_PORT
#define ECHO_SEND_PORT 7777
#endif

/* Define packet pool.  */
#define PACKET_SIZE 1536
#define PACKET_COUNT 30
#define PACKET_POOL_SIZE ((PACKET_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)

/* Define IP stack size.   */
#define IP_STACK_SIZE 2048

/* Define IP thread priority.  */
#define IP_THREAD_PRIORITY 1

/* Define stack size of sample thread.  */
#define SAMPLE_THREAD_STACK_SIZE 2048

/* Define priority of sample thread.  */
#define SAMPLE_THREAD_PRIORITY 4

/* Define ARP pool.  */
#define ARP_POOL_SIZE 1024

/* Define UDP socket TTL and receive queue size.  */
#define SAMPLE_SOCKET_TTL 0x80
#define SAMPLE_SOCKET_RX_QUEUE_MAXIMUM 5

/* Define IP stack size.   */
#define IP_STACK_SIZE 2048

/* Define IP thread priority.  */
#define IP_THREAD_PRIORITY 1

/* Define ARP pool.  */
#define ARP_POOL_SIZE 1024

/* Define the ThreadX and NetX object control blocks...  */
NX_PACKET_POOL default_pool;
NX_IP default_ip;
NX_UDP_SOCKET udp_client;
TX_THREAD client_thread;

/* Define memory buffers.  */
ULONG pool_area[PACKET_POOL_SIZE >> 2];
ULONG ip_stack[IP_STACK_SIZE >> 2];
ULONG arp_area[ARP_POOL_SIZE >> 2];
ULONG client_thread_stack[SAMPLE_THREAD_STACK_SIZE >> 2];

/* Define the counters used in the demo application...  */
ULONG error_counter;

VOID client_thread_entry(ULONG thread_input);
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
    NX_PARAMETER_NOT_USED(first_unused_memory);
    UINT ip, mask;
    UINT status;
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

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create the sample thread.  */
     tx_thread_create(&client_thread, "Client Thread", client_thread_entry, 0,
                      (VOID *)client_thread_stack, sizeof(client_thread_stack),
                      SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Create a packet pool.  */
    status = nx_packet_pool_create(&default_pool, "NetX Main Packet Pool",
        PACKET_SIZE, (VOID *)pool_area, sizeof(pool_area));

    /* Check for packet pool create errors.  */
    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&default_ip, "NetX IP Instance 0", ip, mask,
        &default_pool, _nx_driver_hpm,
        (VOID *)ip_stack, sizeof(ip_stack), IP_THREAD_PRIORITY);

    /* Check for IP create errors.  */
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&default_ip, (VOID *)arp_area, sizeof(arp_area));

    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP */
    status = nx_icmp_enable(&default_ip);

    /* Check for ICMP enable errors.  */
    if (status)
        error_counter++;

    /* Enable UDP */
    status = nx_udp_enable(&default_ip);

    /* Check for UDP enable errors.  */
    if (status)
        error_counter++;

    assert(error_counter == 0);
    /* Output IP address and network mask.  */
    printf("NetXDuo is running\r\n");

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    /* Enable UDP traffic.  */
    status = nx_udp_enable(&default_ip);

    /* Check for UDP enable errors.  */
    if (status)
        error_counter++;
#endif
}

/* Client thread entry.  */
VOID client_thread_entry(ULONG thread_input)
{
    static UINT cnt = 0;
    UINT status;
    NX_PACKET *packet_ptr;
    NXD_ADDRESS echo_server_address;
    TX_PARAMETER_NOT_USED(thread_input);
    UINT server_ip;
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(ECHO_SERVER_ADDRESS), &server_ip)) {
        printf("ECHO_SERVER_ADDRESS(%s) is not correct\n", HPM_STRINGIFY(ECHO_SERVER_ADDRESS));
        while (1) {
        }
    }
#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    ULONG actual_status;
    ULONG temp;

    /* Create the DHCP instance.  */
    printf("DHCP In Progress...\r\n");

    nx_dhcp_create(&dhcp_client, &default_ip, "dhcp_client");

    /* Start the DHCP Client.  */
    nx_dhcp_start(&dhcp_client);

    /* Wait util address is solved. */
    status = nx_ip_status_check(&default_ip, NX_IP_ADDRESS_RESOLVED, &actual_status, 8000);

    if (status) {
        /* DHCP Failed...  no IP address! */
        printf("Can't resolve address\r\n");
    } else {
        /* Get IP address. */
        nx_ip_address_get(&default_ip, (ULONG *)&ip_address[0], (ULONG *)&network_mask[0]);

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
    /* Set echo server address.  */
    echo_server_address.nxd_ip_version = NX_IP_VERSION_V4;
    echo_server_address.nxd_ip_address.v4 = server_ip;

    /* Create a UDP socket.  */
    status = nx_udp_socket_create(&default_ip, &udp_client, "UDP Echo Client", NX_IP_NORMAL, NX_FRAGMENT_OKAY,
        SAMPLE_SOCKET_TTL, SAMPLE_SOCKET_RX_QUEUE_MAXIMUM);

    /* Check status.  */
    if (status) {
        error_counter++;
        return;
    }

    /* Bind the UDP socket to any port.  */
    status = nx_udp_socket_bind(&udp_client, ECHO_SEND_PORT, NX_WAIT_FOREVER);

    /* Check status.  */
    if (status) {
        error_counter++;
        return;
    }

    /* Loop to send data to echo server.  */
    for (;;) {
        /* Allocate a packet.  */
        status = nx_packet_allocate(&default_pool, &packet_ptr, NX_UDP_PACKET, NX_WAIT_FOREVER);

        /* Check status.  */
        if (status != NX_SUCCESS) {
            error_counter++;
            break;
        }

        /* Write ABCs into the packet payload.  */
        status = nx_packet_data_append(packet_ptr, ECHO_DATA, sizeof(ECHO_DATA), &default_pool, NX_WAIT_FOREVER);

        /* Check status.  */
        if (status != NX_SUCCESS) {
            error_counter++;
            break;
        }

        /* Send data to echo server.  */
        status = nxd_udp_socket_send(&udp_client, packet_ptr, &echo_server_address, ECHO_SERVER_PORT);

        cnt++;
        /* Check status.  */
        if (status != NX_SUCCESS) {
            nx_packet_release(packet_ptr);
            error_counter++;
            break;
        }

        /* Receive data from echo server.  */
        status = nx_udp_socket_receive(&udp_client, &packet_ptr, ECHO_RECEIVE_TIMEOUT);
        if (status) {
            /* No response received.  */
            printf("No response from server %d\n", cnt);
        } else {

            /* Response received from server.  */
            printf("Receive response: %.*s %d\n",
                (INT)(packet_ptr->nx_packet_length), packet_ptr->nx_packet_prepend_ptr, cnt);
            nx_packet_release(packet_ptr);

            /* Sleep 1 second.  */
            tx_thread_sleep(NX_IP_PERIODIC_RATE);
        }
    }
    printf("ERROR occurred %d\n", error_counter);
    /* Cleanup the UDP socket.  */
    nx_udp_socket_unbind(&udp_client);
    nx_udp_socket_delete(&udp_client);
}