.. _usb_msc_u_disk:

USB MSC U Disk
============================

Overview
--------

This example project shows access to a U disk (Attention: The `Cluster Size` of the USB disk can't exceed 32KB, it is recommended that the format parameter is FAT32/32KB).

  .. image:: ./doc/UDisk_Format.png
     :alt: UDisk_Format

- A usb host from MCU reads out all of folders and files in the root path of a U disk, and the test result of writing a test file will be output.

Board Setting
-------------

* Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

* Connect a U disk to one of USB port on the development board with a USB Type-C to USB-A cable

Software Setting
-----------------------

- In the CMakeLists.txt file, if sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1) is commented, FATFS supports data transfer by using cacheable buffer. If sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1) is uncommented, FATFS only supports data transfer by using noncacheable buffer.
- When sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1) is commented, it is recommended to align cache line size for the buffer address, otherwise it will affect transmission performance.

Project Configuration
---------------------

- File `CMakeLists.txt`：

  When BOARD_TUH_RHPORT is defined as 0, it means to use USB0.

  When BOARD_TUH_RHPORT is defined as 1, it means to use USB1.
- The property of USB host can be configured by "tusb_config.h" or "CMakeListx.txt"

Running the example
-------------------

When the project runs correctly

- View the U disk information and the result of writing a test file

  .. image:: doc/1646400992752.png
     :alt: 1646400992752.png
