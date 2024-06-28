# QEOV2_SEI2ABZ

## Overview

The QEOV2_SEI2ABZ example project demonstrates obtaining sensor position information through the SEI interface (this example uses the Tamagawa sensor TS5700N8401). Based on the position information obtained by SEI, the width of the QEO output ABZ signal is dynamically adjusted to achieve uniform output of ABZ signal.

## Board Setting

- Connect the Tamagawa sensor TS5700N8401 to the corresponding pin DATA_P/DATA_N on the board, and supply 5V power.
- Connect the ABZ output pins QEO_A/QEO_B/QEO_Z of the board to the logic analyzer to capture the output waveform (please refer to the {ref} ` pin description<labwboard_source>` section).

## Running the example

When the project runs correctly, the serial port terminal will receive the following information. Rotating the sensor, the logic analyzer will capture a uniform ABZ signal that is consistent with the rotating state:
```console
QEO SEI2ABZ example
QEO generate ABZ signal with hardware(SEI) provide postion
device init finish
```
