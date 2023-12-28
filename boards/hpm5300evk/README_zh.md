# HPM5300EVK开发板

## 概述

HPM5300EVK提供了一系列HPM5300系列微控制器特色外设的接口，包括一个ADC输入SMA接口，一个先楫标准的电机控制接口，一个正交编码输出接口，一个RS485/422接口。HPM5300EVK同时集成了1个2x20 pin IO扩展接口，连接了HPM5300 MCU的大部分IO，供用户自由评估。HPM5300EVK为MCU外扩了NOR Flash存储，并集成了板载调试器。

![hpm5300evk](doc/hpm5300evk.png "hpm5300evk")

## 拨码开关

- Bit 1，2控制启动模式

| Bit[2:1] | 功能描述                |
| -------- | ----------------------- |
| OFF, OFF | Quad SPI NOR flash 启动 |
| OFF, ON  | 串行启动                |
| ON, OFF  | 在系统编程              |

## 按键

(lab_hpm5300evk_board_buttons)=

| 名称     | 功能                        |
| -------- | --------------------------- |
| WAKEUP   | 唤醒按键                    |
| RESET    | Reset 按键                  |
| USER KEY | TinyUF2 Boot按键, GPIO 按键 |

## 引脚描述

(lab_hpm5300_evk_board)=

- LIN引脚

| 功能 | 位置  |
| ---- | ----- |
| LIN3 | J9[5] |

- UART引脚

| 功能      | 位置   |
| --------- | ------ |
| UART2.TXD | P1[8]  |
| UART2.RXD | P1[10] |
| UART2.DE  | P1[38] |

- SPI引脚：

| 功能      | 位置   |
| --------- | ------ |
| SPI1.CSN  | P1[24] |
| SPI1.SCLK | P1[23] |
| SPI1.MISO | P1[21] |
| SPI1.MOSI | P1[19] |

- I2C引脚：

| 功能     | 位置   |
| -------- | ------ |
| I2C0.SCL | P1[28] |
| I2C0.SDA | P1[27] |

- ACMP引脚

| 功能           | 位置  |
| -------------- | ----- |
| ACMP.CMP1.INN4 | J7[4] |
| ACMP.COMP_1    | J7[2] |

- GPTMR引脚

| 功能          | 位置  |
| ------------- | ----- |
| GPTMR0.CAPT_0 | P1[3] |
| GPTMR0.COMP_0 | P1[5] |
| GPTMR0.COMP_1 | P1[8] |

- ADC16引脚

| 功能       | 位置   |
| ---------- | ------ |
| ADC0.INA4  | J7[15] |
| ADC0.INA11 | J7[20] |
| ADC1.INA1  | J7[2]  |
| ADC1.INA2  | J7[6]  |
| ADC1.INA3  | J7[4]  |
| ADC1.INA5  | J7[13] |

- DAC引脚

| 功能     | 位置   |
| -------- | ------ |
| DAC0.OUT | J7[20] |
| DAC1.OUT | J7[2]  |

- PWM引脚

| 功能    | 位置   |
| ------- | ------ |
| PWM0.P2 | J7[11] |
| PWM0.P3 | J7[12] |

- CAN引脚

| 功能  | 位置  |
| ----- | ----- |
| CAN_L | J9[1] |
| CAN_H | J9[3] |

- PLB输出引脚

| 功能    | 位置   |
| ------- | ------ |
| PLB.OUT | P1[24] |

- OPAMP引脚

| 功能      | 位置   |
| --------- | ------ |
| OPAMP.OUT | RF1    |
| OPAMP.IN  | P1[31] |

- RDC引脚

| 功能      | evk板位置 | RDC板位置 |
| --------- | ------ | ------ |
| RDC.PWM   | J7[9]  | J2[7]  |
| RDC.ADC0  | J7[13] | J2[13]  |
| RDC.ADC1  | J7[14] | J2[14]  |
| GND       | J7[32] | J2[17]  |

- QEO输出ABZ引脚

| 功能      | 位置   |
| --------- | ------ |
| QEO0.A  | J7[26] |
| QEO0.B  | J7[24] |
| QEO0.Z  | J7[22] |

- QEO控制PWM输出引脚

| 功能    | EVK板位置   | 先楫步进驱动板位置 |
| ------- | ------ | ------------- |
| PWM0.P2 | J7[11] | PWM_A1(J8[9]) |
| PWM0.P3 | J7[12] | PWM_A2(J8[10]) |
| PWM0.P4 | J7[9]  | PWM_B1(J8[11]) |
| PWM0.P5 | J7[10] | PWM_B2(J8[12]) |

- SEI引脚

| 功能      | 位置   |
| --------- | ------ |
| SEI1.CLKI_N  | J7[31] |
| SEI1.CLKI_P  | J7[29] |
| SEI1.CLKO_P  | J7[27] |
| SEI1.CLKO_N  | J7[25] |
| SEI1.DATA_P  | J7[23] |
| SEI1.DATA_N  | J7[21] |

- SEI CLK选择

| 功能      | 位置   | 说明 |
| --------- | ------ |------|
| SEI1.CLK选择  | J5 | Master侧，选择CLKO；Slave侧，选择CLKI |

- 电机引脚

  参考 [DRV-LV50A-MP1907 电机驱动板章节](lab_drv_lv50a_mp1907) 进行连接
