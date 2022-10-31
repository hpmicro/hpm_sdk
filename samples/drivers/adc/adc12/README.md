# ADC12

## Overview

This example shows ADC12 conversions and results in four working modes.

## Board Setting

Input voltage at the specified pin. （Please refer to  [Pin Description](lab_board_resource)）

## Running the example

- Running log is shown in the serial terminal as follows

```
This is an ADC12 demo:
1. Oneshot    mode
2. Period     mode
3. Sequence   mode
4. Preemption mode
Please enter one of ADC conversion modes above (e.g. 1 or 2 ...):
```

- Select one of ADC working modes to start ADC conversion,  and then watch conversion results

  - Oneshot mode

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 1
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  ```

  - Period mode

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
  Period Mode - ADC0 [channel 11] - Result: 0xffb0
  Period Mode - ADC0 [channel 11] - Result: 0xfff0
  Period Mode - ADC0 [channel 11] - Result: 0xffd0
  ```

  - Sequence mode

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
  Sequence Mode - ADC0 - Sequence Number:00 Cycle Bit: 00 ADC Channel: 00 Result: 0x0000
  Sequence Mode - ADC0 - Sequence Number:00 Cycle Bit: 01 ADC Channel: 11 Result: 0x0fe3
  Sequence Mode - ADC0 - Sequence Number:00 Cycle Bit: 00 ADC Channel: 11 Result: 0x0fe7
  ```

  - Preemption mode

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
  Preemption Mode - ADC0 - Cycle Bit: 01 ADC Channel: 11 Trig Index: 00 Trig Channel: 00 Result: 0x0ffb
  Preemption Mode - ADC0 - Cycle Bit: 01 ADC Channel: 11 Trig Index: 00 Trig Channel: 00 Result: 0x0ffd
  Preemption Mode - ADC0 - Cycle Bit: 01 ADC Channel: 11 Trig Index: 00 Trig Channel: 00 Result: 0x0ffc
  ```
