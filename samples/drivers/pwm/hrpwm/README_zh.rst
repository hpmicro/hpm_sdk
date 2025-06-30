.. _hrpwm_output:

HRPwm输出
==============

概述
------

**pwm** 工程演示了如何通过MCU的HRPWM模块配置和输出多种PWM波形，包括：

- 强制输出模式：通过软件强制控制PWM引脚输出高电平或低电平
- 失效模式：演示外部故障信号触发PWM输出保护及自动恢复
- 边沿对齐PWM：输出占空比可变的边沿对齐PWM波形
- 中心对齐PWM：输出占空比可变的中心对齐PWM波形
- 频率可变PWM：演示PWM输出频率的动态调整

配置
------

- 一个双通道示波器
- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数
- 查看 :ref:`HRPWM_P0和HRPWM_P2引脚 <board_resource>`，根据板子型号查看具体信息

运行现象
------------

- 上电后，串口会依次输出各功能模式的提示信息
- 示波器可观测到P0、P2引脚依次输出高电平、低电平、边沿对齐PWM、中心对齐PWM、频率可变PWM等波形
- 边沿对齐和中心对齐模式下，P0为目标波形，占空比逐步变化，P2为参考波形
- 失效模式下，外部故障信号触发后，PWM输出会进入保护状态，并在硬件或软件事件下自动恢复
- 频率可变模式下，PWM输出频率会逐步变化
- 串口打印信息如下：

.. code-block:: console

   hr pwm example

   >> Test force HRPWM output on P0 and P2
   Output high
   Output low

   >> Generate edge aligned waveform
   Two waveforms will be generated, HRPWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100; HRPWM P2 is a reference

   >> Generate central aligned waveform
   Two waveforms will be generated, HRPWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100; HRPWM P2 is a reference

   >> Generate frequency-variable waveforms
   whose frequency will be updated; HRPWM P0 is the target waveform
   test done

.. note::

   只依次输出一次PWM波形，如需再次观测波形，请重新运行程序。
