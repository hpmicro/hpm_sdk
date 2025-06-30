步进电机闭环控制
================

概述
------

主要演示了步进电机闭环控制功能：
- FOC 矢量控制算法
- 速度和位置闭环控制
- 电流环控制
- 支持速度模式和位置模式切换

配置
------

- 42步进电机，24v电压，极对数为50
- 步进电机驱动板 **24V_STEP_NONE_2A**
    - 硬件连接详情请参考 :ref:`24v_step_none_2a_revb`

运行演示
----------------

程序启动后可以选择两种运行模式：

1. 位置模式 (输入0)
   - 通过串口输入目标位置值
   - 电机会转动到指定位置并保持

2. 速度模式 (输入1)
   - 通过串口输入目标速度值(范围: -10 ~ 10 r/s)
   - 电机会按照指定速度持续运转

串口打印信息示例:

.. code-block:: console

   step motor demo.
   loop current tick: xxx
   Mode selection:
   0. Location mode.
   1. Speed mode.
   Enter mode code:

   // 速度模式示例:
   Speed mode, motor run, speed is: 1.000000.
   Input speed:
   2
   loop current tick: xxx
   Speed mode, motor run, speed is: 2.000000.
   Input speed:

   // 位置模式示例:
   Location mode, motor run, The location is: 0.
   Input Location:
   1000
   loop current tick: xxx
   Location mode, motor run, The location is: 1000.
   Input Location:

.. warning::

   - 注意电机电流大小，如有异常请及时断电
   - 步进电机长时间工作时请注意电机温度

