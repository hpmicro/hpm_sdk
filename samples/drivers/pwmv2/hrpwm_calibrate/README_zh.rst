.. _hrpwm_calibration:

HRPWM校准
==============

概述
------

**pwm** 工程主要演示了高精度pwm使用中，如何校准温度产生的影响。

配置
------

- 一个双通道示波器

- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数

-  :ref:`PWM_P0和PWM_P1引脚 <board_resource>` 根据板子型号查看具体信息

运行现象
------------

- 上电后，通过示波器可依次观测到P0、P1输出占空比百分之50的PWM波形。其中P0的高精度部分以0-255分之一个时钟周期变化。芯片温度变化时，程序会进行校准，并通过串口打印完成或者失败。

- 串口打印如下信息:


.. code-block:: console

   pwm example


   >> Generate edge aligned waveform
   Two waveforms will be generated, PWM P1 is the target waveform
   whose hrpwm cycle will be updated from 0 - 255 and back to 0; PWM P2 is a reference

   calibration done

