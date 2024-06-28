# FreeRTOS Tickless Stop Mode

## Overview

The freertos_tickless example shows freertos tickless stop mode. Soc will enter stop mode in idle states to feature reduce power consumption. The led will turn on when freertos is running at normal mode, and will turn off during tickless mode.

## Board Setting

Please refer to the README documentation of the development board and connect PUART to the USB serial converter. If there is no special description, it can be connected to the DEBUG serial port by default.

## Running the example

When the example runs successfully, the LED starts flashing and following message is displayed in the terminal:
```console
task2 is running.
task1 is running.
task1 is running.
task1 is running.
task1 is running.
```
