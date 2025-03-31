.. _sdxc_low_level_api_demo:

SDXC low-level API demo
==============================================

Overview
--------

The demo provides the following features:
- Write the last sector and verify the data

- Write the last 1024 sectors and verify the data

This demo can work under `polling` or `interrupt` mode, and the default mode is `polling` mode. Please refer to the comments in  `CMakeLists.txt` for switching to `interrupt` mode.

Board Setting
-------------

No special settings are required

Running the example
-------------------

When the example runs successfully, the board shows the following menu, please input 1, 2, 3 or 4 for testing.


.. code-block:: console

   -----------------------------------------------------------------------------------
   *                                                                                 *
   *                   SD Card Low-level test demo                                   *
   *                                                                                 *
   *        1. Write & Read the last block                                           *
   *        2. Write & Read the last 1024 blocks                                     *
   *        3. Hot plug test                                                         *
   *        4. SD Stress test (Write / Read 200MBytes)                               *
   *                                                                                 *
   *---------------------------------------------------------------------------------*

