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

#define SAMPLE_PRIMARY_INTERFACE 0
#ifndef ECHO_SERVER_PORT
#define ECHO_SERVER_PORT 7777
#endif
#define ECHO_DATA "ABCDEFGHIJKLMNOPQRSTUVWXYZ "
#define ECHO_RECEIVE_TIMEOUT NX_IP_PERIODIC_RATE
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
/* Define TCP socket TTL and window size.  */
#define SAMPLE_SOCKET_TTL 0x80
#define SAMPLE_SOCKET_WINDOW_SIZE 65535
/* Define time wait for IPv6 DAD process.  */
#define SAMPLE_DAD_WAIT (3 * NX_IP_PERIODIC_RATE)

/* Define the ThreadX and NetX object control blocks...  */
NX_PACKET_POOL default_pool;
NX_IP default_ip;
NX_TCP_SOCKET server_socket;
TX_THREAD client_thread;
ULONG thread_1_counter;
/* Define memory buffers.  */
ULONG pool_area[PACKET_POOL_SIZE >> 2];
ULONG ip_stack[IP_STACK_SIZE >> 2];
ULONG arp_area[ARP_POOL_SIZE >> 2];
ULONG client_thread_stack[SAMPLE_THREAD_STACK_SIZE >> 2];
/* Define the counters used in the demo application...  */
ULONG error_counter;

/* Define thread prototypes.  */
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
    UINT ip, mask;
    UINT status;

    NX_PARAMETER_NOT_USED(first_unused_memory);
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
        SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY,
        TX_NO_TIME_SLICE, TX_AUTO_START);
    /* Create a packet pool.  */
    status = nx_packet_pool_create(&default_pool, "NetX Main Packet Pool",
        PACKET_SIZE, (VOID *)pool_area, sizeof(pool_area));
    /* Check for packet pool create errors.  */
    if (status)
        error_counter++;
    /* Create an IP instance.  */
    status = nx_ip_create(&default_ip, "NetX IP Instance 0",
        ip, mask,
        &default_pool, _nx_driver_hpm,
        (VOID *)ip_stack, sizeof(ip_stack),
        IP_THREAD_PRIORITY);
    /* Check for IP create errors.  */
    if (status)
        error_counter++;
    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&default_ip, (VOID *)arp_area, sizeof(arp_area));
    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP for both ICMPv4 and ICMPv6. */
    status = nxd_icmp_enable(&default_ip);
    /* Check for ICMP enable errors.  */
    if (status)
        error_counter++;
    /* Enable TCP */
    status = nx_tcp_enable(&default_ip);
    /* Check for TCP enable errors.  */
    if (status)
        error_counter++;

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    /* Enable UDP traffic.  */
    status = nx_udp_enable(&default_ip);

    /* Check for UDP enable errors.  */
    if (status)
        error_counter++;
#endif
}

VOID thread_1_connect_received(NX_TCP_SOCKET *server_socket, UINT port);
VOID thread_1_disconnect_received(NX_TCP_SOCKET *server_socket);
/* Client thread entry.  */
VOID client_thread_entry(ULONG thread_input)
{
    UINT status;
    NX_PACKET *packet_ptr, *packet_ptr2;
    ULONG actual_status;
    ULONG length;

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
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
    NX_PARAMETER_NOT_USED(thread_input);

#ifndef NX_DISABLE_IPV6

    /* Wait 5 seconds for the IP thread to finish its initialization and
       for the IPv6 stack to finish DAD process. */
    tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);

#else
    /* Wait 1 second for the IP thread to finish its initialization. */
    tx_thread_sleep(NX_IP_PERIODIC_RATE * 4);
#endif

    /* Ensure the IP instance has been initialized.  */
    status = nx_ip_status_check(&default_ip, NX_IP_INITIALIZE_DONE, &actual_status, NX_IP_PERIODIC_RATE);

    /* Check status...  */
    if (status != NX_SUCCESS) {

        error_counter++;
        return;
    }

    /* Create a socket.  */
    status = nx_tcp_socket_create(&default_ip, &server_socket, "Server Socket",
        NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 100,
        NX_NULL, thread_1_disconnect_received);

    /* Check for error.  */
    if (status) {
        error_counter++;
    }

    /* Setup this thread to listen.  */
    status = nx_tcp_server_socket_listen(&default_ip, ECHO_SERVER_PORT, &server_socket, 5, thread_1_connect_received);

    /* Check for error.  */
    if (status) {
        error_counter++;
    }

    /* Loop to create and establish server connections.  */
    while (1) {

        /* Increment thread 1's counter.  */
        thread_1_counter++;

        /* Accept a client socket connection.  */
        status = nx_tcp_server_socket_accept(&server_socket, NX_WAIT_FOREVER);

        /* Check for error.  */
        if (status) {
            error_counter++;
        }
        printf("A client is connected to the server.\n");
        /* Receive a TCP message from the socket.  */
        status = nx_tcp_socket_receive(&server_socket, &packet_ptr, NX_IP_PERIODIC_RATE * 10);

        /* Check for error.  */
        if (status) {
            printf("Failed to receive from the socket, reason %d.\n", status);
            error_counter++;
        } else {
            printf("Receive data from client.\n");
            /* Allocate a packet.  */
            status = nx_packet_allocate(&default_pool, &packet_ptr2, NX_TCP_PACKET, NX_WAIT_FOREVER);

            /* Check status.  */
            if (status != NX_SUCCESS) {
                printf("Failed to alloc a packet.\n");
                error_counter++;
            } else {

                /* Write data into the packet payload!  */
                nx_packet_data_append(packet_ptr2, packet_ptr->nx_packet_prepend_ptr, packet_ptr->nx_packet_length, &default_pool, TX_WAIT_FOREVER);

                status = nx_packet_length_get(packet_ptr2, &length);
                if ((status) || (length != packet_ptr->nx_packet_length)) {
                    printf("Failed to get the data length.\n");
                    error_counter++;
                } else {

                    /* Send the packet out!  */
                    status = nx_tcp_socket_send(&server_socket, packet_ptr2, NX_IP_PERIODIC_RATE);

                    /* Determine if the status is valid.  */
                    if (status) {
                        printf("Failed to send data back to client %d.\n", status);
                        error_counter++;
                        nx_packet_release(packet_ptr2);
                    } else {
                        printf("Send data back to client success.\n");
                    }
                }
            }
            /* Release the packet.  */
            nx_packet_release(packet_ptr);
        }
        printf("Close the connection\n");
        /* Disconnect the server socket.  */
        status = nx_tcp_socket_disconnect(&server_socket, NX_IP_PERIODIC_RATE);

        /* Check for error.  */
        if (status) {
            printf("Failed to disconnect the client %d.\n", status);
            error_counter++;
        }

        /* Unaccept the server socket.  */
        status = nx_tcp_server_socket_unaccept(&server_socket);

        /* Check for error.  */
        if (status) {
            printf("Failed to unaccept the socket %d.\n", status);
            error_counter++;
        }

        /* Setup server socket for listening again.  */
        status = nx_tcp_server_socket_relisten(&default_ip, ECHO_SERVER_PORT, &server_socket);

        /* Check for error.  */
        if (status) {
            printf("Failed to relisten the socket %d.\n", status);
            error_counter++;
        }
    }
}
VOID thread_1_connect_received(NX_TCP_SOCKET *socket_ptr, UINT port)
{

    /* Check for the proper socket and port.  */
    if ((socket_ptr != &server_socket) || (port != 12)) {
        error_counter++;
    }
}

VOID thread_1_disconnect_received(NX_TCP_SOCKET *socket)
{

    /* Check for proper disconnected socket.  */
    if (socket != &server_socket) {
        error_counter++;
    }
}