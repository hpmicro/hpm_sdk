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
#include "nxd_sntp_client.h"

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
#include "nxd_dhcp_client.h"
#endif

#ifndef TEST_RTC
#define TEST_RTC HPM_RTC
#endif

/* Define SNTP packet size. */
#define NX_SNTP_CLIENT_PACKET_SIZE (NX_UDP_PACKET + 100)

/* Define SNTP packet pool size. */
#define NX_SNTP_CLIENT_PACKET_POOL_SIZE (40 * (NX_SNTP_CLIENT_PACKET_SIZE + sizeof(NX_PACKET)))

/* Define how often the demo checks for SNTP updates. */
#define DEMO_PERIODIC_CHECK_INTERVAL (1 * NX_IP_PERIODIC_RATE)

/* Define how often we check on SNTP server status. We expect updates from the SNTP
   server about every hour using the SNTP Client defaults. For testing make this (much) shorter. */
#define CHECK_SNTP_UPDATES_TIMEOUT (180 * NX_IP_PERIODIC_RATE)

/* Application defined services of the NetX SNTP Client. */

UINT leap_second_handler(NX_SNTP_CLIENT *client_ptr, UINT leap_indicator);
UINT kiss_of_death_handler(NX_SNTP_CLIENT *client_ptr, UINT KOD_code);
VOID time_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time);

/* Set up client thread and network resources. */

NX_PACKET_POOL client_packet_pool;
NX_IP client_ip;
TX_THREAD demo_client_thread;
NX_SNTP_CLIENT demo_sntp_client;
TX_EVENT_FLAGS_GROUP sntp_flags;

#define DEMO_SNTP_UPDATE_EVENT 1

/* Configure the SNTP Client to use IPv6. If not enabled, the
   Client will use IPv4.  Note: IPv6 must be enabled in NetX Duo
   for the Client to communicate over IPv6.    */
#ifdef FEATURE_NX_IPV6
/* #define USE_IPV6 */
#endif /* FEATURE_NX_IPV6 */

/* Configure the SNTP Client to use unicast SNTP. */
#define USE_UNICAST

/* Define sample IP address.  */
#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
#define CLIENT_IP_ADDRESS 0.0.0.0
#define SAMPLE_IPV4_MASK 0.0.0.0

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(64) NX_DHCP dhcp_client;
UCHAR ip_address[4];
UCHAR network_mask[4];
#else
#ifndef CLIENT_IP_ADDRESS
#define CLIENT_IP_ADDRESS 192.168.1.223
#endif
#ifndef SAMPLE_IPV4_MASK
#define SAMPLE_IPV4_MASK 255.255.255.0
#endif
#endif

#ifndef SERVER_IP_ADDRESS
#define SERVER_IP_ADDRESS 192.168.1.2
#endif

#define DEMO_STACK_SIZE 1024
#define PACKET_SIZE 1536
#define PACKET_COUNT 30
#define PACKET_POOL_SIZE ((PACKET_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)
#define IP_STACK_SIZE 2048
#define ARP_POOL_SIZE 1024

TX_THREAD thread_0;
ULONG demo_stack[DEMO_STACK_SIZE >> 2];
ULONG pool_area[PACKET_POOL_SIZE >> 2];
ULONG ip_stack[IP_STACK_SIZE >> 2];
ULONG arp_area[ARP_POOL_SIZE >> 2];

/* Set up the SNTP network and address index; */
UINT iface_index;
UINT prefix = 64;
UINT address_index;

/* Set up client thread entry point. */
VOID demo_client_thread_entry(ULONG info);
VOID thread_0_entry(ULONG thread_input);

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
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(CLIENT_IP_ADDRESS), &ip)) {
        printf("CLIENT_IP_ADDRESS(%s) is not correct\n", HPM_STRINGIFY(CLIENT_IP_ADDRESS));
        while (1) {
        }
    }
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SAMPLE_IPV4_MASK), &mask)) {
        printf("SAMPLE_IPV4_MASK(%s) is not correct\n", HPM_STRINGIFY(SAMPLE_IPV4_MASK));
        while (1) {
        }
    }
    /* Initialize the NetX system. */
    nx_system_initialize();

    /* Create client packet pool. */
    status = nx_packet_pool_create(&client_packet_pool, "SNTP Client Packet Pool",
        PACKET_SIZE, (VOID *)pool_area, sizeof(pool_area));

    /* Check for errors. */
    if (status != NX_SUCCESS) {

        return;
    }

    /* Create Client IP instances */
    status = nx_ip_create(&client_ip, "SNTP IP Instance", ip,
        mask, &client_packet_pool, _nx_driver_hpm,
        (VOID *)ip_stack, sizeof(ip_stack), 1);

    /* Check for error. */
    if (status != NX_SUCCESS) {

        return;
    }

#ifndef NX_DISABLE_IPV4
    /* Enable ARP and supply ARP cache memory. */
    status = nx_arp_enable(&client_ip, (VOID *)arp_area, sizeof(arp_area));

    /* Check for error. */
    if (status != NX_SUCCESS) {

        return;
    }
#endif /* NX_DISABLE_IPV4  */

    /* Enable UDP for client. */
    status = nx_udp_enable(&client_ip);

    /* Check for error. */
    if (status != NX_SUCCESS) {

        return;
    }

#ifndef NX_DISABLE_IPV4
    status = nx_icmp_enable(&client_ip);

    /* Check for error. */
    if (status != NX_SUCCESS) {

        return;
    }
#endif /* NX_DISABLE_IPV4  */

    /* Create the client thread */
    status = tx_thread_create(&demo_client_thread, "SNTP Client Thread", demo_client_thread_entry,
        (ULONG)(&demo_sntp_client), (VOID *)demo_stack, sizeof(demo_stack),
        4, 4, TX_NO_TIME_SLICE, TX_DONT_START);

    /* Check for errors */
    if (status != TX_SUCCESS) {

        return;
    }

    /* Create the event flags. */
    status = tx_event_flags_create(&sntp_flags, "SNTP event flags");

    /* Check for errors */
    if (status != TX_SUCCESS) {

        return;
    }

    /* set the SNTP network interface to the primary interface. */
    iface_index = 0;

    /* Create the SNTP Client to run in broadcast mode.. */
    status = nx_sntp_client_create(&demo_sntp_client, &client_ip, iface_index, &client_packet_pool,
        leap_second_handler,
        kiss_of_death_handler,
        NULL /* no random_number_generator callback */);

    /* Check for error. */
    if (status != NX_SUCCESS) {

        /* Bail out!*/
        return;
    }

    tx_thread_resume(&demo_client_thread);

    return;
}

/* Define the client thread.  */
VOID demo_client_thread_entry(ULONG info)
{
    UINT status;
    UINT spin;
    UINT server_status;
    ULONG base_seconds;
    ULONG base_fraction;
    ULONG seconds = 0;
    ULONG microseconds = 0;
    ULONG fraction = 0;
    ULONG milliseconds = 0;
    UINT wait = 0;
    UINT error_counter = 0; /* NOLINT */
    ULONG events = 0;
#ifdef USE_IPV6
    NXD_ADDRESS sntp_server_address;
    NXD_ADDRESS client_ip_address;
#endif

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    ULONG actual_status;
    ULONG temp;

    /* Create the DHCP instance.  */
    printf("DHCP In Progress...\r\n");

    nx_dhcp_create(&dhcp_client, &client_ip, "dhcp_client");

    /* Start the DHCP Client.  */
    nx_dhcp_start(&dhcp_client);

    /* Wait util address is solved. */
    status = nx_ip_status_check(&client_ip, NX_IP_ADDRESS_RESOLVED, &actual_status, 8000);

    if (status) {
        /* DHCP Failed...  no IP address! */
        printf("Can't resolve address\r\n");
    } else {
        /* Get IP address. */
        nx_ip_address_get(&client_ip, (ULONG *)&ip_address[0], (ULONG *)&network_mask[0]);

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
    UINT server_ip;
    if (!nx_ipv4addr_aton(HPM_STRINGIFY(SERVER_IP_ADDRESS), &server_ip)) {
        printf("SERVER_IP_ADDRESS(%s) is not correct\n", HPM_STRINGIFY(SERVER_IP_ADDRESS));
        while (1) {
        }
    }
    /* Output IP address and network mask.  */
    printf("NetXDuo is running\r\n");
    printf("IP address: %s\r\n", HPM_STRINGIFY(CLIENT_IP_ADDRESS));
    printf("Mask: %s\r\n", HPM_STRINGIFY(SAMPLE_IPV4_MASK));

#endif
    NX_PARAMETER_NOT_USED(info);

    /* Give other threads (IP instance) initialize first. */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);

#ifdef USE_IPV6
    /* Set up IPv6 services. */
    status = nxd_ipv6_enable(&client_ip);

    status += nxd_icmp_enable(&client_ip);

    if (status != NX_SUCCESS)
        return;

    client_ip_address.nxd_ip_address.v6[0] = 0x20010db8;
    client_ip_address.nxd_ip_address.v6[1] = 0x0000f101;
    client_ip_address.nxd_ip_address.v6[2] = 0x0;
    client_ip_address.nxd_ip_address.v6[3] = 0x101;
    client_ip_address.nxd_ip_version = NX_IP_VERSION_V6;

    /* Set the IPv6 server address. */
    sntp_server_address.nxd_ip_address.v6[0] = 0x20010db8;
    sntp_server_address.nxd_ip_address.v6[1] = 0x0000f101;
    sntp_server_address.nxd_ip_address.v6[2] = 0x0;
    sntp_server_address.nxd_ip_address.v6[3] = 0x00000106;
    sntp_server_address.nxd_ip_version = NX_IP_VERSION_V6;

    /* Establish the link local address for the host. The RAM driver creates
       a virtual MAC address. */
    status = nxd_ipv6_address_set(&client_ip, iface_index, NX_NULL, 10, NULL);

    /* Check for link local address set error.  */
    if (status != NX_SUCCESS) {
        return;
    }

    /* Set the host global IP address. We are assuming a 64
      bit prefix here but this can be any value (< 128). */
    status = nxd_ipv6_address_set(&client_ip, iface_index, &client_ip_address, prefix, &address_index);

    /* Check for global address set error.  */
    if (status != NX_SUCCESS) {
        return;
    }

    /* Wait while NetX Duo validates the global and link local addresses. */
    tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);

#endif

    /* Setup time update callback function. */
    nx_sntp_client_set_time_update_notify(&demo_sntp_client, time_update_callback);

    /* Set up client time updates depending on mode. */
#ifdef USE_UNICAST

    /* Initialize the Client for unicast mode to poll the SNTP server once an hour. */
#ifdef USE_IPV6
    /* Use the duo service to set up the Client and set the IPv6 SNTP server. Note: this
       can take either an IPv4 or IPv6 address. */
    status = nxd_sntp_client_initialize_unicast(&demo_sntp_client, &sntp_server_address);
#else
    /* Use the IPv4 service to set up the Client and set the IPv4 SNTP server. */
    status = nx_sntp_client_initialize_unicast(&demo_sntp_client, server_ip);
#endif /* USE_IPV6 */

#else /* Broadcast mode */

    /* Initialize the Client for broadcast mode, no roundtrip calculation required and a broadcast SNTP service. */
#ifdef USE_IPV6
    /* Use the duo service to initialize the Client and set IPv6 SNTP all hosts multicast address.
       (Note: This can take either an IPv4 or IPv6 address.)*/
    status = nxd_sntp_client_initialize_broadcast(&demo_sntp_client, &sntp_server_address, NX_NULL);
#else

    /* Use the IPv4 service to initialize the Client and set IPv4 SNTP broadcast address. */
    status = nx_sntp_client_initialize_broadcast(&demo_sntp_client, NX_NULL, server_ip);
#endif /* USE_IPV6 */
#endif /* USE_UNICAST */

    /* Check for error. */
    if (status != NX_SUCCESS) {
        return;
    }

    /* Set the base time which is approximately the number of seconds since the turn of the last century.
       If this is not available in SNTP format, the nx_sntp_client_utility_add_msecs_to_ntp_time service
       can convert milliseconds to fraction.  For how to compute NTP seconds from real time, read the
       NetX SNTP User Guide.

       Otherwise set the base time to zero and set NX_SNTP_CLIENT_IGNORE_MAX_ADJUST_STARTUP to NX_TRUE for
       the SNTP CLient to accept the first time update without applying a minimum or maximum adjustment
       parameters (NX_SNTP_CLIENT_MIN_TIME_ADJUSTMENT and NX_SNTP_CLIENT_MAX_TIME_ADJUSTMENT). */

    base_seconds = 0xd2c96b90; /* Jan 24, 2012 UTC */
    base_fraction = 0xa132db1e;

    /* Apply to the SNTP Client local time.  */
    status = nx_sntp_client_set_local_time(&demo_sntp_client, base_seconds, base_fraction);

    /* Check for error. */
    if (status != NX_SUCCESS) {
        return;
    }

    /* Run whichever service the client is configured for. */
#ifdef USE_UNICAST
    status = nx_sntp_client_run_unicast(&demo_sntp_client);
#else
    status = nx_sntp_client_run_broadcast(&demo_sntp_client);
#endif /* USE_UNICAST */

    if (status != NX_SUCCESS) {
        return;
    }

    spin = NX_TRUE;

    /* Now check periodically for time changes. */
    while (spin) {
        /* Wait for a server update event. */
        tx_event_flags_get(&sntp_flags, DEMO_SNTP_UPDATE_EVENT, TX_OR_CLEAR, &events, DEMO_PERIODIC_CHECK_INTERVAL);

        if (events == DEMO_SNTP_UPDATE_EVENT) {

            /* Check for valid SNTP server status. */
            status = nx_sntp_client_receiving_updates(&demo_sntp_client, &server_status);

            if ((status != NX_SUCCESS) || (server_status == NX_FALSE)) {

                /* We do not have a valid update. Skip processing any time data. */

                /* If this happens repeatedly, consider stopping the SNTP Client thread, picking another
                   SNTP server and resuming the SNTP Client thread task (more details about that in the
                   comments at the end of this function).

                   If SNTP Client configurable parameters are too restrictive, such as Max Adjustment, that may also cause
                   valid server updates to be rejected. Configurable parameters, however, cannot be changed at run time.*/

                continue;
            }

            /* We have a valid update.  Get the SNTP Client time.  */
            status = nx_sntp_client_get_local_time_extended(&demo_sntp_client, &seconds, &fraction, NX_NULL, 0);

            /* Convert fraction to microseconds. */
            nx_sntp_client_utility_fraction_to_usecs(fraction, &microseconds);

            milliseconds = ((microseconds + 500) / 1000);

            if (status != NX_SUCCESS) {
                printf("Internal error with getting local time 0x%x\n", status);
                error_counter++;
            } else {
                printf("\nSNTP updated\n");
                printf("Time: %lu.%03lu sec.\r\n", seconds, milliseconds);
            }

            /* Clear all events in our event flag. */
            events = 0;
        } else {

            /* No SNTP update event.

               In the meantime, if we have an RTC we might want to check its notion of time.
               In this demo, we simulate the passage of time on our 'RTC' really just the CPU counter,
               assuming that seconds and milliseconds have previously been set to a base (starting) time
               (as was the SNTP Client before running it) */

            seconds += 1;      /* This is the sleep time (1 second) so is pretty close to an RTC */
            milliseconds += 1; /* We don't know this value but for demonstration purposes we change it */

            /* Update our timer. */
            wait += DEMO_PERIODIC_CHECK_INTERVAL;

            /* Check if it is time to display the local 'RTC' time. */
            if (wait >= CHECK_SNTP_UPDATES_TIMEOUT) {
                /* It is. Reset the timeout and print local time. */
                wait = 0;

                printf("Time: %lu.%03lu sec.\r\n", seconds, milliseconds);
            }
        }
    }

    /* We can stop the SNTP service if for example we think the SNTP server has stopped sending updates.

       To restart the SNTP Client, simply call the nx_sntp_client_initialize_unicast or nx_sntp_client_initialize_broadcast
       using another SNTP server IP address as input, and resume the SNTP Client by calling nx_sntp_client_run_unicast or
       nx_sntp_client_run_broadcast. */
    status = nx_sntp_client_stop(&demo_sntp_client);

    if (status != NX_SUCCESS) {
        error_counter++;
    }

    /* When done with the SNTP Client, we delete it */
    status = nx_sntp_client_delete(&demo_sntp_client);

    if (status != NX_SUCCESS) {
        error_counter++;
    }

    return;
}

/* This application defined handler for handling an impending leap second is not
   required by the SNTP Client. The default handler below only logs the event for
   every time stamp received with the leap indicator set.  */

UINT leap_second_handler(NX_SNTP_CLIENT *client_ptr, UINT leap_indicator)
{
    NX_PARAMETER_NOT_USED(client_ptr);
    NX_PARAMETER_NOT_USED(leap_indicator);

    /* Handle the leap second handler... */

    return NX_SUCCESS;
}

/* This application defined handler for handling a Kiss of Death packet is not
   required by the SNTP Client. A KOD handler should determine
   if the Client task should continue vs. abort sending/receiving time data
   from its current time server, and if aborting if it should remove
   the server from its active server list.

   Note that the KOD list of codes is subject to change. The list
   below is current at the time of this software release. */

UINT kiss_of_death_handler(NX_SNTP_CLIENT *client_ptr, UINT KOD_code)
{

    UINT remove_server_from_list = NX_FALSE;
    UINT status = NX_SUCCESS;

    NX_PARAMETER_NOT_USED(client_ptr);

    /* Handle kiss of death by code group. */
    switch (KOD_code) {

    case NX_SNTP_KOD_RATE:
    case NX_SNTP_KOD_NOT_INIT:
    case NX_SNTP_KOD_STEP:

        /* Find another server while this one is temporarily out of service.  */
        status = NX_SNTP_KOD_SERVER_NOT_AVAILABLE;

        break;

    case NX_SNTP_KOD_AUTH_FAIL:
    case NX_SNTP_KOD_NO_KEY:
    case NX_SNTP_KOD_CRYP_FAIL:

        /* These indicate the server will not service client with time updates
           without successful authentication. */

        remove_server_from_list = NX_TRUE;

        break;

    default:

        /* All other codes. Remove server before resuming time updates. */

        remove_server_from_list = NX_TRUE;
        break;
    }

    /* Removing the server from the active server list? */
    if (remove_server_from_list) {

        /* Let the caller know it has to bail on this server before resuming service. */
        status = NX_SNTP_KOD_REMOVE_SERVER;
    }

    return status;
}

static VOID print_beijing_time(ULONG seconds_1900)
{
    const UINT days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    ULONG seconds_1970 = 0;
    ULONG year = 1970;
    ULONG month = 1;
    ULONG day = 1;
    ULONG hour = 0;
    ULONG min = 0;
    ULONG second = 0;

    UINT Pass4year;
    UINT hours_per_year;

    seconds_1970 = seconds_1900 + 8 * 60 * 60 - 2208988800;
    second = seconds_1970 % 60;
    seconds_1970 /= 60;
    min = seconds_1970 % 60;
    seconds_1970 /= 60;

    Pass4year = seconds_1970 / (1461 * 24);

    year = 1970 + Pass4year * 4;

    seconds_1970 = seconds_1970 % (1461 * 24);

    for (;;) {

        hours_per_year = 365 * 24;

        if ((((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0)) {

            hours_per_year += 24;
        }
        if (seconds_1970 < hours_per_year) {
            break;
        }
        year++;
        seconds_1970 -= hours_per_year;
    }

    hour = seconds_1970 % 24;

    seconds_1970 /= 24;

    seconds_1970 += 1;

    if ((((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0)) {
        if (seconds_1970 > 60) {
            seconds_1970--;
        } else {
            if (seconds_1970 == 60) {
                day = 29; /* NOLINT */
                month = 1; /* NOLINT */
                return;
            }
        }
    }

    for (month = 0; days[month] < seconds_1970; month++) {
        seconds_1970 -= days[month];
    }
    day = seconds_1970;
    month = month + 1;
    printf("year [%ld] :month [%ld]: day [%ld] :hour  [%ld]: min  [%ld]: second  [%ld]", year, month, day, hour, min, second);
    return;
}

/* This application defined handler for notifying SNTP time update event.  */

VOID time_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time)
{
    NX_PARAMETER_NOT_USED(time_update_ptr);

    print_beijing_time(local_time->seconds);
    tx_event_flags_set(&sntp_flags, DEMO_SNTP_UPDATE_EVENT, TX_OR);
}