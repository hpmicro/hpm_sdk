.. _threadx_lowpower:

ThreadX 低功耗
======================

概述
------

该工程演示了threadx的低功耗模式。

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端会周期性输出线程的运行状态统计信息：

.. code-block:: console

   thread_0_entry
   ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro

              thread 0 events sent:          0
              thread 1 messages sent:        0
              thread 2 messages received:    0
              thread 3 obtained semaphore:   0
              thread 4 obtained semaphore:   0
              thread 5 events received:      0
              thread 6 mutex obtained:       0
              thread 7 mutex obtained:       0

   thread_5_entry
   thread_3_and_4_entry
   thread_3_and_4_entry
   thread_6_and_7_entry
   thread_6_and_7_entry
   thread_1_entry
   thread_2_entry
   ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro

              thread 0 events sent:          1
              thread 1 messages sent:        135701
              thread 2 messages received:    135659
              thread 3 obtained semaphore:   2
              thread 4 obtained semaphore:   2
              thread 5 events received:      0
              thread 6 mutex obtained:       2
              thread 7 mutex obtained:       2

