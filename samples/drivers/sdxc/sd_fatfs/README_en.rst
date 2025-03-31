.. _sdcard_fatfs_demo:

SDCard FATFS demo
==================================

Overview
--------

The demo provides the following features:
- Format an SD card with FAT32 filesystem

- Create hello.txt

- Read 1st line from hello.txt

- Directy creation/deletion

- Large file write test

This demo can work under `polling` or `interrupt` mode, and the default mode is `polling` mode. Please refer to the comments in  `CMakeLists.txt` for switching to `interrupt` mode.

Board Setting
-------------

No special settings are required

Running the example
-------------------

When the example runs successfully, the board shows the following menu:


.. code-block:: console

   SD FATFS demo
   -----------------------------------
   1 - Format the SD card with FATFS
   2 - Create hello.txt
   3 - Read 1st line from hello.txt
   4 - Directory related test
   s - Large file write test
   Others - Show menu

