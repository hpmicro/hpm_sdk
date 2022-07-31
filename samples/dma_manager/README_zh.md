# DMA Manager
## 概述

DMA Manager 展示了如下功能。
- 申请/释放DMA资源
- 使用相应DMA资源的中断
- 安装DMA资源的中断回调函数

## DMA Manager使用提示

- DMA Manager组件内部已经内置了中断处理函数，用户程序中不应当再声明中断处理函数

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
``` shell
 ============================================================================


    This example demonstrates the following features with DMA manager

      1. Request/release DMA resource
      2. Enable DMA interrupt
      3. DMA callback installation


=============================================================================
DMA resource 0 completed transfer, no errors
DMA resource 1 completed transfer, no errors
DMA resource 2 completed transfer, no errors
DMA resource 3 completed transfer, no errors
DMA resource 4 completed transfer, no errors
DMA resource 5 completed transfer, no errors
DMA resource 6 completed transfer, no errors
DMA resource 7 completed transfer, no errors
DMA resource 8 completed transfer, no errors
DMA resource 9 completed transfer, no errors
DMA resource 10 completed transfer, no errors
DMA resource 11 completed transfer, no errors
DMA resource 12 completed transfer, no errors
DMA resource 13 completed transfer, no errors
DMA resource 14 completed transfer, no errors
DMA resource 15 completed transfer, no errors
DMA resource 0 completed transfer, no errors
DMA resource 1 completed transfer, no errors
DMA resource 2 completed transfer, no errors
DMA resource 3 completed transfer, no errors
DMA resource 4 completed transfer, no errors
DMA resource 5 completed transfer, no errors
DMA resource 6 completed transfer, no errors
DMA resource 7 completed transfer, no errors
DMA resource 8 completed transfer, no errors
DMA resource 9 completed transfer, no errors
DMA resource 10 completed transfer, no errors
DMA resource 11 completed transfer, no errors
DMA resource 12 completed transfer, no errors
DMA resource 13 completed transfer, no errors
DMA resource 14 completed transfer, no errors
DMA resource 15 completed transfer, no errors
DMA Manager test PASSED

```
