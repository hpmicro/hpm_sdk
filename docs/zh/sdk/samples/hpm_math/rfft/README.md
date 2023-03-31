# RFFT 性能测试
## 概述

rfft性能测试主要测试了DSP进行rfft变换的性能

## 程序设计

软件生成生成几个幅值和相位不同的正弦波并叠加生成一个波形，然后使用rfft变换求出幅频特性数据，几个正弦波如下所示:

- 幅值11，频率8
- 幅值19，频率17
- 幅值13，频率24

## 硬件设置

无特殊设置

## 性能优化

- segger已经默认开启O3优化

## 运行现象

当工程正确运行后，串口终端会输出采样点数，以及幅频特性的数据，以及每一个单元的运行时间，并在最后输出PASS或者ERROR NUM。

串口输出如下所示：

```console

Rfft demo: 64.
total times:5133 tick.
------------------------------------
DC component amplitude:5.000004.
Frequency:8.000000, Amplitude:11.000010.
Frequency:17.000000, Amplitude:18.999992.
Frequency:24.000000, Amplitude:13.000002.
------------------------------------
Rfft demo: 128.
total times:7956 tick.
------------------------------------
DC component amplitude:4.999997.
Frequency:8.000000, Amplitude:11.000002.
Frequency:17.000000, Amplitude:18.999996.
Frequency:24.000000, Amplitude:13.000004.
------------------------------------
Rfft demo: 256.
total times:25266 tick.
------------------------------------
DC component amplitude:4.999996.
Frequency:8.000000, Amplitude:11.000001.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:13.000004.
------------------------------------
Rfft demo: 512.
total times:37868 tick.
------------------------------------
DC component amplitude:4.999998.
Frequency:8.000000, Amplitude:11.000000.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:13.000000.
------------------------------------
Rfft demo: 1024.
total times:122305 tick.
------------------------------------
DC component amplitude:4.999998.
Frequency:8.000000, Amplitude:11.000000.
Frequency:17.000000, Amplitude:19.000000.
Frequency:24.000000, Amplitude:13.000000.
------------------------------------
PASS.
----------------END--------------------

```
性能如下表所示：

| 采样点              | 64    | 128   | 256   | 512    | 1024   |
|------------------|-------|-------|-------|--------|--------|
| DSP rfft          | 5133  | 7956    | 25266 | 37868      | 122305 |

