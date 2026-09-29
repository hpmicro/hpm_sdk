.. _adc16:

ADC16
==========

概述
------

本示例展示ADC16在四种工作模式的转换及结果显示

注：

* 当ADC配置为非读取转换模式（oneshot模式）时，不能读取ADC寄存器列表，否则会出现总线阻塞

* 当ADC配置为读取转换模式（oneshot模式）时，CPU必须在ADC完成初始化以后，才可以读取ADC寄存器列表

硬件设置
------------

- 在指定的管脚输入电压 (请参考  :ref:`引脚描述 <board_resource>`  部分)

- 根据开发板设计，如有必要，请连接VREF管脚处的跳帽  (请参考  :ref:`引脚描述 <board_resource>`  部分)

注： ADC管脚的输入电压范围: 0~VREFH

运行现象
------------

- 串口终端显示如下信息


  .. code-block:: console

   This is an ADC16 demo:
   ADC16 clock: input=200000000 Hz, div=4, conv=50000000 Hz
   ADC16 sample: cycle=20, convert=21, fs=1219512 Hz
   1. Oneshot    mode
   2. Period     mode
   3. Sequence   mode
   4. Preemption mode
   Please enter one of ADC conversion modes above (e.g. 1 or 2 ...):

- 以上时钟与 ``ADC3`` 行为 ``hpm6750evk2`` 默认 AHB 200 MHz 的打印。其他板按本板 AHB 与 ``BOARD_APP_ADC16`` 实例显示（例如 ``hpm5300evk`` 为 160 MHz / 4 = 40 MHz；``hpm6e00evk`` 实例为 ADC0）。

- 选择ADC转换模式，启动ADC转换，并观察转换结果

  - Oneshot 模式


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 1
     Oneshot Mode - ADC3 [channel 2] - Result: 0xffe8
     Oneshot Mode - ADC3 [channel 2] - Result: 0xfff4
     Oneshot Mode - ADC3 [channel 2] - Result: 0xffff



  - Period 模式


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
     ADC16 period: target=500000000 ns, actual=500695040 ns (prescale=17 prd=191)
     Period Mode - ADC3 [channel 2] - Result: 0xfff3
     Period Mode - ADC3 [channel 2] - Result: 0xfff5
     Period Mode - ADC3 [channel 2] - Result: 0xfff7



  - Sequence 模式


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
     Sequence Mode - ADC3 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 02 - Result: 0xffff
     Sequence Mode - ADC3 - Cycle Bit: 00 - Sequence Number:00 - ADC Channel: 02 - Result: 0xffd9
     Sequence Mode - ADC3 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 02 - Result: 0xffff



  - Preemption 模式


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
     Preemption Mode - ADC3 - Trigger Channel: 00 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 02 - Result: 0xffff
     Preemption Mode - ADC3 - Trigger Channel: 00 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 02 - Result: 0xffff
     Preemption Mode - ADC3 - Trigger Channel: 00 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 02 - Result: 0xffff



注意
------

- 如何使用WDOG功能

  - 通道初始化

    - 设置ch_cfg. wdog_int_en为true

    - 设置ch_cfg.thshdh/ch_cfg.thshdl

      ch_cfg.thshdh/ch_cfg.thshdl可配置范围从0~65535，如果任何一次ADC的转换结果超出阈值，WDOG中断将会产生。
  - 调用adc16_init_channel () API

  - 中断服务程序

    - 根据ADC通道设置一个或多个WDOG事件标志

    - 禁用一个或多个相应的WDOG中断

  - 主循环

    - 处理WDOG事件

    - 使能一个或多个相应的WDOG中断

- 触发源

  - 本示例中使用PWM作为序列模式和抢占模式的触发源，也可以选择其他外设作为触发源

  - 触发信号频率（默认为20KHz）可以在sample级的CMakeLists.txt中重新定义(例如：sdk_compile_definitions(-DAPP_ADC16_TRIG_SRC_FREQUENCY=20000))

- 异常退出

  按空格键退出测试，重新选择测试模式
