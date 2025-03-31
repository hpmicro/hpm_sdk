.. _tsn_stmid_frer_ingress:

tsn_stmid_frer_ingress
============================================

概述
------

本示例展示了TSN 帧识别证以及帧冗余传输(IEEE802.1 CB)

- 观察帧识别结果

  - 帧成功识别后，由STMID模块生成递增的序列号，并传递给FRER模块添加R-Tag

- 冗余传输可使用多播机制实现

  - 由于受限于开发板硬件设计，此功能未做展示 (TSW外设不具备数据帧分裂功能)

- 在接收端， 需要另外一块开发板运行tsn_stmid_frer_egress,  观察数据帧的恢复

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

     This is a TSW demo: CB Stmid Frer Ingress
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     Stream Match Count: 1,  SeqNo: 1
     Stream Match Count: 2,  SeqNo: 2
     Stream Match Count: 3,  SeqNo: 3
     Stream Match Count: 4,  SeqNo: 4
     Stream Match Count: 5,  SeqNo: 5
     Stream Match Count: 6,  SeqNo: 6



