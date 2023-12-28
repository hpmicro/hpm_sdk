# uC/OS-III Task Suspend Resume

## Overview

The example shows uC/OS-III task suspend and resume function.

## Board Setting

No special settings are required

## Running the example

When the example runs successfully, task float will suspend led0_task when the nummer is 0.11, task float will resume led0_task when the number is 0.30, following message is displayed in the terminal:
```console
led0_task loop for 1 times
float_num: 0.0200
float_num: 0.0300
float_num: 0.0400
float_num: 0.0500
led0_task loop for 2 times
float_num: 0.0600
float_num: 0.0700
float_num: 0.0800
led0_task loop for 3 times
float_num: 0.0900
float_num: 0.1000
float_num: 0.1100
suspend task led
float_num: 0.1200
float_num: 0.1300
float_num: 0.1400
float_num: 0.1500
float_num: 0.1600
float_num: 0.1700
float_num: 0.1800
float_num: 0.1900
float_num: 0.2000
float_num: 0.2100
float_num: 0.2200
float_num: 0.2300
float_num: 0.2400
float_num: 0.2500
float_num: 0.2600
float_num: 0.2700
float_num: 0.2800
float_num: 0.2900
float_num: 0.3000
led0_task loop for 4 times
resume task led
float_num: 0.0100
float_num: 0.0200
float_num: 0.0300
led0_task loop for 5 times
float_num: 0.0400
float_num: 0.0500
float_num: 0.0600
led0_task loop for 6 times
float_num: 0.0700
float_num: 0.0800
```
