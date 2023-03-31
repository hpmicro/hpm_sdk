# OTP BLOW PLAN

## Overview

This demo provides the following features:
- Program the necessary OTP words to blank chip

## Board Setting

- The BOOT PIN must be set to 2'b10


## Run the project

- Generate the bootable image by the img_util, steps：
    - ./img_util.exe -- gen_image <app_binary_path> <bootable_image_file> 0x90 0 0 0

- Load the image to board via:
  ./rom_host.exe -p <COMx> -- load-image <bootable_image_file>


- Boot ROM will run the application after the "load-image", open the console port, users will see the OTP blown result from the console.
