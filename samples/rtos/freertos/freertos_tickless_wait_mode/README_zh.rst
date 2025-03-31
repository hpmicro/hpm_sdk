.. _freertos_tickless_wait_mode:

FreeRTOS Tickless低功耗模式-Wait模式
==========================================================

概述
------

该工程演示了freertos的tickless低功耗模式。SOC会在空闲时进入等待模式。LED灯会在freeRTOS运行时亮起，在tickless低功耗模式期间保持熄灭。

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，LED会开始闪烁，串口终端会输出如下信息：

.. code-block:: console

   task2 is running.
   task1 is running.
   task1 is running.
   task1 is running.
   task1 is running.

