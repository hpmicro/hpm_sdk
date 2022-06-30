# l1 cache
## Overview
***
**l1c** This example project shows the configuration of l1 cache.

## Port Settings
***
- Serial port baud rate is set to ``115200bps``, with ``one stop bit`` and ``no parity bit``

## Running the example
***
When the project runs correctly, the serial port terminal will output the following information (taking HPM6750 for example):
```

l1 cache driver example
icache config: set: 128
  way: 4-way
  size: 64 bytes
  lock: with locking support
  ecc: No parity/ECC
  local memory ecc: No parity/ECC
dcache config:
  set: 128
  way: 4-way
  size: 64 bytes
  lock: with locking support
  ecc: No parity/ECC
  local memory ecc: No parity/ECC
dcache is enabled
icache is enabled

```
