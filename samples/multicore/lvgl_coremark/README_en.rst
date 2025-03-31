.. _lvgl_coremark_demo:

LVGL Coremark Demo
====================================

Overview
--------

Multi-core LVGL coremark example project runs the "LittleVGL coremark" example on core0 and core1.

In this project:
 - There are two buttons shown on the GUI, users can run the dual-core coremark by clicking "Start" button and switch among different frequencies by clicking the button with the current core frequency shown on it.

Board Setting
-------------

  BOOT_PIN should be configured to 0-OFF, 1-OFF

Generate, Build and Debug Multi-core projects
---------------------------------------------

Please reference :ref:`Multicore General Description <multicore_general_description>`

Running the example
-------------------

- When the project runs successfully, there are two buttons:

  - "Start"

  - "Frequency Switch"

- Users can clock the "Start" button to start the dual-core coremark benchmarking

- User can click the Frequency Switch button to switch different frequencies
