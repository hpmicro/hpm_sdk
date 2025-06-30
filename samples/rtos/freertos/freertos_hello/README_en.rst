.. _freertos_hello:

FreeRTOS Hello
============================

Overview
--------

The freertos_hello example shows freertos task scheduling function.

Board Setting
-------------

No special settings are required

Project Configuration
---------------------

set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1) select GPTMR as FreeRTOS's tick source。
set(CONFIG_FREERTOS_TIMER_RESOURCE_PWM 1) select PWM as FreeRTOS's tick source。

sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1) disable interrupt preemption
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=0) enable interrupt preemption

Running the example
-------------------

When the example runs successfully, the LED starts flashing and following message is displayed in the terminal:

.. code-block:: console

   task2 is running.
   Toggle the led pin level.
   LED is off.
   LED is off.
   LED is off.
   LED is off.
   task2 is running.
   Toggle the led pin level.
   LED is on.
   LED is on.
   LED is on.
   task2 is running.
   Toggle the led pin level.
   LED is off.
   LED is off.
   LED is off.
   LED is off.

