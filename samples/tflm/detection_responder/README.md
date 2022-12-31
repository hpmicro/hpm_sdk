# TensorFlowLiteMicro-PersonDetection
## Overview

The TFLM example demos the use of a camera to capture an image, perform human detection via the integrated TFLM machine learning platform, and display the results on screen

## Configurations

- Connect the camera device to the CAM interface on the board
- Connect LCD to the LCD interface on the board
- Click [DRV-LV50A-MP1907 Motor Driver Board ](lab_drv_lv50a_mp1907) section and configure

## Code Options

- Add `sdk_compile_options("-mabi=ilp32f") sdk_compile_options("-march=rv32gc") set(CONFIG_HPM_MATH_NN_RVP32 1)` to the project's CMakeLists.txt to get a higher performance

```{warning}

Please use `flash_sdram_xip` option for compilation due

```

## Running the example

When the example runs successfully, The LCD will display the live screen, the processed image and the recognition result, while the serial terminal will output the score of whether someone is present or not

```
person score:-23 no person score 23
```
