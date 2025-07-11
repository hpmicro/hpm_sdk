.. _hall:

HALL
========

概述
------

**Hall** 工程主要演示霍尔传感器触发 **hall** 中断后，通过串口将霍尔定时器的时间数据打印出来。

- 霍尔中断配置和处理
- 霍尔`U\V\W\TIMER`寄存器的数据读取
- TRGM（触发管理器）的配置

实现细节
--------

- 初始化配置：
  - 配置 TRGM 输入源，分别设置 U、V、W 三相霍尔信号
  - 配置并使能中断
  - 配置霍尔计数器和相位计数
  - 配置在 U 相信号变化时触发数据读取

- 中断处理：
  - 在霍尔中断中清除状态标志
  - 设置数据就绪标志，触发主循环中的数据读取

- 数据读取：
  - 在主循环中等待数据就绪标志
  - 读取 U、V、W 相位计数值和定时器计数值
  - 通过串口打印数据

配置
------

- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数

- 电机使用的是雷赛智能 **BLM57050-1000**  无刷电机，电机具体参数请参考`雷赛智能官网 <https://leisai.com/>`_。

- 点击查看  :ref:`DRV-LV50A-MP1907电机驱动板 <drv_lv50a_mp1907>`  章节并进行配置

运行现象
------------

- 上电后，打开串口终端，打印启动信息
- 转动电机轴时，会触发霍尔中断并依次打印如下信息：
  - u: U相霍尔计数器的值
  - v: V相霍尔计数器的值
  - w: W相霍尔计数器的值
  - timer: 定时器计数值
- 数据随着转动的快慢和方向变化，数据的具体含义请参考用户手册


.. code-block:: console

   > u: 0xe0000000
   > v: 0x87b6d17c
   > w: 0x46999a8
   > timer: 0xe149af9

