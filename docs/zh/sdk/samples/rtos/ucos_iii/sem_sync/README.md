# uC/OS-III 信号量用作同步例程

## 概述

该工程演示了使用uC/OS-III信号量用作任务间同步。

## 硬件设置

无特殊设置

## 运行现象

例程构建了两个任务，使用OS_SEM信号量来实现同步的功能。
float task会阻塞，直到led task释放信号量。
当工程正确运行后，串口终端会输出如下信息：
```console
led0 task sfloat task weait Mutex ait Mutex SeSem tm to Sync.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
```
