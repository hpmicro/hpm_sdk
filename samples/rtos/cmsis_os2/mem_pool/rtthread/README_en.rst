.. _cmsis_os2_mem_pool_rtthread:

CMSIS OS2 MEM POOL (RTThread)
==========================================================

Overview
--------

This project demonstrates the memory pool and message queue function of CMSIS OS2 RTThread. This project creates 2 threads, one to send messages, one to receive messages and print them.

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
    / | \     3.1.5 build Dec 22 2023
    2006 - 2020 Copyright by rt-thread team
   do components initialization.
   initialize rti_board_end:0 done
   initialize finsh_system_init:0 done
   msh >app_msg: received [cmd = 1, data = 0x1]
   app_msg: received [cmd = 1, data = 0x2]
   app_msg: received [cmd = 1, data = 0x3]
   app_msg: received [cmd = 1, data = 0x4]
   app_msg: received [cmd = 1, data = 0x5]
   app_msg: received [cmd = 1, data = 0x6]
   app_msg: received [cmd = 1, data = 0x7]
   app_msg: received [cmd = 1, data = 0x8]
   app_msg: received [cmd = 1, data = 0x9]
   app_msg: received [cmd = 1, data = 0xA]
   app_msg: received [cmd = 1, data = 0xB]
   app_msg: received [cmd = 1, data = 0xC]
   app_msg: received [cmd = 1, data = 0xD]
   app_msg: received [cmd = 1, data = 0xE]
   app_msg: received [cmd = 1, data = 0xF]
   app_msg: received [cmd = 1, data = 0x10]
   app_msg: received [cmd = 1, data = 0x11]
   app_msg: received [cmd = 1, data = 0x12]
   app_msg: received [cmd = 1, data = 0x13]

