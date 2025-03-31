.. _gptmr_external_counter_mode:

定时器外部计数模式例子
======================

概述
------

该示例工程演示了GPTMR外部计数模式功能，计数器在输入捕获信号的上升沿递增，当计数值达到重载值时发生重载中断，中断函数进行翻转LED。

硬件设置
------------

请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
连接外部PWM至GPTMR输入捕获引脚。

注意
------

例子重载值默认为0xFFFF, 为了更好的观察LED闪烁，建议外部的PWM频率50KHz以上。

运行现象
------------

运行工程，示例执行GPTMR的外部计数模式功能，串口终端输出如下信息，并且板载的LED会闪烁

.. code-block:: console

   gptmr cnt_mode test
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
   reload trigger
