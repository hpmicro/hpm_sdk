.. _cmsis_os2_blinky_rtthread:

CMSIS OS2 Blinky (RTThread)
======================================================

概述
------

该工程演示了CMSIS OS2 RTThread的内核调度功能。该工程的5个线程相互唤醒，统计5个线程的运行次数，并在串口中打印出来。

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   initialize rti_board_start:0 done

    \ | /
   - RT -     Thread Operating System
    / | \     3.1.5 build Dec 18 2023
    2006 - 2020 Copyright by rt-thread team
   do components initialization.
   initialize rti_board_end:0 done
   initialize finsh_system_init:0 done
   run times of phaseA, phaseB, phaseC, phaseD, threadclock
   msh >1, 0, 0, 0, 0
   1, 0, 0, 0, 1
   1, 1, 0, 0, 2
   1, 1, 0, 0, 3
   1, 1, 1, 0, 4
   1, 1, 1, 0, 5
   1, 1, 1, 1, 6
   1, 1, 1, 1, 7
   2, 1, 1, 1, 8
   2, 1, 1, 1, 9
   2, 2, 1, 1, 10
   2, 2, 1, 1, 11
   2, 2, 2, 1, 12
   2, 2, 2, 1, 13
   2, 2, 2, 2, 14
   2, 2, 2, 2, 15
   3, 2, 2, 2, 16
   3, 2, 2, 2, 17
   3, 3, 2, 2, 18

