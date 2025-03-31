.. _cmsis_os2_mem_pool_rtthread:

CMSIS OS2 MEM POOL (RTThread)
==========================================================

概述
------

该工程演示了CMSIS OS2 RTThread的内存池和消息队列功能。该工程创建2个线程，一个发送消息，一个接收消息并打印。

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

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

