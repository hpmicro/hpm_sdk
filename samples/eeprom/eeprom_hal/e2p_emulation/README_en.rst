.. _eeprom_hal_emulation:

EEPROM HAL EMULLATION Functional Test
==========================================

Overview
---------

This EEPROM HAL EMULATION example demonstrates how to use the EEPROM HAL driver interface to access EEPROM devices. By default, the example utilizes the EEPROM emulation component to simulate device behavior. To use a physical EEPROM device, the corresponding driver can be enabled through configuration options.

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

    (The following information is printed only when using the EEPROM emulation component:)

    ------------ flash->eeprom init ok -----------
    start address: 0x80fc0000
    sector count: 16
    flash earse granularity: 4096
    version: 0x4553
    end address: 0x80fd0000
    data write addr = 0x80fc013c, info write addr = 0x80fcff84, remain flash size = 0xfe48
    valid count percent info count( 3 / 6 )
    ----------------------------------------------

    Testing Output:

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


