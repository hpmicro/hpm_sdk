.. _host_dual_port:

Host Dual Port
============================

Overview
--------

This example project shows two USB ports as Host. Which support HID class(Keyboard and Mouse) and MSC class(Attention: The `Format Type` support FAT, FAT32 or exFAT).

  .. image:: ../../doc/UDisk_Format.png
     :alt: UDisk_Format

Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on the development board USB0 with a USB Type-C convert Type-A cable

- Connect a USB port on the development board USB1 with a USB Type-C convert Type-A cable

Software Setting
-----------------------

- In the CMakeLists.txt file, if sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1) is uncommented, FATFS supports data transfer by using cacheable buffer. If sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1) is commented, FATFS only supports data transfer by using noncacheable buffer.
- When sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1) is uncommented, it is recommended to align cache line size for the buffer address, otherwise it will affect transmission performance.

Running the example
-------------------

- Download the program to the development board to run, and use the serial port debugging assistant to view the output log.

- When the USB0 or UBS1 of the development board is connected with a USB flash disk

    - First, a "cherryusb _msc_test.txt" file will be created in the USB flash drive, and the string "cherryusb fatfs demo..." will be written into the file, with a total of 100 lines or 2500 bytes; Then read out the data of the file, with a total length of 2500.

    - Secondly, it will scan the directories and files under the root directory of the USB drive and print out the directory name and file name.

    - the serial port debugging assistant will display the following data:

    .. code-block:: console

        FATFS cherryusb mount succeeded!
        test fatfs write
        write success, write len：2500
        test fatfs read
        read success, read len：2500

        Direcotry Path: /
        Folder Name: LOST.DIR
        Folder Name: DCIM
        Folder Name: MIUI
        Folder Name: Android
        Folder Name: tencent
        File   Name: USBHost.txt
        File   Name: cherryusb_msc_test.txt


- When the USB0 or UBS1 of the development board is connected with keyboard

    - The serial port debugging assistant will display the following data:

    .. code-block:: console

        [D/USB] Port change:0x02
        [D/USB] Port 1 change
        [D/USB] port 1, status:0x101, change:0x01
        [D/USB] Port 1, status:0x101, change:0x00
        [D/USB] Port 1, status:0x101, change:0x00
        [D/USB] Port 1, status:0x101, change:0x00
        [D/USB] Port 1, status:0x101, change:0x00
        [D/USB] Port 1, status:0x101, change:0x00
        [I/USB] New low-speed device on Hub 1, Port 1 connected
        [I/USB] New device found,idVendor:413c,idProduct:2113,bcdDevice:0108
        [I/USB] The device has 2 interfaces
        [I/USB] Enumeration success, start loading class driver
        [I/USB] Loading hid class driver
        [I/USB] Ep=81 Attr=03 Mps=8 Interval=24 Mult=00
        [I/USB] Register HID Class:/dev/input0
        [E/USB] do not support Class:0x03,Subclass:0x00,Protocl:0x00
        mount a keyboard


    - When the keyboard is pressed, the data sent by the keyboard will be displayed:

    .. code-block:: console

        0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x16 0x07 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x07 0x00 0x00 0x00 0x00 0x00 nbytes:8
        0x00 0x00 0x16 0x00 0x00 0x00 0x00 0x00 nbytes:8


- When the USB0 or UBS1 of the development board is connected with mouse

    - The serial port debugging assistant will display the following data:

    .. code-block:: console

        [D/USB] Port change:0x02
        [D/USB] Port 1 change
        [D/USB] port 1, status:0x100, change:0x01
        [D/USB] Port 1, status:0x101, change:0x00
        [D/USB] Port 1, status:0x101, change:0x00
        [D/USB] Port 1, status:0x101, change:0x00
        [D/USB] Port 1, status:0x101, change:0x00
        [D/USB] Port 1, status:0x101, change:0x00
        [I/USB] New full-speed device on Hub 1, Port 1 connected
        [I/USB] New device found,idVendor:046d,idProduct:c542,bcdDevice:0302
        [I/USB] The device has 1 interfaces
        [I/USB] Enumeration success, start loading class driver
        [I/USB] Loading hid class driver
        [I/USB] Ep=82 Attr=03 Mps=8 Interval=04 Mult=00
        [I/USB] Register HID Class:/dev/input0
        mount a mouse

    - When moving or clicking the mouse, the data sent by the mouse will be displayed:

    .. code-block:: console

        0x01 0x00 0xfa 0xff 0x06 0x00 0x00 nbytes:7
        0x01 0x00 0xf8 0xff 0x07 0x00 0x00 nbytes:7
        0x01 0x00 0xf7 0xff 0x07 0x00 0x00 nbytes:7
        0x01 0x00 0xf5 0xff 0x09 0x00 0x00 nbytes:7
        0x01 0x00 0xf4 0xff 0x08 0x00 0x00 nbytes:7

