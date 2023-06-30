# Tamagawa encoder communication
## Overview

Communication with Tamagawa encoders using peripherals such as PLA, DMA, SPI, etc.

- Get data with different DataIDs
- Read and write EEPROM
- Error messages (CRC errors, communication errors, encoder error messages)
- Communication delay compensation

## Board Setting

- Expansion board `TMAGAWA RevA` to plug into the core board
- Connect encoder `TS5700N8401`

## Running the example

When the project is running correctly, the serial terminal will output the following information, and the position information will change when the encoder axis is rotated:
```console

cmd:3 time:638, abs pos:4170741, abm pos:0, id:23.
cmd:3 time:631, abs pos:4170741, abm pos:0, id:23.
cmd:3 time:631, abs pos:4170740, abm pos:0, id:23.
cmd:3 time:634, abs pos:4170740, abm pos:0, id:23.
cmd:3 time:639, abs pos:4170741, abm pos:0, id:23.
cmd:3 time:632, abs pos:4170741, abm pos:0, id:23.
cmd:3 time:630, abs pos:4170742, abm pos:0, id:23.

```

:::{note}

- Call `tmgw_message_process` function to update the data of `tmgw_msg`, which contains the information of the above function
- Switching command, you need to modify `tmgw_dev.comm_time_delay_config` to turn off the communication function, and then do the configuration of PLA , DMA. Otherwise, it will generate an error message once and then return to normal

:::
