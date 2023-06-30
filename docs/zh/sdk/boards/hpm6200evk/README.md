# HPM6200EVK开发板

## 概述

HPM6200EVK提供了一系列HPM6200系列微控制器特色外设的接口，包括一个ADC输入SMA接口，一个高分辨率PWM输出的SMA接口，和一个先楫标准的电机控制接口。HPM6200EVK同时集成了2个2x20 pin IO扩展接口，连接了HPM6200 MCU的大部分IO，供用户自由评估。HPM6200EVK为MCU外扩了NOR Flash存储，并集成了板载调试器。

![hpm6200evk](../../../../assets/sdk/boards/hpm6200evk/hpm6200evk.png "hpm6200evk")

## 拨码开关 SW1

- Bit 1，2控制启动模式

| Bit[2:1] | 功能描述                |
| -------- | ----------------------- |
| OFF, OFF | Quad SPI NOR flash 启动 |
| OFF, ON  | 串行启动                |
| ON, OFF  | 在系统编程              |

## 按键

(lab_hpm6200evk_board_buttons)=

| 名称       | 功能                                  |
| ---------- | ------------------------------------- |
| PBUTN (SW3) | 电源按键, TinyUF2 Boot按键, GPIO 按键 |
| RESET (SW2) | Reset 按键                            |

## 引脚描述

(lab_hpm6200_evk_board)=

- CORE1调试串口引脚：

| 功能       | 位置   |
| ---------- | ------ |
| UART2.TXD | P1[8] |
| UART2.RXD | P1[10] |

- LIN引脚

| 功能 | 位置 |
| ---------- | ------ |
| LIN0.TX    | J1[3] |
| LIN0.RX    | J1[6] |

- SDM引脚

| 功能 | 位置 |
| ---------- | ------ |
| SDM0.CLK_3    | P2[27] |
| SDM0.DAT_3    | P2[28] |

- SPI引脚：

| 功能      | 位置   |
| --------- | ------ |
| SPI1.CSN  | P2[29] |
| SPI1.SCLK | P2[32] |
| SPI1.MISO | P2[31] |
| SPI1.MOSI | P2[33] |

- I2C引脚：

| 功能     | 位置   |
| -------- | ------ |
| I2C0.SCL | P1[5] |
| I2C0.SDA | P1[3] |

- ACMP引脚

| 功能       | 位置   |
| ---------- | ------ |
| ACMP.CMP1.INN5 | J4[6] |
| ACMP.COMP_1 | J4[8]  |

- GPTMR引脚

| 功能          | 位置   |
| ------------- | ------ |
| GPTMR2.CAPT_0 | J4[15] |
| GPTMR2.COMP_0 | J4[17] |
| GPTMR2.COMP_1 | J4[1]  |

- ADC16引脚

| 功能                           | 位置  |
| ------------------------------ | ----- |
| ADC0.INA1                      | J4[5] |
| ADC0.INA12/ADC1.INA8/ADC2.INA4 | J4[6] |
| ADC0.INA13/ADC1.INA9/ADC2.INA5 | J4[7] |
| ADC0.INA11/ADC1.INA7.ADC2.INA3 | J4[8] |

- DAC引脚

| 功能     | 位置  |
| -------- | ----- |
| DAC0.OUT | J2[1] |
| DAC1.OUT | J2[2] |

- PWM引脚

| 功能     | 位置  |
| -------- | ----- |
| PWM0.P0 | J4[14] |
| PWM0.P1 | J4[13] |

- HRPWM引脚

| 功能     | 位置  |
| -------- | ----- |
| HRPWM0.P0 | RF2 |
| HRPWM0.P2 | P2[29] |

- PLA输出引脚

| 功能     | 位置  |
| -------- | ----- |
| PLA.OUT | P1[26] |

- 电机引脚

    参考 [DRV-LV50A-MP1907 电机驱动板章节](lab_drv_lv50a_mp1907) 进行连接