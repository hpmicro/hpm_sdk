# BUTN

## 概述

BUTN工程演示了如何使用PBUTN按键进入和退出掉电状态。

##注意

当进入掉电模式时， RAM中的代码会丢失，为了演示这个工程，不能将代码存放在RAM中。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，按PBUTN 16s, 进入掉电模式。在掉电状态下， 按PBUTN 0.5s, 将退出掉电状态。
在运行状态时，串口终端会输出如下信息, 并且能回显输入字符， 进入掉电状态后，串口不会回显输入字符：
```console
Please press PBUTN 16s to enter power down mode, then press PBUTN 0.5s to wake up from power down mode.
```
