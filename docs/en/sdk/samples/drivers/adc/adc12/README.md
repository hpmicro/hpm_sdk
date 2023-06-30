# ADC12

## Overview

This example shows ADC12 conversions and results in four working modes.

Notes:

* When ADC is configured to non-bus-read (oneshot) mode, it can't be accessed otherwise bus will be stuck.
* When ADC is configured to bus-read (oneshot) mode,  CPU has to complete initialization before accessing to the ADC register list.

## Board Setting

Input voltage at the specified pin. （Please refer to  [Pin Description](lab_board_resource)）

Note:  The input voltage range at ADC input pins: 0~VREFH

## Running the example

- Running log is shown in the serial terminal as follows

```console
This is an ADC12 demo:
1. Oneshot    mode
2. Period     mode
3. Sequence   mode
4. Preemption mode
Please enter one of ADC conversion modes above (e.g. 1 or 2 ...):
```

- Select one of ADC working modes to start ADC conversion,  and then watch conversion results

  - Oneshot mode

  ```console
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 1
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  ```
  - Period mode

  ```console
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
  Period Mode - ADC0 [channel 11] - Result: 0x0ffb
  Period Mode - ADC0 [channel 11] - Result: 0x0fff
  Period Mode - ADC0 [channel 11] - Result: 0x0ffd
  ```
  - Sequence mode

  ```console
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
  Sequence Mode - ADC0 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0fff
  Sequence Mode - ADC0 - Cycle Bit: 00 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0ffb
  Sequence Mode - ADC0 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0ff7
  ```
  - Preemption mode

  ```console
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
  Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9
  Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9
  Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9
  ```
