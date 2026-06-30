.. _uart_trigger_dfu:

UART Trigger DFU
======================

概述
------

uart_trigger_dfu示例工程演示了UART触发DFU功能。该应用程序会监视控制台UART以获取特定的触发序列。当用户在控制台UART上输入"dfu"时，设备将重启进入DFU模式，从而可以通过USB或其他DFU接口进行固件升级。此示例需要配合DFU bootloader使用，确保设备已经正确烧录了DFU bootloader。

硬件设置
------------

- 将控制台UART连接到PC

运行现象
------------

当工程正确运行时，串口终端会输出如下信息：

.. code-block:: console

   DFU UART trigger demo
   Type 'dfu' on console UART to reboot into DFU mode
   dfu trigger received, reboot to DFU bootloader...

在控制台UART上输入"dfu"即可触发重启进入DFU模式。
