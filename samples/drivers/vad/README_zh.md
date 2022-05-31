# VAD
## 概述

该工程演示了使用VAD记录声音并通过DAO播放的功能。

## 硬件设置

连接扬声器到板上DAO接口。

## 运行现象

当工程正确运行后，按照log提示发出声音，声音会被记录并播放出来。串口终端会输出如下信息：
```
> vad example
> vad calibration done
> Recording...
> Please make some sound to mic0
> Got something...
> Recorded 204800 bytes data
> Now, playing...
```
