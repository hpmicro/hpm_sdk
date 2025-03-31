.. _fix_duty_cycle:

固定占空比输出
==============

概述
------

**pwm** 支持占空比固定，修改频率的功能。该工程展示了通过硬件关联或者软件计算两种方式实现修改频率保证波形占空比不改变的功能。

配置
------

- 一个双通道示波器

- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数

-  :ref:`PWM_P0引脚 <board_resource>` 根据板子型号查看具体信息

运行现象
------------

- 上电后，通过示波器可以观察到P0引脚输出的波形频率在不断变化，占空比保持不变

- 串口打印信息如下：


.. code-block:: console

       pwmv2 fix duty cycle example


       >> Generate fix duty cycle by calc
       Two waveforms will be generated, PWM P0 is the target waveform


       >> Generate fix duty cycle by shadow
       Two waveforms will be generated, PWM P0 is the target waveform
       test done



- 展示部分时刻波形图

    .. image:: ../doc/fix_dc_1.png
       :alt: 波形图

    .. image:: ../doc/fix_dc_2.png
       :alt: 波形图

    .. image:: ../doc/fix_dc_3.png
       :alt: 波形图


.. note::

   只依次输出一次PWM波形，如果需要再次观测波形，需要重新运行程序。

