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

## Supported picture formats
***
- Supports standard JPEG images：
  JPEG picture format components：SOI（File header）+APP0（Image identification information）+ DQT（Define quantification table）+ SOF0（Basic image information）+ DHT（Define Huffman table） + DRI（Define restart interval）+ SOS（Scan line start）+ EOI（End of document），the SOI is composed of two hexadecimal bytes (ffd8)。
- The sampling factor supported by the JPEG picture is：YUV420、YUV422H、YUV422V、YUV444、YUV400，Such as a JPG image in 800 * 400 YUV420 format，Through the worm EXIF viewer[Graphic insect EXIF viewer official website](https://exif.tuchong.com//)available：
    FileType	        JPEG
    FileTypeExtension	jpg
    MIMEType	        image/jpeg
    ImageWidth	        800
    ImageHeight	        480
    EncodingProcess	    Baseline DCT, Huffman coding
    BitsPerSample	    8
    ColorComponents	    3
    YCbCrSubSampling  **YCbCr4:2:0 (2 2)**

## Image size and resolution supported by this routine
***
- JPG image is no more than 10M(hpm6750evk development board) / 4M(hpm6750evkmini development board)
- JPG image resolution (width * height) is not greater than 8M (hpm6750evk development board) / 3.5M (hpm6750evkmini development board)

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

