# PPI ASYNC SRAM example
## Overview

This example project shows read and write data in external async SRAM by PPI.

The PPI memory address region is 0xF8000000~0xFFFFFFFF, base address should be aligned by 1MB.

## Board Setting

External async SRAM module is required. The tested async SRAM is IS62WV51216EBLL45TLI.

This use address and data Non-Mux mode, DQ0~DQ15 as data lines, DQ16~DQ31 as address lines. The high address lines above A16 of IS62WV51216EBLL45TLI must be connected to GND.

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
sram example
comparison test: from 0xf8000000 to 0xf8020000
**  u32 comparison succeeded
**  u16 comparison succeeded
**  u8 comparison succeeded
```
