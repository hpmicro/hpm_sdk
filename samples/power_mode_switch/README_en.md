# Power Mode Switch

## Overview

power_mode_switch shows how different power modes are switched to/from

## Board Setting

Connect serial port (from PUART, refer to [PUART pins](lab_board_puart_pin)) to PC

__Note__:
  - Please power cycle the board (boot from flash) is needed to run the demo application after this firmware is downloaded.

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:

```console
Power mode switch demo:
  1 - Enter wait mode
  2 - Enter stop mode
  3 - Enter standby mode
  4 - Enter shutdown mode
```

Switch to different power mode by enter different option on serial port terminal.
