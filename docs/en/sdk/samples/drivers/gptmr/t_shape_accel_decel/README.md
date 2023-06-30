# T_shape_accel_decel_pulses

## Overview

The example shows generate T shape aceel decel pulses.

## Board Setting

Please refer to [Pin Description](lab_board_resource) for specific board.
Connect GPTMR compare output pin to oscilloscope.


## Running the example

Running the example,   and use an oscilloscope to observe the output comparison pins.  the serial terminal outputs will print the reload value of different pulses after power-on, and then pulses with fixed pulse width and different frequencies will be output, the LED lights up to indicate that the generation is complete. and the serial port terminal outputs the following information.
```console

generate T_shape_accel_decel test
shape_table[0] :10000
shape_table[1] :9900
shape_table[2] :9800
shape_table[3] :9700
shape_table[4] :9600
shape_table[5] :9500
shape_table[6] :9400
shape_table[7] :9300
shape_table[8] :9200
shape_table[9] :9100
shape_table[10] :9000
shape_table[11] :8900
shape_table[12] :8800
shape_table[13] :8700
shape_table[14] :8600
shape_table[15] :8500
shape_table[16] :8400
shape_table[17] :8300
shape_table[18] :8200
shape_table[19] :8100
shape_table[20] :8000
shape_table[21] :7900
shape_table[22] :7800
....
```