sent编码
========

概述
----

- sent_signal_encode示例工程演示了使用SPI实现sent编码。
- 例子进行相关sent编码初始化之后, 会一直发送sent信号。

硬件设置
--------

-  请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
-  sent输出信号即是SPI的MOSI引脚,可配合接到sent/decoder示例工程的对应输入引脚进行测试或使用逻辑分析仪查看波形。

提示
----

-  缺省产生的SENT带暂停信号，若不需要暂停信号，可重新定义\ ``SENT_ENABLE_PAUSE_NIBBLE``\ 为\ ``0``\ 或者在工程的CMakeLists.txt中增加\ ``sdk_compile_definitions(-DSENT_ENABLE_PAUSE_NIBBLE=0)``\ 。
-  缺省SENT的空闲极性是低电平，若需要更改为高电平，可重新定义\ ``SENT_IDLE_POLARITY``\ 为\ ``1``\ 或者在工程的CMakeLists.txt中增加\ ``sdk_compile_definitions(-DSENT_IDLE_POLARITY=1)``\ 。

运行现象
--------

- 先运行decode,再运行encode

- 运行工程，示例执行输出sent信号，串口终端输出如下信息，分别对应的一帧完整的sent协议帧。

.. code:: console

   sent signal encode demo
   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true
