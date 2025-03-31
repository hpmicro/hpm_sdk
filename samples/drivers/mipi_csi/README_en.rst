.. _mipi_csi:

MIPI_CSI
================

Overview
--------

The example project demonstrates capturing video data from a camera using the `MIPI_CSI` interface. The captured video data is received by `CAM` and displayed on the screen through the buffer.

Hardware Setup
--------------

1. Connect the camera and screen to the video expansion board.
2. Connect the video expansion board to the development board.

Project Configuration
---------------------

- the sample does not support RGB LCD screen due to the hardware design conflict between onboard RGB and mipi csi pins.

modify `board/xxx/CMakeLists.txt` to select the corresponding panel type (LVDS or MIPI).

Notes
-----

- Camera support: OV5640

- Panel support: MIPI or LVDS interface

Running Phenomenon
------------------

Once the project runs correctly, the screen will display the video data from the camera. The serial terminal will output the following information:


.. code-block:: console

   mipi_csi example
   I2C bus is ready
   name: cc10128007, lcdc_clk: 74250khz
   mipi csi clk freq: 13813953 Hz
