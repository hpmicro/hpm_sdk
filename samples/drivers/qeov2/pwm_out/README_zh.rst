.. _qeov2_pwm_out:

QEOV2_PWM_OUT
==========================

概述
------

QEO_PWM_OUT示例工程展示了QEOv2 (Quadrature Encoder Output Version 2) 外设根据位置信息生成PWM波形的功能。QEOv2可以接收软件注入或硬件（如MTG、SEI）提供的位置信息，结合预设的分辨率线数，通过PWM外设生成PWM信号输出。

工作流程
-----------

本示例提供了两种工作模式演示：

1. 软件位置注入模式
   配置步骤：

   - 初始化QEO模块，配置PWM输出模式
   - 配置PWM信号生成参数：

     * 设置分辨率线数（4线）
     * 配置PWM强制输出表
     * 使能PWM信号输出

   - 软件位置注入流程：

     * 计算位置增量值
     * 使能软件位置注入
     * 循环注入位置值进行旋转
     * 等待PWM信号生成
     * 关闭位置注入

2. 硬件位置输入模式

   本例中使用硬件生成器提供位置信息给QEO，QEO根据接收到的位置信息实时生成PWM信号输出。

   配置步骤：

   - 初始化QEO模块，配置与软件模式相同的PWM参数
   - 配置硬件位置输入：

     * 配置硬件位置生成器
     * 配置PWM安全模式表
     * 使能PWM信号输出

   - 使能电机系统时间戳功能

硬件设置
------------

使用示波器/逻辑分析仪观察指定PWM引脚的输出波形，(请参考 :ref:`引脚描述 <board_resource>` 部分)。

在MCU不支持MTG外设的情况下，需要使用QEI外设接ABZ编码器(4000线)提供硬件位置输入，进行测试。


运行现象
------------

当工程正确运行后，可以观察到以下现象：

1. 串口终端输出信息：

.. code-block:: console

   QEO PWM example
   QEO generate PWM signal with software inject postion
   QEO generate PWM signal with hardware provide postion

2. PWM信号输出：

使用示波器/逻辑分析仪可以观察到两种PWM波形：
- 软件位置注入模式下的PWM波形：

.. image:: doc/qeo_pwm_1.png
   :alt: 软件注入模式PWM波形

- 硬件位置输入模式下的PWM波形：

.. image:: doc/qeo_pwm_2.png
   :alt: 硬件输入模式PWM波形
