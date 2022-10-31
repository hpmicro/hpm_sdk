# PLL controller v2
## Overview
***
**pllctl** The pllctl driving example project of PLL controller shows that the frequency division coefficient of the system clock source is configured through the PLL controller in different modes to achieve the purpose of stabilizing the main frequency, frequency division or frequency doubling of the system.
- In the example code, the PLL controller logic is realized by configuring the reference clock frequency division coefficient, feedback frequency division coefficient, post frequency division coefficient, etc
- PLL setting is realized by configuring the MFI and MFD register
- Set the system frequency from **400 MHz to 1 GHz** to show the actual deviation from the expected dominant frequency

## Board Setting
***
- No special settings

## Running the example
***
When the project runs correctly, the serial port terminal will output the following information:
```shell
PLLCTLV2 example
Now configure PLL at integer mode
Update PLL2 to 400000000Hz
Actual PLL frequency is 400000000Hz
Update PLL2 to 401000000Hz
Actual PLL frequency is 401000000Hz
Update PLL2 to 402000000Hz
Actual PLL frequency is 402000000Hz
Update PLL2 to 403000000Hz
Actual PLL frequency is 403000000Hz
Update PLL2 to 404000000Hz
Actual PLL frequency is 404000000Hz
Update PLL2 to 405000000Hz
Actual PLL frequency is 405000000Hz
Update PLL2 to 406000000Hz
Actual PLL frequency is 406000000Hz
Update PLL2 to 407000000Hz
...
Update PLL2 to 998000000Hz
Actual PLL frequency is 998000000Hz
Update PLL2 to 999000000Hz
Actual PLL frequency is 999000000Hz
Update PLL2 to 1000000000Hz
Actual PLL frequency is 1000000000Hz
PLL2 ss has been enabled

PLLCTLV2 example finished

```