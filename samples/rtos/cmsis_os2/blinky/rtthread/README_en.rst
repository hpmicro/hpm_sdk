.. _cmsis_os2_blinky_rtthread:

CMSIS OS2 Blinky (RTThread)
======================================================

Overview
--------

This project demonstrates the kernel scheduling function of CMSIS OS2 RTThread. The 5 threads of this project wake up each other, count the number of runs of the 5 threads, and print them out on the serial port.

Board Setting
-------------

No special settings are required

Running the example
-------------------

When the example runs successfully, the following message is displayed in the terminal:

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

