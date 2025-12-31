.. _freertos_tickless_stop_mode:

FreeRTOS Tickless低功耗模式-Stop模式
==========================================================

概述
------

该工程演示了freertos的tickless低功耗模式。为了进一步降低功耗，SOC会在空闲时进入停止模式。LED灯会在freeRTOS运行时亮起，在tickless低功耗模式期间保持熄灭。

硬件设置
------------

- 有PUART的开发板

  - 使用PUART作为Console

  - 使用PUART作为唤醒源

  - 将串口(PUART, 具体引脚信息请参考 :ref:`PUART引脚 <board_resource>` )连接到PC.

- 没有PUART的开发板

  - 使用正常的UART Console

  - 使用WKUP引脚作为唤醒源

运行现象
------------

当工程正确运行后，LED会开始闪烁. LED灯亮表示freertos处于正常运行模式，过一段时间后LED灯熄灭，表示freertos进入了tickless低功耗模式。再过一段时间后LED灯再次亮起，表示freertos退出了tickless低功耗模式。有PUART的开发板通过PUART发送数据也会唤醒freertos退出停止模式，没有PUART的开发板通过WKUP引脚作为唤醒源。串口终端会输出如下信息：

.. code-block:: console

   task2 is running.
   task1 is running.
   Enter stop mode, tick 1
   Exit stop mode
   task2 is running.
   Enter stop mode, tick 4002
   Exit stop mode
   task2 is running.
   Enter stop mode, tick 8003
   Exit stop mode
   task1 is running.
   Enter stop mode, tick 10002
   Exit stop mode
   task2 is running.
   Enter stop mode, tick 12004
