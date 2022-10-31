# TCP/UDP 吞吐量测试

## 概述

本示例通过iperf (https://iperf.fr/iperf-download.php) 测试TPC/UDP吞吐性能，共有如下四种模式：

- MCU用作TCP服务端 / PC用作TCP客户端
- MCU用作TCP客户端 / PC用作TCP服务端
- MCU用作UDP服务端 / PC用作UDP客户端
- MCU用作UDP客户端 / PC用作UDP服务端

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用以太网线缆连接PC以太网端口和开发板RGMII或RMII端口

## 工程配置

- 在文件[CMakeLists.txt](./CmakeLists.txt)中,  根据开发板原理图，设置匹配的以太网PHY类型，例如："set(COFNIG_ENET_PHY_DP83848 1)"

## 运行示例

* 编译下载程序
* 串口终端显示如下信息：

  ```
  This is an ethernet demo: Iperf
  LwIP Version: 2.1.2
  Local IP: 192.168.11.201
  Speed Rate:100Mbps
  Reference Clock: Internal Clock
  Enet phy init passes !
  Static IP: 192.168.11.201
  NETMASK  : 255.255.255.0
  Gateway  : 192.168.11.1

  1: TCP Server Mode
  2: TCP Client Mode
  3: UDP Server Mode
  4: UDP Client Mode
  Please enter the test mode code:
  ```
* 选择测试模式并运行iperf

  - TCP服务端模式

    - 按开发板RESETN键，并等待提示

      ```
      1: TCP Server Mode
      2: TCP Client Mode
      3: UDP Server Mode
      4: UDP Client Mode
      Please enter one of modes above (e.g. 1 or 2 ...):
      ```
    - 在串口终端输入测试模式的代号

      ```
      Please enter the test mode code: 1
      ```
    - 在命令行中运行iperf

      ```
      iperf -c 192.168.11.201 -i 1
      ```
    - 观察结果

      ```
      ------------------------------------------------------------
      Client connecting to 192.168.11.201, TCP port 5001
      TCP window size: 64.0 KByte (default)
      ------------------------------------------------------------
      [304] local 192.168.11.5 port 59602 connected with 192.168.11.201 port 5001
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
      ```
  - TCP客户端模式

    - 按开发板RESETN键，并等待提示

      ```
      1: TCP Server Mode
      2: TCP Client Mode
      3: UDP Server Mode
      4: UDP Client Mode
      Please enter the test mode code:
      ```
    - 在命令行中运行iperf

      ```
      iperf -s -i 1
      ```
    - 在串口终端输入测试模式的代号

      ```
      Please enter the test mode code: 2
      ```
    - 观察结果

      ```
      ------------------------------------------------------------
      Server listening on TCP port 5001
      TCP window size: 64.0 KByte (default)
      ------------------------------------------------------------
      [364] local 192.168.11.5 port 5001 connected with 192.168.11.201 port 49153
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
      ```
  - UDP服务端模式

    - 按开发板RESETN键，并等待提示

      ```
      1: TCP Server Mode
      2: TCP Client Mode
      3: UDP Server Mode
      4: UDP Client Mode
      Please enter the test mode code:
      ```
    - 在串口终端输入测试模式的代号

      ```
      Please enter the test mode code: 3
      ```
    - 在命令行中运行iperf

      ```
      iperf -u -c 192.168.11.201 -i 1 -b 100M
      ```
    - 观察结果

      ```
      ------------------------------------------------------------
      Client connecting to 192.168.11.201, UDP port 5001
      Sending 1470 byte datagrams
      UDP buffer size: 64.0 KByte (default)
      ------------------------------------------------------------
      [304] local 192.168.11.5 port 61844 connected with 192.168.11.201 port 5001
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
      ```
  - UDP客户端模式

    - 按开发板RESETN键，并等待提示

      ```
      1: TCP Server Mode
      2: TCP Client Mode
      3: UDP Server Mode
      4: UDP Client Mode
      Please enter the test mode code:
      ```
    - 在命令行中运行iperf

      ```
      iperf -u -s -i 1
      ```
    - 在串口终端输入测试模式的代号

      ```
      Please enter the test mode code: 4
      ```
    - 观察结果

      ```
       ------------------------------------------------------------
      Server listening on UDP port 5001
      Receiving 1470 byte datagrams
      UDP buffer size: 64.0 KByte (default)
      ------------------------------------------------------------
      [  3] local 192.168.11.5 port 5001 connected with 192.168.11.201 port 49153
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
      ```
