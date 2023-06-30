# uC/OS-III 时间片调度

## 概述

该工程演示了uC/OS-III的时间片调度功能。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，float任务会打印运行次数和5个数字，led0任务会打印运行次数和"LED : 12345"。
这两个任务被赋予了同等的优先级。当没有时间片调度时，应当只有其中一个任务会运行。由于时间片调度的存在，同等优先级的任务可以被调度。
因为打印前进入了临界段，运行次数的消息会被完整地打印到终端中。其他消息有可能被随机的打断。

串口终端会输出如下信息：

```console
led0_task run 0 times
LED : 1234float run 0 times
5 float : 6789
0
LED : 12float : 345
67890
LED floa: 12345
t : 67890
LED : 12345 float : 678
90
LED : 1234float :5
 67890
led0_task run 1 times
LED :float run 1 times
 12float : 6789345
0
LED float : : 12345
67890
floLED : 12345 at : 67890

LED : 123float : 6745
890
LED : float 12345
: 67890
led0_task run 2 times
float run 2 times
LED float : 6789: 12345
0
float :LED : 12345 67890

flLED : 123oat : 67890 45

LED :float : 67 12345
890
float LED : 12345 : 67890
```
