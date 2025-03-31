.. _tsn_frame_preemption_ingress:

tsn_frame_preemption_ingress
========================================================

概述
------

本示例展示了TSN 帧抢占(IEEE802.1 Qbu)输出

- 观察可抢占流与快速流发送后，MMS的fragment tx count的计数值

  - 递增表示成功发送了可抢占流

- 在接收端，需要另外一块开发板运行tsn_frame_preemption_egress, 观察数据帧的接收顺序以及assembly ok count的计数值

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用以太网线缆连接两块开发板的RGMIII端口

工程配置
------------

- 无

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：


.. code-block:: console

     This is a TSW demo: Frame Preemption Ingress
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     FPE MMS Fragment Tx Counter: 1
     ==================================================================
     FPE MMS Fragment Tx Counter: 2
     ==================================================================
     FPE MMS Fragment Tx Counter: 3
     ==================================================================
     FPE MMS Fragment Tx Counter: 4
     ==================================================================
     FPE MMS Fragment Tx Counter: 5
     ==================================================================
     FPE MMS Fragment Tx Counter: 6
     ==================================================================
     FPE MMS Fragment Tx Counter: 7
     ==================================================================
     FPE MMS Fragment Tx Counter: 8
     ==================================================================


