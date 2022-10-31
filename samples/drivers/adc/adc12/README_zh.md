# ADC12

## 概述

本示例展示ADC12在四种工作模式的转换及结果显示

## 硬件设置

在指定的管脚输入电压 (请参考{ref}`引脚描述 <lab_board_resource>`部分)

## 运行现象

- 串口终端显示如下信息

```
This is an ADC12 demo:
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
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  Oneshot Mode - ADC0 [channel 11] - Result: 0x0ffb
  ```

  - Period 模式

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
  Period Mode - ADC0 [channel 11] - Result: 0xffb0
  Period Mode - ADC0 [channel 11] - Result: 0xfff0
  Period Mode - ADC0 [channel 11] - Result: 0xffd0
  ```

  - Sequence 模式

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
  Sequence Mode - ADC0 - Sequence Number:00 Cycle Bit: 00 ADC Channel: 00 Result: 0x0000
  Sequence Mode - ADC0 - Sequence Number:00 Cycle Bit: 01 ADC Channel: 11 Result: 0x0fe3
  Sequence Mode - ADC0 - Sequence Number:00 Cycle Bit: 00 ADC Channel: 11 Result: 0x0fe7
  ```

  - Preemption 模式

  ```
  Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
  Preemption Mode - ADC0 - Cycle Bit: 01 ADC Channel: 11 Trig Index: 00 Trig Channel: 00 Result: 0x0ffb
  Preemption Mode - ADC0 - Cycle Bit: 01 ADC Channel: 11 Trig Index: 00 Trig Channel: 00 Result: 0x0ffd
  Preemption Mode - ADC0 - Cycle Bit: 01 ADC Channel: 11 Trig Index: 00 Trig Channel: 00 Result: 0x0ffc
  ```
