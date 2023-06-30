# HPM6750EVK

## Overview

The HPM6750 is a dual-core flashless MCU running 816Mhz. It has a 2MB continuous on-chip ram. Also, it provides various memory interfaces, including SDRAM, Quad SPI NOR Flash, SD/eMMC. It integrates rich audio and video interfaces, including LCD, pixel DMA, camera， and I2S audio interfaces.

 ![hpm6750evk](../../../../assets/sdk/boards/hpm6750evk/hpm6750evk.png "hpm6750evk")

## Hardware

- HPM6750IVM MCU (816Mhz, 2MB OCRAM)
- Onboard Memory
  - 256Mb SDRAM
  - 128Mb Quad SPI NOR Flash
- Display & Camera
  - LCD connector
  - Camera (DVP)
- Ethernet
  - 1000 Mbits PHY
  - 100 Mbits PHY
- USB
  - USB type C (USB 2.0 OTG) connector x3
- Audio
  - Line in
  - Mic
  - Speaker
  - DAO
- Others
  - TF Slot
  - FT2232
  - Beeper
  - RGB LED
  - CAN
- Expansion port
  - Motor control

## DIP Switch S1

- Bit 2 and 3 controls boot mode

| bit[2:3] | Description                  |
| -------- | ---------------------------- |
| OFF, OFF | Boot from Quad SPI NOR flash |
| OFF, ON  | Serial boot                  |
| ON, OFF  | ISP                          |

- Change the position of bit 1 to select between PWM and Gigabit Ethernet

| Bit1 | Description       |
| ---- | ----------------- |
| OFF  | Gigabit Ethernet |
| ON   | PWM               |

(lab_hpm6750_evk_board)=

## Button

(lab_hpm6750_evk_board_buttons)=

| Name        | FUNCTIONS                                      |
| ----------- | ---------------------------------------------- |
| PBUTN (S2)  | Power Button, TinyUF2 Boot Button, GPIO Button |
| WBUTN (S3)  | WAKE UP Button                                 |
| RESETN (S4) | Reset Button                                   |

## Pin Description

- PWM Pin:

| Function                                                                                | Position |
| --------------------------------------------------------------------------------------- | -------- |
| PWM2.P0                                                                                 | J12[14]  |
| PWM2.P1                                                                                 | J12[13]  |
| ![image-1](../../../../assets/sdk/boards/hpm6750evk/hpm6750evk_pwm_output_pin.png "image-1") |          |

- SPI Pin：

| Function  | Position |
| --------- | -------- |
| SPI2.CSN  | J20[7]   |
| SPI2.SCLK | J20[8]   |
| SPI2.MISO | J20[9]   |
| SPI2.MOSI | J20[10]  |

- I2C Pin：

| Function | Position |
| -------- | -------- |
| I2C0.SCL | J20[3]   |
| I2C0.SDA | J20[4]   |

- UART for core1 debug console

| Function   | Position |
| ---------- | -------- |
| UART13.TXD | J20[5]   |
| UART13.RXD | J20[6]   |

- ACMP Pin

| Function   | Position |
| ---------- | -------- |
| CMP.INN6   | J12[8]   |
| CMP.COMP_1 | J12[6]   |

- GPTMR Pin

| Function      | Position |
| ------------- | -------- |
| GPTMR4.CAPT_1 | J12[6]   |
| GPTMR5.COMP_2 | J12[18]  |
| GPTMR5.COMP_3 | J12[16]  |

- ADC12 Pin

| Function                        | Position |
| ------------------------------- | -------- |
| ADC12 Reference Voltage Setting | N/A      |
| ADC0/1/2.VINP11                 | J12[6]   |
| ADC0/1/2.VINP10                 | J12[7]   |
| ADC0/1/2.VINP7                  | J12[8]   |

- ADC16 Pin

| Function                         | Position |
| :------------------------------- | -------- |
| ADC16 Reference Voltage Setting | N/A      |
| ADC3.INA2                        | J12[5]   |

- headphone interface

| Function        | Position |
| --------------- | -------- |
| 3.5mm headphone | J14      |

- audio input interface

| Function     | Position |
| ------------ | -------- |
| 3.5mm LIN_IN | J17      |

- DAO interface

| Function | Position |
| -------- | -------- |
| Speaker  | J18      |

- Ethernet PPS Pin

| Function    | Position |
| ----------- | -------- |
| ENET0.EVTO0 | U29[2]   |

- UART pin for uart_software_rx_idle sample or uart_rx_timeout sample

| Function   | Position |
| ---------- | -------- |
| UART13.TXD | J20[5]   |
| UART13.RXD | J20[6]   |

- TRGMUX pin for uart_software_rx_idle sample

| Function   | Position |
| ---------- | -------- |
| TRGM2_P9(PD19)  | J12[20]   |

- Motor Pin:

  Refer to section [DRV-LV50A-MP1907 Motor Driver Board ](lab_drv_lv50a_mp1907) for configuration
