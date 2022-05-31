# Core Mark
## 概述

coremark示例工程展示了运行coremark测试程序的情况。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出coremark测试结果：
```

> 2K performance run parameters for coremark.
> CoreMark Size    : 666
> Total ticks      : 363094999
> Total time (secs): 15.128958
> Iterations/Sec   : 3965.904251
> Iterations       : 60000
> Compiler version : GCC10.2.1 20200726
> Compiler flags   : -Wall -Wno-format -fomit-frame-pointer -fno-builtin -ffunction-sections -fdata-sections -g -O3 -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting -mstrict-align
> Memory location  : STACK
> seedcrc          : 0xe9f5
> [0]crclist       : 0xe714
> [0]crcmatrix     : 0x1fd7
> [0]crcstate      : 0x8e3a
> [0]crcfinal      : 0xbd59
> Correct operation validated. See README.md for run and reporting rules.
> CoreMark 1.0 : 3965.904251 / GCC10.2.1 20200726 -Wall -Wno-format -fomit-frame-pointer -fno-builtin -ffunction-sections -fdata-sections -g -O3 -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting -mstrict-align / STACK

```