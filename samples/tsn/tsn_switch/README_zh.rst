.. _tsn_switch:

tsn_switch
==========

概述
------

本示例展示TSW作为基本交换机的功能

硬件设置
------------

任意选择交换机的两个端口，分别接到两台PC（或网络设备）

运行现象
------------

与交换机相连接的两台PC（或网络设备）可以相互通讯

启动完成后，串口会先打印三个端口的配置信息，并在链路状态变化时打印对应端口信息，例如：

.. code-block:: console

   Port1: MII 100Mbps, Switch MAC 98:2c:bc:b1:9f:17, Lookup dest MAC 38:14:28:14:90:78
   Port2: MII 100Mbps, Switch MAC 98:2c:bc:b1:9f:18, Lookup dest MAC 38:14:28:14:90:79
   Port3: RGMII 1000Mbps, Switch MAC 98:2c:bc:b1:9f:19, Lookup dest MAC 38:14:28:14:90:7a

   ================ TSW PHY on Port1 ================
   Link Status: Up
   Link Speed:  100Mbps
   Link Duplex: Full duplex

   ================ TSW PHY on Port2 ================
   Link Status: Up
   Link Speed:  100Mbps
   Link Duplex: Full duplex

   ================ TSW PHY on Port3 ================
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex

注意
------

1. 两台PC（或网络设备）的IP必须在同一网段
2. 请查阅errata关于使用预防措施的详细信息
