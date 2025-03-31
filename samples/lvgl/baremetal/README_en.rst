.. _lvgl_baremetal:

lvgl_baremetal
============================

Overview
--------

This example project demonstrates the running effect of LVGL's official demo in a bare-metal environment.

Hardware Configuration
----------------------

Connect the supported screen to the development board, with the default being an RGB screen. If you need to connect another type of screen, you need to modify `board/xxx/CMakeLists.txt` accordingly to select the corresponding screen.

Build Types
-----------

- flash_sdram_xip

- flash_sdram_xip_release

Running the example
-------------------

Once this program runs correctly, the demo's entry button will be displayed on the screen. Click the corresponding button to execute the respective demo.
