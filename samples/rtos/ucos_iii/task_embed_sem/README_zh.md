# uC/OS-III 任务内嵌信号量例程

## 概述

该工程演示了uC/OS-III任务信号量的使用。

## 硬件设置

无特殊设置

## 运行现象
例程构建了两个任务，使用TASK_SEM信号量来实现同步的功能。
float task会阻塞，直到led task对float_task释放TASK_SEM信号。
当工程正确运行后，串口终端会输出如下信息：

```console
float task wait Task Sem .
led0 task task sem.
float task get Task Sem .
float task wait Task Sem .
led0 task task sem.
float task get Task Sem .
float task wait Task Sem .
led0 task task sem.
float task get Task Sem .
float task wait Task Sem .
led0 task task sem.
float task get Task Sem .
float task wait Task Sem .
```
