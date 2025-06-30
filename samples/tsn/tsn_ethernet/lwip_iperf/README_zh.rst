.. _tsn_lwip_iperf:

lwip_iperf
========================

概述
------

本示例通过iperf (https://iperf.fr/iperf-download.php) 测试TCP/UDP吞吐性能，共有如下四种模式：

- MCU用作TCP服务端 / PC用作TCP客户端

- MCU用作TCP客户端 / PC用作TCP服务端

- MCU用作UDP服务端 / PC用作UDP客户端

- MCU用作UDP客户端 / PC用作UDP服务端

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用以太网线缆连接PC以太网端口和开发板以太网端口

工程配置
------------

- 以太网端口配置：参考 : `以太网通用工程设置 <../doc/Ethernet_Common_Project_Settings_zh>`_

- 以太网DHCP配置

  - lwIP sample:  在`CMakeLists.txt`中，支持如下配置:

    - sdk_compile_definitions(-DLWIP_DHCP=0): 禁用DHCP功能

    - sdk_compile_definitions(-DLWIP_DHCP=1): 启用DHCP功能

    - sdk_compile_definitions(-DENABLE_TSW_RECEIVE_INTERRUPT=0): 禁用TSW CPU端口接收中断

    - sdk_compile_definitions(-DENABLE_TSW_RECEIVE_INTERRUPT=1): 启用TSW CPU端口接收中断

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：


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


* 选择测试模式并运行iperf

  - TCP服务端模式

    - 在串口终端按空格键，并等待提示


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):


    - 在串口终端输入测试模式的代号


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 1


    - 在命令行中运行iperf


      .. code-block:: console

         iperf -c 192.168.100.10 -i 1


    - 观察结果


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


  - TCP客户端模式

    - 在串口终端按空格键，并等待提示


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):


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


  - UDP服务端模式

    - 在串口终端按空格键，并等待提示


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):


    - 在串口终端输入测试模式的代号


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 3


    - 在命令行中运行iperf


      .. code-block:: console

         iperf -u -c 192.168.100.10 -i 1 -b 1000M



      **注：如果选择的是RGMII网口，最大带宽能设置为1000M**

    - 观察结果


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


  - UDP客户端模式

    - 在串口终端按空格键，并等待提示


      .. code-block:: console

         1: TCP Server Mode
         2: TCP Client Mode
         3: UDP Server Mode
         4: UDP Client Mode
         Please enter one of modes above (e.g. 1 or 2 ...):



    - 在命令行中运行iperf


      .. code-block:: console

         iperf -u -s -i 1



    - 在串口终端输入测试模式的代号


      .. code-block:: console

         Please enter one of modes above (e.g. 1 or 2 ...): 4


    - 观察结果


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


- 异常退出

  按空格键退出测试，然后重新选择测试模式
