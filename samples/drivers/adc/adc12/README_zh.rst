.. _adc12:

ADC12
==========

概述
------

本示例展示ADC12在四种工作模式的转换及结果显示

注：

* 当ADC配置为非读取转换（oneshot）模式时，不能读取ADC寄存器列表，否则会出现总线阻塞

* 当ADC配置为读取转换 （oneshot）模式时， CPU必须在ADC完成初始化以后，才可以读取ADC寄存器列表

硬件设置
------------

在指定的管脚输入电压 (请参考  :ref:`引脚描述 <board_resource>`  部分)

注： ADC管脚的输入电压范围: 0~VREFH

运行现象
------------

- 串口终端显示如下信息


  .. code-block:: console

   This is an ADC12 demo:
   1. Oneshot    mode
   2. Period     mode
   3. Sequence   mode
   4. Preemption mode
   Please enter one of ADC conversion modes above (e.g. 1 or 2 ...):


- 选择ADC转换模式，启动ADC转换，并观察转换结果

  - Oneshot 模式


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 1
     Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
     Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
     Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb


  - Period 模式


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
     Period Mode - ADC0 [channel 11] - Result: 0x0ffb
     Period Mode - ADC0 [channel 11] - Result: 0x0fff
     Period Mode - ADC0 [channel 11] - Result: 0x0ffd


  - Sequence 模式


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
     Sequence Mode - ADC0 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0fff
     Sequence Mode - ADC0 - Cycle Bit: 00 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0ffb
     Sequence Mode - ADC0 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0ff7


  - Preemption 模式


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
     Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9
     Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9
     Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9



注意
------

- 如何使用WDOG功能

  - 通道初始化

    - 设置ch_cfg. wdog_int_en为true

    - 设置ch_cfg.thshdh/ch_cfg.thshdl

      ch_cfg.thshdh/ch_cfg.thshdl可配置范围从0~4095，如果任何一次ADC的转换结果超出阈值，WDOG中断将会产生。

  - 调用adc16_init_channel () API

  - 中断服务程序

    - 根据ADC通道设置一个或多个WDOG事件标志

    - 禁用一个或多个相应的WDOG中断

  - 主循环

    - 处理WDOG事件

    - 使能一个或多个相应的WDOG中断

- 触发源

  - 本示例中使用PWM作为序列模式和抢占模式的触发源，也可以选择其他外设作为触发源

  - 触发信号频率（默认为20KHz）可以在sample级的CMakeLists.txt中重新定义(例如：sdk_compile_definitions(-DAPP_ADC12_TRIG_SRC_FREQUENCY=20000))

- 异常退出

  按空格键退出测试，重新选择测试模式
