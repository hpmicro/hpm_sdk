.. _eeprom_emulation_base_test:

EEPROM Emulation Base Test
===========================

Overview
--------

This example demonstrates how to use the EEPROM emulation component (eeprom_emulation)
to implement basic power-loss protected data storage. The example uses NOR Flash to
emulate EEPROM characteristics, supporting data write, read, update, delete, and
defragmentation operations.

Component Features
-------------------

- **Power-loss protection**: Dual half-area design ensures safe recovery from any power-loss scenario
- **CRC32 verification**: Automatic data integrity verification on read
- **Auto defragmentation**: Triggers flush automatically when space is insufficient
- **Portability**: Abstracts Flash drivers via function pointers, supporting different media

Test Contents
-------------

This example includes the following function tests:

1. **Write Test**

   - Write 4 test variables (var1 ~ var4) with fixed block IDs (HPMC, A, AB, ABC)
   - Write string data via `e2p_write()`
   - Check write return value

2. **Read Test**

   - Read previously written 4 variables
   - Read data via `e2p_read()`
   - Verify read data consistency with written data

3. **Update Test**

   - Modify values of var1 and var2
   - Read again and verify update result
   - Verify old data is overwritten by new data

4. **Delete Test**

   - Delete var4 variable
   - Mark entry as invalid via `e2p_delete()`
   - Verify delete operation returns success

5. **Flush Test**

   - Force trigger data defragmentation
   - Organize data via `e2p_flush(E2P_FLUSH_FORCE)`
   - Reclaim deleted/obsolete data space and switch active area
   - Call `e2p_show_info()` to display status after flush

6. **Clear Test**

   - Erase entire storage area (both half-areas)
   - Clear all data via `e2p_clear()`
   - Display status after clear

7. **Status Display**

   - Print current storage area status info (option 7)
   - Display start address, sector count, remaining space, valid/total entries, etc.

Flash Area Layout
-----------------

EEPROM emulation uses a continuous region of NOR Flash, divided into two half-areas (A and B):

.. code-block:: text

    +--------------------------------------------------+
    |                  Half Area A                     |
    |  +--------------------------------------------+  |
    |  | Data (growing ↑)  |  Info (growing ↓)     |  |
    |  |  - User data      |  - Block descriptors  |  |
    |  |  - Tail: Header   |                        |  |
    |  +--------------------------------------------+  |
    +--------------------------------------------------+
    |                  Half Area B                     |
    |  (Same layout as above)                         |
    +.--------------------------------------------------+

- The front segment of each half-area stores **Data** (grows from low to high address)
- The back segment of each half-area stores **Info** descriptors (grows from high to low address)
- Info descriptors contain: `block_id`, `data_addr`, `length`, `valid_state`, `crc32`
- Active area and backup area are switched via state machine for power-loss protection

State Machine and Power-Loss Protection
---------------------------------------

The component uses a 4-bit state machine (implemented via NOR Flash 1→0 bit transitions, no erase needed):

.. list-table::
   :widths: 20 30 55
   :header-rows: 1

   * - State Value
     - State Name
     - Description
   * - 0
     - valid
     - Normal operating state
   * - 8
     - finish
     - Data copy complete, ready to erase old area
   * - 12
     - write
     - Writing data to new area
   * - 14
     - start
     - Defragmentation started
   * - 15
     - invalid
     - Uninitialized/erased state

**State transition flow**:

.. code-block:: text

    invalid(15) → start(14) → write(12) → finish(8) → valid(0)

**Power-loss recovery strategy**:

.. list-table::
   :widths: 35 15 15 55
   :header-rows: 1

   * - Power-loss Timing
     - Area A
     - Area B
     - Recovery Action
   * - First boot
     - 15
     - 15
     - Select Area A, mark as valid
   * - After flush, old erased
     - 0 (valid)
     - 8 (finish)
     - Erase Area A, switch to B, mark B as valid
   * - During flush write
     - 0 (valid)
     - 12 (write)
     - Erase B, re-flush
   * - Before flush start
     - 0 (valid)
     - 14 (start)
     - Erase B, re-flush

Configuration Parameters
------------------------

This example uses the following configuration parameters (in `main.c`):

.. list-table::
   :widths: 25 60
   :header-rows: 1

   * - Macro
     - Description
   * - DEMO_ERASE_SIZE
     - Flash erase unit size (default 4096)
   * - DEMO_SECTOR_CNT
     - Sector count, must be even (default 32)
   * - DEMO_MANAGE_SIZE
     - Total managed size (DEMO_ERASE_SIZE * DEMO_SECTOR_CNT)
   * - DEMO_MANAGE_OFFSET
     - Offset from flash base (BOARD_FLASH_SIZE - DEMO_MANAGE_SIZE * 2)

Single variable data size must not exceed `E2P_FLUSH_BUF_SIZE` (default 512 bytes), configurable in `user_config.h`.

Usage Steps
-----------

1. Initialize NOR Flash port

   .. code-block:: c

      nor_flash_init(&g_nor_cfg);

2. Configure eeprom_emulation context

   .. code-block:: c

      g_e2p_ctx.config.start_addr = g_nor_cfg.base_addr + DEMO_MANAGE_OFFSET;
      g_e2p_ctx.config.erase_size = DEMO_ERASE_SIZE;
      g_e2p_ctx.config.sector_cnt = DEMO_SECTOR_CNT;
      g_e2p_ctx.config.version = 0x4553;  /* 'E' 'S' */
      g_e2p_ctx.config.flash_read = demo_flash_read;
      g_e2p_ctx.config.flash_write = demo_flash_write;
      g_e2p_ctx.config.flash_erase = demo_flash_erase;

3. Initialize eeprom_emulation

   .. code-block:: c

      e2p_config(&g_e2p_ctx);

4. Generate block_id (e.g. for var1 ~ var4)

   .. code-block:: c

      uint32_t var1 = e2p_generate_id("HPMC");

5. Write data

   .. code-block:: c

      e2p_write(var1, sizeof("test"), (uint8_t *)"test");

6. Read data

   .. code-block:: c

      uint8_t buf[32];
      e2p_read(var1, sizeof(buf), buf);

7. Delete data (optional)

   .. code-block:: c

      e2p_delete(var1);

8. Defragment data (optional; also auto-triggered when space is low)

   .. code-block:: c

      e2p_flush(E2P_FLUSH_FORCE);

Notes
-----

- Single write data length should not exceed `E2P_FLUSH_BUF_SIZE`
- `sector_cnt` must be even (two half-areas are required)
- Delete operation only marks entry as invalid; space reclamation completes on next flush
- When version number in config changes, the component automatically erases and reinitializes

Running the Example
-------------------

After the example starts, serial output shows a menu:

.. code-block:: console

   ========================================
     EEPROM Emulation Demo Menu
   ========================================
     1 - Write test data
     2 - Read test data
     3 - Update test data
     4 - Delete a variable
     5 - Flush (defragment)
     6 - Clear all data
     7 - Show status info
     i - Show this menu
   ========================================

**Suggested operation order**:

1. Press `1` to write 4 test variables
2. Press `2` to read and verify data
3. Press `3` to update var1 and var2
4. Press `4` to delete var4
5. Press `5` to force defragment (reclaim deleted space)
6. Press `7` to view current status info
7. Press `6` to clear all data (optional)

**Operation Example**:

1. Press `1` to write 4 test variables
2. Press `2` to read and verify data
3. Press `3` to update var1 and var2
4. Press `4` to delete var4
5. Press `5` to force defragment (reclaim deleted space)
6. Press `6` to clear all data
7. Press `7` to view current status info

**Console Output Example**:

.. code-block:: console

   1
   Writing 4 variables...
   Write completed successfully
   2
   Reading variables...
   var1 = abcdef
   var2 = 1234
   var3 = hello,world
   var4 = eeprom_demo
   3
   Updating var1 and var2...
   var1 = qwe
   var2 = 5678
   Update completed successfully
   4
   Deleting var4...
   Delete successful
   5
   Flushing (defragmenting)...
   Flush completed successfully
   [E2P INFO] ------------ e2p init ok -----------
   [E2P INFO] start address : 0x80fd0000
   [E2P INFO] sector count  : 16
   [E2P INFO] erase size    : 4096
   [E2P INFO] version       : 0x4553
   [E2P INFO] end address   : 0x80fe0000
   [E2P INFO] data_ptr=0x80fd0015  info_ptr=0x80fdffb4  remain=0xff9f
   [E2P INFO] entries valid / total : 3 / 3
   [E2P INFO] ------------------------------------
   6
   Clearing all data...
   Clear completed
   [E2P INFO] ------------ e2p init ok -----------
   [E2P INFO] start address : 0x80fd0000
   [E2P INFO] sector count  : 16
   [E2P INFO] erase size    : 4096
   [E2P INFO] version       : 0x4553
   [E2P INFO] end address   : 0x80fe0000
   [E2P INFO] data_ptr=0x80fd0015  info_ptr=0x80fdffb4  remain=0xff9f
   [E2P INFO] entries valid / total : 0 / 3
   [E2P INFO] ------------------------------------

