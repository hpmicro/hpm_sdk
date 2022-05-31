# PLIC
## Overview

The PLIC example uses ptpc/gpio/plicsw interrupts to shows interrupt nesting function.

## Board Setting

No special settings are required

## Running the example

Running the example, interrupt nesting will occur when pressing the button according to the log prompt. The following message is displayed in the terminal:
```
> interrupt test
> ptpc interrupt start
> Press the button immediately, enter the gpio interrupt
> gpio interrupt start
> plicsw start
> plicsw end
> gpio interrupt end
> ptpc interrupt end
```
