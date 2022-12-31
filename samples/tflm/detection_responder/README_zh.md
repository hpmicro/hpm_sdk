# TensorFlowLiteMicro-PersonDetection
## 概述

TFLM示例工程展示了使用摄像头采集图像信息，通过集成的TFLM机器学习平台进行人体检测，并通过屏幕显示结果

## 配置

- 连接摄像头设备到开发板的CAM接口
- 连接LCD到开发板的LCD接口
- 连接显示屏到相应接口，显示屏型号[LCD-800480W070TC](lab_lcd_800480w070tc)

```{warning}

由于空间问题，请使用`flash_sdram_xip`选项进行编译

```
## 代码选项

- 如果需要更快的代码运行速度请在该工程的CMakeLists.txt中添加`sdk_compile_options("-mabi=ilp32f") sdk_compile_options("-march=rv32gc") set(CONFIG_HPM_MATH_NN_RVP32 1)`语句，获得较高的性能提升

## 运行现象

当工程正确运行后，LCD会显示摄像头采集的实时画面，以及处理后的图像和识别结果，而串口终端会输出是否有人的得分

```
person score:-23 no person score 23
```
