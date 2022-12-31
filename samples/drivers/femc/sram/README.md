# SRAM example
## Overview

SMA example project shows performance of reading and writing data in internal or external SRAM memory.

## Board Setting

External SRAM module is required.

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
sram example
comparison test: from 0x48000000 to 0x48001000
**  u32 comparison succeeded
**  u16 comparison succeeded
**  u8 comparison succeeded
```

## Hint
When SRAM interface is used to access external devices, such as FPGA, SRAM address space should be configured as non-cacheable.