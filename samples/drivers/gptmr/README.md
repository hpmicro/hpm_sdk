# GPTMR
## Overview

The GPTMR example shows comparison, capture, PWM measurement and PWM generation functions.

## Board Setting

Please refer to [Pin Description](lab_board_resource) for specific board.
Connect external PWM signal to GPTMR capture pin, connect GPTMR compare output pin to oscilloscope.


## Running the example

Running the example, the example will execute comparison, capture, PWM measurement and PWM generation functions test, The following message is displayed in the terminal and the PWM signal with changing duty cycle can be seen through the oscilloscope:
```
> timer testing
> time is up: 0x1059
> captured rising edge: 0x36f3f
> measured period: 0x10001
> measured duty cycle: 0x8000
> test generate waveform edge aligned pwm
```
