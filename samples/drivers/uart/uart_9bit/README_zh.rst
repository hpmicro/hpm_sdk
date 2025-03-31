.. _uart_9bit:

UART_9bit
==================

概述
------

该示例工程演示UART工作在9bit模式，进行地址匹配通信的功能。UART主设备可以向具有特定地址的从设备发送数据，而从设备只有在被寻址时才能接收数据。
在本示例中将UART的TX与RX引脚相连，初始化UART并设置地址。UART发送地址和数据，当发出的地址与设置的地址匹配，则UART会接收到发出的数据， 否则不能接收到数据。

硬件设置
------------

- UART的RX与TX引脚相连(请参考  :ref:`引脚描述 <board_resource>`  部分)

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   uart 9bit address match transmit example
   uart send address: 0x76 and data:
   uart not receive any data
   uart send address: 0x77 and data:
   uart receive data:
   0x77 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17

