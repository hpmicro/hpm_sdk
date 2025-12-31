.. _freertos_tickless_stop_mode:

FreeRTOS Tickless Stop Mode
======================================================

Overview
--------

The freertos_tickless example shows freertos tickless stop mode. Soc will enter stop mode in idle states to feature reduce power consumption. The led will turn on when freertos is running at normal mode, and will turn off during tickless mode.

Board Setting
-------------

- There is a PUART development board available

  - Using PUART as Console

  - Using PUART as a wake-up source

  - Connect serial port (from PUART, refer to  :ref:`PUART pins <board_resource>` ) to PC

- There is no PUART development board available

  - Use normal uart console

  - Using WKUP pin as wake-up source

Running the example
-------------------

When the example runs successfully, the LED starts flashing, the led indicates freertos is running in normal mode. After a while, the led turns off, indicating freertos has entered tickless stop mode. After some time, the led turns on again, indicating freertos has exited tickless stop mode. Send data through PUART will also wake up freeRTOS from stop mode. For boards without PUART, the WKUP pin is used as the wake-up source. Following message is displayed in the terminal:

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
