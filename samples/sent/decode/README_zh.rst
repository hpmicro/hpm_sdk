sent解码
========

概述
----

- sent_signal_decode示例工程演示了GPTMR的测量PWM功能并且使用DMA来采集并且解码sent信号
- 实例初始化sent信号解码相关初始化之后, 会一直等待编码端的sent信号。
- 实例中的pwm_process_sent API函数用于解码sent信号, 如解析完整一帧sent信号则返回成功,若解析不完整信号或者非sent信号则返回错误。
- 实例中串口只输出解码成功的sent信号。对于解码失败的信号, 会过滤接着等待下一个完整的sent信号。


硬件设置
--------

-  请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
-  连接sent信号至GPTMR输入捕获引脚。sent信号可用sent/encode实例产生的信号。

运行现象
--------

- 运行工程，示例执行采集sent信号，若有sent信号输入，串口终端输出如下信息，分别对应的一帧完整的sent协议帧。

.. code:: console

   sent signal decode demo
   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

   stat:00 data:01 02 03 04 05 06 07 08 crc:0b  pause:true

