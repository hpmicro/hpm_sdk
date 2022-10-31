# ADC16

## Overview

This example shows ADC16 conversions and results in four working modes.

## Board Setting

Input voltage at the specified pin. （Please refer to  [Pin Description](lab_board_resource)）

## Running the example

- Running log is shown in the serial terminal as follows

```
This is an ADC16 demo:
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
  Oneshot Mode - ADC3 [channel 2] - Result: 0xffe8
  Oneshot Mode - ADC3 [channel 2] - Result: 0xfff4
  Oneshot Mode - ADC3 [channel 2] - Result: 0xffff
  ```

  - Period mode

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
  Period Mode - ADC3 [channel 2] - Result: 0xfff3
  Period Mode - ADC3 [channel 2] - Result: 0xfff5
  Period Mode - ADC3 [channel 2] - Result: 0xfff7
  ```

  - Sequence mode

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
  Sequence Mode - ADC3 - Sequence Number:00 Cycle Bit: 01 ADC Channel: 02 Result: 0xffff
  Sequence Mode - ADC3 - Sequence Number:00 Cycle Bit: 00 ADC Channel: 02 Result: 0xffff
  Sequence Mode - ADC3 - Sequence Number:00 Cycle Bit: 01 ADC Channel: 02 Result: 0xfff2
  ```

  - Preemption mode

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
  Preemption Mode - ADC3 - Cycle Bit: 01 ADC Channel: 02 Trig Index: 00 Trig Channel: 00 Result: 0xffff
  Preemption Mode - ADC3 - Cycle Bit: 01 ADC Channel: 02 Trig Index: 00 Trig Channel: 00 Result: 0xfffa
  Preemption Mode - ADC3 - Cycle Bit: 01 ADC Channel: 02 Trig Index: 00 Trig Channel: 00 Result: 0xffff
  ```