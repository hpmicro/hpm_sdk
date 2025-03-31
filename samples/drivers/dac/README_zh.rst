.. _dac:

DAC
======

概述
------

本示例展示DAC在三种工作模式的转换及波形显示

硬件设置
------------

- 在指定的管脚输出电压 (请参考  :ref:`引脚描述 <board_resource>`  部分)

- 根据开发板设计，如有必要，请连接VREF管脚处的跳帽  (请参考  :ref:`引脚描述 <board_resource>`  部分)

运行现象
------------

- 串口终端显示如下信息


  .. code-block:: console

   This is a DAC demo:
   1. Direct mode
   2. Step   mode
   3. Buffer mode
   Please enter the DAC mode code:


- 选择DAC转换模式，启动DAC转换，并通过示波器观察波形

  - Direct 模式


    .. code-block:: console

     Please enter the DAC mode code: 1
     Set DAC to output data in direct mode
     DAC is outputting a triangle waveform in direct mode



    .. image:: doc/dac_direct_mode.png
       :alt:

  - Step 模式


    .. code-block:: console

     Please enter the DAC mode code: 2
     Set DAC to output data in step mode
     DAC is outputting a saw tooth waveform in step mode



    .. image:: doc/dac_step_mode.png
       :alt:

  - Buffer 模式


    .. code-block:: console

     Please enter the DAC mode code: 3
     Set DAC to output data in buffer mode
     DAC is outputting a sine waveform in buffer mode



    .. image:: doc/dac_buffer_mode.png
       :alt:

注意
------

- 异常退出

  1. 在step模式或buffer模式中，按空格键退出测试，重新选择测试模式
  2. 在direct模式中，在测试完成后，按空格键退出循环，重新选择测试模式
