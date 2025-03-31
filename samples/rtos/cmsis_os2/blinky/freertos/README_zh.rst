.. _cmsis_os2_blinky_freertos:

CMSIS OS2 Blinky (FreeRTOS)
======================================================

概述
------

该工程演示了CMSIS OS2 FreeRTOS的内核调度功能。该工程的5个线程相互唤醒，统计5个线程的运行次数，并在串口中打印出来。

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   run times of phaseA, phaseB, phaseC, phaseD, clock
   1, 0, 0, 0, 0
   1, 0, 0, 0, 1
   1, 1, 0, 0, 2
   1, 1, 0, 0, 3
   1, 1, 1, 0, 4
   1, 1, 1, 0, 5

