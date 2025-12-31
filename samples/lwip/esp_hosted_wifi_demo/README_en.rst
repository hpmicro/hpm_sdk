.. _esp_hosted_wifi_demo:

esp_hosted_wifi_demo
==========================

Overview
--------

- Espressif's esp-hosted MCU solution enables a host MCU to communicate with ESP32/ESP32-C3 Wi-Fi chips via SPI/SDIO, providing wireless networking capability. This port currently supports only the SPI interface.
- This example uses FreeRTOS as the real-time operating system and LwIP as the network stack, with CherryShell providing a command-line interface.
- The port is based on FreeRTOS and LwIP, supporting common Wi-Fi operation commands and network test commands.
- The reference hardware setup uses Espressif's ESP32-WROVER-E module (with ESP32 chip) as the Wi-Fi module, connected to the HPMicro host board via SPI.

Main features:
- Supports STA/AP/APSTA mode switching
- Command support for Wi-Fi scan, connect, disconnect, etc.
- Supports DHCP, ping, iperf, and other network functions
- Command-line interaction via CherryShell
- Adapted to HPMicro SPI driver, with DMA acceleration

CMake Configuration Parameters
-----------------------------------

This project is built with CMake. Main parameters:

- ``CONFIG_FREERTOS``: Enable FreeRTOS RTOS (must be enabled)
- ``CONFIG_ESP_HOST_MCU``: Enable esp-hosted-mcu middleware
- ``CONFIG_ESP_HOSTED_SPI``: Use SPI as the communication interface between host and ESP chip
- ``CONFIG_HPM_SPI``: Enable HPMicro platform SPI component
- ``CONFIG_DMA_MGR``: Enable DMA manager for efficient SPI transfer
- ``CONFIG_LWIP``: Enable LwIP stack
- ``CONFIG_LWIP_DHCP``: Enable DHCP client
- ``CONFIG_CHERRYSH``: Enable CherryShell command line
- ``CONFIG_CHERRYSH_INTERFACE``: Command line interface type (e.g. uart)
- ``configTOTAL_HEAP_SIZE``: FreeRTOS heap size (e.g. 61440 bytes)
- ``CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_PIN``: SPI handshake pin
- ``CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_PIN``: Data ready pin
- ``CONFIG_ESP_HOSTED_SPI_CLOCK_FREQ_MHZ``: SPI clock frequency, set to 10MHz in this example. ESP32 SPI slave supports up to 10MHz; adjust according to actual chip capability.
- ``CONFIG_ESP_HOSTED_DFLT_TASK_STACK``: Stack size for esp-hosted SPI communication and data processing tasks
- ``CONFIG_ESP_HOSTED_RPC_TASK_STACK_SIZE``: Stack size for esp-hosted RPC listener/handler tasks
- ``DCONFIG_ESP_HOSTED_SPI_MODE``: SPI mode selection. This example uses mode 2 (CPOL=1, CPHA=0), must match ESP32 firmware.
- ``DCONFIG_SLAVE_IDF_TARGET_ESP32``： Specify the slave chip type as ESP32. For other models like ESP32-C3, change to the corresponding macro definition DCONFIG_SLAVE_IDF_TARGET_ESP32C3.

See ``CMakeLists.txt`` for details.

How to Run the Example
----------------------

* Build and flash the program

* Serial terminal output example:

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
        cherry@hpm5301evklite:/$ [I/H_API] I ESP-Hosted starting. Hosted_Tasks: prio:23, stack: 512 RPC_task_stack: 1024

        [I/H_API] I ** add_esp_wifi_remote_channels **

        [I/transport] I Add ESP-Hosted channel IF[1]: S[0] Tx[80057f10] Rx[8005f7ce]

        [I/transport] I Add ESP-Hosted channel IF[2]: S[0] Tx[8003c50c] Rx[8005f7ce]

        [I/spi_wrapper] I Transport: SPI, Mode:2 Freq:10MHz TxQ:2000 RxQ:2000
        GPIOs: CLK:0 MOSI:0 MISO:0 CS:0 HS:44 DR:45 SlaveReset:9

        [I/transport] I Attempt connection with slave: retry[0]

        [I/spi] I Resetting slave

        [I/spi] I Resetting slave on SPI bus with pin 9

        [I/spi] I Staring SPI task

        [I/transport] I Waiting for esp_hosted slave to be ready

        [I/transport] I Received INIT event from ESP32 peripheral

        [I/transport] I EVENT: 12

        [I/transport] I Identified slave [esp32]

        [I/transport] I EVENT: 11

        [I/transport] I capabilities: 0xf8

        [I/transport] I Features supported are:

        [I/transport] I            - HCI over SPI

        [I/transport] I            - BT/BLE dual mode

        [I/transport] I EVENT: 13

        [I/transport] I ESP board type is : 0


        [I/transport] I Base transport is set-up, TRANSPORT_TX_ACTIVE

        [I/H_API] I Transport active

        [I/transport] I Slave chip Id[12]

        [I/transport] I raw_tp_dir[-], flow_ctrl: low[0] high[0]

        [I/transport] I transport_delayed_init

        [I/spi] I Received INIT event

        [I/esp_wlan_ops] I AP started, SSID: hpm_ap_test

        [I/RPC_WRAP] I ESP Event: wifi station started

        [I/RPC_WRAP] I ESP Event: wifi station started

        [I/RPC_WRAP] I ESP Event: softap started

        [I/RPC_WRAP] I ESP Event: softap started

        [W/rpc_rsp] W Hosted RPC_Resp [0x21a], uid [7], resp code [12298]


Command Line Usage
------------------

After power-up, the serial terminal will output startup information. Type ``help`` to see all supported commands. Common commands:

- ``wifi scan``: Scan for nearby Wi-Fi APs
- ``wifi join <SSID> <PASSWORD>``: Connect to a specified Wi-Fi
- ``wifi stop``: Disconnect from current Wi-Fi
- ``mem``: View memory usage
- ``thread``: View FreeRTOS task info
- ``iperf``: Network performance test
- ``ping <ip/domain>``: Network connectivity test

Typical Usage Flow
------------------

.. code-block:: console

   wifi scan
   wifi join MySSID MyPassword
   ping 192.168.1.1
   iperf -c 192.168.1.100

Porting Notes and Precautions
-----------------------------

- SPI communication parameters must match those on the ESP32 firmware side (mode, clock, pins, etc.).
- The Wi-Fi module must be flashed with esp-hosted MCU slave firmware.
- If you encounter connection failures or scan errors, please check hardware wiring and SPI configuration.

References
----------

- [Espressif esp-hosted documentation](https://components.espressif.com/components/espressif/esp_hosted/)

