.. _tsn_stmid_frer_egress:

tsn_stmid_frer_egress
==========================================

概述
------

本示例展示了TSN帧识别和冗余消除(IEEE802.1 CB)

- 观察帧信息

  - 所观察到的帧为经过冗余消除算法后的帧

  - 帧序号与原始帧一致

  - "Presendted Frames"表示当前已恢复的帧数量

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

     This is a TSW demo: CB Stmid Frer Egress
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     Valid: 1, SID: 1, SeqNo: 0
     RX-Time: 20.295925680
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 1

     Valid: 1, SID: 1, SeqNo: 1
     RX-Time: 20.799703960
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 2

     Valid: 1, SID: 1, SeqNo: 2
     RX-Time: 21.303476990
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 3

     Valid: 1, SID: 1, SeqNo: 3
     RX-Time: 21.807250010
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 4

     Valid: 1, SID: 1, SeqNo: 4
     RX-Time: 22.311024700
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 5

     Valid: 1, SID: 1, SeqNo: 5
     RX-Time: 22.814797840
     RX Frame Length: 54
     38 14 28 14 90 78 98 2c bc b1 9f 17 08 06 00 01 08 00 06 04 00 01 98 2c bc b1 9f 17 c0 a8 64 0a 00 00 00 00 00 00 c0 a8 64 05 00 00 00 00 00 00 00 00 00 00 00 00
     Presented Frames: 6



