.. _canopen_master:

CANOPEN_MASTER
==============

概述
------

The canopen_master_demo 例程提供如下功能：

- HeartBeat 心跳测试

- NMT 测试

- SDO 测试

- PDO 测试

硬件设置
------------

一块开发板下载 master 例程, 一块开发板下载 slave 例程, 连接两块开发板

运行步骤
------------

程序下载成功后, 会进入循环等待从栈信息, 如果从栈是 initial/ pre-optional/ optional 状态, 读取 nodeid, 否则继续循环等待

根据 nodeid 对从栈通过 PDO 读取 信息

根据 nodeid 对从栈通过 SDO 读取/写入 信息

注意: 这个 sample 是为了简单呈现 canopen 通信流程, canopen 支持一主多从, 但是本 sample 只支持一主一从

运行现象
------------

当该例程成功运行后，会在串口终端打印如下:


.. code-block:: console

   ----------------------------------------------------------------------
   $$\   $$\ $$$$$$$\  $$\      $$\ $$\
   $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
   $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
   $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
   $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
   $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
   $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
   \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
   ----------------------------------------------------------------------
   enter master mode and find slave device ~~
   rxSize = 11, txSize = 8
   excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)

测试
~~~~~~

当前例程为 canopen 的 master role, 开发板运行当前例程, 连接运行 slave role 的开发板。


测试成功时: Master 端打印如下所示：

.. code-block:: console

   enter master mode and find slave device ~~
   rxSize = 11, txSize = 8
   excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
   find device, id:70a, status preoptional
   slave enter to optional mode
   PDO test successfully!
   slave HBtime: 1000 ms
   slave HBtime: 2000 ms
   SDO test successfully!
   canopen test done

