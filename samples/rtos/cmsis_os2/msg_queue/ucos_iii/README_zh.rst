.. _cmsis_os2_msg_queue_ucos_iii:

CMSIS OS2 MSG QUEUE (ucos_iii)
============================================================

概述
------

该工程演示了CMSIS OS2 ucos_iii的消息队列功能。该工程创建2个线程，一个发送消息，一个接收消息并打印。

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   app_msg: received [cmd = 1, data = 0x1]
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

