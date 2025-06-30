.. _pwm_capture:

PWM 捕获
============

概述
------

**pwm 捕获** 工程主要演示了通过PWM CMP的捕获一个中心对称的PWM波形，并通过计算获取PWM的占空比。

- PWM捕获通道

- 中心对齐PWM

配置
------

- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数

-  :ref:`PWM_P0和PWM_P1引脚 <board_resource>` 根据板子型号查看具体信息

- 连接PWM_P0和PWM_P1引脚

详细介绍
------------

本示例通过PWM模块的比较器（CMP）功能，演示了如何捕获中心对齐的PWM波形，并通过捕获的上升沿和下降沿计数值计算PWM的占空比。主要流程如下：

1. 初始化板级外设和PWM相关引脚。
2. 配置PWM输出为中心对齐模式，周期为5ms，占空比在一定范围内变化。
3. 配置PWM输入捕获功能，捕获PWM输出引脚的波形。
4. 在主循环中，动态调整PWM输出的占空比，并通过捕获功能获取实际的占空比，进行对比验证。
5. 若捕获到的占空比与设置的占空比误差在允许范围内，则打印PASS，否则打印FAIL。

主要函数说明
------------

- `generate_central_aligned_waveform`：配置并启动中心对齐的PWM输出。
- `config_pwm_input_capture`：配置PWM输入捕获功能，并使能中断。
- `isr_pwm_cap`：PWM捕获中断服务函数，获取上升沿和下降沿的计数值。
- `test_pwm_cap`：主测试流程，动态调整占空比并验证捕获结果。


运行现象
------------

- 上电后，PWM输出中心对称的PWM，间隔改变占空比

- PWM捕获当前的PWM波形，计算后输出占空比

- 成功捕获到PWM信号，且占空比一致，打印占空比信息，程序返回PASS

- 没有捕获到PWM信号或计算出的占空比和输出占空比不一致返回FAIL

- 串口打印如下信息:


.. code-block:: console

   pwm capture example


   >> Test PWM output on P4, PWM capture P5

   Pwm output duty: 0.049995, Pwm cap duty: 0.054994.
   Pwm output duty: 0.049995, Pwm cap duty: 0.049995.
   Pwm output duty: 0.039996, Pwm cap duty: 0.044996.
   Pwm output duty: 0.039996, Pwm cap duty: 0.039996.
   Pwm output duty: 0.029997, Pwm cap duty: 0.034996.
   Pwm output duty: 0.029997, Pwm cap duty: 0.029997.
   Pwm output duty: 0.019998, Pwm cap duty: 0.024998.
   Pwm output duty: 0.019998, Pwm cap duty: 0.019998.
   Pwm output duty: 0.009999, Pwm cap duty: 0.014998.
   Pwm output duty: 0.009999, Pwm cap duty: 0.009999.
   PASS.
   test done

注意事项
------------

- 示例中占空比的变化步进由`TEST_LOOP`宏定义控制，步进越小，测试越细致。
- 捕获到的占空比与设置值误差大于0.1时，判定为FAIL。
- 需确保PWM输出与捕获引脚物理连接正确，否则无法正常捕获信号。
- 示例使用串口打印信息，请确保串口终端参数配置正确。

