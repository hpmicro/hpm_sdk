# 锁相环控制器PLLCTLV2
## 概述
***
**pllctl**锁相环控制器PLLCTL驱动示例工程展示了通过锁相环控制器，在不同模式下，对系统时钟源进行分频系数配置，达到稳定系统主频及分频或倍频的目的
- 示例代码中，通过配置参考时钟分频系数、反馈分频系数、后分频系数等，实现PLL控制器实现逻辑
- 通过配置PLL实现PLL设置
- 设置系统频率从**400MHz到1GHz**，展示实际与预期的主频偏差数值

## 硬件设置
***
-  无特殊设置

## 运行现象
***
当工程正确运行后，串口终端会输出如下信息：
```shell
PLLCTLV2 example
Now configure PLL at integer mode
Update PLL2 to 400000000Hz
Actual PLL frequency is 400000000Hz
Update PLL2 to 401000000Hz
Actual PLL frequency is 401000000Hz
Update PLL2 to 402000000Hz
Actual PLL frequency is 402000000Hz
Update PLL2 to 403000000Hz
Actual PLL frequency is 403000000Hz
Update PLL2 to 404000000Hz
Actual PLL frequency is 404000000Hz
Update PLL2 to 405000000Hz
Actual PLL frequency is 405000000Hz
Update PLL2 to 406000000Hz
Actual PLL frequency is 406000000Hz
Update PLL2 to 407000000Hz
...
Update PLL2 to 998000000Hz
Actual PLL frequency is 998000000Hz
Update PLL2 to 999000000Hz
Actual PLL frequency is 999000000Hz
Update PLL2 to 1000000000Hz
Actual PLL frequency is 1000000000Hz
PLL2 ss has been enabled

PLLCTLV2 example finished

```
