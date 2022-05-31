# BUTN
## 概述

BUTN工程演示了如何使用PBUTN和WBUTN按键进入和退出掉电状态。

##注意

当进入掉电模式时， RAM中的代码会丢失，为了演示这个工程，不能将代码存放在RAM中。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，按PBUTN 16s, 进入掉电模式。 在掉电状态下， 按PBUTN或PBUTN 0.5s, 将退出掉电状态。
按键按下的时间可以通过PLED和WLED观察：
1. 按键输入高电平（按键未按下）时，输出为开漏高电平，LED 关闭
2. 按键输入低电平保持0.5 秒后（一次有效按键），LED 开始快速闪耀
3. 按键输入低电平约4 秒后（一次有效长按键），LED 变为慢速闪耀
4. 按键输入低电平约16 秒后（一次有效超长按键），LED 再次关闭

串口终端会输出如下信息, 并且回显输入的数据：
```
> Please press PBUTN 16s to enter power down mode, then press PBUTN or WBUTN 0.5s to wake up from power down mode.
```
