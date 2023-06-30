# GPTMR measure PWM

## Overview

The example shows measure PWM wave.

## Board Setting

Please refer to [Pin Description](lab_board_resource) for specific board.
Connect external PWM signal to GPTMR capture pin.


## Running the example

Running the example,if there is a PWM wave input, the serial terminal outputs the following infomation,which are the frequency and duty cycle of the PWM wave.
```console
gptmr pwm measure test
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.01%
measured frequency: 10000.000000
measured duty cycle: 30.01%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.01%
measured frequency: 10000.000000
measured duty cycle: 30.01%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
measured frequency: 10000.000000
measured duty cycle: 30.00%
```