.. _pwmv2_output:

PWM输出
==========

概述
------

**pwm** 工程主要演示了如何配置几种不同类型的pwm波形并通过MCU的引脚进行输出。

- 强制输出
- 边沿对齐PWM
- 中心对齐PWM
- 中心对齐互补PWM
- PWM故障保护
- 四路比较器输出（支持AND、XOR、CD等不同逻辑模式）
- PWM DAC输出
- 带抖动补偿的边沿对齐PWM

配置
------

- 一个双通道示波器
- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数
- :ref:`PWM_P0和PWM_P1引脚 <board_resource>` 根据板子型号查看具体信息

运行现象
------------

- 上电后，通过示波器可依次观测到：
  - P0、P1输出高电平和低电平
  - 占空比从0%到100%变化的边沿对齐PWM
  - 占空比从0%到100%变化的中心对齐PWM
  - 占空比从0%到100%变化的中心对齐互补PWM
  - 不同逻辑模式的四路比较器输出
  - 占空比可变的PWM DAC输出
  - 带抖动补偿的边沿对齐PWM

- 失效引脚给定高电平后，P0和P1引脚PWM输出低电平，失效引脚给定低电平后，P0和P1引脚PWM恢复正常

- 串口打印如下信息:

.. code-block:: console

   pwm example

   >> Test force PWM output on P0 and P1
   Output high
   Output low

   >> Generate edge aligned waveform
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

   >> Generate four cmp waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   >> Generate pwm dac waveform
   Two waveforms will be generated, PWM P0 is the target waveform
   whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

   test done

.. note::

   演示程序会依次输出每种PWM波形一次。如果需要再次观测波形，需要重新运行程序。

