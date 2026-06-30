.. _dfu_bootloader:

DFU Bootloader
======================

Overview
--------

dfu_bootloader example project demonstrates the USB DFU (Device Firmware Update) bootloader functionality using CherryUSB. This bootloader allows firmware updates via USB interface. It first attempts to jump to a valid application in flash, and if no valid application is found, it enters DFU mode to receive firmware updates. To build applications that loaded by the dfu/bootloader, configure HPM_BUILD_TYPE as flash_dfu or flash_sdram_dfu when generating the project.

Board Setting
-------------

- Connect USB device port to PC

Driver Install
---------------
WinUSB driver must be installed through zadig on windows to use the Demo.

.. image:: ./doc/zadig_list_devices.png
   :alt: list_all_devices

After list all devices, find CherryUSB DFU Demo and install WinUSB driver.

.. image:: ./doc/zadig.png
   :alt: install_driver

Running the example
-------------------

When the project runs correctly:

1. If a valid application exists in flash, the bootloader will jump to it and run the application
2. If no valid application is found, the device will enumerate as a USB DFU device on the PC
3. Users can use dfu-util or similar tools to upload new firmware via USB
   - use dfu-util -l to get alt number of DFU device
   - then use dfu-util -D ${bin_file_path} -s ${start_addr_of_bin}:leave -a ${number_alt}

.. code-block:: console

   Found DFU: [0483:df11] ver=0200, devnum=39, cfg=1, intf=0, path="1-3.2", alt=1, name="@Option bytes /0x00000000/01*512 g", serial="2022123456"
   Found DFU: [0483:df11] ver=0200, devnum=39, cfg=1, intf=0, path="1-3.2", alt=0, name="@Internal Flash /0x80020000/1016*16Kg", serial="2022123456"

.. code-block:: console

   Warning: Invalid DFU suffix signature
   A valid DFU suffix will be required in a future dfu-util release
   Opening DFU capable USB device...
   Device ID 0483:df11
   Device DFU version 011a
   Claiming USB DFU Interface...
   Setting Alternate Interface #0 ...
   Determining device status...
   DFU state(2) = dfuIDLE, status(0) = No error condition is present
   DFU mode device DFU version 011a
   Device returned transfer size 512
   DfuSe interface name: "Internal Flash "
   Downloading element to address = 0x80020400, size = 56664
   Erase           [=========================] 100%        56664 bytes
   Erase    done.
   Download        [=========================] 100%        56664 bytes
   Download done.

.. code-block:: console

   HPMicro Cherryusb DFU Demo
   @Internal Flash /0x80020000/1016*16Kg
   @Option bytes /0x00000000/01*512 g

The device will appear as a USB DFU device when in DFU mode.
