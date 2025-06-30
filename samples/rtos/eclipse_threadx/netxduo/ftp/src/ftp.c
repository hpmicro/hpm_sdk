/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "nx_api.h"
#include "tx_api.h"
#include "fx_api.h"
#include <stdio.h>
#include "hpmicro_netx_driver.h"
#include "nxd_ftp_server.h"

#define DEMO_DATA "ABCDEFGHIJKLMNOPQRSTUVWXYZ "

#ifdef FEATURE_NX_IPV6
#define USE_IPV6
#endif /* FEATURE_NX_IPV6 */

/* Uncomment the following line to enable block mode.  */
/*
#define BLOCK_MODE
*/

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
#include "nxd_dhcp_client.h"
#endif

/* Define the ThreadX, NetX, and FileX object control blocks...  */

TX_THREAD server_thread;
NX_PACKET_POOL server_pool;
NX_IP server_ip;
FX_MEDIA ram_disk;

/* Define the NetX FTP object control blocks.  */

NX_FTP_SERVER ftp_server;

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

/* Define the counters used in the demo application...  */

ULONG error_counter;

/* Define the memory area for the FileX RAM disk.  */
#ifndef RAM_DISK_SIZE
#define RAM_DISK_SIZE (1280 * 1024)
#endif
UCHAR ram_disk_memory[RAM_DISK_SIZE];
UCHAR ram_disk_sector_cache[512];

/* Define the FileX and NetX driver entry functions.  */
VOID _fx_ram_driver(FX_MEDIA *media_ptr);
VOID format_ram_disk(ULONG thread_input);
VOID thread_server_entry(ULONG thread_input);

#ifdef USE_IPV6
/* Define NetX Duo IP address for the NetX Duo FTP Server and Client. */
NXD_ADDRESS server_ip_address;
NXD_ADDRESS client_ip_address;
#endif

/* Define server login/logout functions.  These are stubs for functions that would
   validate a client login request.   */

#ifdef USE_IPV6
UINT server_login6(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, NXD_ADDRESS *client_ipduo_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info);
UINT server_logout6(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, NXD_ADDRESS *client_ipduo_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info);
#else
UINT server_login(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info);
UINT server_logout(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info);
#endif

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
ULONG thread_ftp_stack[DEMO_STACK_SIZE >> 2];

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
    /* Initialize NetX.  */
    nx_system_initialize();

    /* Initialize FileX.  */
    fx_system_initialize();

    /* Create a helper thread for the server. */
    tx_thread_create(&server_thread, "FTP Server thread", thread_server_entry, 0,
        (VOID *)thread_usr_stack, sizeof(thread_usr_stack),
        4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Create the packet pool for the FTP Server.  */
    status = nx_packet_pool_create(&server_pool, "NetX Server Packet Pool", (PACKET_SIZE + sizeof(NX_PACKET)), (VOID *)pool_area, sizeof(pool_area));

    /* Check for errors.  */
    if (status)
        error_counter++;

    /* Create the IP instance for the FTP Server.  */
    status = nx_ip_create(&server_ip, "NetX Server IP Instance", ip, mask,
        &server_pool, _nx_driver_hpm, (VOID *)ip_stack, sizeof(ip_stack), 1);

    /* Check status.  */
    if (status != NX_SUCCESS) {
        error_counter++;
        return;
    }

#ifndef NX_DISABLE_IPV4
    /* Enable ARP and supply ARP cache memory for server IP instance.  */
    nx_arp_enable(&server_ip, (VOID *)arp_area, sizeof(arp_area));

    /* Enable ICMP */
    nx_icmp_enable(&server_ip);
#endif /* NX_DISABLE_IPV4  */

    /* Enable TCP.  */
    nx_tcp_enable(&server_ip);

#if defined(CONFIG_NETX_ENABLE_DHCP) && CONFIG_NETX_ENABLE_DHCP
    /* Enable UDP traffic.  */
    status = nx_udp_enable(&server_ip);

    /* Check for UDP enable errors.  */
    if (status) {
        return;
    }
#endif

#ifdef USE_IPV6

    /* Next set the NetX Duo FTP Server and Client addresses. */
    server_ip_address.nxd_ip_address.v6[3] = 0x105;
    server_ip_address.nxd_ip_address.v6[2] = 0x0;
    server_ip_address.nxd_ip_address.v6[1] = 0x0000f101;
    server_ip_address.nxd_ip_address.v6[0] = 0x20010db8;
    server_ip_address.nxd_ip_version = NX_IP_VERSION_V6;

    client_ip_address.nxd_ip_address.v6[3] = 0x101;
    client_ip_address.nxd_ip_address.v6[2] = 0x0;
    client_ip_address.nxd_ip_address.v6[1] = 0x0000f101;
    client_ip_address.nxd_ip_address.v6[0] = 0x20010db8;
    client_ip_address.nxd_ip_version = NX_IP_VERSION_V6;

    /* Create the FTP server.  */
    status = nxd_ftp_server_create(&ftp_server, "FTP Server Instance", &server_ip, &ram_disk, thread_ftp_stack, sizeof(thread_ftp_stack), &server_pool,
        server_login6, server_logout6);
#else
    /* Create the FTP server.  */
    status = nx_ftp_server_create(&ftp_server, "FTP Server Instance", &server_ip, &ram_disk, thread_ftp_stack, sizeof(thread_ftp_stack), &server_pool,
        server_login, server_logout);
#endif

    /* Check status.  */
    if (status != NX_SUCCESS) {
        error_counter++;
        return;
    }
    printf("FTP Server create success, available storage area is %d bytes\n", RAM_DISK_SIZE);
    return;
}

/* Define the helper FTP server thread.  */
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

    /* Format the RAM disk - the memory for the RAM disk was defined above.  */
    status = fx_media_format(&ram_disk,
        _fx_ram_driver,                /* Driver entry                */
        ram_disk_memory,               /* RAM disk memory pointer     */
        ram_disk_sector_cache,         /* Media buffer pointer        */
        sizeof(ram_disk_sector_cache), /* Media buffer size           */
        "MY_RAM_DISK",                 /* Volume Name                 */
        1,                             /* Number of FATs              */
        32,                            /* Directory Entries           */
        0,                             /* Hidden sectors              */
        sizeof(ram_disk_memory) / 512, /* Total sectors               */
        512,                           /* Sector size                 */
        1,                             /* Sectors per cluster         */
        1,                             /* Heads                       */
        1);                            /* Sectors per track           */

    /* Check status.  */
    if (status != FX_SUCCESS) {
        error_counter++;
        return;
    }
    /* Open the RAM disk.  */
    status = fx_media_open(&ram_disk, "RAM DISK", _fx_ram_driver, ram_disk_memory, ram_disk_sector_cache, sizeof(ram_disk_sector_cache));

    /* Check status.  */
    if (status != FX_SUCCESS) {
        error_counter++;
        return;
    }
    /* Wait till the IP thread and driver have initialized the system. */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);

#ifdef USE_IPV6

    /* Here's where we make the FTP server IPv6 enabled. */
    status = nxd_ipv6_enable(&server_ip);

    /* Check status.  */
    if (status != NX_SUCCESS) {

        error_counter++;
        return;
    }

    status = nxd_icmp_enable(&server_ip);

    /* Check status.  */
    if (status != NX_SUCCESS) {

        error_counter++;
        return;
    }

    /* Set the link local address with the host MAC address. */
    iface_index = 0;

    /* This assumes we are using the primary network interface (index 0). */
    status = nxd_ipv6_address_set(&server_ip, iface_index, NX_NULL, 10, &address_index);

    /* Check for link local address set error.  */
    if (status) {

        error_counter++;
        return;
    }

    /* Set the host global IP address. We are assuming a 64
       bit prefix here but this can be any value (< 128). */
    status = nxd_ipv6_address_set(&server_ip, iface_index, &server_ip_address, 64, &address_index);

    /* Check for global address set error.  */
    if (status) {

        error_counter++;
        return;
    }

    /* Wait while NetX Duo validates the link local and global address. */
    tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);

#endif /* USE_IPV6 */

    /* OK to start the FTP Server.   */
    status = nx_ftp_server_start(&ftp_server);

    if (status != NX_SUCCESS)
        error_counter++;

    printf("Server started!\n");

    /* FTP server ready to take requests! */

    /* Let the IP threads execute.    */
    tx_thread_relinquish();

    return;
}

#ifdef USE_IPV6
UINT server_login6(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, NXD_ADDRESS *client_ipduo_address, UINT client_port,
    CHAR *name, CHAR *password, CHAR *extra_info)
{
    NX_PARAMETER_NOT_USED(ftp_server_ptr);
    NX_PARAMETER_NOT_USED(client_ipduo_address);
    NX_PARAMETER_NOT_USED(client_port);
    NX_PARAMETER_NOT_USED(name);
    NX_PARAMETER_NOT_USED(password);
    NX_PARAMETER_NOT_USED(extra_info);

    printf("Logged in6!\n");

    /* Always return success.  */
    return NX_SUCCESS;
}

UINT server_logout6(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, NXD_ADDRESS *client_ipduo_address, UINT client_port,
    CHAR *name, CHAR *password, CHAR *extra_info)
{
    NX_PARAMETER_NOT_USED(ftp_server_ptr);
    NX_PARAMETER_NOT_USED(client_ipduo_address);
    NX_PARAMETER_NOT_USED(client_port);
    NX_PARAMETER_NOT_USED(name);
    NX_PARAMETER_NOT_USED(password);
    NX_PARAMETER_NOT_USED(extra_info);

    printf("Logged out6!\n");

    /* Always return success.  */
    return NX_SUCCESS;
}
#else
UINT server_login(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info)
{
    NX_PARAMETER_NOT_USED(ftp_server_ptr);
    NX_PARAMETER_NOT_USED(client_ip_address);
    NX_PARAMETER_NOT_USED(client_port);
    NX_PARAMETER_NOT_USED(name);
    NX_PARAMETER_NOT_USED(password);
    NX_PARAMETER_NOT_USED(extra_info);

    printf("Logged in!\n");
    /* Always return success.  */
    return NX_SUCCESS;
}

UINT server_logout(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info)
{
    NX_PARAMETER_NOT_USED(ftp_server_ptr);
    NX_PARAMETER_NOT_USED(client_ip_address);
    NX_PARAMETER_NOT_USED(client_port);
    NX_PARAMETER_NOT_USED(name);
    NX_PARAMETER_NOT_USED(password);
    NX_PARAMETER_NOT_USED(extra_info);

    printf("Logged out!\n");

    /* Always return success.  */
    return NX_SUCCESS;
}
#endif /* USE_IPV6 */