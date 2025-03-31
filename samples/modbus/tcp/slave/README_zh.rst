.. _modbus_tcp_slave:

modbus tcp slave
================================

概述
------

- 该工程演示了modbus tcp主机功能

- 在这个示例工程中， 一个板子作为tcp master， 另一个板子作为tcp slave。 Master发送读取保持寄存器等待slave回应请求数据。

注意
------

- 使用以太网进行modbus通信，与tcp从机通信，必须先复位从机再复位主机。

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
   Static IP: 192.168.100.10
   Netmask  : 255.255.255.0
   Gateway  : 192.168.100.1
   modbus tcp slave example
   Link Status: Down
   server litened
   parse ok, recv master msg len:12
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12
   parse ok, recv master msg len:12

