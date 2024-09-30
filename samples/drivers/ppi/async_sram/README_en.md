# PPI ASYNC SRAM example
## Overview

This example project shows read and write data in external async SRAM by PPI.

The PPI memory address region is 0xF8000000~0xFFFFFFFF, base address should be aligned by 1MB.

## Board Setting

External async SRAM module is required. The tested async SRAM is IS62WV51216EBLL45TLI. When in AD Mux mode, the address latch chip used in this sample is SN74HC573APWR.

The PPI component supports both AD MUX mode and AD Non-MUx mode.

[1] When AD Non-Mux mode, `DQ0-DQ15` as data lines, `DQ16-DQ31` as address lines. The high address lines above A16 of IS62WV51216EBLL45TLI must be connected to GND.
[2] When AD Mux mode, `DQ0-DQ15` as data lines and address lines. The high address lines above A16 of IS62WV51216EBLL45TLI must be connected to GND.

In this example, select the AD mode using the macro `BOARD_PPI_ASYNC_SRAM_AD_MUX_MODE`, select the CS signal using the macro `BOARD_PPI_ASYNC_SRAM_CS_INDEX`, select the ADV signal using the macro `BOARD_PPI_ASYNC_SRAM_ADV_CTRL_PIN`, select the REL signal using the macro `BOARD_PPI_ASYNC_SRAM_OE_CTRL_PIN`, and select the WEL signal using the macro `BOARD_PPI_ASYNC_SRAM_WE_CTRL_PIN`. Users can make modifications based on their actual schematic diagram.

For more information on PPI components, please refer to: [PPI Components Readme.md](../../../../components/ppi/README_en.md)

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
sram example
comparison test: from 0xf8000000 to 0xf8020000
**  u32 comparison succeeded
**  u16 comparison succeeded
**  u8 comparison succeeded
```
