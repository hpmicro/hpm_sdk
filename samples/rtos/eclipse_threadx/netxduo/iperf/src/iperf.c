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

/* Define demo stack size.   */
#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 300)
#define DEMO_STACK_SIZE 2048
#define HTTP_STACK_SIZE 2048
#define IPERF_STACK_SIZE 2048
#define IP_STACK_SIZE 2048
#define ARP_POOL_SIZE 1024

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
/* Define the ThreadX and NetX object control blocks...  */
TX_THREAD thread_0;
NX_PACKET_POOL pool_0;
NX_IP ip_0;

#ifdef FEATURE_NX_IPV6
NXD_ADDRESS ipv6_address;
#endif

/* Define memory buffers.  */
ULONG pool_area[NX_PACKET_POOL_SIZE >> 2];
ULONG ip_stack[IP_STACK_SIZE >> 2];
ULONG arp_area[ARP_POOL_SIZE >> 2];
ULONG thread_usr_stack[DEMO_STACK_SIZE >> 2];
ULONG thread_http_stack[HTTP_STACK_SIZE >> 2];
ULONG thread_iperf_stack[IPERF_STACK_SIZE >> 2];

/* Define the counters used in the demo application...  */
ULONG error_counter;

/* Define thread prototypes.  */
VOID thread_0_entry(ULONG thread_input);
extern VOID nx_iperf_entry(NX_PACKET_POOL *pool_ptr, NX_IP *ip_ptr, UCHAR *http_stack, ULONG http_stack_size, UCHAR *iperf_stack, ULONG iperf_stack_size);

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
    UINT status;
    UINT ip, mask;

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

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool",
        (1536 + sizeof(NX_PACKET)),
        (VOID *)pool_area, sizeof(pool_area));

    /* Check for packet pool create errors.  */
    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", ip, mask, &pool_0, _nx_driver_hpm,
        (VOID *)ip_stack, sizeof(ip_stack), 7);

    /* Check for IP create errors.  */
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status = nx_arp_enable(&ip_0, (VOID *)arp_area, sizeof(arp_area));

    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP */
    status = nx_icmp_enable(&ip_0);

    /* Check for ICMP enable errors.  */
    if (status)
        error_counter++;

    /* Enable UDP traffic.  */
    status = nx_udp_enable(&ip_0);

    /* Check for UDP enable errors.  */
    if (status)
        error_counter++;

    /* Enable TCP traffic.  */
    status = nx_tcp_enable(&ip_0);

    /* Check for TCP enable errors.  */
    if (status)
        error_counter++;

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,
        (VOID *)thread_usr_stack, sizeof(thread_usr_stack),
        4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

#ifdef FEATURE_NX_IPV6
    /* Set up the IPv6 address here. */
    ipv6_address.nxd_ip_address.v6[3] = 0x3;
    ipv6_address.nxd_ip_address.v6[2] = 0x0;
    ipv6_address.nxd_ip_address.v6[1] = 0x0;
    ipv6_address.nxd_ip_address.v6[0] = 0xfe800000;
    ipv6_address.nxd_ip_version = NX_IP_VERSION_V6;

    /* Enable ICMPv6 services. */
    status = nxd_icmp_enable(&ip_0);
    if (status)
        error_counter++;

    /* Enable IPv6 services. */
    status = nxd_ipv6_enable(&ip_0);
    if (status)
        error_counter++;

    status = nxd_ipv6_address_set(&ip_0, 0, &ipv6_address, 10, NX_NULL);
    if (status)
        error_counter++;
#endif
}

/* Define the test threads.  */
VOID thread_0_entry(ULONG thread_input)
{
#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    UINT status;
    ULONG actual_status;
    ULONG temp;

    /* Create the DHCP instance.  */
    printf("DHCP In Progress...\r\n");

    nx_dhcp_create(&dhcp_client, &ip_0, "dhcp_client");

    /* Start the DHCP Client.  */
    nx_dhcp_start(&dhcp_client);

    /* Wait util address is solved. */
    status = nx_ip_status_check(&ip_0, NX_IP_ADDRESS_RESOLVED, &actual_status, 8000);

    if (status) {
        /* DHCP Failed...  no IP address! */
        printf("Can't resolve address\r\n");
    } else {
        /* Get IP address. */
        nx_ip_address_get(&ip_0, (ULONG *)&ip_address[0], (ULONG *)&network_mask[0]);

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
    TX_PARAMETER_NOT_USED(thread_input);
#ifdef FEATURE_NX_IPV6
    tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);
#endif

    /* Call entry function to start iperf test.  */
    nx_iperf_entry(&pool_0, &ip_0, (UCHAR *)thread_http_stack, sizeof(thread_http_stack), (UCHAR *)thread_iperf_stack, sizeof(thread_iperf_stack));
}