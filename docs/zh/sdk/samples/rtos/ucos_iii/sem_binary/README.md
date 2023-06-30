# uC/OS-III 信号量用作互斥量例程

## 概述

该工程演示了将uC/OS-III信号量作为互斥量的使用。

## 硬件设置

无特殊设置

## 运行现象
本例程中使用OS_SEM来保护两个任务间共享的资源。使用mutex代替OS_SEM会有相同的效果。
本例程中led0 task应当输出"led0 task write", float task应当输出"float task write"。
当工程正确运行后，串口终端会输出如下信息：

```console
led0 task use share_data, prio : 4
led0 task : led0 task write.
float task use share_data, prio : 4
float task : float task write.
led0 task use share_data, prio : 4
led0 task : led0 task write.
float task use share_data, prio : 4
float task : float task write.
led0 task use share_data, prio : 4
led0 task : led0 task write.
float task use share_data, prio : 4
float task : float task write.
led0 task use share_data, prio : 4
led0 task : led0 task write.
float task use share_data, prio : 4
```
