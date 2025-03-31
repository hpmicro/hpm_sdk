.. _uart_rx_timeout:

UART接收超时
================

概述
------

该示例工程，UART可以使用接收超时机制实现不定长数据接收（数据量小于程序定义的BUFF_SIZE）。
该方法相对于uart_irq，可以减少接收触发的中断次数倍数。比如接收1000个字节，单字节接收需要1000次中断，而该方法只需要77次。而且可以利用FIFO超时判断是否总线空闲，以此来接收不定长数据。
如果硬件支持接收空闲检测，则可使用uart_hardware_rx_idle的例程。
如果想软件支持接收空闲检测，则可使用uart_software_rx_idle的例程

端口设置
------------

-  串口波特率设置为 ``115200bps`` ，``1个停止位`` ，``无奇偶校验位``

硬件设置
------------

- 使用usb转ttl模块进行测试，将模块的TX接在开发板待测的RX，模块的RX接在开发板待测的TX，模块的GND接在开发板的GND

- 请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。

运行现象
------------

当工程正确运行后，通过串口输入一些数据，串口对应会发送对应的数据。串口终端会以如下形式输出信息：

.. code-block:: console

   UART RX FIFO timeout example
   uart rx data size:10 interrupt count is :1
   uart rx data size:20 interrupt count is :2
   uart rx data size:30 interrupt count is :3
   uart rx data size:40 interrupt count is :4
   uart rx data size:50 interrupt count is :4
   uart rx data size:60 interrupt count is :5
   uart rx data size:70 interrupt count is :6
   uart rx data size:80 interrupt count is :7
   uart rx data size:90 interrupt count is :7
   uart rx data size:100 interrupt count is :8
   uart rx data size:200 interrupt count is :16
   uart rx data size:300 interrupt count is :24
   uart rx data size:400 interrupt count is :31
   uart rx data size:500 interrupt count is :39
   uart rx data size:600 interrupt count is :47
   uart rx data size:700 interrupt count is :54
   uart rx data size:800 interrupt count is :62
   uart rx data size:900 interrupt count is :70
   uart rx data size:1000 interrupt count is :77


- 串口工具现象

   .. image:: doc/uart_rxfifo_timeout.png
      :alt:
