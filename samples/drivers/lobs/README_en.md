# LOBS

## Overview

The LOBS example project demonstrates the trace function.

In this example, the Trace Buffer setting can record 1024 records of data, each containing 16 bytes. The design is to capture 64 data before the trigger point, and after the trigger point, capture (1024-64)=960 data. Trigger signal pins can be found in the readme section of the board folder, which trigger by high level.

Users can modify the macro definition LOBS_USE_TWO_GROUP in the CMakeLists.txt file to select the group working mode.

## Board Setting

No special settings are required

## Running the example

- Connect the trigger signal pin to GND low level.
- Download the program to the development board and run it.
- Connect the trigger signal pin to high level, and after completing the trigger, the serial port will print `trace finish`.

Note: The Trace data is stored in AHB RAM, and users can view the memory through the debugger.

```console
lobs example

lobs trace finish!
trace memory base addr: 0xf0200000, trace data final addr: 0xf020150c
```
