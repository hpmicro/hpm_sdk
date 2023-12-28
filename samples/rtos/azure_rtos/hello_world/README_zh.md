# ThreadX Hello

## 概述

该工程演示了threadx的任务调度功能。本例程创建了8个线程。线程0周期性的打印状态消息到终端上，并且唤醒线程5。线程1发送数据到消息队列上，线程2从消息队列上读取消息。线程3和4试图获得同一个信号量，并持有一段时间。线程6和7试图获得同一个互斥量，并持有一段时间。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会周期性输出线程的运行状态统计信息：
```console
ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro

           thread 0 events sent:          2
           thread 1 messages sent:        210731
           thread 2 messages received:    210635
           thread 3 obtained semaphore:   3
           thread 4 obtained semaphore:   3
           thread 5 events received:      1
           thread 6 mutex obtained:       3
           thread 7 mutex obtained:       3


ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro

           thread 0 events sent:          3
           thread 1 messages sent:        454838
           thread 2 messages received:    454818
           thread 3 obtained semaphore:   6
           thread 4 obtained semaphore:   5
           thread 5 events received:      2
           thread 6 mutex obtained:       6
           thread 7 mutex obtained:       5
```
