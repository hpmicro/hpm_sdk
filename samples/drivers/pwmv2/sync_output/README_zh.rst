.. _pwm_synchronous_output:

PWM同步输出
==============

概述
------

**PWM** 支持多通道同步输出功能。该示例展示了通过电机系统的 **synt** 同步三路pwm，pwm波形通过相应MCU引脚输出。

配置
------

- 示波器

- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数

-  :ref:`PWM_P0,PWM_P2引脚和PWM_P4引脚 <board_resource>` 根据板子型号查看具体信息

运行
------

- 上电后，三路pwm在同时刻同相位输出，然后通过 **synt** 的触发信号完成不同相位同步输出

- P0的输出和P2的输出相差45度相位，P0和P4的输出相差135度相位

- 串口打印如下信息:


.. code-block:: console

       pwmv2 three pwm submodule synchronous example
       choose PWM output channel [P0 P2 P4]


       >> P0 P2 P4 generate waveform at same time
       P0 is a reference


       >> Phase different P0 P2 P4 is 45 degrees 135 degrees
       P0 is a reference
       test done



- 所得波形如图所示, 黄色为P0输出，粉色为P2输出，蓝色为P4输出

    .. image:: ../doc/sync.png
       :alt: 波形图


.. note::

   只依次输出一次PWM波形，如果需要再次观测波形，需要重新运行程序。

