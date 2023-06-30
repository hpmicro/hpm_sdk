# HPM6300EVK

## Overview
HPM6300EVK provides interfaces for most of the peripherals of the HPM6300 microcontroller, including 1 SD card slot, 1 USB Type-C interface, a 100M Ethernet port, CAN FD interface, etc., and expands NOR Flash, SDRAM and other external memories , and integrates an on-board debugger.

![hpm6300evk](../../../../assets/sdk/boards/hpm6300evk/hpm6300evk.png "hpm6300evk")

## Hardware

- HPM6360IPA MCU
- Onboard Memory
  - 16bit 256Mb SDRAM
  - 128Mb Quad SPI NOR Flash
- USB
  - USB type C (USB 2.0 OTG) connector x2
- Ethernet
  - 100Mb PHY
- Others
  - TF Slot
  - CAN FD
  - FT2232
- Expansion port
  - Motor control port
  - Raspberry PI extension port

:::{note}

The sdram pins are multiplexed with the 15-20pin of the motor interface J26, so the motor interface and sdram can't be used at the same time.

:::

## DIP Switch S1

- Bit 1 and 2 controls boot mode

| bit[2:1] | Description                  |
| -------- | ---------------------------- |
| OFF, OFF | Boot from Quad SPI NOR flash |
| OFF, ON  | Serial boot                  |
| ON, OFF  | ISP                          |

(lab_hpm6300_evk_board)=

## Button

(lab_hpm6300_evk_board_buttons)=

| Function     | Position                                       |
| ------------ | ---------------------------------------------- |
| PBUTN (SW1)  | Power Button, TinyUF2 Boot Button, GPIO Button |
| WBUTN (SW2)  | WAKE UP Button                                 |
| RESETN (SW3) | Reset Button                                   |

Plug-in

- `J108` ADC/DAC reference voltage selection
  | Connection | Description         |
  | ---------- | ------------------- |
  | 1, 2       | 3.3V                |
  | 2, 3       | High precision 3.3V |

## Pin Description

- SPI Pin：

| Function  | Position |
| --------- | -------- |
| SPI3.CSN  | J28[24]  |
| SPI3.SCLK | J28[23]  |
| SPI3.MISO | J28[21]  |
| SPI3.MOSI | J28[19]  |

- I2C Pin：

| Function | Position |
| -------- | -------- |
| I2C0.SCL | J28[13]  |
| I2C0.SDA | J28[15]  |

- ACMP Pin

| Function   | Position |
| ---------- | -------- |
| CMP.INN5   | J26[7]   |
| CMP.COMP_1 | J26[5]   |

- GPTMR Pin

| Function      | Position |
| ------------- | -------- |
| GPTMR2.CAPT_0 | J28[40] |
| GPTMR2.COMP_0 | J28[35] |
| GPTMR2.COMP_1 | J28[12] |

- ADC16 Pin

| Function                       | Position |
| ------------------------------ | -------- |
| ADC16 Reference Voltage        | J108[2]  |
| ADC0.INA13/ADC1.INA9/ADC2.INA5 | J26[6]   |

- DAC Pin

| Function                | Position |
| ----------------------- | -------- |
| ADC16 Reference Voltage | J108[2]  |
| DAC0.OUT                | J26[11]  |

- PWM Pin

| Function | Position |
| -------- | -------- |
| PWM2.P0  | J26[14]  |
| PWM2.P1  | J26[13]  |

- Ethernet PPS Pin

| Function    | Position |
| ----------- | -------- |
| ENET0.EVTO0 | J28[19]  |
| ENET0.EVTO1 | J28[23]  |

- UART pin for uart_software_rx_idle sample or uart_rx_timeout sample

| Function   | Position |
| ---------- | -------- |
| UART2.TXD | J28[18]   |
| UART2.RXD | J28[22]   |

- TRGMUX pin for uart_software_rx_idle sample

| Function   | Position |
| ---------- | -------- |
| TRGM1_P4(PA24)  | J28[16]   |

- Motor Pin:

  Refer to section [DRV-LV50A-MP1907 Motor Driver Board ](lab_drv_lv50a_mp1907) for configuration
