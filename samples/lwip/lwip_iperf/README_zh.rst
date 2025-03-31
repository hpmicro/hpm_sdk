.. _lwip_iperf:

lwip_iperf
====================

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

* 使用以太网线缆连接PC以太网端口和开发板RGMII或RMII端口

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


* 选择测试模式并运行iperf

  - TCP服务端模式

    - 按开发板“RESETN”键，并等待提示


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
         [336] local 192.168.100.5 port 56608 connected with 192.168.100.10 port 5001
         [ ID] Interval       Transfer     Bandwidth
         [336]  0.0- 1.0 sec  11.3 MBytes  95.0 Mbits/sec
         [336]  1.0- 2.0 sec  11.3 MBytes  94.5 Mbits/sec
         [336]  2.0- 3.0 sec  11.2 MBytes  94.3 Mbits/sec
         [336]  3.0- 4.0 sec  11.3 MBytes  94.4 Mbits/sec
         [336]  4.0- 5.0 sec  11.3 MBytes  94.4 Mbits/sec
         [336]  5.0- 6.0 sec  11.3 MBytes  94.6 Mbits/sec
         [336]  6.0- 7.0 sec  11.3 MBytes  94.4 Mbits/sec
         [336]  7.0- 8.0 sec  11.3 MBytes  94.5 Mbits/sec
         [336]  8.0- 9.0 sec  11.3 MBytes  94.4 Mbits/sec
         [336]  9.0-10.0 sec  11.3 MBytes  94.5 Mbits/sec
         [336]  0.0-10.0 sec   113 MBytes  94.4 Mbits/sec


  - TCP客户端模式

    - 按开发板“RESETN”键，并等待提示


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
         [388] local 192.168.100.5 port 5001 connected with 192.168.100.10 port 49153
         [ ID] Interval       Transfer     Bandwidth
         [388]  0.0- 1.0 sec  10.6 MBytes  89.2 Mbits/sec
         [388]  1.0- 2.0 sec  11.2 MBytes  94.2 Mbits/sec
         [388]  2.0- 3.0 sec  11.2 MBytes  94.1 Mbits/sec
         [388]  3.0- 4.0 sec  11.2 MBytes  94.1 Mbits/sec
         [388]  4.0- 5.0 sec  11.2 MBytes  94.2 Mbits/sec
         [388]  5.0- 6.0 sec  11.2 MBytes  94.1 Mbits/sec
         [388]  6.0- 7.0 sec  11.2 MBytes  94.1 Mbits/sec
         [388]  7.0- 8.0 sec  11.2 MBytes  94.1 Mbits/sec
         [388]  8.0- 9.0 sec  11.2 MBytes  94.1 Mbits/sec
         [388]  9.0-10.0 sec  11.2 MBytes  94.1 Mbits/sec
         [388]  0.0-10.0 sec   112 MBytes  93.6 Mbits/sec


  - UDP服务端模式

    - 按开发板“RESETN”键，并等待提示


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

         iperf -u -c 192.168.100.10 -i 1 -b 100M



      **注：如果选择的是RGMII网口，最大带宽能设置为1000M**

    - 观察结果


      .. code-block:: console

         ------------------------------------------------------------
         Client connecting to 192.168.100.10, UDP port 5001
         Sending 1470 byte datagrams
         UDP buffer size: 64.0 KByte (default)
         ------------------------------------------------------------
         [336] local 192.168.100.5 port 60607 connected with 192.168.100.10 port 5001
         [ ID] Interval       Transfer     Bandwidth
         [336]  0.0- 1.0 sec  11.5 MBytes  96.3 Mbits/sec
         [336]  1.0- 2.0 sec  11.4 MBytes  95.7 Mbits/sec
         [336]  2.0- 3.0 sec  11.4 MBytes  95.7 Mbits/sec
         [336]  3.0- 4.0 sec  11.4 MBytes  95.7 Mbits/sec
         [336]  4.0- 5.0 sec  11.4 MBytes  95.7 Mbits/sec
         [336]  5.0- 6.0 sec  11.4 MBytes  95.7 Mbits/sec
         [336]  6.0- 7.0 sec  11.4 MBytes  95.7 Mbits/sec
         [336]  7.0- 8.0 sec  11.4 MBytes  95.7 Mbits/sec
         [336]  8.0- 9.0 sec  11.4 MBytes  95.7 Mbits/sec
         [336]  9.0-10.0 sec  11.4 MBytes  95.6 Mbits/sec
         [336]  0.0-10.0 sec   114 MBytes  95.8 Mbits/sec
         [336] Server Report:
         [336]  0.0-10.0 sec   114 MBytes  95.7 Mbits/sec  0.007 ms    0/81430 (0%)
         [336] Sent 81432 datagrams


  - UDP客户端模式

    - 按开发板“RESETN”键，并等待提示


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


- 异常退出

  按空格键退出测试，然后重新选择测试模式
