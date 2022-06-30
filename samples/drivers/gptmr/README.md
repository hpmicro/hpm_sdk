# GPTMR
## Overview

The GPTMR example shows compare and capture function.

## Board Setting

Please refer to [Pin Description](lab_board_resource) for specific board.
Connect external PWM signal to GPTMR capture pin, connect GPTMR compare output pin to oscilloscope.


## Running the example

Running the example, the PWM signal with changing duty cycle can be seen through the oscilloscope. The following message is displayed in the terminal:
```
> timer testing
> time is up: 0x1059
> captured rising edge: 0x36f3f
> measured period: 0x10001
> measured duty cycle: 0x8000
```
