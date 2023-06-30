# ADC12

## 概述

本示例展示ADC12在四种工作模式的转换及结果显示

注：

* 当ADC配置为非读取转换（oneshot）模式时，不能读取ADC寄存器列表，否则会出现总线阻塞
* 当ADC配置为读取转换 （oneshot）模式时， CPU必须在ADC完成初始化以后，才可以读取ADC寄存器列表

## 硬件设置

在指定的管脚输入电压 (请参考{ref}`引脚描述 <lab_board_resource>`部分)

注： ADC管脚的输入电压范围: 0~VREFH

## 运行现象

- 串口终端显示如下信息

```console
This is an ADC12 demo:
1. Oneshot    mode
2. Period     mode
3. Sequence   mode
4. Preemption mode
Please enter one of ADC conversion modes above (e.g. 1 or 2 ...):
```

- 选择ADC转换模式，启动ADC转换，并观察转换结果

  - Oneshot 模式

  ```console
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 1
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  ```
  - Period 模式

  ```console
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
  Period Mode - ADC0 [channel 11] - Result: 0x0ffb
  Period Mode - ADC0 [channel 11] - Result: 0x0fff
  Period Mode - ADC0 [channel 11] - Result: 0x0ffd
  ```
  - Sequence 模式

  ```console
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
  Sequence Mode - ADC0 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0fff
  Sequence Mode - ADC0 - Cycle Bit: 00 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0ffb
  Sequence Mode - ADC0 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 11 - Result: 0x0ff7
  ```
  - Preemption 模式

  ```console
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
  Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9
  Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9
  Preemption Mode - ADC0 - Trigger Channel: 11 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 11 - Result: 0x0ff9
  ```
