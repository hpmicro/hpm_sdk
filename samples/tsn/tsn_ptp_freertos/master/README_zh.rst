.. _tsn_ptp_freertos_master:

tsn_ptp_freertos_master
==============================================

概述
------

本示例展示了TSN PTP Master的功能

- Master与Slave通过gPTP协议通讯，从而实现Slave端的时钟同步

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用以太网线缆连接开发板TSN RGMII端口另一个块开发板的TNS RGMII端口

工程配置
------------

- 无

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：


.. code-block:: console

   This is a TSW demo: PTP Master
   TSW phy init passed !
   starting task 'syn eth0' ... done
   PTP4TSYNC | add domain 0
   PTP4TSYNC | set ptp traffic priority=0 (Background) for domain 0
   PTP4TSYNC | add eth0 to domain 0
   Link Status: Down
   Link Status: Up
   Link Speed:  1000Mbps
   Link Duplex: Full duplex
   domain   0: domaintime=1726133403.019751880 or Thu Sep 12 09:30:03 2024
   domain   0: domaintime=1726133406.025912960 or Thu Sep 12 09:30:06 2024
   domain   0: domaintime=1726133409.031911990 or Thu Sep 12 09:30:09 2024

