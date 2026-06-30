.. _eeprom_hal_emulation:

EEPROM HAL EMULLATION Functional Test
======================================

Overview
--------

This sample demonstrates using the EEPROM HAL driver interface to access EEPROM storage.
By default it uses the **e2p_emulation** component (Flash-based EEPROM emulation) so that
no physical EEPROM hardware is required. The same HAL API can be used with a physical
EEPROM device by registering a different driver.

Board Setup
-----------

- No special hardware setup is required when using the emulation backend (NOR Flash is used).

Configuration
-------------

Emulation parameters are defined in `main.c`:

.. list-table::
   :widths: 32 60
   :header-rows: 1

   * - Macro
     - Description
   * - BOARD_APP_EEPROM_EMU_ERASE_SIZE
     - Flash erase unit size (default 4096 bytes)
   * - BOARD_APP_EEPROM_EMU_SECTOR_COUNT
     - Sector count, must be even (default 32)
   * - BOARD_APP_EEPROM_EMU_START_ADDR
     - Start address of emulation region (derived from flash size and above)

The emulation region is placed at the end of the board's NOR Flash (two half-areas for power-loss protection).

Test Content
-------------

The sample runs two test suites:

1. **Basic I/O (Small Writes)**

   - Single-byte write and read at a fixed address (e.g. 0x42)
   - Small non-aligned write (7 bytes at address 5) and read-back verification

2. **Large Block, Multi-Page, Non-Aligned I/O**

   - Write 150 bytes from a non-aligned address (0xA)
   - Verifies cross-page and anti-wraparound behavior
   - Read-back and compare

All tests use the HAL API: `hpm_eeprom_write_block()` and `hpm_eeprom_read_block()`.

Usage Steps
-----------

1. Register emulation parameters with the device structure:

   .. code-block:: c

      e2p_emu_xpi_host_config_t emu_host = { .base = BOARD_APP_XPI_NOR_XPI_BASE };
      e2p_emu_xpi_attribute_config_t emu_config = {
          .flash_base_addr = BOARD_FLASH_BASE_ADDRESS,
          .version = 0x4553,
          .opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR,
          .opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0,
          .opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1,
          .start_addr = BOARD_APP_EEPROM_EMU_START_ADDR,
          .erase_size = BOARD_APP_EEPROM_EMU_ERASE_SIZE,
          .sector_cnt = BOARD_APP_EEPROM_EMU_SECTOR_COUNT,
      };
      hpm_eeprom_device_t dev = {
          .host = &emu_host,
          .flag = EEPROM_FLAG_EMULATION_XPI,
          .config = &emu_config,
      };
      e2p_emulation_xpi_register_param(&dev);

2. Register the device with the HAL and open it:

   .. code-block:: c

      hpm_eeprom_register(&dev, "e2p_test");
      hpm_eeprom_device_t *device = hpm_eeprom_find("e2p_test");
      hpm_eeprom_init(device);

3. Use HAL read/write API:

   .. code-block:: c

      hpm_eeprom_write_block(device, addr, len, data);
      hpm_eeprom_read_block(device, addr, len, buf);

Running the Example
-------------------

After the project runs, the serial console shows init and test results. When using the
e2p_emulation component, init output looks like:

.. code-block:: console

   EEPROM HAL sample test start.

   ------------ flash->eeprom init ok -----------
   start address: 0x80fc0000
   sector count: 16
   flash erase granularity: 4096
   version: 0x4553
   end address: 0x80fd0000
   data write addr = 0x80fc013c, info write addr = 0x80fcff84, remain flash size = 0xfe48
   valid count percent info count( 3 / 6 )
   ----------------------------------------------

Then the test output:

.. code-block:: console

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

