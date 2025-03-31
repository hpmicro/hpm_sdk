.. _freertos_tickless_wait_mode:

FreeRTOS Tickless-Wait Mode
======================================================

Overview
--------

The freertos_tickless example shows freertos tickless mode. Soc will enter wait mode in idle states. The led will turn on when freertos is running at normal mode, and will turn off during tickless mode.

Board Setting
-------------

No special settings are required

Running the example
-------------------

When the example runs successfully, the LED starts flashing and following message is displayed in the terminal:

.. code-block:: console

   task2 is running.
   task1 is running.
   task1 is running.
   task1 is running.
   task1 is running.

