.. _tsn_time_aware_scheduling:

tsn_time_aware_scheduling
==================================================

概述
------

本示例展示了TSN TAS(IEEE802.1 Qbv))流量整形输出

- 在PC端通过网络抓包工具来观察流量整形输出

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用以太网线缆连接PC以太网端口和开发板RGMIII端口

工程配置
------------

- 无

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：


.. code-block:: console

     This is a TSW demo: Time Aware Scheduing
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex


* 打开以太网抓包工具

  - 设置过滤规则：eth.src == 98:2c:bc:b1:9f:17 and arp

    **注：实际需要根据tsn mac地址设置eth source address**
  - 观察流量整形输出：流量的间隔/流量的周期

    .. image:: doc/tsn_time_aware_scheduling.png
       :alt: img
