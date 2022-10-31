# JPEG encode
## Overview
***
JPEG decoding example project demonstrates how to encode an image capture from camera sensor to a JPG file using either JPEG module or software jpeg library

## Board Setting
***
- Connect the **USB disk** to the development board ``usb0``, or insert the **SD memory card** into the development board ``SD card``
- Connect **LCD screen** and **Cam camera** to development board

## Engineering configuration
***
- File [CMakeLists.txt](./CMakeLists.txt). If ``set (JPEG_USE_SDCARD 1)`` is configured, jpeg files are stored in SD card or ``set (JPEG_USE_UDISK 1)``, they will be saved to udisk.

## Store picture size
***
- The image format stored in this routine is JPG, and the resolution size is 640*480

## Running the example
***
- Configure storage in CMakeLists.txt
- Connect camera senor, LCD pannel and storage to board
- Power on the board
- Download built elf into development board and run the demo
- If everything goes well:
  - camera sensor preview will be shown on LCD panel
  - press GPIO button (please check [Button](lab_board_resource) section of specific board) will save current frame and encode to jpeg file and save to target storage
  - encoded image will be decoded and shown on LCD pannel until GPIO button is pressed again. Once it's been pressed, preview will be resumed.

