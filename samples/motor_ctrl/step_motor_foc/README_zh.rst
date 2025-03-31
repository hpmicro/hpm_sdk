.. _step_motor_foc:

步进电机矢量控制
================

概述
------

主要演示了步进电机矢量控制
- 矢量控制算法

- 梯形加减速曲线

- 步进电机电流为两相差90度的正弦波，如图为示波器采集到的电流信号


.. image:: doc/step_motor_current.png
   :alt:

配置
------

- 42步进电机，24v电压，极对数为50.

- 步进电机驱动板 **24V_STEP_NONE_2A** .

    - 分别连接J2和J4的跳冒

    - M1连接电机

- 通过电机接口连接步进电机驱动板

Running the demo
----------------

步进电机不停机进行速度和方向的切换，并通过串口打印速度信息，也可以通过电流钳表观测到电流波形始终为正弦波。

串口打印信息:


.. code-block:: console

   step motor demo.
   speed: 1 r/s.
   speed: -2 r/s.
   speed: 3 r/s.
   speed: -4 r/s.
   speed: 5 r/s.
   speed: -6 r/s.
   speed: 1 r/s.
   speed: -2 r/s.
   speed: 3 r/s.
   speed: -4 r/s.
   speed: 5 r/s.
   speed: -6 r/s.
   speed: 1 r/s.
   speed: -2 r/s.
   speed: 3 r/s.



.. warning::

   - 注意电机电流

   - 步进电机长时间工作，注意电机温度

