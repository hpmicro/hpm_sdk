# Filter Output
## 概述

演示了将PLB配置成滤波器，支持滤刺模式、延时模式、滤峰模式和滤谷模式。

## 硬件设置

- 输入信号和输出信号参见[PLB Filter引脚](lab_board_app_plb_filter_pins)
- 对输入信号输入高低电平（接3.3V为高电平，断开为低电平），使用示波器或逻辑分析仪观察输入信号和滤波后的信号。

## 概要设计

- 请参考[PLB Filter组件readme](../../../../components/plb/filter/doc/README_zh.md)

## 运行程序

- 编译下载程序，运行程序后串口调试台打印如下信息：
``` console
PLB Filiter Demo.

Please Select Filter Mode:
  0 - Filter rapid change mode
  1 - Filter delay mode
  2 - Filter stable low mode
  3 - Filter stable high mode
```

- 请输入0-3，选择滤波模式

- 对输入信号输入高低电平（接3.3V为高电平，断开为低电平），使用示波器或逻辑分析仪观察输入信号和滤波后的信号。
