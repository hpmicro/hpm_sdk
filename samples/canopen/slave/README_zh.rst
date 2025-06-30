.. _canopen_slave:

CANOPEN_SLAVE
==============

概述
------

canopen_slave_demo 例程提供如下功能：

- HeartBeat 心跳测试

- NMT 测试

- SDO 测试

- PDO 测试

硬件设置
------------

一块开发板下载 master 例程, 一块开发板下载 slave 例程, 连接两块开发板

运行步骤
------------

程序下载成功后, 会主动上发 pre-optional 信息, 等待主栈下发指令

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
   rxSize = 11, txSize = 8
   excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
   enter slave mode ~ ~


测试
~~~~~~

The current routine is a canopen slave role. The board runs the current routine and connects to the board running the master role.

Slave 端打印如下所示：

.. code-block:: console

   rxSize = 11, txSize = 8
   excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
   enter slave mode ~ ~

Slave 端不会有特殊打印, 判断测试结果以 master 端打印为准。

