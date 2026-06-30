.. _pdgo:

PDGO
========

概述
------

PDGO  例程提供如下功能：

- PDGO 关电（turn-off)

- PDGO 关电并唤醒一次(turn-off and one-shot wake-up)

- PDGO 关电并开启自动唤醒(turn-off and auto wake-up)

- PDGO 通用寄存器数据掉电保持

- WKUP 引脚唤醒关电模式

- WUIO 引脚唤醒关电模式（4个可配置的 WUIO 引脚）

- PCAP（脉冲捕获）唤醒关电模式

- PCNT（脉冲计数）唤醒关电模式

.. note::

   WKUP 引脚、WUIO 引脚、PCAP 和 PCNT 唤醒功能仅在部分SOC系列上支持。
   在其他 SoC 上，选择这些选项将打印"not supported"提示信息。

硬件设置
------------

无特殊设置

测试外部信号唤醒时 (部分SOC系列支持)：

- WKUP 引脚唤醒：将信号源连接到 WKUP 引脚

- WUIO 引脚唤醒：将信号源连接到 WUIO 引脚

- PCAP 唤醒：将脉冲信号连接到配置的 WUIO 引脚

- PCNT 唤醒：将编码器信号（A/B/Z/H）连接到 WUIO 引脚

运行现象
------------

当该例程成功运行后，会在串口终端打印如下菜单:

.. code-block:: console

   ***********************************************************************
   *                                                                     *
   *     DGO Turn-off and Wake-up test                                   *
   *                                                                     *
   *     a - DGO Turn off                                                *
   *     b - DGO Turn off, System one-shot wakeup                        *
   *     c - DGO Turn off, System auto wakeup                            *
   *         Note: After wake-up, use option 'a' to test auto wake-up    *
   *     d - DGO GPR retention, System one-shot wakeup                   *
   *     e - DGO Turn off, WKUP pin wakeup                               *
   *     f - DGO Turn off, WUIO pins wakeup                              *
   *     g - DGO Turn off, PCAP wakeup                                   *
   *     h - DGO Turn off, PCNT wakeup                                   *
   *                                                                     *
   ***********************************************************************
