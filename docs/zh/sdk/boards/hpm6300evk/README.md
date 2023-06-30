# HPM6300EVK开发板

## 概述

HPM6300EVK提供了HPM6300微控制器大部分外设的接口，包括1个SD卡槽，1个USB Type-C接口，一个百兆网口，CAN FD接口等，并扩展了NOR Flash，SDRAM等外接存储器，并集成了板载调试器。

![hpm6300evk](../../../../assets/sdk/boards/hpm6300evk/hpm6300evk.png "hpm6300evk")

## 板上硬件资源

- HPM6360IPA微控制器
- 板载存储
  - 128Mbit（16MB）QSPI NOR Flash
  - 16位，256MBit SDRAM
- USB
  - 1个USB Type-C接口
- 以太网
  - 1个百兆网口
- 其他
  - TF 卡槽
  - 1个CAN FD接口
  - 板载FT2232调试器
- 扩展口
  - 电机控制接口
  - Raspberry PI IO扩展接口

:::{note}

开发板sdram的地址引脚和电机接口J26的15-20pin存在复用，所以电机接口和sdram不能同时使用。

:::

## 拨码开关 S1

- Bit 1，2控制启动模式

| Bit[2:1] | 功能描述                |
| -------- | ----------------------- |
| OFF, OFF | Quad SPI NOR flash 启动 |
| OFF, ON  | 串行启动                |
| ON, OFF  | 在系统编程              |

(lab_hpm6300_evk_board)=

## 按键

(lab_hpm6300_evk_board_buttons)=

| 名称         | 功能                                  |
| ------------ | ------------------------------------- |
| PBUTN (SW1)  | 电源按键, TinyUF2 Boot按键, GPIO 按键 |
| WBUTN (SW2)  | WAKE UP 按键                          |
| RESETN (SW3) | Reset 按键                            |

## **插件**

- `J108` ADC/DAC参考电压选择

| 连接 | 描述        |
| ---- | ----------- |
| 1, 2 | 3.3V        |
| 2, 3 | 高精度 3.3V |

## 引脚描述

- SPI引脚：

| 功能      | 位置    |
| --------- | ------- |
| SPI3.CSN  | J28[24] |
| SPI3.SCLK | J28[23] |
| SPI3.MISO | J28[21] |
| SPI3.MOSI | J28[19] |

- I2C引脚：

| 功能     | 位置    |
| -------- | ------- |
| I2C0.SCL | J28[13] |
| I2C0.SDA | J28[15] |

- ACMP引脚

| 功能       | 位置   |
| ---------- | ------ |
| CMP.INN5   | J26[7] |
| CMP.COMP_1 | J26[5] |

- GPTMR引脚

| 功能          | 位置    |
| ------------- | ------- |
| GPTMR2.CAPT_0 | J28[40] |
| GPTMR2.COMP_0 | J28[35] |
| GPTMR2.COMP_1 | J28[12] |

- ADC16引脚

| 功能                           | 位置    |
| ------------------------------ | ------- |
| ADC16参考电压                  | J108[2] |
| ADC0.INA13/ADC1.INA9/ADC2.INA5 | J26[6]  |

- DAC引脚

| 功能     | 位置    |
| -------- | ------- |
| DAC0.OUT | J26[11] |

- PWM引脚

| 功能    | 位置    |
| ------- | ------- |
| PWM2.P0 | J26[14] |
| PWM2.P1 | J26[13] |

- 以太网PPS引脚

| 功能        | 位置    |
| ----------- | ------- |
| ENET0.EVTO0 | J28[19] |
| ENET0.EVTO1 | J28[23] |

- UART引脚用于uart_software_rx_idle或uart_rx_timeout或uart_lin工程

| 功能        | 位置    |
| ---------- | -------- |
| UART2.TXD | J28[18]   |
| UART2.RXD | J28[22]   |

- TRGMUX引脚用于uart_software_rx_idle工程

| 功能        | 位置    |
| ---------- | -------- |
| TRGM1_P4(PA24)  | J28[16]   |

- 电机引脚

  参考 [DRV-LV50A-MP1907 电机驱动板章节](lab_drv_lv50a_mp1907) 进行连接
