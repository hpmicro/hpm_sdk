.. _freertos_hello:

FreeRTOS Hello
============================

概述
------

该工程演示了freertos的任务调度功能。

硬件设置
------------

无特殊设置

工程配置
-------------------

set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)可以选择使用GPTMR作为FreeRTOS的系统时钟来源。
set(CONFIG_FREERTOS_TIMER_RESOURCE_PWM 1)可以选择使用PWM作为FreeRTOS的系统时钟来源。

sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)禁止中断抢占功能
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=0)启用中断抢占功能

运行现象
------------

当工程正确运行后，LED会开始闪烁，串口终端会输出如下信息：

.. code-block:: console

   Toggle the led pin level.
   task2 is running.
   LED is off.
   LED is off.
   LED is off.
   LED is off.
   Toggle the led pin level.
   task2 is running.
   LED is on.
   LED is on.
   LED is on.
   Toggle the led pin level.
   task2 is running.
   LED is off.
   LED is off.
   LED is off.
   LED is off.

