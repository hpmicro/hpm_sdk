.. _modbus_tcp_master:

modbus tcp master
==================================

概述
------

- 该工程演示了modbus tcp主机功能

- 在这个示例工程中， 一个板子作为tcp master， 另一个板子作为tcp slave。 Master发送读取保持寄存器等待slave回应请求数据。

注意
------

- 使用以太网进行modbus通信，与tcp进行通信，必须先复位从机再复位主机。

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 将两个板子的RGMII或RMII端口相连。

运行现象
------------

- 先运行slave，再运行master

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   This is an ethernet demo: modbus tcp (Polling Usage)
   LwIP Version: 2.1.2
   Enet phy init passed !
   Static IP: 192.168.100.11
   Netmask  : 255.255.255.0
   Gateway  : 192.168.100.1
   modbus tcp master example
   Link Status: Down
   client connected server
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:29
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

