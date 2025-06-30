.. _lwip_iperf:

lwip_iperf
====================

Overview
--------

This example shows TCP/UDP throughput performance test through iperf (https://iperf.fr/iperf-download.php) with a total of four modes as follows:

- MCU acts as TCP Server / PC acts as TCP Client

- MCU acts as TCP Client / PC acts as TCP Server

- MCU acts as UDP Server / PC acts as UDP Client

- MCU acts as UDP Client / PC acts as UDP Server

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect an Ethernet port on PC to a RGMII port or a RMII port on the development board with an Ethernet cable

Project Configurations
----------------------

- Ethernet Port Settings: Refer to `Ethernet common project settings <../doc/Ethernet_Common_Project_Settings_en>`_

- Ethernet DHCP Configurations

  - the configurations in the `CMakeLists.txt` as follows:

    - sdk_compile_definitions(-DLWIP_DHCP=0): Disable DHCP feature

    - sdk_compile_definitions(-DLWIP_DHCP=1): Enable DHCP feature

    - sdk_compile_definitions(-DENABLE_TSW_RECEIVE_INTERRUPT=0): Disable TSW receive interrupt on CPU port

    - sdk_compile_definitions(-DENABLE_TSW_RECEIVE_INTERRUPT=1): Enable TSW receive interrupt on CPU port


Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


  .. code-block:: console

    This is a TSW demo: Iperf (Polling Usage)
    LwIP Version: 2.1.2
    TSW phy init passed !
    Link Status: Down
    Link Status: Up
    Link Speed:  1000Mbps
    Link Duplex: Full duplex
    IPv4 Address: 192.168.100.10
    IPv4 Netmask: 255.255.255.0
    IPv4 Gateway: 192.168.100.1

    1: TCP Server Mode
    2: TCP Client Mode
    3: UDP Server Mode
    4: UDP Client Mode
    Please enter one of modes above (e.g. 1 or 2 ...):


- Select a test mode and Run iperf

  - TCP Server mode

    - Press the "space" key in the serial terminal, and wait for the hints


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):


    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter the test mode code: 1


    - Run iperf  in command line


      .. code-block:: console

         iperf -c 192.168.100.10 -i 1


    - Watch the result


      .. code-block:: console

          ------------------------------------------------------------
          Client connecting to 192.168.100.10, TCP port 5001
          TCP window size: 64.0 KByte (default)
          ------------------------------------------------------------
          [360] local 192.168.100.5 port 64459 connected with 192.168.100.10 port 5001
          [ ID] Interval       Transfer     Bandwidth
          [360]  0.0- 1.0 sec  42.6 MBytes   357 Mbits/sec
          [360]  1.0- 2.0 sec  42.5 MBytes   357 Mbits/sec
          [360]  2.0- 3.0 sec  42.6 MBytes   358 Mbits/sec
          [360]  3.0- 4.0 sec  42.5 MBytes   356 Mbits/sec
          [360]  4.0- 5.0 sec  42.6 MBytes   357 Mbits/sec
          [360]  5.0- 6.0 sec  42.6 MBytes   358 Mbits/sec
          [360]  6.0- 7.0 sec  42.6 MBytes   357 Mbits/sec
          [360]  7.0- 8.0 sec  42.6 MBytes   357 Mbits/sec
          [360]  8.0- 9.0 sec  42.5 MBytes   357 Mbits/sec
          [360]  9.0-10.0 sec  42.5 MBytes   357 Mbits/sec
          [360]  0.0-10.0 sec   426 MBytes   357 Mbits/sec


  - TCP Client mode

    - Press the "space" key in the serial terminal, and wait for the hints


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):


    - Run iperf  in command line


      .. code-block:: console

         iperf -s -i 1


    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 2


    - Watch the result


      .. code-block:: console

          ------------------------------------------------------------
          Server listening on TCP port 5001
          TCP window size: 64.0 KByte (default)
          ------------------------------------------------------------
          [412] local 192.168.100.5 port 5001 connected with 192.168.100.10 port 49153
          [ ID] Interval       Transfer     Bandwidth
          [412]  0.0- 1.0 sec  18.1 MBytes   152 Mbits/sec
          [412]  1.0- 2.0 sec  19.1 MBytes   160 Mbits/sec
          [412]  2.0- 3.0 sec  19.1 MBytes   160 Mbits/sec
          [412]  3.0- 4.0 sec  19.1 MBytes   160 Mbits/sec
          [412]  4.0- 5.0 sec  19.1 MBytes   160 Mbits/sec
          [412]  5.0- 6.0 sec  19.1 MBytes   160 Mbits/sec
          [412]  6.0- 7.0 sec  19.1 MBytes   160 Mbits/sec
          [412]  7.0- 8.0 sec  19.1 MBytes   160 Mbits/sec
          [412]  8.0- 9.0 sec  19.1 MBytes   160 Mbits/sec
          [412]  9.0-10.0 sec  19.0 MBytes   160 Mbits/sec
          [412]  0.0-10.0 sec   190 MBytes   159 Mbits/sec


  - UDP Server mode

    - Press the "space" key in the serial terminal, and wait for the hints


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):


    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 3


    - Run iperf in command line


      .. code-block:: console

         iperf -u -c 192.168.100.10 -i 1 -b 1000M



      **Note: If the RGMII port is selected, the maximum bandwidth can be set to 1000M** .

    - Watch test result


      .. code-block:: console

          ------------------------------------------------------------
          Client connecting to 192.168.100.10, UDP port 5001
          Sending 1470 byte datagrams
          UDP buffer size: 64.0 KByte (default)
          ------------------------------------------------------------
          [360] local 192.168.100.5 port 49290 connected with 192.168.100.10 port 5001
          [ ID] Interval       Transfer     Bandwidth
          [360]  0.0- 1.0 sec  55.4 MBytes   465 Mbits/sec
          [360]  1.0- 2.0 sec  68.7 MBytes   576 Mbits/sec
          [360]  2.0- 3.0 sec  73.5 MBytes   617 Mbits/sec
          [360]  3.0- 4.0 sec  78.3 MBytes   657 Mbits/sec
          [360]  4.0- 5.0 sec  80.1 MBytes   672 Mbits/sec
          [360]  5.0- 6.0 sec  81.7 MBytes   685 Mbits/sec
          [360]  6.0- 7.0 sec  82.6 MBytes   693 Mbits/sec
          [360]  7.0- 80 sec  84.2 MBytes   706 Mbits/sec
          [360]  8.0- 9.0 sec  84.4 MBytes   708 Mbits/sec
          [360]  9.0-10.0 sec  79.6 MBytes   667 Mbits/sec
          [360]  0.0-10.0 sec   768 MBytes   645 Mbits/sec
          [360] Server Report:
          [360]  0.0- 9.8 sec   303 MBytes   259 Mbits/sec  0.179 ms 154530/216247 (71%)
          [360]  0.0- 9.8 sec  88679 datagrams received out-of-order
          [360] Sent 548137 datagrams


  - UDP Client mode

    - Press the "space" key in the serial terminal, and wait for the hints


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):


    - Run iperf  in command line


      .. code-block:: console

         iperf -u -s -i 1


    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 4


    - Watch test result


      .. code-block:: console

          ------------------------------------------------------------
          Server listening on UDP port 5001
          Receiving 1470 byte datagrams
          UDP buffer size: 64.0 KByte (default)
          ------------------------------------------------------------
          [344] local 192.168.100.5 port 5001 connected with 192.168.100.10 port 49153
          [ ID] Interval       Transfer     Bandwidth       Jitter   Lost/Total Datagrams
          [344]  0.0- 1.0 sec  20.7 MBytes   174 Mbits/sec  0.047 ms 2668/17445 (15%)
          [344]  1.0- 2.0 sec  20.6 MBytes   173 Mbits/sec  0.069 ms    0/14727 (0%)
          [344]  2.0- 3.0 sec  20.7 MBytes   173 Mbits/sec  0.139 ms    0/14739 (0%)
          [344]  3.0- 4.0 sec  20.7 MBytes   173 Mbits/sec  0.284 ms    0/14746 (0%)
          [344]  4.0- 5.0 sec  20.7 MBytes   173 Mbits/sec  0.044 ms    0/14739 (0%)
          [344]  5.0- 6.0 sec  20.7 MBytes   173 Mbits/sec  0.054 ms    0/14736 (0%)
          [344]  6.0- 7.0 sec  20.7 MBytes   173 Mbits/sec  0.080 ms    0/14743 (0%)
          [344]  7.0- 8.0 sec  20.7 MBytes   173 Mbits/sec  0.122 ms    0/14751 (0%)
          [344]  8.0- 9.0 sec  20.6 MBytes   173 Mbits/sec  0.046 ms    0/14721 (0%)
          [344]  0.0- 9.8 sec   203 MBytes   173 Mbits/sec  0.414 ms 2668/147330 (1.8%)


- Exception exit

  Press the "space" key to abort the test, and then a test mode can be reselected.
