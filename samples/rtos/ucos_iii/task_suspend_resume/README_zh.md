# uC/OS-III 任务暂停和恢复

## 概述

该工程演示了uC/OS-III的任务暂停和恢复功能。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，float task会在数字达到0.11时暂停led0_task，在数字达到0.30时，恢复led0_task，串口终端会输出如下信息：
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
