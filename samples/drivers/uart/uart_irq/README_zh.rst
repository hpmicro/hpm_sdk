.. _uart_interrupt:

UART中断
======================

概述
------

**uart** 通用异步收发器UART驱动示例工程展示了UART的配置使用方法，使用中断实现串口数据的接收和发送
- 示例代码中，进行UART串口资源的初始化配置，设置串口时钟及收发FIFO深度，配置中断，实现串口收据的发送与接收

端口设置
------------

-  串口波特率设置为 ``115200bps`` ，``1个停止位`` ，``无奇偶校验位``

注意
------

-  在工程的CMakeLists.txt文件中, ``sdk_compile_definitions(-DCONFIG_UART_FIFO_MODE=1)``,默认使能FIFO, 若需要禁能FIFO, 请在工程中更改``sdk_compile_definitions(-DCONFIG_UART_FIFO_MODE=0)``或者删除``sdk_compile_definitions(-DCONFIG_UART_FIFO_MODE=1)``

-  在FIFO模式下，接收FIFO中的数据只有达到设置的接收FIFO阈值，才会进入一次有效接收数据中断。

-  示例中的接收FIFO阈值设置是接收FIFO深度的一半，每个SOC的接收FIFO深度不同，接收FIFO阈值也不同，FIFO深度可通过``uart_get_fifo_size``API或者``UART_SOC_FIFO_SIZE``宏定义获取。

-  示例在使能FIFO模式下，接收每达到设置的接收FIFO阈值会进入一次接收中断，进而进行发送。按照FIFO阈值的整数倍进行回显。

-  示例中在禁能FIFO模式下，每接收一字节就会进入一次接收中断，进而进行发送。接收多少字节就回显多少字节。

-  示例中在使能FIFO模式下，如果输入的字符串不是FIFO阈值的整数倍，那么回显的字符串会以FIFO阈值的整数倍进行回显，剩余的字节会等待下一次接收补充回显。

运行现象
------------

- 当工程正确运行后，通过串口手动输入字符串，如：``1234567890987654``，则串口终端会收到如下信息：

.. code-block:: console

    uart driver example
    use fifo mode, the uart RX/TX FIFO depth:16 bytes
    set fifo threshold to 8 bytes; uart will send back received characters on every 8 bytes
    1234567890987654

- 从串口终端信息可看到，实例使用的是FIFO模式，接收FIFO深度为16字节，设置的接收FIFO阈值为8字节。