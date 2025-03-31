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

- Ethernet Port Settings: Refer to :ref:`Ethernet common project settings <ethernet_port_configurations_en>`

- Ethernet DHCP Configurations

  - the configurations in the `CMakeLists.txt` as follows:

    - sdk_compile_definitions(-DLWIP_DHCP=0): Disable DHCP feature

    - sdk_compile_definitions(-DLWIP_DHCP=1): Enable DHCP feature

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


  .. code-block:: console

     This is an ethernet demo: Iperf
     LwIP Version: 2.1.2
     Enet phy init passed !
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

    - Press "RESETN" key on the development board, and wait for the hints


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
         [304] local 192.168.100.5 port 56608 connected with 192.168.100.10 port 5001
         [ ID] Interval       Transfer     Bandwidth
         [304]  0.0- 1.0 sec  11.3 MBytes  94.9 Mbits/sec
         [304]  1.0- 2.0 sec  11.3 MBytes  94.5 Mbits/sec
         [304]  2.0- 3.0 sec  11.3 MBytes  94.4 Mbits/sec
         [304]  3.0- 4.0 sec  11.3 MBytes  94.5 Mbits/sec
         [304]  4.0- 5.0 sec  11.3 MBytes  94.4 Mbits/sec
         [304]  5.0- 6.0 sec  11.3 MBytes  94.5 Mbits/sec
         [304]  6.0- 7.0 sec  11.3 MBytes  94.4 Mbits/sec
         [304]  7.0- 8.0 sec  11.3 MBytes  94.5 Mbits/sec
         [304]  8.0- 9.0 sec  11.3 MBytes  94.4 Mbits/sec
         [304]  9.0-10.0 sec  11.3 MBytes  94.4 Mbits/sec
         [304]  0.0-10.0 sec   113 MBytes  94.4 Mbits/sec


  - TCP Client mode

    - Press "RESETN" key on the development board, and wait for the hints


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
         [364] local 192.168.100.5 port 5001 connected with 192.168.100.10 port 49153
         [ ID] Interval       Transfer     Bandwidth
         [364]  0.0- 1.0 sec  10.7 MBytes  89.6 Mbits/sec
         [364]  1.0- 2.0 sec  11.2 MBytes  94.1 Mbits/sec
         [364]  2.0- 3.0 sec  11.2 MBytes  94.1 Mbits/sec
         [364]  3.0- 4.0 sec  11.2 MBytes  94.1 Mbits/sec
         [364]  4.0- 5.0 sec  11.2 MBytes  94.2 Mbits/sec
         [364]  5.0- 6.0 sec  11.2 MBytes  94.1 Mbits/sec
         [364]  6.0- 7.0 sec  11.2 MBytes  94.1 Mbits/sec
         [364]  7.0- 8.0 sec  11.2 MBytes  94.2 Mbits/sec
         [364]  8.0- 9.0 sec  11.2 MBytes  94.0 Mbits/sec
         [364]  9.0-10.0 sec  11.2 MBytes  94.2 Mbits/sec
         [364]  0.0-10.0 sec   112 MBytes  93.6 Mbits/sec


  - UDP Server mode

    - Press "RESETN" key on the development board, and wait for the hints


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):


    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 3


    - Run iperf  in command line


      .. code-block:: console

         iperf -u -c 192.168.100.10 -i 1 -b 100M



      **Note: If the RGMII port is selected, the maximum bandwidth can be set to 1000M** .

    - Watch test result


      .. code-block:: console

         ------------------------------------------------------------
         Client connecting to 192.168.100.10, UDP port 5001
         Sending 1470 byte datagrams
         UDP buffer size: 64.0 KByte (default)
         ------------------------------------------------------------
         [304] local 192.168.100.5 port 60607 connected with 192.168.100.10 port 5001
         [ ID] Interval       Transfer     Bandwidth
         [304]  0.0- 1.0 sec  11.3 MBytes  94.6 Mbits/sec
         [304]  1.0- 2.0 sec  11.3 MBytes  94.4 Mbits/sec
         [304]  2.0- 3.0 sec  11.3 MBytes  94.5 Mbits/sec
         [304]  3.0- 4.0 sec  11.2 MBytes  94.2 Mbits/sec
         [304]  4.0- 5.0 sec  11.3 MBytes  94.8 Mbits/sec
         [304]  5.0- 6.0 sec  11.2 MBytes  94.1 Mbits/sec
         [304]  6.0- 7.0 sec  11.3 MBytes  94.8 Mbits/sec
         [304]  7.0- 8.0 sec  11.2 MBytes  94.2 Mbits/sec
         [304]  8.0- 9.0 sec  11.2 MBytes  94.4 Mbits/sec
         [304]  9.0-10.0 sec  11.3 MBytes  94.7 Mbits/sec
         [304]  0.0-10.0 sec   113 MBytes  94.5 Mbits/sec
         [304] Server Report:
         [304]  0.0-10.0 sec   113 MBytes  94.5 Mbits/sec  0.002 ms    0/80328 (0%)
         [304] Sent 80328 datagrams


  - UDP Client mode

    - Press "RESETN" key on the development board, and wait for the hints


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
         [  3] local 192.168.100.5 port 5001 connected with 192.168.100.10 port 49153
         [ ID] Interval       Transfer     Bandwidth        Jitter   Lost/Total Datagrams
         [  3]  0.0- 1.0 sec  11.3 MBytes  94.6 Mbits/sec   0.208 ms  137/ 8183 (1.7%)
         [  3] 0.00-1.00 sec  6033 datagrams received out-of-order
         [  3]  1.0- 2.0 sec  11.2 MBytes  94.1 Mbits/sec   0.203 ms    0/ 8000 (0%)
         [  3] 1.00-2.00 sec  6000 datagrams received out-of-order
         [  3]  2.0- 3.0 sec  11.2 MBytes  94.1 Mbits/sec   0.208 ms    0/ 8000 (0%)
         [  3] 2.00-3.00 sec  6000 datagrams received out-of-order
         [  3]  3.0- 4.0 sec  11.2 MBytes  94.1 Mbits/sec   0.207 ms    0/ 8000 (0%)
         [  3] 3.00-4.00 sec  6000 datagrams received out-of-order
         [  3]  4.0- 5.0 sec  11.2 MBytes  94.1 Mbits/sec   0.208 ms    0/ 8000 (0%)
         [  3] 4.00-5.00 sec  6000 datagrams received out-of-orderS
         [  3]  5.0- 6.0 sec  11.2 MBytes  94.1 Mbits/sec   0.207 ms    0/ 8000 (0%)
         [  3] 5.00-6.00 sec  6000 datagrams received out-of-order
         [  3]  6.0- 7.0 sec  11.2 MBytes  94.1 Mbits/sec   0.208 ms    0/ 8000 (0%)
         [  3] 6.00-7.00 sec  6000 datagrams received out-of-order
         [  3]  7.0- 8.0 sec  11.2 MBytes  94.1 Mbits/sec   0.209 ms    0/ 8000 (0%)
         [  3] 7.00-8.00 sec  6000 datagrams received out-of-order
         [  3]  8.0- 9.0 sec  11.2 MBytes  94.1 Mbits/sec   0.206 ms    0/ 8000 (0%)
         [  3] 8.00-9.00 sec  6000 datagrams received out-of-order
         [  3]  0.0-10.0 sec   112 MBytes  94.1 Mbits/sec   0.234 ms  135/79952 (0.17%)
         [  3] 0.00-9.97 sec  59861 datagrams received out-of-order


- Exception exit

  Press the "space" key to abort the test, and then a test mode can be reselected.
