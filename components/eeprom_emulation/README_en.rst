.. _eeprom_emulation_component:

EEPROM Emulation Component
==========================

Overview
--------

The EEPROM emulation component (eeprom_emulation) implements power-loss protected
data storage similar to EEPROM on NOR Flash. The component ensures data safety
and consistency under any power-loss scenario through a dual-area design and
state machine mechanism.

Component Architecture
----------------------

.. code-block:: text

    +--------------------------------------------------+
    |            Application Layer                     |
    |  - Data operations via e2p_write/read/delete     |
    +--------------------------------------------------+
                         |
                         | API Calls
                         |
    +--------------------------------------------------+
    |      EEPROM Emulation Core Layer                 |
    |  - eeprom_emulation.h / eeprom_emulation.c       |
    |  - State management, index table, CRC verification |
    +--------------------------------------------------+
                         |
                         | Function Pointers
                         |
    +------------------------+-------------------------+
    |                        |                         |
    +-----------------------+  |   +----------------------+
    |   NOR Flash Port      |  |   |  Other Flash Port    |
    | (port/hpm_nor_flash)  |  |   |  (to be extended)    |
    +-----------------------+  |   +----------------------+
    |                        |                         |
    +--------------------------------------------------+
    |          Flash Driver Layer                      |
    |  - ROM API / XPI Driver                         |
    +--------------------------------------------------+
                         |
    +--------------------------------------------------+
    |              Hardware Layer                      |
    |              XPI External Flash                  |
    +--------------------------------------------------+

Key Features
------------

- **Power-loss Protection**

  - Dual half-area design ensures safe recovery from any power-loss scenario
  - State machine implemented via 1→0 bit transitions (no erase needed)
  - Automatic detection and recovery of incomplete flush operations

- **Data Integrity**

  - Each entry includes CRC32 checksum
  - Automatic verification on read
  - Corrupted data returns error code

- **Space Management**

  - Automatic defragmentation: triggers when space is insufficient
  - Delete marking: `e2p_delete()` marks entries as invalid
  - Obsolete data reclamation: old data for same block_id is cleaned during flush

- **Portability**

  - Abstracts Flash drivers via function pointers
  - Supports different Flash media (NOR Flash, SPI Flash, etc.)
  - Port layer independent, easy to extend

- **Flexible Configuration**

  - Configurable erase block size (erase_size)
  - Configurable sector count (sector_cnt)
  - Configurable version number (version changes trigger automatic erase)

Flash Area Layout
-----------------

EEPROM emulation uses a continuous NOR Flash region, divided into two half-areas (A and B):

.. code-block:: text

    +--------------------------------------------------+
    |                   Half Area A                    |
    |  +------------------------------------------+    |
    |  | Data Region (growing ↑)                   |    |
    |  |  - User data                              |    |
    |  |  - Data blocks: block_id + data           |    |
    |  +------------------------------------------+    |
    |  | Info Region (growing ↓)                   |    |
    |  |  - Info descriptor array                  |    |
    |  |  - Each Info: 16 bytes                    |    |
    |  |  - Tail: Header (12 bytes)                |    |
    |  +------------------------------------------+    |
    +--------------------------------------------------+
    |                   Half Area B                    |
    |  (Same layout as above)                         |
    +--------------------------------------------------+

- **Data Region**: Stores actual user data, grows from low to high address within each half-area
- **Info Region**: Stores data block descriptors, grows from high to low address within each half-area
- **Area Header**: 12 bytes at tail of each half-area, contains: version, magic, state
- **Info Descriptor Structure**:

.. code-block:: c

    typedef struct {
        uint32_t block_id;      /* Block ID (user-defined) */
        uint32_t data_addr;     /* Data address in Data Region */
        uint16_t length;        /* Data length */
        uint16_t valid_state;   /* Validity marker */
        uint32_t crc;          /* CRC32 checksum */
    } e2p_block_t;

State Machine and Power-Loss Protection
---------------------------------------

The component uses a 4-bit state machine, implemented via NOR Flash 1→0 bit transitions (no erase needed):

.. list-table::
   :widths: 20 30 60
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

**State Transition Flow**:

.. code-block:: text

    invalid(15) → start(14) → write(12) → finish(8) → valid(0)

**Power-loss Recovery Strategy**:

.. list-table::
   :widths: 35 15 15 60
   :header-rows: 1

   * - Power-loss Timing
     - Area A State
     - Area B State
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

Primary APIs
------------

Initialization and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_config(e2p_t *e2p);

- **Function**: Initialize EEPROM emulation, verify flash headers, recover power-loss state, rebuild index table
- **Parameters**:
  - `e2p`: Pointer to context structure, must have flash operation function pointers pre-configured
- **Returns**: `E2P_STATUS_OK` on success, error code on failure

Write Data
~~~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_write(uint32_t block_id, uint16_t length, const uint8_t *data);

- **Function**: Write data, automatically updates old data for same block_id
- **Parameters**:
  - `block_id`: User-defined identifier (generated via `e2p_generate_id()`)
  - `length`: Data length in bytes
  - `data`: Pointer to data buffer
- **Returns**: `E2P_STATUS_OK` on success, error code on failure

Read Data
~~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_read(uint32_t block_id, uint16_t length, uint8_t *data);

- **Function**: Read data, automatically verifies CRC32
- **Parameters**:
  - `block_id`: User-defined identifier
  - `length`: Buffer size (can be less than stored length)
  - `data`: Pointer to output buffer
- **Returns**: `E2P_STATUS_OK` on success, `E2P_ERROR_NOT_FOUND` if not found, `E2P_ERROR_CRC` if verification fails

Delete Data
~~~~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_delete(uint32_t block_id);

- **Function**: Delete data for specified block_id (marks as invalid)
- **Parameters**:
  - `block_id`: User-defined identifier
- **Returns**: `E2P_STATUS_OK` on success, `E2P_ERROR_NOT_FOUND` if not found

Defragment Data
~~~~~~~~~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_flush(uint8_t flag);

- **Function**: Defragment data, reclaim space
- **Parameters**:
  - `flag`: `E2P_FLUSH_TRY` (flush only when space insufficient) or `E2P_FLUSH_FORCE` (force flush)
- **Returns**: `E2P_STATUS_OK` on success, error code on failure

Generate ID
~~~~~~~~~~~

.. code-block:: c

    uint32_t e2p_generate_id(const char *name);

- **Function**: Generate 32-bit block_id from string
- **Parameters**:
  - `name`: String pointer (first 4 characters are packed)
- **Returns**: 32-bit block_id

Clear Data
~~~~~~~~~~

.. code-block:: c

    void e2p_clear(void);

- **Function**: Erase entire storage area (both half-areas)

Show Status Info
~~~~~~~~~~~~~~~~

.. code-block:: c

    void e2p_show_info(void);

- **Function**: Print current EEPROM emulation status (active area, remaining size, etc.)
- **Parameters**: None
- **Returns**: None

Configuration Parameters
------------------------

Core configuration structure:

.. code-block:: c

    typedef struct {
        uint32_t start_addr;
        uint32_t sector_cnt;
        uint32_t erase_size;
        uint32_t version;

        hpm_stat_t (*flash_read)(uint8_t *buf, uint32_t addr, uint32_t size);
        hpm_stat_t (*flash_write)(const uint8_t *buf, uint32_t addr, uint32_t size);
        hpm_stat_t (*flash_erase)(uint32_t start_addr, uint32_t size);
    } e2p_config_t;

.. list-table::
   :widths: 25 60
   :header-rows: 1

   * - Field Name
     - Description
   * - start_addr
     - Flash start address
   * - sector_cnt
     - Sector count (must be even)
   * - erase_size
     - Erase block size in bytes
   * - version
     - Version number (triggers automatic erase when changed)
   * - flash_read/write/erase
     - Flash operation function pointers

User configuration (user_config.h):

.. list-table::
   :widths: 30 60
   :header-rows: 1

   * - Configuration
     - Description
   * - E2P_DEBUG_LEVEL
     - Debug level (0-4)
   * - E2P_MAX_VAR_CNT
     - Maximum variable count (default 100)
   * - E2P_FLUSH_BUF_SIZE
     - Defragmentation buffer size (default 512)

Quick Start
-----------

1. Include the header and NOR Flash port:

   .. code-block:: c

       #include "eeprom_emulation.h"
       #include "hpm_nor_flash.h"

2. Configure NOR Flash and EEPROM emulation context (see Usage Example below).
3. Call `nor_flash_init()` then `e2p_config()`.
4. Use `e2p_generate_id("NAME")` for block IDs, then `e2p_write()` / `e2p_read()`.

Usage Example
-------------

Basic usage example:

.. code-block:: c

    #include "eeprom_emulation.h"
    #include "hpm_nor_flash.h"

    static nor_flash_config_t g_nor_cfg;
    static e2p_t g_e2p_ctx;

    /* Flash operation wrappers */
    static hpm_stat_t my_read(uint8_t *buf, uint32_t addr, uint32_t size) {
        return nor_flash_read(&g_nor_cfg, buf, addr, size);
    }
    static hpm_stat_t my_write(const uint8_t *buf, uint32_t addr, uint32_t size) {
        return nor_flash_write(&g_nor_cfg, buf, addr, size);
    }
    static hpm_stat_t my_erase(uint32_t start_addr, uint32_t size) {
        return nor_flash_erase(&g_nor_cfg, start_addr, size);
    }

    int main(void) {
        /* Configure NOR Flash port */
        g_nor_cfg.xpi_base = BOARD_APP_XPI_NOR_XPI_BASE;
        g_nor_cfg.base_addr = BOARD_FLASH_BASE_ADDRESS;
        g_nor_cfg.opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
        g_nor_cfg.opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
        g_nor_cfg.opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

        /* Configure EEPROM emulation */
        g_e2p_ctx.config.start_addr = 0x80080000;
        g_e2p_ctx.config.erase_size = 4096;
        g_e2p_ctx.config.sector_cnt = 128;
        g_e2p_ctx.config.version = 0x4553;
        g_e2p_ctx.config.flash_read = my_read;
        g_e2p_ctx.config.flash_write = my_write;
        g_e2p_ctx.config.flash_erase = my_erase;

        /* Initialize */
        nor_flash_init(&g_nor_cfg);
        if (e2p_config(&g_e2p_ctx) != E2P_STATUS_OK) {
            printf("Init failed\n");
            return -1;
        }

        /* Write data */
        uint32_t var_id = e2p_generate_id("TEST");
        const char *data = "hello,world";
        e2p_write(var_id, strlen(data), (uint8_t *)data);

        /* Read data */
        uint8_t buf[32];
        if (e2p_read(var_id, sizeof(buf), buf) == E2P_STATUS_OK) {
            printf("Read: %s\n", buf);
        }

        return 0;
    }

Extending Flash Port
--------------------

To support other Flash media (e.g., SPI Flash, internal Data Flash), add driver files under `port/`:

.. code-block:: c

    /* port/my_flash.h */
    typedef struct {
        /* Port-specific configuration */
    } my_flash_config_t;

    hpm_stat_t my_flash_init(my_flash_config_t *cfg);
    hpm_stat_t my_flash_read(my_flash_config_t *cfg,
                             uint8_t *buf, uint32_t addr, uint32_t size);
    hpm_stat_t my_flash_write(my_flash_config_t *cfg,
                              const uint8_t *buf, uint32_t addr, uint32_t size);
    hpm_stat_t my_flash_erase(my_flash_config_t *cfg,
                              uint32_t start_addr, uint32_t size);

Usage is the same as NOR Flash port: register the function pointers in `e2p_config_t`.

Error Codes
-----------

.. list-table::
   :widths: 25 10 60
   :header-rows: 1

   * - Error Code
     - Value
     - Description
   * - E2P_STATUS_OK
     - 0
     - Success
   * - E2P_ERROR
     - 1
     - Generic error
   * - E2P_ERROR_NO_MEM
     - 2
     - Insufficient space
   * - E2P_ERROR_INIT_ERR
     - 3
     - Initialization error
   * - E2P_ERROR_BAD_ID
     - 4
     - Invalid block_id
   * - E2P_ERROR_BAD_ADDR
     - 5
     - Invalid address
   * - E2P_ERROR_OVERFLOW
     - 6
     - Index table overflow
   * - E2P_ERROR_CRC
     - 7
     - CRC verification failed
   * - E2P_ERROR_NOT_FOUND
     - 8
     - Not found

Best Practices
--------------

- **Block size**: Keep single variable length ≤ `E2P_FLUSH_BUF_SIZE` (default 512 bytes); split large data into multiple variables.
- **Flush**: Avoid calling `e2p_flush(E2P_FLUSH_FORCE)` in time-critical paths; flush can be slow due to erase.
- **Critical section**: Keep code inside `E2P_CRITICAL_ENTER` / `E2P_CRITICAL_EXIT` short; avoid long operations.
- **Version**: Changing `version` in config triggers full erase and re-init; use when layout or format changes.

Troubleshooting
---------------

- **Init failed**: Check `start_addr`, `erase_size`, `sector_cnt` (must be even), and that flash read/write/erase pointers are set.
- **E2P_ERROR_NO_MEM**: Run `e2p_flush(E2P_FLUSH_FORCE)` to reclaim space, or reduce variable count/size.
- **E2P_ERROR_CRC**: Stored data may be corrupted; re-write the variable or check flash hardware.
- **E2P_ERROR_NOT_FOUND**: Ensure the block was written and `block_id` matches (e.g. from `e2p_generate_id()`).

Notes
-----

1. **sector_cnt must be even**

   Component requires two half-areas for power-loss protection.

2. **Single write size limit**

   Single variable length should not exceed `E2P_FLUSH_BUF_SIZE` (default 512 bytes).
   Large data should be split into multiple variables.

3. **Version number changes**

   Component automatically erases and reinitializes when `version` field changes.

4. **Critical section protection**

   `E2P_CRITICAL_ENTER` / `E2P_CRITICAL_EXIT` macros should be as short as possible.
   Avoid time-consuming operations within critical sections.

5. **Flash erase time**

   Erase operations typically take long time.
   Avoid calling flush in time-critical code paths.
