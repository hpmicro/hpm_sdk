# CAM
## Overview

The cam example shows how to use cam interface to get the image collected by the camera device and display in LCD.

## Board Setting

1. Connect camera device to the CAM interface on the board
2. Connect LCD to the LCD interface on the board

## Project Setting

For this example, user should set used camera device module in CMakeList.txt
1. if use ov7725 camera device, configure `set(CONFIG_CAMERA "ov7725")` in CMakeList.txt
2. if use ov5640 camera device, configure `set(CONFIG_CAMERA "ov5640")` in CMakeList.txt

## Note

When using ov5640 camera, the ram and flash_xip target is not supported since ov5640 `framebuffer` is too large， please use flash_sdram_xip target.

## Running the example

When the example runs successfully, LCD will display the camera device collected image and the following message is displayed in the terminal:
```
> cam example
```

