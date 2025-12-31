.. _eeprom_hal_device_test:

EEPROM HAL DEVICE Functional Test
==========================================

Overview
---------

The EEPROM HAL DEVICE example demonstrates how to access an EEPROM device using the EEPROM HAL driver interface. This example uses a physical EEPROM device with a board-level I2C driver by default.

Board Setup
------------

- No special hardware setup is required.

Test Content
------------

- Basic Functionality: Single-byte read/write tests.
- Small Block Test: Non-aligned read/write tests within a single page.
- Large Block Test: Cross-page, non-aligned read/write tests (to verify the anti-wraparound feature).

Running the Example
--------------------

Once the project is running on the board, the following information will be printed to the serial console:

.. code-block:: console

    EEPROM HAL sample test start.

    --- Running Test: Basic I/O (Small Writes) ---
    Testing single byte write at a known-good address (e.g., 0x42)...
    [PASS] Single Byte I/O: Data verified successfully.
    Testing a small, non-aligned write...
    [PASS] Small Non-aligned Write: Data verified successfully.
    --- [PASS] Basic I/O (Small Writes) ---

    --- Running Test: Large Block, Multi-Page, Non-Aligned I/O ---
    Writing a large block (150 bytes) from a non-aligned address (0xA)...
    [PASS] Large Block Verification: Data verified successfully.
    --- [PASS] Large Block, Multi-Page, Non-Aligned I/O ---
    Result: ALL TESTS PASSED!
    EEPROM HAL sample test done.

Extension
------------
- The EEPROM HAL component supports SPI-driven physical EEPROMs. If your board has an SPI EEPROM device or an external SPI-driven EEPROM, you can refer to the current example and modify the CMake configuration to enable SPI device support. The specific modification method is as follows:

    set(CONFIG_EEPROM_DEVICE_SPI 1)

- After enabling the SPI EEPROM driver macros, the component will be automatically added to the project. For instructions on how to use the SPI EEPROM driver, please refer to the README under the `eeprom_hal` component.

