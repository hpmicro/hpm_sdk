.. _dmav2_half_transfer_complete_interrupt:

DMAV2半传输中断示例
========================

概述
------

- DMAV2新增了半传输完成中断，本示例工程展示了使用循环传输方式接收UART数据，DMA BUFFER收到一半数据时和收满数据时都会产生中断。该特性可以实现类似于双Buffer乒乓收数据的功能，且DMA不会停止。

硬件设置
------------

- 使用串口调试助手进行发送、接收数据。

- 串口波特率设置为 ``115200bps`` ，``1个停止位`` ，``无奇偶校验位``

运行现象
------------

- 使用串口调试助手发送32字节数据，开发板会回发收到的32字节数据，示例如下：


.. code-block:: text

   » ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   « ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   » ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   « ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   » ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   « ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   » ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF
   « ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF

