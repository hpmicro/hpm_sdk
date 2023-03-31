# machine timer

## Overview

**Mchtmr** machine timer drive example project shows how to realize the timing function through the machine timer module.
- The example code configures **LED8** as the output function, configures the machine timer cycle **3 seconds**, and makes the LED light flash regularly according to the timer configuration

## Board Setting

- No special settings

## Running the example

When the project runs correctly, ``led`` is lit and flashes regularly every 3 seconds if LED on board, and the serial port terminal will output the information like::
```console
mchtmr example
LED shall be flashing every 3s
mchtmr interrupt!
```
