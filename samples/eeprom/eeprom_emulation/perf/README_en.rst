.. _eeprom_emulation_performance_test:

EEPROM Emulation Performance Test
=================================

Overview
--------

This example tests the performance of the EEPROM emulation component. It evaluates
efficiency through large numbers of read/write and defragmentation operations, and
reports timing for config (init), write, flush, read, and delete.

Test Parameters
---------------

Default configuration (modify in `main.c`):

.. list-table::
   :widths: 28 60
   :header-rows: 1

   * - Macro
     - Description
   * - DEMO_TEST_VAR_CNT
     - Number of test variables (default 50)
   * - DEMO_WRITE_CYCLE
     - Write/update cycles per variable (default 5)
   * - DEMO_ERASE_SIZE
     - Flash erase unit size in bytes (default 4096)
   * - DEMO_SECTOR_CNT
     - Sector count, must be even (default 32)

Total writes in write performance test = ``DEMO_TEST_VAR_CNT × DEMO_WRITE_CYCLE`` (e.g. 250).

Performance Metrics
-------------------

The example measures the following:

1. **Configuration (init) time**

   - Clear storage, then measure `e2p_config()` execution time
   - Includes header verification, state recovery, index table rebuild

2. **Write performance**

   - Clear and init, then write all test variables (each updated DEMO_WRITE_CYCLE times)
   - Report total time and average time per write

3. **Flush (defragmentation) performance**

   - Clear, init, write test data, then measure `e2p_flush(E2P_FLUSH_FORCE)` time
   - Includes state switching, data copy to backup area, old area erase

4. **Read performance**

   - Clear, init, write test data, then measure total time to read all variables
   - Report total time and average time per read (includes CRC32 verification)

5. **Delete performance**

   - Clear, init, write test data, then measure total time to delete all variables
   - Delete only marks entries invalid; space is reclaimed on next flush

Usage
-----

After the example starts, choose a test via serial input:

.. code-block:: console

   ========================================
     EEPROM Emulation Perf Test
   ========================================
     1 - Test config/init time
     2 - Test write performance
     3 - Test flush performance
     4 - Test read performance
     5 - Test delete performance
     6 - Show status info
     i - Show this menu
   ========================================

**Test operations**:

- Press `1` — Test initialization (config) performance
- Press `2` — Test write performance
- Press `3` — Test defragmentation (flush) performance
- Press `4` — Test read performance
- Press `5` — Test delete performance
- Press `6` — Show current status info

Output Examples
---------------

**Config (init) performance**:

.. code-block:: console

   === Test: Configuration (Init) ===
   [E2P INFO] ------------ e2p init ok -----------
   [E2P INFO] start address : 0x80fd0000
   [E2P INFO] sector count  : 16
   [E2P INFO] erase size    : 4096
   [E2P INFO] version       : 0x4553
   [E2P INFO] end address   : 0x80fe0000
   [E2P INFO] data_ptr=0x80fd0000  info_ptr=0x80fdffa4  remain=0xffa4
   [E2P INFO] entries valid / total : 0 / 0
   [E2P INFO] ------------------------------------
     Config time: 32451 us

**Write performance**:

.. code-block:: console

   === Test: Write Performance ===
   Writing 50 variables, 5 updates each...
   Write completed: 50 variables x 5 updates = 250 total writes
     Total writes: 250
     Total time: 123456 us
     Avg per write: 493 us

**Flush performance**:

.. code-block:: console

   === Test: Flush Performance ===
   Writing 50 variables, 5 updates each...
   Write completed: 50 variables x 5 updates = 250 total writes
     Flush time: 346190 us

**Read performance**:

.. code-block:: console

   === Test: Read Performance ===
     Success reads: 50 / 50
     Total time: 1450 us
     Avg per read: 29 us

**Delete performance**:

.. code-block:: console

   === Test: Delete Performance ===
     Success deletes: 50 / 50
     Total time: 890 us
     Avg per delete: 17 us

Notes
-----

.. list-table::
   :widths: 28 60
   :header-rows: 1

   * - Item
     - Suggestion
   * - erase_size
     - Larger erase blocks increase flush time. Choose 4KB–64KB based on data size.
   * - sector_cnt
     - Must be even. More sectors reduce flush frequency but increase init/scan time.
   * - Single entry size
     - Keep single write ≤ E2P_FLUSH_BUF_SIZE (user_config.h).
   * - Critical section
     - Keep E2P_CRITICAL_ENTER/EXIT scope minimal; avoid long operations inside.
