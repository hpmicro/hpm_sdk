.. _threadx_hello:

ThreadX Hello
==========================

概述
------

该工程演示了threadx的任务调度功能。本例程创建了8个线程。线程0周期性的打印状态消息到终端上，并且唤醒线程5。线程1发送数据到消息队列上，线程2从消息队列上读取消息。线程3和4试图获得同一个信号量，并持有一段时间。线程6和7试图获得同一个互斥量，并持有一段时间。

硬件设置
------------

无特殊设置

工程配置
-------------------

set(CONFIG_ECLIPSE_THREADX_TIMER_RESOURCE_MTIMER 1)配置使用mchtmr作为系统的滴答源。

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

