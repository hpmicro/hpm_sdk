.. _hpm_jpeg:

HPM_JPEG
================

Overview
--------

The HPM_JPEG sample performs encoding and decoding through the HPM_JPEG component. After decoding, the file is displayed on the screen, and the decoded image is re-encoded and stored.

Hardware Setup
--------------

- **USB Disk**  is connected to the development board's ``USB0`` port (Attention: The `Cluster Size` of the USB disk can't exceed 32KB, it is recommended that the format parameter is FAT32/32KB), or the **SD card**  is inserted into the development board's ``sd card``  slot.

  .. image:: ../common/doc/UDisk_Format.png
     :alt: UDisk_Format

- **LCD screen**  is connected to the development board.

Project Configuration
---------------------

- File `CMakeLists.txt`:

  - ``set (JPEG_USE_SDCARD 1)`` : Read the test.jpg file from the SD card;

  - ``set (JPEG_USE_UDISK 1)`` : Read the test.jpg file from the USB drive. These two options are mutually exclusive.

Running Behavior
----------------

- Modify the `CMakeLists.txt` file as needed to configure the target storage/decoding method.

- Copy the test.jpg file to the root directory of the target storage.

- Connect the LCD screen and storage medium to the development board.

- Power on the development board.

- Download the compiled ELF file to the development board.

- If everything is set up correctly, the test.jpg file from the storage medium will be displayed on the screen, aligned with the top-left corner. The decoded image will not be scaled; if the image size exceeds the screen size, only the top-left portion will be shown. If the decoded image is smaller than the screen, the remaining areas will be filled with black pixels to highlight the image boundary.

- After execution, the decoded image will be re-encoded into a test_yuv422.jpg with a 90% compression ratio and a test_yuv420.jpg with a 40% compression ratio, and saved to the root directory of the target storage.

- If the codec fails because the encoded image is too large, increase the HEAP_SIZE

.. code-block:: console

    SD card has been mounted successfully
    Read picture data by SD card
    djob0 callback hw_status: 0x8000001c
    decode: width: 640
    decode: height: 480
    decode: stride: 1280
    ejob0 callback hw_status: 0x1c
    ejob1 callback hw_status: 0x1c
    save 36682 bytes to /test_yuv422.jpg finished
    save 10736 bytes to /test_yuv420.jpg finished
    hpm_jpeg sample end!
