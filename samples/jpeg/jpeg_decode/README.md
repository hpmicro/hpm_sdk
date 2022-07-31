# JPEG_DECODE decode
## Overview
***
**libjpeg_turbo_decode** decoding example project shows to convert JPG picture data(Default scan storage device root directory) into RGB data.
- JPEG images stored in **USB** or **SD card** are converted into
rgb565 data through **software libjpeg_turbo library** or **hardware JPEG module**, the decoded data is displayed on **the LCD screen**.

## Board Setting
***
- Connect the **USB disk** to the development board ``usb0``, or insert the **SD memory card** into the development board ``SD card``, and store JPG pictures in the storage medium. The example pictures can be used under the JPEG path : **example.jpg**
- Connect **LCD screen** to development board ``lcd1``

## Engineering configuration
***
- File [cmakelists.txt](./CmakeLists.txt). If ``set (config_sdfatfs_mode 1)`` is configured, JPG pictures are read from the **SD card** by default; to configure ``Set (config_tinyusbhost_mode 1)``, JPG pictures are read from **USB** by default
- - File [cmakelists.txt](./CmakeLists.txt). If ``set (CONFIG_JPEG_TURBO_MODE 1)`` is configured, the decoding method is through **software libjpeg_turbo library** complete data decoding by default; to configure ``Set (CONFIG_JPEG_HARDWARE_MODE 1)``, the decoding method is through **hardware JPEG module** complete data decoding  by default

## Running the example
***
- SD card mode (Store sample pictures : example.jpg) & software libjpeg_turbo library decoding mode:
When the project runs correctly, the LCD screen correctly displays the picture information in the storage device, and the serial port terminal will output the following information:
```
> Read picture data by SD card
> Libjpeg-turbo decode completed
```

- USB mode (Store sample pictures : example.jpg) & software libjpeg_turbo library decoding mode:
When the project runs correctly, the LCD screen correctly displays the picture information in the storage device, and the serial port terminal will output the following information:
```
> Read picture data by usb-mode
> Libjpeg-turbo decode completed
```

- SD card mode (Store sample pictures : example.jpg) & hardware JPEG module decoding mode:
When the project runs correctly, the LCD screen correctly displays the picture information in the storage device, and the serial port terminal will output the following information:
```
> Read picture data by SD card
> jpeg-hardware decode completed
```

- USB mode (Store sample pictures : example.jpg) & hardware JPEG module decoding mode:
When the project runs correctly, the LCD screen correctly displays the picture information in the storage device, and the serial port terminal will output the following information:
```
> Read picture data by usb-mode
> jpeg-hardware decode completed
```

