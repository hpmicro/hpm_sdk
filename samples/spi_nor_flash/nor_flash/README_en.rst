.. _spi_nor_flash:

SPI NOR FLASH
================================

Overview
--------

This example demonstrates how to operate NOR Flash memory via the SPI interface using the ``serial_nor`` component,
which supports SFDP-compliant NOR Flash read, write, and erase operations.

Features:
  - Auto-detect Flash SFDP parameters (capacity, page size, sector size, block size, etc.)
  - Blocking/non-blocking erase (chip erase, sector erase)
  - Blocking program and non-blocking page program
  - Read-back data verification
  - Read/write speed measurement


CMakeLists Configuration
---------------------------

Component Dependencies
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   set(CONFIG_SPI_NOR_FLASH 1)
   set(CONFIG_DMA_MGR 1)

- ``CONFIG_SPI_NOR_FLASH`` : Enables the SPI NOR Flash component (serial_nor), which provides SFDP parsing and Flash operation APIs.
- ``CONFIG_DMA_MGR`` : Enables the DMA Manager component; serial_nor uses DMA for SPI transfers to improve performance.

DMA Manager Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **Enable DMA Manager (recommended)**: With ``CONFIG_DMA_MGR=1``, the DMA Manager automatically allocates free DMA channels for serial_nor, eliminating manual channel number and base address configuration and avoiding resource conflicts with other peripherals.

- **Disable DMA Manager**: If ``CONFIG_DMA_MGR=0``, you must manually configure the following DMA-related macros in ``hpm_serial_nor_host_port.c``:

  .. code-block:: c

     #define PORT_SPI_NOR_DMA           BOARD_APP_DMA0      /* DMA instance base address */
     #define PORT_SPI_NOR_DMAMUX        BOARD_APP_DMAMUX     /* DMAMUX instance base address */
     #define PORT_SPI_RX_DMA_CH         0                    /* SPI RX DMA channel number */
     #define PORT_SPI_TX_DMA_CH         1                    /* SPI TX DMA channel number */

  When manually assigning channels, ensure the specified DMA channels are not used by other peripherals in the system, otherwise transfer conflicts or data errors may occur.

Source & Header Paths
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   sdk_inc(../common/port)
   sdk_app_src(../common/port/hpm_serial_nor_host_port.c)
   sdk_app_src(src/main.c)

- ``sdk_inc`` adds ``common/port`` to the header search path for ``hpm_serial_nor_host_port.h``.
- ``sdk_app_src`` specifies the source files to compile:
  - ``hpm_serial_nor_host_port.c`` : Board-level porting layer for SPI NOR Flash, encapsulating SPI pin initialization and clock configuration.
  - ``main.c`` : Application main logic.

SPI Configuration Macros
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   sdk_compile_definitions(-DPORT_SPI_CLK_FREQUENCY=80000000)
   sdk_compile_definitions(-DPORT_SPI_IO_MODE=SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE)

- ``PORT_SPI_CLK_FREQUENCY`` : SPI SCLK frequency in Hz. Set to 80 MHz in this example; adjust according to the target Flash datasheet (e.g., 50 MHz, 100 MHz).
- ``PORT_SPI_IO_MODE`` : SPI IO mode. Available values:

  .. code-block:: c

     SERIAL_NOR_HOST_SUPPORT_STANDARD_SPI           /* Single SPI */
     SERIAL_NOR_HOST_SUPPORT_DUAL_IO_MODE           /* Dual SPI  */
     SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE           /* Quad SPI  */

  When using quad mode, ensure that IO2 and IO3 pins are physically connected and initialized.

Optimization
^^^^^^^^^^^^^^

.. code-block:: cmake

   sdk_compile_options("-O3")

- The ``-O3`` optimization level improves Flash read/write performance, especially for large block transfers.


Board Settings
--------------

- :ref:`SPI pins <board_resource>` vary by board; refer to the board-specific documentation.
- SPI pins must be connected to the NOR Flash module pins as follows:

  ================ ====================
  SPI Signal       NOR Flash Pin
  ================ ====================
  SCLK             CLK
  CS               CS#
  MOSI (IO0)       DI (IO0)
  MISO (IO1)       DO (IO1)
  IO2              WP# (IO2)
  IO3              HOLD# (IO3)
  ================ ====================

- For quad SPI, IO2/IO3 must be connected; they can be omitted in single or dual mode.
- The ``serial_nor_get_board_host()`` API automatically retrieves the SPI base address and DMA resources based on board configuration.


Notes
-----

- This example depends on the **serial_nor** component and the **DMA Manager** component; enable the corresponding macros in CMakeLists.
- The serial_nor component supports SFDP (JESD216) compliant NOR Flash, not limited to a specific model (W25Q64, W25Q128, W25Q256, MX25L128, etc.).
- Different Flash chips have different maximum SCLK frequencies; consult the target Flash datasheet when modifying ``PORT_SPI_CLK_FREQUENCY``.
- In quad mode, some Flash chips require a QPI enable command; this is handled internally by the serial_nor component.
- SPI transfers are managed by the DMA Manager, so no manual DMA channel management is required.


Running the Example
-------------------

When the example runs successfully, the serial console will output:

.. code-block:: console

    spi nor flash init ok
    the flash sfdp version:6
    the flash size:16384 KB
    the flash page_size:256 Byte
    the flash sector_size:4 KB
    the flash block_size:64 KB
    the flash sector_erase_cmd:0x20
    the flash block_erase_cmd:0xd8
    wbuff and rbuff compare finsh ok
    write_speed:702.40 KB/s read_speed:35669.09 KB/s
    page program: wbuff and rbuff compare finsh ok

Output description:

- **sfdp version** : Flash SFDP standard version
- **flash size** : Total Flash capacity
- **page_size** : Page program size (typically 256 bytes)
- **sector_size / block_size** : Minimum erase unit (sector / block)
- **sector_erase_cmd / block_erase_cmd** : Erase command codes
- **write_speed / read_speed** : Write and read speed in KB/s, affected by SPI clock frequency and IO mode
- **page program** : Non-blocking page program verification result
