# System control module
## Overview
***
**synt** The system control module driver example project shows the module management of the system power domain, the opening of resource nodes, the configuration of clock sources, low-power management and so on.
- In the example code, first enable the cpu0 external resource node switch, and configure **UART clock source**, **ana clock source** and **adc clock source** respectively.
- Test the state of writing **cpu0-gpr register** when **cpu0-lock register** is locked and unlocked respectively, and test the low-power enable configuration.
- Perform system reset operation.

## Board Setting
***
- No special settings

## Running the example
***
When the project runs correctly, the serial port terminal will output the following information:
```
> sysctl example
> test enable resource
> test clock config
> config uart2 to pll3_clk0 (@77529088 Hz) divided by 130: 4726272 Hz
> test clock adc/i2s config
> config ana2 clock source to pll1_clk1
> config adc2 to ana2_clk
> access_cpu_gpr: GPR lock disabled
> write data to gpr @0, 3
> [expected]: Write CPU GPR without lock done
> access_cpu_gpr: GPR lock enabled
> write data to gpr @0, 3
> [expected]: Write CPU GPR with lock done
> access_cpu_gpr: GPR lock enabled
> write data to gpr @0, 3
> result: 0xabd 0x31df 0x237c
> [expected]: Failed to write CPU GPR with lock
> access_cpu_gpr: GPR lock disabled
> write data to gpr @3, 4
> [expected]: Write CPU GPR without lock done
> reset_soc
```
If the segger embedded studio project build type adopts flash_ XIP or Flash_ sdram_ XIP, the following information will be printed additionally:
```
> Software reset has occurred
```