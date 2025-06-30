.. _video_mjpeg:

Video MJPEG
====================

Overview
--------

This example project demonstrates a USB UVC device that uses the CAM DVP interface to obtain image information collected by the camera device, and encode it into JPEG image format using the `components/jpeg`, then sends the JPEG image information to the PC host through USB.

Board Setting
-------------

1. Connect the OV7725 camera device to the CAM interface of the development board. If using OV5640, set CONFIG_CAMERA to OV5640 in the CMakeLists.txt file, with the command `set(CONFIG_CAMERA "ov5640")`.
2. Connect USB0 port to PC host

Running the example
-------------------

After the project is running correctly, the PC will list a camera device, which can be tested through the `Tencent Meeting - Settings - Video` option. Select `HPMicro UVC MJPEG` for the camera.
