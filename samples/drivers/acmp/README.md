# ACMP
## Overview

The ACMP example shows how to use ACMP peripheral to compare one analog input level and internal DAC output level.
In this example, ACMP's MIN signal connets analog input pin, PIN signal connects to internl DAC output. The program gradually increases the DAC output vlevel until it is higher than MIN signal level, the ACMP output level will toggle.

## Board Setting

Please refer to [Pin Description](lab_board_resource) for specific board
1. Connect analog input signal to ACMP MIN pin
2. Connect ACMP output pin to oscilloscope or multimeter

## Running the example

When the example runs successfully, the ACMP output pin level reversal can be observed through the oscilloscope or multimeter, and the following message is displayed in the terminal(dac set value varies with the input analog signal level):
```

> acmp example
> acmp out toggled, the dac set value is 0x8e

```
