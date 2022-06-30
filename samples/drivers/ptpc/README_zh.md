# PTPC
## 概述
***
PTPC示例程序演示了时间戳比较与秒脉冲中断

## 硬件设置
***
无需特殊设置

## 运行现象
***
当工程正确运行后，板载LED会周期性闪烁，同时串口终端会输出如下信息：
```
> ptpc example
> Config target timestamp: 3 s, 80000 ns
> Got event at 3 s, 81180 ns
> timer_compare done
> Config pps IRQ frequency: 1 Hz
> pps_interrupt done
```
