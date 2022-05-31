# 锁相环控制器PLLCTL
## 概述
***
**pllctl**锁相环控制器PLLCTL驱动示例工程展示了通过锁相环控制器，在不同模式下，对系统时钟源进行分频系数配置，达到稳定系统主频及分频或倍频的目的
- 示例代码中，通过配置参考时钟分频系数、反馈分频系数、后分频系数等，实现PLL控制器实现逻辑
- 分别通过配置PLL两种工作模式，即**整数模式**和**小数模式**实现PLL设置
- 设置系统频率从**380000000hz到840000000hz**，展示实际与预期的主频偏差数值

## 硬件设置
***
-  无特殊设置

## 运行现象
***
当工程正确运行后，串口终端会输出如下信息：
```
> PLLCTL example
> Now configure PLL at integer mode
> Update PLL0 to 380000000Hz @integer mode
> Actual f = 372000000Hz
> Update PLL0 to 400000000Hz @integer mode
> Actual f = 396000000Hz
> Update PLL0 to 420000000Hz @integer mode
> Actual f = 420000000Hz
> Update PLL0 to 440000000Hz @integer mode
> Actual f = 432000000Hz
> Update PLL0 to 460000000Hz @integer mode
> Actual f = 456000000Hz
> Update PLL0 to 480000000Hz @integer mode
> Actual f = 480000000Hz
> Update PLL0 to 500000000Hz @integer mode
> Actual f = 492000000Hz
> Update PLL0 to 520000000Hz @integer mode
> Actual f = 516000000Hz
> Update PLL0 to 540000000Hz @integer mode
> Actual f = 540000000Hz
> Update PLL0 to 560000000Hz @integer mode
> Actual f = 552000000Hz
> Update PLL0 to 580000000Hz @integer mode
> Actual f = 576000000Hz
> Update PLL0 to 600000000Hz @integer mode
> Actual f = 600000000Hz
> Update PLL0 to 620000000Hz @integer mode
> Actual f = 612000000Hz
> Update PLL0 to 640000000Hz @integer mode
> Actual f = 636000000Hz
> Update PLL0 to 660000000Hz @integer mode
> Actual f = 660000000Hz
> Update PLL0 to 680000000Hz @integer mode
> Actual f = 672000000Hz
> Update PLL0 to 700000000Hz @integer mode
> Actual f = 696000000Hz
> Update PLL0 to 720000000Hz @integer mode
> Actual f = 720000000Hz
> Update PLL0 to 740000000Hz @integer mode
> Actual f = 732000000Hz
> Update PLL0 to 760000000Hz @integer mode
> Actual f = 756000000Hz
> Update PLL0 to 780000000Hz @integer mode
> Actual f = 780000000Hz
> Update PLL0 to 800000000Hz @integer mode
> Actual f = 792000000Hz
> Update PLL0 to 820000000Hz @integer mode
> Actual f = 816000000Hz
> Now configure PLL at fraction mode
> Update PLL0 to 380000000Hz @fraction mode
> Actual f = 379999999Hz
> Update PLL0 to 400000000Hz @fraction mode
> Actual f = 399999999Hz
> Update PLL0 to 420000000Hz @fraction mode
> Actual f = 420000000Hz
> Update PLL0 to 440000000Hz @fraction mode
> Actual f = 439999999Hz
> Update PLL0 to 460000000Hz @fraction mode
> Actual f = 459999999Hz
> Update PLL0 to 480000000Hz @fraction mode
> Actual f = 480000000Hz
> Update PLL0 to 500000000Hz @fraction mode
> Actual f = 499999999Hz
> Update PLL0 to 520000000Hz @fraction mode
> Actual f = 519999999Hz
> Update PLL0 to 540000000Hz @fraction mode
> Actual f = 540000000Hz
> Update PLL0 to 560000000Hz @fraction mode
> Actual f = 559999999Hz
> Update PLL0 to 580000000Hz @fraction mode
> Actual f = 579999999Hz
> Update PLL0 to 600000000Hz @fraction mode
> Actual f = 600000000Hz
> Update PLL0 to 620000000Hz @fraction mode
> Actual f = 619999999Hz
> Update PLL0 to 640000000Hz @fraction mode
> Actual f = 639999999Hz
> Update PLL0 to 660000000Hz @fraction mode
> Actual f = 660000000Hz
> Update PLL0 to 680000000Hz @fraction mode
> Actual f = 679999999Hz
> Update PLL0 to 700000000Hz @fraction mode
> Actual f = 699999999Hz
> Update PLL0 to 720000000Hz @fraction mode
> Actual f = 720000000Hz
> Update PLL0 to 740000000Hz @fraction mode
> Actual f = 739999999Hz
> Update PLL0 to 760000000Hz @fraction mode
> Actual f = 759999999Hz
> Update PLL0 to 780000000Hz @fraction mode
> Actual f = 780000000Hz
> Update PLL0 to 800000000Hz @fraction mode
> Actual f = 799999999Hz
> Update PLL0 to 820000000Hz @fraction mode
> Actual f = 819999999Hz
> PLL0 ss has been enabled
> PLLCTL example finished
```

