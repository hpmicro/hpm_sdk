.. _mbx_dualcore:

MBX dualcore
========================

概述
------

该示例程序演示了MBX如何工作。Core 0会发送信息给Core 1，Core 1收到之后并将之返回。

硬件设置
------------

使用USB转串口模块，连接评估板上Core 1调试串口终端(请确认具体开发板   :ref:`引脚描述 <board_resource>`   部分描述)

生成、编译和调试多核工程
------------------------------------

请参见 :ref:`Multicore 通用说明 <multicore_general_description>`

运行现象
------------

- 当示例正常运行的时候，串口终端会打印如下信息:

  - Core 0:

  .. image:: doc/mbx_core0_console_output.png
     :alt: core0_console_output

  - Core 1:

  .. image:: doc/mbx_core1_console_output.png
     :alt: core1_console_output

  - 一旦Core1程序启动之后, Core 1将会将从Core 0接收到的信息返回给Core 0，测试将在Core 0的调试串口将可以看到:

  .. image:: doc/mbx_core0_console_output2.png
     :alt: core0_console_output2
