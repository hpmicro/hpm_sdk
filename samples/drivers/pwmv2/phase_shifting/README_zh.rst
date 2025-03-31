.. _the_phase_shifting_of_pwm:

PWM波移相
============

概述
------

**pwm** 支持输出波形相位移动功能。该实例展示了在接收到外部触发输入时，对pwm波进行移相的操作。

配置
------

- 示波器

- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数

-  :ref:`PWM_P0,PWM_P2 <board_resource>` 根据板子型号查看具体信息

运行
------

- 上电后，P0和P2输出同相位的波形，每3秒钟，P2的相位会移动10度，第一次变化相差60度

- 串口打印如下信息:


.. code-block:: console

       pwmv2 two pwm submodule phase sync example
       choose PWM output channel [P0 P2]


       >> P0 and P2 generate same phase firstly
       Two waveforms will be generated
       Please press trigger pin, P2 phase will shift
       P0 is a reference

       P0 and P2 now have a phase difference of 60 degrees

       P0 and P2 now have a phase difference of 70 degrees

       P0 and P2 now have a phase difference of 80 degrees

       P0 and P2 now have a phase difference of 90 degrees

       P0 and P2 now have a phase difference of 100 degrees
       ...


