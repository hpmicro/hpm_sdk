.. _tamp:

TAMP
========

概述
------

- 本示例演示 **tamper** 侵入检测模块的功能，有主动模式和被动模式。

- 主动模式有2个引脚（一对），1个引脚输出特定的波形序列，1个引脚接收特定的波形序列，当收到数据序列与输出序列不一致时，代表侵入事件发生。

- 被动模式有1个引脚，检测电平信号。本示例中，非侵入状态电平为低电平，侵入状态电平为高电平。

硬件设置
------------

- 引脚配置参见对应board文件夹下的readme.md

- 将主动模式的2个引脚使用杜邦线连接

- 将被动模式的引脚接GND

运行现象
------------

- 当工程正确运行后，串口终端会输出如下信息：


.. code-block:: console

   Tamper example

- 当将主动模式的2个引脚的连接断开，串口终端会输出如下信息：


.. code-block:: console

   Tamper active mode trigged!
   Tamper active mode trigged!
   Tamper active mode trigged!
   Tamper active mode trigged!
   Tamper active mode trigged!

- 当将被动模式的引脚的接高电平，串口终端会输出如下信息：


.. code-block:: console

   Tamper passive mode trigged!
   Tamper passive mode trigged!
   Tamper passive mode trigged!
   Tamper passive mode trigged!
   Tamper passive mode trigged!

