/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpmicro_netx_driver_dual_port.h"
#include "nx_api.h"
#include "tx_api.h"
#include <stdio.h>

#define DEMO_STACK_SIZE 1024

/* Define sample IP address.  */
#ifndef SAMPLE_ETH0_IPV4_ADDRESS
#define SAMPLE_ETH0_IPV4_ADDRESS 192.168.100.223
#endif

#ifndef SAMPLE_ETH1_IPV4_ADDRESS
#define SAMPLE_ETH1_IPV4_ADDRESS 192.168.99.223
#endif

#ifndef SAMPLE_ETH0_MASK
#define SAMPLE_ETH0_MASK 255.255.255.0
#endif

#ifndef SAMPLE_ETH1_MASK
#define SAMPLE_ETH1_MASK 255.255.255.0
#endif

/* Define stack size of sample thread.  */
#define SAMPLE_THREAD_STACK_SIZE 2048
/* Define priority of sample thread.  */
#define SAMPLE_THREAD_PRIORITY 4

#ifndef ECHO_SERVER_PORT
#define ECHO_SERVER_PORT 7777
#endif

#ifndef SERVER_IPV4_ADDRESS
#define SERVER_IPV4_ADDRESS 192.168.100.3
#endif

#define ECHO_DATA "ABCDEFGHIJKLMNOPQRSTUVWXYZ "
#define ECHO_RECEIVE_TIMEOUT NX_IP_PERIODIC_RATE
/* Define packet pool.  */
#define PACKET_SIZE 1536
#define PACKET_COUNT 30
#define PACKET_POOL_SIZE ((PACKET_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)
/* Define TCP socket TTL and window size.  */
#define SAMPLE_SOCKET_TTL 0x80
#define SAMPLE_SOCKET_WINDOW_SIZE 65535
/* Define time wait for IPv6 DAD process.  */
#define SAMPLE_DAD_WAIT (3 * NX_IP_PERIODIC_RATE)
/* Define IP stack size.   */
#define IP_STACK_SIZE 2048
/* Define IP thread priority.  */
#define IP_THREAD_PRIORITY 1
/* Define ARP pool.  */
#define ARP_POOL_SIZE 1024

/* Define the ThreadX and NetX object control blocks...  */
NX_IP ip_enet0, ip_enet1;
NX_PACKET_POOL pool_enet0, pool_enet1;
NX_TCP_SOCKET tcp_client, server_socket;
TX_THREAD thread_0, server_thread, client_thread;
ULONG demo_stack[DEMO_STACK_SIZE >> 2];

/* Define memory buffers.  */
ULONG pool_area_enet1[PACKET_POOL_SIZE >> 2];
ULONG pool_area_enet0[PACKET_POOL_SIZE >> 2];
ULONG ip_stack_enet1[IP_STACK_SIZE >> 2];
ULONG ip_stack_enet0[IP_STACK_SIZE >> 2];
ULONG arp_area_enet1[ARP_POOL_SIZE >> 2];
ULONG arp_area_enet0[ARP_POOL_SIZE >> 2];
ULONG server_thread_stack[SAMPLE_THREAD_STACK_SIZE >> 2];
ULONG client_thread_stack[SAMPLE_THREAD_STACK_SIZE >> 2];

/* Define the counters used in the demo application...  */
ULONG error_counter;

VOID server_thread_entry(ULONG thread_input);
VOID client_thread_entry(ULONG thread_input);
VOID thread_0_entry(ULONG thread_input);
VOID thread_1_connect_received(NX_TCP_SOCKET *server_socket, UINT port);
VOID thread_1_disconnect_received(NX_TCP_SOCKET *server_socket);

/* Define main entry point.  */
INT main(VOID)
{
    board_init();

    /* Initialize GPIOs */
    board_init_multiple_enet_pins();

    /* Reset Enet PHYs */
    board_reset_multiple_enet_phy();

    /* Initialize Enet Clock */
    board_init_multiple_enet_clock();

    /* Start a board timer */
    board_timer_create(2000, sys_timer_callback);

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */
VOID tx_application_define(VOID *first_unused_memory)
{
    UINT status;
    UINT eth1_ip, eth1_mask;
    UINT eth0_ip, eth0_mask;

    NX_PARAMETER_NOT_USED(first_unused_memory);
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SAMPLE_ETH1_IPV4_ADDRESS), &eth1_ip)) {
        printf("SAMPLE_ETH1_IPV4_ADDRESS(%s) is not correct\n", HPM_STRINGIFY(SAMPLE_ETH1_IPV4_ADDRESS));
        while (1) {
        }
    }
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SAMPLE_ETH1_MASK), &eth1_mask)) {
        printf("SAMPLE_ETH1_MASK(%s) is not correct\n", HPM_STRINGIFY(SAMPLE_ETH1_MASK));
        while (1) {
        }
    }

    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SAMPLE_ETH0_IPV4_ADDRESS), &eth0_ip)) {
        printf("SAMPLE_ETH0_IPV4_ADDRESS(%s) is not correct\n", HPM_STRINGIFY(SAMPLE_ETH0_IPV4_ADDRESS));
        while (1) {
        }
    }
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SAMPLE_ETH0_MASK), &eth0_mask)) {
        printf("SAMPLE_ETH0_MASK(%s) is not correct\n", HPM_STRINGIFY(SAMPLE_ETH0_MASK));
        while (1) {
        }
    }

    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,
        demo_stack, DEMO_STACK_SIZE,
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_enet0, "NetX Second Packet Pool", PACKET_SIZE, pool_area_enet0, sizeof(pool_area_enet0));

    /* Check for packet pool create errors.  */
    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(
        &ip_enet0, "NetX IP Instance 0", eth0_ip, eth0_mask, &pool_enet0, _nx_driver_hpm, (VOID *)ip_stack_enet0, sizeof(ip_stack_enet0), IP_THREAD_PRIORITY);

    /* Check for IP create errors.  */
    if (status)
        error_counter++;

    /* Register netx ip instance to driver */
    register_nx_ip(&ip_enet0, board_get_enet_base(0), true, board_get_enet_phy_itf(0));
    nx_driver_set_interface_name(0, "ETH0");

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&ip_enet0, (VOID *)arp_area_enet0, sizeof(arp_area_enet0));

    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP */
    status = nx_icmp_enable(&ip_enet0);

    /* Check for ICMP enable errors.  */
    if (status)
        error_counter++;

    /* Enable TCP */
    status = nx_tcp_enable(&ip_enet0);
    /* Check for TCP enable errors.  */
    if (status)
        error_counter++;

    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_enet1, "NetX Main Packet Pool", PACKET_SIZE, pool_area_enet1, sizeof(pool_area_enet1));

    /* Check for packet pool create errors.  */
    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(
        &ip_enet1, "NetX IP Instance 1", eth1_ip, eth1_mask, &pool_enet1, _nx_driver_hpm, (VOID *)ip_stack_enet1, sizeof(ip_stack_enet1), IP_THREAD_PRIORITY);

    /* Check for IP create errors.  */
    if (status)
        error_counter++;

    /* Register netx ip instance to driver */
    register_nx_ip(&ip_enet1, board_get_enet_base(1), false, board_get_enet_phy_itf(1));
    nx_driver_set_interface_name(1, "ETH1");

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&ip_enet1, (VOID *)arp_area_enet1, sizeof(arp_area_enet1));

    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP */
    status = nx_icmp_enable(&ip_enet1);

    /* Check for ICMP enable errors.  */
    if (status)
        error_counter++;

    /* Enable TCP */
    status = nx_tcp_enable(&ip_enet1);
    /* Check for TCP enable errors.  */
    if (status)
        error_counter++;

    /* Create the tcp server thread.  */
    tx_thread_create(&server_thread, "Server Thread", server_thread_entry, 0,
        (VOID *)server_thread_stack, sizeof(server_thread_stack),
        SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY,
        TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Create the tcp client thread.  */
    tx_thread_create(&client_thread, "Client Thread", client_thread_entry, 0,
        (VOID *)client_thread_stack, sizeof(client_thread_stack),
        SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY,
        TX_NO_TIME_SLICE, TX_AUTO_START);
}

VOID thread_0_entry(ULONG thread_input)
{
    TX_PARAMETER_NOT_USED(thread_input);
    /* Output IP address and network mask.  */
    printf("NetXDuo is running\r\n");
    printf("ETH0 IP address: %s\r\n", HPM_STRINGIFY(SAMPLE_ETH0_IPV4_ADDRESS));
    printf("ETH0 Mask: %s\r\n", HPM_STRINGIFY(SAMPLE_ETH0_MASK));
    printf("ETH1 IP address: %s\r\n", HPM_STRINGIFY(SAMPLE_ETH1_IPV4_ADDRESS));
    printf("ETH1 Mask: %s\r\n", HPM_STRINGIFY(SAMPLE_ETH1_MASK));

    while (1) {
        NX_IP *ip_ptr = &ip_enet0;
        if (ip_ptr != NULL) {
            printf("ETH0 received %lu ICMP packets, ", ip_ptr->nx_ip_icmp_total_messages_received);
        }
        ip_ptr = &ip_enet1;
        if (ip_ptr != NULL) {
            printf("ETH1 received %lu ICMP packets\n", ip_ptr->nx_ip_icmp_total_messages_received);
        }
        tx_thread_sleep(100);
    }
}

VOID server_thread_entry(ULONG thread_input)
{
    UINT status;
    NX_PACKET *packet_ptr, *packet_ptr2;
    ULONG actual_status;
    ULONG length;

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
    status = nx_ip_status_check(&ip_enet1, NX_IP_INITIALIZE_DONE, &actual_status, NX_IP_PERIODIC_RATE);

    /* Check status...  */
    if (status != NX_SUCCESS) {

        error_counter++;
        return;
    }

    /* Create a socket.  */
    status = nx_tcp_socket_create(&ip_enet1, &server_socket, "Server Socket",
        NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 100,
        NX_NULL, thread_1_disconnect_received);

    /* Check for error.  */
    if (status) {
        error_counter++;
    }

    /* Setup this thread to listen.  */
    status = nx_tcp_server_socket_listen(&ip_enet1, ECHO_SERVER_PORT, &server_socket, 5, thread_1_connect_received);

    /* Check for error.  */
    if (status) {
        error_counter++;
    }

    /* Loop to create and establish server connections.  */
    while (1) {

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
            status = nx_packet_allocate(&pool_enet1, &packet_ptr2, NX_TCP_PACKET, NX_WAIT_FOREVER);

            /* Check status.  */
            if (status != NX_SUCCESS) {
                printf("Failed to alloc a packet.\n");
                error_counter++;
            } else {

                /* Write data into the packet payload!  */
                nx_packet_data_append(packet_ptr2, packet_ptr->nx_packet_prepend_ptr, packet_ptr->nx_packet_length, &pool_enet1, TX_WAIT_FOREVER);

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
        status = nx_tcp_server_socket_relisten(&ip_enet1, ECHO_SERVER_PORT, &server_socket);

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
/* Client thread entry.  */
VOID client_thread_entry(ULONG thread_input)
{
    UINT status;
    NX_PACKET *packet_ptr;
    NXD_ADDRESS echo_server_address;
    UINT server_ip;

    TX_PARAMETER_NOT_USED(thread_input);
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SERVER_IPV4_ADDRESS), &server_ip)) {
        printf("SERVER_IPV4_ADDRESS(%s) is not correct\n", HPM_STRINGIFY(SERVER_IPV4_ADDRESS));
        while (1) {
        }
    }
    echo_server_address.nxd_ip_version = NX_IP_VERSION_V4;
    echo_server_address.nxd_ip_address.v4 = server_ip;
    /* Create a TCP socket.  */
    status = nx_tcp_socket_create(&ip_enet0, &tcp_client,
        "TCP Echo Client", NX_IP_NORMAL,
        NX_DONT_FRAGMENT, SAMPLE_SOCKET_TTL,
        SAMPLE_SOCKET_WINDOW_SIZE, NX_NULL,
        NX_NULL);
    /* Check status.  */
    if (status) {
        error_counter++;
        return;
    }
    /* Bind the TCP socket to any port.  */
    status = nx_tcp_client_socket_bind(&tcp_client, NX_ANY_PORT,
        NX_WAIT_FOREVER);
    /* Check status.  */
    if (status) {
        error_counter++;
        return;
    }
    tx_thread_sleep(NX_IP_PERIODIC_RATE * 2);
    while (1) {
        /* Connect to server.  */
        printf("Connecting to server: ");
        status = nxd_tcp_client_socket_connect(&tcp_client, &echo_server_address,
            ECHO_SERVER_PORT,
            NX_WAIT_FOREVER);
        /* Check status.  */
        if (status) {
            printf("Not connected\r\n");
            tx_thread_sleep(100);
        } else {
          break;
        }
    }

    printf("Connected\r\n");
    /* Loop to send data to echo server.  */
    for (;;) {
        /* Allocate a packet.  */
        status = nx_packet_allocate(&pool_enet0, &packet_ptr,
            NX_TCP_PACKET, NX_WAIT_FOREVER);
        /* Check status.  */
        if (status != NX_SUCCESS) {
            error_counter++;
            break;
        }
        /* Write ABCs into the packet payload.  */
        status = nx_packet_data_append(packet_ptr, ECHO_DATA,
            sizeof(ECHO_DATA), &pool_enet0,
            NX_WAIT_FOREVER);
        /* Check status.  */
        if (status != NX_SUCCESS) {
            error_counter++;
            break;
        }
        /* Send data to echo server.  */
        status = nx_tcp_socket_send(&tcp_client, packet_ptr,
            NX_WAIT_FOREVER);
        /* Check status.  */
        if (status != NX_SUCCESS) {
            nx_packet_release(packet_ptr);
            error_counter++;
            break;
        }
        /* Receive data from echo server.  */
        status = nx_tcp_socket_receive(&tcp_client, &packet_ptr,
            NX_WAIT_FOREVER);
        if (status) {
            /* No response received.  */
            printf("No response from server\n");
            break;
        } else {
            /* Response received from server.  */
            printf("Receive response: %.*s\n",
                (INT)(packet_ptr->nx_packet_length),
                packet_ptr->nx_packet_prepend_ptr);
            nx_packet_release(packet_ptr);
        }
    }
    /* Cleanup the TCP socket.  */
    nx_tcp_socket_disconnect(&tcp_client, NX_WAIT_FOREVER);
    nx_tcp_client_socket_unbind(&tcp_client);
    nx_tcp_socket_delete(&tcp_client);
}