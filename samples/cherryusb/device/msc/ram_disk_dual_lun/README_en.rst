.. _msc_ram_disk_dual_lun:

MSC RAM Disk Dual LUN
==========================================

Overview
--------

This example project shows USB MSC devcie(Set CONFIG_USBDEV_MSC_MAX_LUN=2 in CMakeLists.txt).

- PC sees two USB disk devices via Device Manager

Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

Running the example
-------------------

- Download the program and run. The computer can automatically recognize and enumerate two USB disk devices.

- You can copy a file to the USB disk, and then copy it from the USB disk.
