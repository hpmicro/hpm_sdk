# PTPC
## Overview
***
This PTPC sample covers timestamp comparison and pps interrupt generation

## Board Setting
***
Nothing special

## Running the example
***
When the example runs successfully, board LED will be flashing periodically and the following message should be printed on debug console:
```
> ptpc example
> Config target timestamp: 3 s, 80000 ns
> Got event at 3 s, 81180 ns
> timer_compare done
> Config pps IRQ frequency: 1 Hz
> pps_interrupt done
```
