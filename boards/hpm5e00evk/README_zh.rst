.. _hpm5e00evk:

HPM5E00EVK开发板
================

概述
----

HPM5E00是一款基于RISC-V内核的MCU，具有很高的算力和丰富的外设资源。

HPM5E00EVK为HPM5E00系列MCU的评估板，提供了一系列接口，包括ADC输入SMA接口、SDM输入SMA接口，电机控制接口(QEO/QEI/PWM/ADC)、CAN接口、Ethernet接口、EtherCAT接口、USB接口、PPI接口和树莓派接口，并集成板载调试器FT2232，方便用户进行调试。

.. image:: doc/hpm5e00evk.png
   :alt: hpm5e00evk

板上硬件资源
------------

- HPM5E31 微控制器 (1MB片内Flash)
- 以太网
  - 1000 Mbits PHY
- EtherCAT
  - 2 端口
- USB
  - USB type C (USB 2.0 OTG) connector x1
- 电机
- 模拟采样
  - NSI1306W25
- 其他
  - RGB LED
  - CAN

.. note::

   - **当需要使用PPI外设时，请在PPI接口插入相应的扩展板**

     - HPM5E00EVK标配的扩展板如下，有1个16bits的SRAM和1路EtherCAT接口，供评估使用。

     .. image:: doc/hpm5e00evk_ext.png
        :alt: hpm5e00evk_ext

     - PPI扩展板的SRAM与1Wire接口和QEI接口复用，使用时PPI SRAM时，需断开JP6并让QEI引脚悬空。
     - PPI接口具有很高的灵活性，若需要评估其他并口设备，例如FPGA、其他并口设备等，可自行设计扩展板或联系我们。

拨码开关 SW2
------------

- Bit 1，2控制启动模式

.. list-table::
   :header-rows: 1

   * - Bit[2:1]
     - 功能描述
   * - OFF, OFF
     - Quad SPI NOR flash 启动
   * - ON, OFF
     - 在系统编程

.. _hpm5e00evk_buttons:

按键
----

.. list-table::
   :header-rows: 1

   * - 名称
     - 功能
   * - PB24 (KEYA)
     - GPIO 按键A
   * - PB25 (KEYB)
     - GPIO 按键B
   * - WBUTN (WKUP)
     - WAKE UP 按键
   * - RESETN (RESET)
     - Reset 按键

插件
----

调试器接口选择
^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
     - 说明
   * - 调试器选择
     - J17
     - 全部连接：使用板载ft2232，全部断开：使用标准JTAG接口

PPI接口
^^^^^^^

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
     - 说明
   * - PPI接口
     - CN1
     - 接PPI扩展板，PPI扩展板的SRAM与1Wire接口和QEI接口复用，使用时PPI SRAM时，需断开JP6并让QEI引脚悬空

.. _hpm5e00evk_pins:

引脚描述
--------

- UART0串口引脚

  UART0用于Core0的调试控制台串口。

.. list-table::
   :header-rows: 1

   * - 功能
     - 引脚
     - 位置
   * - UART0.TX
     - PA00
     - DEBUGUART0
   * - UART0.RX
     - PA01
     - DEBUGUART0

- UART4串口引脚

  UART4用于Core1的调试控制台串口或一些使用UART的功能测试，例如MICROROS_UART，USB_CDC_ACM_UART, MODBUS_RTU, lin等。

.. list-table::
   :header-rows: 1

   * - 功能
     - 引脚
     - 位置
   * - UART4.TX
     - PC16
     - P5[8]
   * - UART4.RX
     - PC17
     - P5[10]
   * - UART4.break
     - PD13
     - P5[18]

- CAN 接口

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
   * - CAN_H
     - J7[0]
   * - CAN_L
     - J7[2]

- ADC 接口

.. list-table::
   :header-rows: 1

   * - 功能
     - 引脚
     - 位置
   * - ADC16输入
     - PF26
     - J3[6]
   * - SDM ADC输入
     - J4
     - 需要连接跳帽JP2、JP4、JP5

- ACMP

.. list-table::
   :header-rows: 1

   * - 功能
     - 引脚
     - 位置
   * - CMP0.INN6
     - PF26
     - J3[6]

- 正交旋转编码器接口

.. note::
  QEO ABZ引脚丝印标错，须按照实际功能连接
  连接电机HALL传感器时，HALL.U/V/W无法直接对插需要飞线连接

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
   * - QEI.A / HALL.U
     - J3[1]
   * - QEI.B / HALL.V
     - J3[3]
   * - QEI.Z / HALL.W
     - J3[5]
   * - QEO.A
     - J3[24]
   * - QEO.B
     - J3[22]
   * - QEO.Z
     - J3[26]

- PWM 输出接口

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
   * - PWM.WL
     - J3[12]
   * - PWM.WH
     - J3[11]
   * - PWM.VL / PWM1.P3
     - J3[10]
   * - PWM.VH / PWM1.P2
     - J3[9]
   * - PWM.UL / PWM1.P1
     - J3[8]
   * - PWM.UH / PWM1.P0
     - J3[7]
   * - PWM.FAULT
     - J3[5]

- QEIV2 Sin/Cos引脚

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
     - 备注
   * - ADC2.INA11
     - J3[15]
     - ADC_IW (Cos)
   * - ADC0.INA14
     - J3[13]
     - ADC_IU (Sin)

- PLB Pulse输出引脚

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
   * - PLB.PULSE_OUT
     - J3[5]

- PLB LIN Clock引脚

  用于UART LIN Slave波特率自适应例程检测RX信号的时钟, 需要去掉C151，避免电容影响信号波形

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
   * - PLB.TRGM_IN
     - J3[5]

- PLB Filter输出引脚

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
   * - PLB.Filter_IN
     - J3[9]
   * - PLB.Filter_OUT
     - J3[11]

- LOBS 触发信号

.. list-table::
   :header-rows: 1

   * - 功能
     - 引脚
     - 位置
   * - Trig Pin0
     - PC11
     - P5[24]
   * - Trig Pin1
     - PC10
     - P5[23]

- GPTMR引脚

.. list-table::
   :header-rows: 1

   * - 功能
     - 引脚
     - 位置
     - 备注
   * - GPTMR0.CAPT_2
     - PC00
     - P5[7]
     -
   * - GPTMR0.CAPT_3
     - PC08
     - P5[5]
     -
   * - GPTMR0.COMP_2
     - PD07
     - P5[26]
     - SPI模拟I2S的BLCK
   * - GPTMR0.COMP_3
     - PD15
     - P5[29]
     - SPI模拟I2S的LRCK
   * - GPTMR1.COMP_3
     - PC03
     - P5[11]
     - SPI模拟I2S的MCLK
   * - GPTMR1.CAPT_2
     - PC00
     - P5[7]
     - QEI A 相信号
   * - GPTMR1.CAPT_3
     - PC08
     - P5[5]
     - QEI B 相信号

- SPI模拟I2S CS引脚

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
     - 备注
   * - PD24
     - P5[38]
     - 控制SPI从机CS的引脚

- SPI引脚

.. list-table::
   :header-rows: 1

   * - 功能
     - 引脚
     - 位置
   * - SPI1.CSN
     - PC11
     - P5[24]
   * - SPI1.SCLK
     - PC10
     - P5[23]
   * - SPI1.MISO
     - PC12
     - P5[21]
   * - SPI1.MOSI
     - PC13
     - P5[19]

- I2C引脚

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
   * - I2C0.SCL
     - P5[5]
   * - I2C0.SDA
     - P5[3]

- ECAT:

.. list-table::
   :header-rows: 1

   * - 功能
     - 引脚
     - 位置
   * - 网口(Port0)
     -
     - P1
   * - 网口(Port1)
     -
     - P2
   * - 网口(Port2)
     -
     - 外接扩展板
   * - 输入IO(2bit)
     - PD06/PD12
     - SW6
   * - 输出IO(2bit)
     - PC23/PC24
     - LED6/LED7

- CLOCK REF 引脚

.. list-table::
   :header-rows: 1

   * - 功能
     - 位置
   * - PC30
     - P5[22]
