# DAC

## 概述

本示例展示DAC在三种工作模式的转换及波形显示

## 硬件设置

在指定的管脚输出电压 (请参考{ref}`引脚描述 <lab_board_resource>`部分)

## 运行现象

- 串口终端显示如下信息

```
This is a DAC demo:
1. Direct mode
2. Step   mode
3. Buffer mode
Please enter the DAC mode code:
```

- 选择DAC转换模式，启动DAC转换，并通过示波器观察波形

  - Direct 模式

  ```
  Please enter the DAC mode code: 1
  Set DAC to output data in direct mode
  DAC is outputting a triangle waveform in direct mode
  ```

  ![](../../../doc/images/samples/DAC_Direct_Mode.png)

  - Step 模式

  ```
  Please enter the DAC mode code: 2
  Set DAC to output data in step mode
  DAC is outputting a saw tooth waveform in step mode
  ```

  ![](../../../doc/images/samples/DAC_Step_Mode.png)

  - Buffer 模式

  ```
  Please enter the DAC mode code: 3
  Set DAC to output data in buffer mode
  DAC is outputting a sine waveform in buffer mode
  ```

  ![](../../../doc/images/samples/DAC_Buffer_Mode.png)