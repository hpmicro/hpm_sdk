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

硬件设置
------------

无特殊设置

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
   *     d - DGO GPR retention, SW one-shot wakeup                       *
   *                                                                     *
   ***********************************************************************

