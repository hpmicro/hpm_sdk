# HPM5300EVK

## Overview

The HPM5300EVK provides a series of interfaces for the characteristic peripherals of the HPM5300 series microcontrollers, including an ADC input SMA interface, a first-class motor control interface, a ABZ output interface, a RS485/422 interface. HPM5300EVK also integrates one 2x20 pin IO expansion interfaces, which connect most of the IOs of HPM5300 MCU for users to freely evaluate. HPM5300EVK expands NOR Flash storage for MCU and integrates an on-board debugger.

![hpm5300evk](doc/hpm5300evk.png "hpm5300evk")

## DIP Switch

- Bit 1 and 2 controls boot mode

| bit[2:1] | Description                  |
| -------- | ---------------------------- |
| OFF, OFF | Boot from Quad SPI NOR flash |
| OFF, ON  | Serial boot                  |
| ON, OFF  | ISP                          |

## Button

(lab_hpm5300evk_board_buttons)=

| Name     | FUNCTIONS                        |
| -------- | -------------------------------- |
| WAKEUP   | Wakeup Button                    |
| RESET    | Reset Button                     |
| USER KEY | TinyUF2 Boot Button, GPIO Button |

## Pin Description

(lab_hpm5300_evk_board)=

- UART Pin:

| Function  | Position |
| --------- | -------- |
| UART2.TXD | P1[8]    |
| UART2.RXD | P1[10]   |
| UART2.DE  | P1[38] |

- LIN Pin:

| Function | Position |
| -------- | -------- |
| LIN3     | J9[5]    |

- SPI Pin：

| Function  | Position |
| --------- | -------- |
| SPI1.CSN  | P1[24]   |
| SPI1.SCLK | P1[23]   |
| SPI1.MISO | P1[21]   |
| SPI1.MOSI | P1[19]   |

- I2C Pin：

| Function | Position |
| -------- | -------- |
| I2C0.SCL | P1[28]   |
| I2C0.SDA | P1[27]   |

- ACMP Pin:

| Function       | Position |
| -------------- | -------- |
| ACMP.CMP1.INN4 | J7[4]    |
| ACMP.COMP_1    | J7[2]    |

- GPTMR Pin:

| Function      | Position |
| ------------- | -------- |
| GPTMR0.CAPT_0 | P1[3]    |
| GPTMR0.COMP_0 | P1[5]    |
| GPTMR0.COMP_1 | P1[8]    |

- ADC16 Pin:

| Function   | Position |
| ---------- | -------- |
| ADC0.INA4  | J7[15]   |
| ADC0.INA11 | J7[20]   |
| ADC1.INA1  | J7[2]    |
| ADC1.INA2  | J7[6]    |
| ADC1.INA3  | J7[4]    |
| ADC1.INA5  | J7[13]   |

- DAC Pin:

| Function | Position |
| -------- | -------- |
| DAC0.OUT | J7[20]   |
| DAC1.OUT | J7[2]    |

- PWM Pin:

| Function | Position |
| -------- | -------- |
| PWM0.P2  | J7[11]   |
| PWM0.P3  | J7[12]   |

- PLB Output Pin:

| Function | Position |
| -------- | -------- |
| PLB.OUT  | P1[24]   |

- CAN Pin:

| Function | Position |
| -------- | -------- |
| CAN_L    | J9[1]    |
| CAN_H    | J9[3]    |

- OPAMP Pin:

| Function  | Position |
| --------- | -------- |
| OPAMP.OUT | RF1      |
| OPAMP.IN  | P1[31]   |

- RDC Pin:

| Function      | evk Position | RDC Position |
| ---------     | ------ | ------ |
| RDC.PWM       | J7[9]  | J2[7]  |
| RDC.ADC0      | J7[13] | J2[13]  |
| RDC.ADC1      | J7[14] | J2[14]  |
| GND           | J7[32] | J2[17]  |

- QEO ABZ Pin:

| Function  | Position |
| --------- | ------ |
| QEO0.A  | J7[26] |
| QEO0.B  | J7[24] |
| QEO0.Z  | J7[22] |

- PWM pin with QEO control：

| Function | EVK Position | HPMicro's stepper drive board Position |
| ------- | ------ | ------------- |
| PWM0.P2 | J7[11] | PWM_A1(J8[9]) |
| PWM0.P3 | J7[12] | PWM_A2(J8[10]) |
| PWM0.P4 | J7[9]  | PWM_B1(J8[11]) |
| PWM0.P5 | J7[10] | PWM_B2(J8[12]) |

- SEI Pin:

| Function  | Position   |
| --------- | ------ |
| SEI1.CLKI_N  | J7[31] |
| SEI1.CLKI_P  | J7[29] |
| SEI1.CLKO_P  | J7[27] |
| SEI1.CLKO_N  | J7[25] |
| SEI1.DATA_P  | J7[23] |
| SEI1.DATA_N  | J7[21] |

- SEI CLK Section:

| Function  | Position   | Note |
| --------- | ------ |------|
| SEI1.CLK Section  | J5 | Master side, CLKO active. Slave side，CLKI active |

- Motor Pin:

  Refer to section [DRV-LV50A-MP1907 Motor Driver Board ](lab_drv_lv50a_mp1907) for configuration
