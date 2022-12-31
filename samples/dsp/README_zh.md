# DSP
## 概述

dsp_demo示例工程演示了DSP CFFT和CIFFT计算，通过串口打印结果。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：

```
--------------------
after CFFT_RD2, maxdiff= 0x0080 [0.00390816]
--------------------
MAE is 0.00079375, RMSD is 0.00176055, NRMSD is 0.00005502, MAXDIFF is 0.00390816, SNR is 67.12823486
CFFT_RD2 out scaleup by 64
after CIFFT_RD2, maxdiff= 0x0009 [0.00029564]
--------------------
MAE is 0.00012192, RMSD is 0.00014900, NRMSD is 0.00007450, MAXDIFF is 0.00029564, SNR is 73.52618408
CIFFT_RD2 out scaleup by 2
CIFFT_RD2 PASS
```