.. _cmsis_os2_blinky_freertos:

CMSIS OS2 Blinky (FreeRTOS)
======================================================

Overview
--------

This project demonstrates the kernel scheduling function of CMSIS OS2 FreeRTOS. The 5 threads of this project wake up each other, count the number of runs of the 5 threads, and print them out on the serial port.

Board Setting
-------------

No special settings are required

Running the example
-------------------

When the example runs successfully, the following message is displayed in the terminal:

.. code-block:: console

   run times of phaseA, phaseB, phaseC, phaseD, clock
   1, 0, 0, 0, 0
   1, 0, 0, 0, 1
   1, 1, 0, 0, 2
   1, 1, 0, 0, 3
   1, 1, 1, 0, 4
   1, 1, 1, 0, 5

