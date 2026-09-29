.. _hpm5100evk:

HPM5100EVK开发板
================

概述
----

HPM5100EVK提供了一系列HPM5100系列微控制器特色外设的接口，包括一个用于PWM输出和ADC采样的电机引脚接口，一个运算放大器接口，一个RS485/422接口。HPM5100EVK同时集成了IO扩展接口，连接了HPM5100 MCU的大部分IO，供用户自由评估。HPM5100EVK为MCU外扩了NOR Flash存储，并集成了板载调试器。

.. image:: doc/hpm5100evk.png
   :alt: hpm5100evk

VCORE电源选择
--------------

HPM5100EVK的VCORE电源可通过JP3跳帽选择，可以选择外部DCDC供电(EXT)或内部LDO供电(INT)。

- 使用外部DCDC供电和内部LDO供电，二者CPU工作的最高主频不一样，请根据Datasheet中的技术指标选择合适的供电方式、配置合适的CPU工作主频。
- 默认情况下，VCORE电源选择DCDC供电，即JP3短接在EXT这侧，CPU主频的配置为400MHz。

拨码开关
--------

- Bit 1，2控制启动模式

  .. list-table::
     :header-rows: 1

     * - Bit[2:1]
       - 功能描述
     * - OFF, OFF
       - Quad SPI NOR flash 启动
     * - OFF, ON
       - 串行启动
     * - ON, OFF
       - 在系统编程

.. _hpm5100evk_buttons:

按键
----

-

  .. list-table::
     :header-rows: 1

     * - 名称
       - 功能
     * - PY07 (WKUP)
       - WAKE UP 按键
     * - PA15 (USER KEY)
       - USER KEY
     * - RESET
       - Reset 按键

.. _hpm5100evk_pins:

引脚描述
--------

- UART0 串口引脚：

  UART0用于Core0的调试控制台串口。

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
     * - UART0.TX
       - PA00
     * - UART0.RX
       - PA01

- UART2 串口引脚：

  UART2用于Core0的一些使用UART的功能测试。

  注意：UART2的TX引脚PY07跟WKUP按键复用，WKUP按键这侧有一个电容，若UART2的工作波特率比较高，建议将这个电容拆除。

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
       - 位置
       - 备注
     * - UART2.TX
       - PY07
       - P1[8]
       -
     * - UART2.RX
       - PY06
       - P1[10]
       -
     * - UART2.break
       - PC14
       - P1[11]
       - 产生uart break信号

- UART3 串口引脚：

  UART3用于UART模拟LIN通信功能测试。

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
       - 位置
     * - UART3.TX
       - PC00
       - P1[32]
     * - UART3.RX
       - PC05
       - P1[18]

- SPI 引脚：

  注意：SPI3的MISO引脚PC15跟ADC功能复用，ADC这侧有一个电容，若SPI3的工作频率比较高，建议将这个电容拆除。

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
       - 位置
     * - SPI3.CSN
       - PC06
       - P1[24]
     * - SPI3.SCLK
       - PC10
       - P1[23]
     * - SPI3.MISO
       - PC15
       - P1[21]
     * - SPI3.MOSI
       - PC13
       - P1[19]

- I2C 引脚：

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
       - 位置
     * - I2C0.SCL
       - PY01
       - P1[28]
     * - I2C0.SDA
       - PY00
       - P1[27]
     * - I2C1.SCL
       - PY03
       - P1[5]
     * - I2C1.SDA
       - PY02
       - P1[3]

- CAN 接口：

  .. list-table::
     :header-rows: 1

     * - 功能
       - 位置
     * - CAN_H
       - J9[1]
     * - CAN_L
       - J9[3]

- ACMP 引脚：

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
       - 位置
     * - ACMP0.CH0.INN1
       - PC00
       - P1[32]

- GPTMR 引脚：

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
       - 位置
       - 备注
     * - GPTMR0.CAPT_0
       - PC10
       - P1[23]
       -
     * - GPTMR0.COMP_0
       - PC15
       - P1[21]
       -
     * - GPTMR0.COMP_1
       - PC13
       - P1[19]
       -
     * - GPTMR0.COMP_3
       - PC11
       - P1[7]
       -

- ADC 接口：

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
       - 位置
     * - ADC16输入
       - PC05
       - P1[18]

- ADC16 差分输入

  用于 ``samples/drivers/adc/adc16_differential`` 等差分采样示例。差分模式使用 ADC2（master）与 ADC3（slave）配对。

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
       - 位置
     * - ADC16 差分 master (ADC2)
       - PB12 (ADC2.IN09)
       - J7[14]（ADC_IV）
     * - ADC16 差分 slave (ADC3)
       - PB11 (ADC3.IN11)
       - J7[13]（ADC_IU）

- I2S 接口：

  .. list-table::
     :header-rows: 1

     * - 功能
       - 引脚
     * - I2S0.BCLK
       - PB09
     * - I2S0.FCLK
       - PB10
     * - I2S0.MCLK
       - PB04
     * - I2S0.RXD_3
       - PB08
     * - I2S0.TXD_0
       - PB00

- PWM 引脚：

  .. list-table::
     :header-rows: 1

     * - 功能
       - 位置
     * - PWM3.P0
       - J7[7]
     * - PWM3.P1
       - J7[8]

- OPAMP引脚 (J11)：

  .. list-table::
     :header-rows: 1

     * - 功能
       - 位置
     * - OPA3.INP1 (PB03)
       - J11[1]
     * - OPA3.OUT0 (PC09)
       - J11[2]
     * - OPA3/OPA2/OPA1.INP (PB07)
       - J11[3]
     * - OPA3/OPA2/OPA1.OUT (PC00)
       - J11[4]

- ESP-HOSTED 引脚接口

  .. list-table::
     :header-rows: 1

     * - 功能
       - 位置
       - 备注
     * - PB05
       - P1[13]
       - RESET引脚
     * - PC07
       - P1[15]
       - HANDSHAKE引脚
     * - PC08
       - P1[16]
       - DATA_READY引脚

.. _motor_pin:

- 电机引脚

  电机PWM输出和ADC采样相关引脚使用J7连接器，引脚分配如下：

  .. list-table::
     :header-rows: 1

     * - 功能
       - J7引脚
       - MCU引脚
     * - PWM_UH
       - J7[7]
       - PA16 (PWM3.P0)
     * - PWM_UL
       - J7[8]
       - PA17 (PWM3.P1)
     * - PWM_VH
       - J7[9]
       - PA30 (PWM3.P6)
     * - PWM_VL
       - J7[10]
       - PA31 (PWM3.P7)
     * - PWM_WH
       - J7[11]
       - PA28 (PWM3.P4)
     * - PWM_WL
       - J7[12]
       - PA29 (PWM3.P5)
     * - ADC_IU
       - J7[13]
       - PB11 (ADC3.IN11)
     * - ADC_IV
       - J7[14]
       - PB12 (ADC2.IN09)
     * - ADC_IW
       - J7[15]
       - PA24 (ADC1.IN12)
     * - GND
       - J7[17]
       -
     * - +5V
       - J7[19]
       -

- BROWNOUT中断指示引脚

  .. list-table::
     :header-rows: 1

     * - 功能
       - 位置
     * - PA27
       - P1[40]

- CLOCK REF 引脚

  .. list-table::
     :header-rows: 1

     * - 功能
       - 位置
     * - PB01
       - P1[12]

