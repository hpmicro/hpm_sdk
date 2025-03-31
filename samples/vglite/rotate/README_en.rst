.. _rotate:

Rotate
============

Overview
--------

This example project demonstrates how to use the vglite interface to draw vector graphics and display them on the screen.

Hardware Configuration
----------------------

Connect to the screen supported by the development board, defaulting to an RGB screen. If you need to connect a different type of screen, you need to make corresponding modifications in `board/xxx/CMakeLists.txt`.

Build Types
-----------

- flash_sdram_xip

- flash_sdram_xip_release

Execution
---------

Once this program runs correctly, a tiger head image with rotating will be displayed on the screen, and the serial terminal will output the following information:


.. code-block:: console

   gpu: name: GCNanoUltraV, id: 0x00000555, rev: 0x00001230, cid: 0x0000041A
