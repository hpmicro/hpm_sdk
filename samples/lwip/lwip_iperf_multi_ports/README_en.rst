.. _lwip_iperf_multi_ports:

lwip_iperf_multi_ports
============================================

Overview
--------

This example shows TCP/UDP throughput performance test  through iperf (https://iperf.fr/iperf-download.php)  on the specified network port with a total of four modes as follows:

- MCU acts as TCP Server / PC acts as TCP Client

- MCU acts as TCP Client / PC acts as TCP Server

- MCU acts as UDP Server / PC acts as UDP Client

- MCU acts as UDP Client / PC acts as UDP Server

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect two Ethernet ports on PC to a RGMII port and a RMII port on the development board with two Ethernet cables

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

     This is an ethernet demo: Iperf On Multiple Ports
     LwIP Version: 2.1.2
     Enet1 Reference Clock: Internal Clock
     Enet0 init passed!
     Enet1 init passed!
     ================ Network Interface 1 ================
     Link Status: Down
     ================ Network Interface 0 ================
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     ================ Network Interface 0 ================
     IPv4 Address: 192.168.100.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.100.1
     ================ Network Interface 1 ================
     Link Status: Up
     Link Speed:  100Mbps
     Link Duplex: Full duplex
     ================ Network Interface 1 ================
     IPv4 Address: 192.168.200.10
     IPv4 Netmask: 255.255.255.0
     IPv4 Gateway: 192.168.200.1


- Select a network interface card


  .. code-block:: console

     ====================================================================
     1: RGMII(ENET0)
     2: RMII(ENET1)
     Please enter one of network interface cards above (e.g. 1 or 2): 1


- Select a test mode


  .. code-block:: console

     ================ Network Interface 0 ================
     1: TCP Server Mode
     2: TCP Client Mode
     3: UDP Server Mode
     4: UDP Client Mode
     Please enter one of modes above (e.g. 1 or 2 ...):


  - TCP Server mode

    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 1


    - Run iperf in command line


      .. code-block:: console

         iperf -c 192.168.100.10 -i 1


      **Note: If RMII port is selected, the host IP address should be 192.168.200.10.**
    - Watch the result in command line


      .. code-block:: console

         ------------------------------------------------------------
         Client connecting to 192.168.100.10, TCP port 5001
         TCP window size: 64.0 KByte (default)
         ------------------------------------------------------------
         [364] local 192.168.100.5 port 60317 connected with 192.168.100.10 port 5001
         [ ID] Interval       Transfer     Bandwidth
         [364]  0.0- 1.0 sec   112 MBytes   936 Mbits/sec
         [364]  1.0- 2.0 sec   112 MBytes   940 Mbits/sec
         [364]  2.0- 3.0 sec   111 MBytes   931 Mbits/sec
         [364]  3.0- 4.0 sec   107 MBytes   900 Mbits/sec
         [364]  4.0- 5.0 sec   101 MBytes   848 Mbits/sec
         [364]  5.0- 6.0 sec   106 MBytes   885 Mbits/sec
         [364]  6.0- 7.0 sec   111 MBytes   930 Mbits/sec
         [364]  7.0- 8.0 sec   112 MBytes   937 Mbits/sec
         [364]  8.0- 9.0 sec   112 MBytes   938 Mbits/sec
         [364]  9.0-10.0 sec   111 MBytes   931 Mbits/sec
         [364]  0.0-10.0 sec  1.07 GBytes   917 Mbits/sec



  - TCP Client mode

    - Run iperf in command line


      .. code-block:: console

         iperf -s -i 1



    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 2



    - Watch the result in command line


      .. code-block:: console

         Server listening on TCP port 5001
         TCP window size: 64.0 KByte (default)
         [412] local 192.168.100.5 port 5001 connected with 192.168.100.10 port 49153
         [ ID] Interval       Transfer     Bandwidth
         [412]  0.0- 1.0 sec  96.6 MBytes   810 Mbits/sec
         [412]  1.0- 2.0 sec   101 MBytes   850 Mbits/sec
         [412]  2.0- 3.0 sec   101 MBytes   849 Mbits/sec
         [412]  3.0- 4.0 sec   101 MBytes   848 Mbits/sec
         [412]  4.0- 5.0 sec   100 MBytes   843 Mbits/sec
         [412]  5.0- 6.0 sec  99.1 MBytes   832 Mbits/sec
         [412]  6.0- 7.0 sec  97.6 MBytes   818 Mbits/sec
         [412]  7.0- 8.0 sec  99.1 MBytes   832 Mbits/sec
         [412]  8.0- 9.0 sec   101 MBytes   850 Mbits/sec
         [412]  9.0-10.0 sec   101 MBytes   848 Mbits/sec
         [412]  0.0-10.0 sec   999 MBytes   837 Mbits/sec


  - UDP Server mode

    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 3



    - Run iperf in command line


      .. code-block:: console

         iperf -u -c 192.168.100.10 -i 1 -b 1000M



      **Note: If the RMII port is selected, the host IP address should be 192.168.200.10 and the maximum bandwidth can only be set to 100M** .

    - Watch the result in command line


      .. code-block:: console

         ------------------------------------------------------------
         Client connecting to 192.168.100.10, UDP port 5001
         Sending 1470 byte datagrams
         UDP buffer size: 64.0 KByte (default)
         ------------------------------------------------------------
         [352] local 192.168.100.5 port 63727 connected with 192.168.100.10 port 5001
         [ ID] Interval       Transfer     Bandwidth
         [352]  0.0- 1.0 sec  81.1 MBytes   681 Mbits/sec
         [352]  1.0- 2.0 sec  87.1 MBytes   731 Mbits/sec
         [352]  2.0- 3.0 sec  86.7 MBytes   727 Mbits/sec
         [352]  3.0- 4.0 sec  81.1 MBytes   680 Mbits/sec
         [352]  4.0- 5.0 sec  77.5 MBytes   650 Mbits/sec
         [352]  5.0- 6.0 sec  79.1 MBytes   663 Mbits/sec
         [352]  6.0- 7.0 sec  83.8 MBytes   703 Mbits/sec
         [352]  7.0- 8.0 sec  84.6 MBytes   710 Mbits/sec
         [352]  8.0- 9.0 sec  87.0 MBytes   730 Mbits/sec
         [352]  9.0-10.0 sec  86.6 MBytes   727 Mbits/sec
         [352]  0.0-10.0 sec   835 MBytes   700 Mbits/sec
         [352] Server Report:
         [352]  0.0-10.0 sec   834 MBytes   700 Mbits/sec  0.000 ms  183/595206 (0.031%)
         [352]  0.0-10.0 sec  1 datagrams received out-of-order
         [352] Sent 595391 datagrams



  - UDP Client mode

    - Run iperf in command line


      .. code-block:: console

         iperf -u -s -i 1


    - Enter a test mode code in the serial terminal


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 4


    - Watch the result in command line


      .. code-block:: console

         ------------------------------------------------------------
         Server listening on UDP port 5001
         Receiving 1470 byte datagrams
         UDP buffer size: 64.0 KByte (default)
         ------------------------------------------------------------
         [340] local 192.168.100.5 port 5001 connected with 192.168.100.10 port 49153
         [ ID] Interval       Transfer     Bandwidth       Jitter   Lost/Total Datagrams
         [340]  0.0- 1.0 sec  60.3 MBytes   506 Mbits/sec  0.000 ms 1598966172/43089 (3.7e+006%)
         [340]  1.0- 2.0 sec  60.3 MBytes   506 Mbits/sec  0.012 ms    6/43000 (0.014%)
         [340]  1.0- 2.0 sec  30994 datagrams received out-of-order
         [340]  2.0- 3.0 sec  60.3 MBytes   506 Mbits/sec  0.014 ms    3/42999 (0.007%)
         [340]  2.0- 3.0 sec  30997 datagrams received out-of-order
         [340]  3.0- 4.0 sec  60.3 MBytes   506 Mbits/sec  0.059 ms   -2/43012 (-0.0046%)
         [340]  3.0- 4.0 sec  31011 datagrams received out-of-order
         [340]  4.0- 5.0 sec  60.2 MBytes   505 Mbits/sec  0.000 ms   25/42987 (0.058%)
         [340]  4.0- 5.0 sec  30966 datagrams received out-of-order
         [340]  5.0- 6.0 sec  60.3 MBytes   506 Mbits/sec  0.009 ms    7/43000 (0.016%)
         [340]  5.0- 6.0 sec  30993 datagrams received out-of-order
         [340]  6.0- 7.0 sec  60.3 MBytes   506 Mbits/sec  0.015 ms   11/43000 (0.026%)
         [340]  6.0- 7.0 sec  30989 datagrams received out-of-order
         [340]  7.0- 8.0 sec  60.2 MBytes   505 Mbits/sec  0.040 ms   98/43034 (0.23%)
         [340]  7.0- 8.0 sec  30950 datagrams received out-of-order
         [340]  8.0- 9.0 sec  60.0 MBytes   503 Mbits/sec  0.072 ms  200/42966 (0.47%)
         [340]  8.0- 9.0 sec  30840 datagrams received out-of-order
         [340]  0.0-10.0 sec   602 MBytes   505 Mbits/sec  0.071 ms  465/429784 (0.11%)
         [340]  0.0-10.0 sec  309495 datagrams received out-of-order



- Exception exit

  Press the "space" key to abort the test, and then a network card and a test mode can be reselected.
