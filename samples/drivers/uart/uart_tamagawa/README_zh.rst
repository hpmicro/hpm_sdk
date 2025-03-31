.. _uart_tomagawa:

UART多摩川通信
==================

概述
------

该示例工程演示UART与多摩川编码器通信的功能。
在本示例中，配置UART发送由硬件触发。UART先写入命令数据，此时命令被保存在FIFO中，等到硬件触发事件发生后，UART会发出命令并接收编码器返回数据， 接收指定长度数据后产生中断，完成一次通信。

注意
------

多摩川传感器输出RS485信号， 需要使用合适的RS485转TTL模块(需要支持2.5Mbps)，将信号转换后与UART相应引脚连接。

硬件设置
------------

- 将多摩川传感器输出信号DAT+/DAT-与RS485转TTL模块的RS485端口相连，并对多摩川模块供电。

- 将开发板UART的RX/TX引脚(请参考  :ref:`引脚描述 <board_resource>`  部分)与RS485转TTL模块的TTL信号相连。

- 根据RS485转TTL模块是否需要方向选择信号， 选择将开发板UART的DE引脚与之相连。

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   uart tamagawa transfer complete.
   uart sent cmd: 0x02
   uart receive excepted count data: 0x02 0x20 0xb4 0xbd 0x1c 0x37
   uart tamagawa transfer complete.
   uart sent cmd: 0x02
   uart receive excepted count data: 0x02 0x20 0x50 0xa9 0x78 0xa3
   uart tamagawa transfer complete.
   uart sent cmd: 0x02
   uart receive excepted count data: 0x02 0x20 0x81 0xe5 0x6e 0x28
   uart tamagawa transfer complete.
   uart sent cmd: 0x02
   uart receive excepted count data: 0x02 0x20 0x71 0x26 0x5c 0x29
   uart tamagawa transfer complete.
   uart sent cmd: 0x02
   uart receive excepted count data: 0x02 0x20 0xa4 0x4d 0x39 0xf2
   uart tamagawa transfer complete.
   uart sent cmd: 0x02
   uart receive excepted count data: 0x02 0x20 0xd6 0x3a 0x17 0xd9
   uart tamagawa transfer complete.
   uart sent cmd: 0x02
   uart receive excepted count data: 0x02 0x20 0x43 0x7f 0x03 0x1d

