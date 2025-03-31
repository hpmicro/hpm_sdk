.. _gptmr_measure_pwm:

GPTMR测量PWM
====================

概述
------

pwm_measure示例工程演示了GPTMR测量PWM波的功能。

硬件设置
------------

请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
连接外部PWM至GPTMR输入捕获引脚。

运行现象
------------

运行工程，示例执行GPTMR的测量PWM波的功能，若有PWM波输入，串口终端输出如下信息，分别是PWM波的频率和占空比。

.. code-block:: console

   gptmr pwm measure test
   [table_0]measured frequency: 1000.049987
   [table_0]measured duty cycle: 50.00%
   [table_1]measured frequency: 1000.049987
   [table_1]measured duty cycle: 50.00%
   [table_2]measured frequency: 1000.049987
   [table_2]measured duty cycle: 50.00%
   [table_3]measured frequency: 1000.049987
   [table_3]measured duty cycle: 50.00%
   [table_4]measured frequency: 1000.049987
   [table_4]measured duty cycle: 50.00%
   [table_5]measured frequency: 1000.049987
   [table_5]measured duty cycle: 50.00%
   [table_6]measured frequency: 1000.049987
   [table_6]measured duty cycle: 50.00%
   [table_7]measured frequency: 1000.049987
   [table_7]measured duty cycle: 50.00%
   [table_8]measured frequency: 1000.049987
   [table_8]measured duty cycle: 50.00%
   [table_9]measured frequency: 1000.059997
   [table_9]measured duty cycle: 50.00%

