.. _lwip_iperf_multi_ports:

lwip_iperf_multi_ports
============================================

概述
------

本示通过iperf (https://iperf.fr/iperf-download.php)在多端口中选择指定端口测试TCP/UDP吞吐性能，共有四种模式：

- MCU用作TCP服务端 / PC用作TCP客户端

- MCU用作TCP客户端 / PC用作TCP服务端

- MCU用作UDP服务端 / PC用作UDP客户端

- MCU用作UDP客户端 / PC用作UDP服务端

硬件设置
------------

* 使用USB Type-C线缆连接PWR DEBUG端口

* 使用两根以太网线缆连接PC以太网端口（或扩展网口）和开发板上RGMII端口以及RMII端口

工程配置
------------

- 以太网端口配置：参考 :ref:`以太网通用工程设置 <ethernet_port_configurations_zh>`

- 以太网DHCP配置

  - lwIP sample:  在`CMakeLists.txt`中，支持如下配置:

    - sdk_compile_definitions(-DLWIP_DHCP=0): 禁用DHCP功能

    - sdk_compile_definitions(-DLWIP_DHCP=1): 启用DHCP功能

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：


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


* 选择网卡


  .. code-block:: console

     ====================================================================
     1: RGMII(ENET0)
     2: RMII(ENET1)
     Please enter one of network interface cards above (e.g. 1 or 2): 1


* 选择测试模式


  .. code-block:: console

     ================ Network Interface 0 ================
     1: TCP Server Mode
     2: TCP Client Mode
     3: UDP Server Mode
     4: UDP Client Mode
     Please enter one of modes above (e.g. 1 or 2 ...):



  - TCP 服务端模式

    - 在串口终端输入测试模式的代号


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 1


    - 在命令行中运行iperf


      .. code-block:: console

         iperf -c 192.168.100.10 -i 1



      **注：如果选择的是RMII网口，IP为192.168.200.10**
    - 在命令行中观察结果


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



  - TCP客户端模式

    - 在命令行中运行iperf


      .. code-block:: console

          iperf -s -i 1



    - 在串口终端输入测试模式的代号


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 2



    - 观察结果


      .. code-block:: console

         ------------------------------------------------------------
         Server listening on TCP port 5001
         TCP window size: 64.0 KByte (default)
         ------------------------------------------------------------
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



  - UDP服务端模式

    - 在串口终端输入测试模式的代号


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 3



    - 在命令行中运行iperf


      .. code-block:: console

         iperf -u -c 192.168.100.10 -i 1 -b 1000M



      **注：如果选择的是RMII网口，IP为192.168.200.10，且最大带宽只能设置为100M**

    - 观察结果


      .. code-block:: console

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



  - UDP客户端模式

    - 在命令行中运行iperf


      .. code-block:: console

         iperf -u -s -i 1



    - 在串口终端输入测试模式的代号


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 4



    - 观察结果


      .. code-block:: console

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



- 异常退出

  按空格键退出测试，然后重新选择网卡和测试模式
