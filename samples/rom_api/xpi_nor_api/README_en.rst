.. _xpi_nor_api_demo:

XPI NOR API Demo
================================

Overview
--------

This demo provides the following features:
- Test FLASH erase/program/verify at the last sector

- Stress test the FLASH via erase/program/verify the whole FLASH region

- Demonstrate accessing FLASH via using instruction sequence and basic XPI API

Board Setting
-------------

No special settings are required

Running the example
-------------------

When the example runs successfully, the API test result is displayed in the terminal, users can select either option for corresponding feature.


.. code-block:: console

   XPI NOR demo
   ----------------------------------------
    1 - Test FLASH at the last Sector
    2 - Stress test on the FLASH
        NOTE: This case cannot be tested on flash_xip build
    3 - Access FLASH via instruction sequence and basic XPI API
    Others - Show menu
