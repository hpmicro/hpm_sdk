# GPTMR decode sent

## Overview

The example shows the measure PWM wave of gptmr and uses DMA to collect and decode sent signal

## Board Setting

Please refer to [Pin Description](lab_board_resource) for specific board.
Connect external SENT signal to GPTMR capture pin.


## Running the example

Running the example,if there is a SENT signal input, the serial terminal outputs the following infomation,which are the sent protocol frame
```console
sent signal decode demo
stat:00 data:01 02 03 04 05 06 crc:02  pause:false

stat:00 data:02 03 06 04 08 crc:04  pause:false

stat:00 data:02 03 06 08 09 0a crc:09  pause:false

stat:00 data:01 02 03 04 05 06 crc:02  pause:false

stat:00 data:02 03 06 04 08 crc:04  pause:false

stat:00 data:02 03 06 08 09 0a crc:09  pause:false

stat:00 data:01 01 02 crc:0b  pause:false

stat:00 data:02 03 05 06 crc:0e  pause:false


```