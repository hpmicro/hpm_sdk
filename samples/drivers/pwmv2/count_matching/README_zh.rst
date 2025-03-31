.. _compare_point_matching:

比较点匹配触发pwm
====================

概述
------

**pwm** 支持在周期内任意时刻去修改 **pwm** 波形。该工程展示了在周期内零点和重载点修改
**pwm** 波形并通过MCU相应引脚输出。

配置
------

- 示波器

- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数

-  :ref:`PWM_P0,PWM_P2引脚和PWM_P4引脚 <board_resource>` 根据板子型号查看具体信息

运行
------

- 上电后，通过示波器可以观察到三路输出，P0输出零点匹配触发，P2输出重载点匹配触发，P4输出中心对齐波形，用作参考波形。

- P0 P2每输出10个波形后，在匹配点修改 **PWM** 输出波形

- 串口打印如下信息:


.. code-block:: console

       pwmv2 point trigger example
       choose PWM output channel [P0 P2 P4]


       >> P0 and P2 generate pwm waveform P4 is reference
       Modify the duty cycle at the midpoint when generating the 11th pulse
       duty cycle will be updated from 0 - 100 and back to 0, the step size is 20%



- 所得波形如图所示, 黄色为P0输出，粉色为P2输出，蓝色为P4输出

    .. image:: ../doc/count_matching.png
       :alt: 波形图
