# ADC16

## 概述

本示例展示ADC16在四种工作模式的转换及结果显示

## 硬件设置

在指定的管脚输入电压 (请参考{ref}`引脚描述 <lab_board_resource>`部分)

## 运行现象

- 串口终端显示如下信息

```
This is an ADC16 demo:
1. Oneshot    mode
2. Period     mode
3. Sequence   mode
4. Preemption mode
Please enter one of ADC conversion modes above (e.g. 1 or 2 ...):
```

- 选择ADC转换模式，启动ADC转换，并观察转换结果

  - Oneshot 模式

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 1
  Oneshot Mode - ADC3 [channel 2] - Result: 0xffe8
  Oneshot Mode - ADC3 [channel 2] - Result: 0xfff4
  Oneshot Mode - ADC3 [channel 2] - Result: 0xffff
  ```

  - Period 模式

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
  Period Mode - ADC3 [channel 2] - Result: 0xfff3
  Period Mode - ADC3 [channel 2] - Result: 0xfff5
  Period Mode - ADC3 [channel 2] - Result: 0xfff7
  ```

  - Sequence 模式

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
  Sequence Mode - ADC3 - Sequence Number:00 Cycle Bit: 01 ADC Channel: 02 Result: 0xffff
  Sequence Mode - ADC3 - Sequence Number:00 Cycle Bit: 00 ADC Channel: 02 Result: 0xffff
  Sequence Mode - ADC3 - Sequence Number:00 Cycle Bit: 01 ADC Channel: 02 Result: 0xfff2
  ```

  - Preemption 模式

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
  Preemption Mode - ADC3 - Cycle Bit: 01 ADC Channel: 02 Trig Index: 00 Trig Channel: 00 Result: 0xffff
  Preemption Mode - ADC3 - Cycle Bit: 01 ADC Channel: 02 Trig Index: 00 Trig Channel: 00 Result: 0xfffa
  Preemption Mode - ADC3 - Cycle Bit: 01 ADC Channel: 02 Trig Index: 00 Trig Channel: 00 Result: 0xffff
  ```