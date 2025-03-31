.. _mhd_wifi_demo:

mhd_wifi_demo
==========================

Overview
--------

This example shows the usage of AP6256/AP6745 Wi-Fi module

The following types of command are supported:

- wifi commands:

  - wifi scan: Scan the AP list

  - wifi scanresults: print out the AP list

  - wifi sta_start <SSID> <PASSWORD> : connect to an AP with the <SSID> and <PASSWORD>

  - wifi sta_stop : disconnect from the AP

- internet related commands:

  - iperf: test the iperf performance

  - ping

  - start_httpserver: Start the http server

- misc.

  - reset: Reset the board

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Select the voltage of the AP6255_EVB as needed (select 1.8V if the SD slot on the board supports 1.8V)

- Connect the AP6255_EVB to the SD slot

- Connect the `WL_REG_ON` to the `BOARD_APP_SDIO_WIFI_WL_REG_ON_PIN` defined in `board.h`

- Connect the `WL_HWAKE` to the `BOARD_APP_SDIO_WIFI_OOB_PIN` defined in `board.h`

- Connect the USB port on the AP6255_EVB to a USB port

- See more definitions in the `board.h`

Project Configurations
----------------------

**Limitation**  :
 - ``nds-gcc`` toolchain version must be v5.40 or higher version

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


.. code-block:: console

     ----------------------------------------------------------------------
     $$\   $$\ $$$$$$$\  $$\      $$\ $$\
     $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
     $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
     $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
     $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
     $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
     $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
     \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
     ----------------------------------------------------------------------
     [WiFi] DHCP CLIENT hostname MHD_WIFI
     [WiFi] WL_REG_ON Down
     [WiFi] WL_REG_ON Up$
     [WiFi] SDIO Base F1134000
     [WiFi] SET F2 blksz 512
     [WiFi] chip_id 4345, chip_rev 9
     [WiFi] Wlan RAM size: c8000
     [WiFi] Time for WiFi FW/NV download: 36 ms
     [WiFi] HT CLK Activated [26]
     [WiFi] SR Enabled
     [WiFi] Wlan Bus Up
     [WiFi] MHD priority[50] stack[0x0] stack_size[4096]
     [WiFi] Turn on Polling mode [1]
     [WiFi] Started MHD Thread
     [WiFi] Turn off TX Glomming
     [WiFi] Turn on APSTA
     [WiFi] Turn on AMPDU TX
     [WiFi] Set country code US
     [WiFi] Set wl UP
     [WiFi] Set GMode
     [WiFi] Chip: 4345/9
     [WiFi] MAC: 08:E9:F6:0E:70:2A
     [WiFi] wl0: Nov 28 2024 08:43:46 version 7.45.96.219 (g27a577ef) FWID 01-e6290e47
     [WiFi] CLM: API: 12.2 Data: 1.0.1 Compiler: 1.29.4 ClmImport: 1.39.2 Creation: 2022-08-16 11:23:10
     [WiFi] MHD: Version 1.0.11 B003 Dec  5 2024 14:11:04

- Type "help" to see the supported commands

- Typical use case:


.. code-block:: console

      wifi scan
      wifi scanresults
      wifi sta_start <SSID> <PASSWORD>
      iperf -c <ip addr>
      ping <ip address / domain name>

