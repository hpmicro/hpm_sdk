# PLIC
## 概述

PLIC示例工程使用ptpc/gpio/plicsw三个中断演示中断嵌套功能。

## 硬件设置

无特殊设置

## 运行现象

运行工程，按照log提示按下按键，会产生中断嵌套，串口终端会输出如下信息：
```
> interrupt test
> ptpc interrupt start
> Press the button immediately, enter the gpio interrupt
> gpio interrupt start
> plicsw start
> plicsw end
> gpio interrupt end
> ptpc interrupt end
```