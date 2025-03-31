.. _mmc_pred_close_loop:

MMC_PRED_CLOSE_LOOP
======================================

概述
------

MMC闭环预测工程演示了MMC根据其他外设(如SEI等)提供的转子位置和运动系统时间戳，预测并补偿转子未来时刻的位置并输出的功能。
本示例工程演示SEI获取传感器位置并输出给MMC，MMC预测并补偿未来时刻的位置， 之后将位置输出给QEO, QEO根据得到的位置，通过PWM外设生成PWM信号。

硬件设置
------------

请参考 :ref:`引脚描述 <board_resource>` 部分
- 连接bissc传感器到开发板SEI接口

- 连接PWM输出信号到先楫步进电机驱动板

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   MMC example
   MMC tracks position from SEI and predicts postion to QEO, QEO controls PWM module generates signals

   speed: 0.00000000
   rev  : 0
   pos  : 0xe5700000

   speed: 0.00000000
   rev  : 0
   pos  : 0xe5700000

   speed: 0.02145767
   rev  : 0
   pos  : 0xe5201335

   speed: 2.12848091
   rev  : 0
   pos  : 0xf5a41e09

   speed: 3.19600106
   rev  : 1
   pos  : 0x4436226c

   speed: 2.19464302
   rev  : 1
   pos  : 0x87cbb8d1

   speed: 3.45587732
   rev  : 1
   pos  : 0xd4acf18b

   speed: 2.91764640
   rev  : 2
   pos  : 0x26966f29

   speed: 2.05099487
   rev  : 2
   pos  : 0x764ed45d

