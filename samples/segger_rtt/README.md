# Segger RTT
## Overview
Segger Real Time Transfer (RTT) examples. https://wiki.segger.com/RTT
- input_echo
- printf_test
- menu

## Board Setting
- Connect J-Link to the board (Make sure on-board debugger is disconnected, TCK/TMS/TRST)

## Running the example
- Power on the board
- Download and the built sample application
- Once it's run successfully:
  - On board LED will be flashing
  - RTT output will be shown in "Debug Terminal" in Segger Embedded Studio

## Suggestion
To use standalone J-Link RTT Viewer, all these configuration has been done correctly in Segger Embedded Studio during project generation stage:
  - Select correct device
  - Set correct RTT control block (symbol in map file: \_SEGGER\_RTT) address which can be found in map file.
  - For the demo of menu, select the fastest transfer speed
