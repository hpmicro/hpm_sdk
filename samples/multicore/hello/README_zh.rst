.. _hello_demo:

Hello 示例
================

概述
------

多核示例工程在Core0上运行串口回显示例，在core1上运行GPTMR中断中GPIO翻转RGB LED示例。

在本工程中:
 - 串口输出 "hello world"; 键盘手动输入字符串信息，通过串口打印出来

 - RGB LED会在红、绿、蓝三色中依次切换

硬件设置
------------

  BOOT_PIN 应该设置为：0-OFF, 1-OFF

生成、编译和调试多核工程
------------------------------------

请参见 :ref:`Multicore 通用说明 <multicore_general_description>`

执行结果
------------

- 以HPM6750EVKMINI为例，当双核示例正常运行的时候，会打印如下信息：


.. code-block:: console

           Copying secondary core image to destination memory: 0x1180000
           Hello world for multicore! Secondary core control RGB leds blinking...


- RGB LED会在红、绿、蓝三色中依次切换
