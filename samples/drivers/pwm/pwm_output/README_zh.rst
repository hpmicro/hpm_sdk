.. _pwm_output:

PWM输出
==========

概述
------

本工程演示了如何配置几种不同类型的PWM波形并通过MCU的引脚进行输出。

演示包括以下PWM输出类型：

- 强制输出（高/低电平）
- 边沿对齐PWM
- 带失效模式的边沿对齐PWM
- 中心对齐PWM
- 中心对齐互补PWM
- 带抖动的边沿对齐PWM

硬件要求
--------

- 双通道示波器
- 用于调试输出的串口终端
- 根据板子型号连接PWM输出引脚（参考 :ref:`板子信息 <board_resource>`）

配置
------

- 安装串口终端并根据 :ref:`板子信息 <board_resource>` 配置参数
- 连接PWM输出引脚（PWM_P0和PWM_P1）到示波器
- 连接示波器探头到PWM输出引脚

运行现象
--------

上电后，示波器将依次显示以下波形：

1. 强制输出：
   - P0和P1输出高电平5秒
   - P0和P1输出低电平5秒

2. 边沿对齐PWM：
   - P0：占空比从0%到100%再回到0%的目标波形
   - P1：50%占空比的参考波形

3. 带失效模式的边沿对齐PWM：
   - 类似于边沿对齐PWM，但启用了失效保护
   - 演示PWM在失效条件下的行为

4. 中心对齐PWM：
   - P0：占空比从0%到100%再回到0%的目标波形
   - P1：50%占空比的参考波形

5. 中心对齐互补PWM：
   - P0和P1：占空比从0%到100%再回到0%的互补波形
   - 互补信号之间包含死区时间

6. 带抖动的边沿对齐PWM：
   - P0：占空比从0%到100%再回到0%的目标波形
   - P1：50%占空比的参考波形
   - 包含抖动效果用于演示

串口将显示每个测试的进度：

.. code-block:: console

   pwm example

   >> Test force PWM output on P0 and P1
   Output high
   Output low

   >> Generate edge aligned waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   >> Generate edge aligned waveform and fault mode enable
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   >> Generate central aligned waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   >> Generate central aligned waveform in pair
   Two waveforms will be generated in pair, PWM P0 and P1 are target
   waveforms whose duty cycle will be updated from 0 - 100 and back to 0

   >> Generate edge aligned jit waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   test done

.. note::

   演示程序会依次运行所有PWM输出类型一次。如需再次观察波形，需要复位并重新运行程序。

