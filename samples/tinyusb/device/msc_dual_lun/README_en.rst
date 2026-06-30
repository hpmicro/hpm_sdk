.. _usb_msc_dual_lun:

USB MSC DUAL LUN
================================

Overview
--------

This example project shows  USB MSC storage device

- PC shows two USB drives

- When the KEY button is pressed, LUN1 will be set to not ready to simulate medium not present (e.g. SD card removed)

Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

Running the example
-------------------

When the project runs correctly

- After a USB cable plugged, you can see two virtual U disks on the PC device manager

- These two virtual U disks can be accessed and store files temporarily before the system powers off or restarts

- Press the KEY button, the LUN1 disk will be ejected (unavailable) to simulate medium removal. Press again to re-insert LUN1.
