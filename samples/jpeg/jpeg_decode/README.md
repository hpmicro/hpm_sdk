# JPEG decode
## Overview
***
JPEG decoding example project demonstrates how to decode a JPG file using either JPEG module or software jpeg library

## Board Setting
***
- Connect the **USB disk** to the development board ``usb0``, or insert the **SD memory card** into the development board ``SD card``
- Connect **LCD screen** to development board

## Configuration
***
- File [CMakelists.txt](./CMakeLists.txt).
  - ``set (JPEG_USE_SDCARD 1)`` to read jpeg file (.jpg) from SD card or ``set (JPEG_USE_UDISK 1)`` to read jpeg file (.jpg) from u-disk. These options are exclusive.
  - ``set (JPEG_HW_MODE 1)`` to decode jpeg file with JPEG module; ``set (JPEG_HW_MODE 0)`` to decode with software library

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
- jpeg file size is expected to be less than 500KB, image resolution is expected to be less than 1024 * 768, defined in common/inc/file_op.h

## Running the example
***
- Configure storage in CMakeLists.txt
- Put jpeg files (with .jpg) into target storage
- Connect LCD pannel and storage to board
- Power on the board
- Download built elf into development board and run the demo
- If everything goes well, jpeg files stored in the target storage will be show on LCD pannel one by one.

