.. _spi_nor_flash_fatfs:

SPI NOR FLASH FATFS
================================

Overview
--------

This example demonstrates how to use the FATFS file system on SPI NOR Flash memory. The ``serial_nor``
component provides low-level Flash read/write operations, while FatFs handles file creation, reading,
directory management, and more.

Features:
  - Auto-detect Flash SFDP parameters (capacity, page size, sector size, block size, etc.)
  - Auto-detect and format FAT file system on first mount
  - File creation and read/write
  - Directory creation and deletion
  - Large file write test (2 MB)
  - Manual file system formatting
  - Supports both baremetal and FreeRTOS modes


CMakeLists Configuration
---------------------------

Component Dependencies
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   set(CONFIG_SPI_NOR_FLASH 1)
   set(CONFIG_FATFS_CUSTOM_PORTABLE 1)
   set(CONFIG_FATFS 1)
   set(CONFIG_DMA_MGR 1)

- ``CONFIG_SPI_NOR_FLASH`` : Enables the SPI NOR Flash component (serial_nor), providing low-level Flash operation APIs.
- ``CONFIG_FATFS`` : Enables the FatFs file system component.
- ``CONFIG_FATFS_CUSTOM_PORTABLE`` : Enables the FatFs custom porting layer, allowing a user-defined diskio implementation (spi_nor_disk in this example).
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

Optional RTOS Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   set(USE_FREERTOS 1)

   if (USE_FREERTOS)
       set(CONFIG_FREERTOS 1)
       set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)
   endif()

- ``USE_FREERTOS`` : Controls FreeRTOS enablement. Set to ``0`` for baremetal, ``1`` to run FATFS tests in a FreeRTOS task.
- ``CONFIG_FREERTOS`` : Enables the FreeRTOS component.
- ``CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR`` : Uses GPTMR as the FreeRTOS system tick source.

Source & Header Paths
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   sdk_inc(../common/port)
   sdk_inc(../common/port/${BOARD})
   sdk_app_src(../common/port/hpm_serial_nor_host_port.c)

   add_subdirectory(fatfs)

   sdk_inc(inc)
   sdk_app_src_glob(src/*.c)

- ``sdk_inc`` adds ``common/port`` to the header search path for the SPI NOR Flash board-level porting headers.
- ``sdk_app_src`` compiles ``hpm_serial_nor_host_port.c`` : the board-level porting layer for SPI NOR Flash.
- ``add_subdirectory(fatfs)`` includes the ``fatfs/`` subdirectory CMakeLists.txt, which contains the FatFs diskio porting layer:

fatfs Directory CMakeLists
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  .. code-block:: cmake

     sdk_inc(port)
     sdk_app_src(port/diskio.c)
     sdk_app_src(port/spi_nor_disk.c)

  - ``diskio.c`` : FatFs disk I/O layer framework.
  - ``spi_nor_disk.c`` : SPI NOR Flash diskio implementation, wrapping serial_nor read/write/erase into sector-level APIs callable by FatFs.

RTOS Compile Macros
^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   if (USE_FREERTOS)
       sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
       sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
       sdk_compile_definitions(-DUSE_FREERTOS=1)
       sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
   endif()

- ``USE_FREERTOS`` : Informs the code to run FATFS logic in a FreeRTOS task.
- ``NO_BOARD_TIMER_SUPPORT`` : Prevents baremetal timer initialization by the board layer.

SPI Configuration Macros
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   sdk_compile_definitions(-DPORT_SPI_CLK_FREQUENCY=80000000)
   sdk_compile_definitions(-DPORT_SPI_IO_MODE=SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE)

- ``PORT_SPI_CLK_FREQUENCY`` : SPI SCLK frequency in Hz. Set to 80 MHz in this example; adjust according to the target Flash datasheet.
- ``PORT_SPI_IO_MODE`` : SPI IO mode. Available values:

  .. code-block:: c

     SERIAL_NOR_HOST_SUPPORT_STANDARD_SPI           /* Single SPI */
     SERIAL_NOR_HOST_SUPPORT_DUAL_IO_MODE           /* Dual SPI  */
     SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE           /* Quad SPI  */

Optimization
^^^^^^^^^^^^^^

.. code-block:: cmake

   sdk_compile_options("-O3")

- The ``-O3`` optimization level improves Flash read/write performance.


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


Notes
-----

- This example depends on the **serial_nor**, **FatFs**, and **DMA Manager** components; enable the corresponding macros in CMakeLists.
- The serial_nor component supports SFDP (JESD216) compliant NOR Flash, not limited to a specific model (W25Q64, W25Q128, W25Q256, MX25L128, etc.).
- On the first run, if the Flash has no FAT file system, the example auto-formats it (larger capacity takes longer; please be patient).
- The FatFs diskio porting layer (``spi_nor_disk.c``) implements sector-aligned reads/writes, automatically using an internal aligned buffer for non-cacheline-aligned user buffers.
- In baremetal mode, ``board_delay_us()`` is used for delays; in FreeRTOS mode, ``vTaskDelay()`` is used.
- ``console_getchar_nonblocking()`` provides non-blocking key input to avoid blocking the menu loop.


Running the Example
-------------------

When the example runs successfully, Flash parameter information and a logo are displayed:

.. code-block:: console

   hpm_sdk: 1.11.0
   ----------------------------------------------------------------------
   $$\   $$\ $$$$$$$\  $$\      $$\ $$\
   $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
   $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
   $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
   $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
   $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
   $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
   \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
   ----------------------------------------------------------------------
   the flash size:8192 KB
   the flash page_size:256 Byte
   the flash sector_size:4 KB
   the flash block_size:64 KB
   the flash sector_erase_cmd:0x20
   the flash block_erase_cmd:0xd8
   spi nor flash init ok

If the Flash has no FAT file system, it will auto-format; be sure to wait for formatting to complete:

.. code-block:: console

   Failed to mount SPI Nor flash, cause: There is no valid FAT volume
   Formatting the SPI Nor flash, depending on the SPI Nor flash capacity, the formatting process may take a long time
   Making file system is successful
   SPI Nor flash has been mounted successfully

After a successful mount, the interactive menu appears:

.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                   spi nor flash fatfs demo                  *
   *                                                             *
   *        1 - Create hello.txt                                 *
   *        2 - Read 1st line from hello.txt                     *
   *        3 - Directory related test                           *
   *        4 - Large file write test                            *
   *        5 - format fs system                                 *
   *-------------------------------------------------------------*

Select a test item:

.. code-block:: console

   1
   Create new file successfully, status=0
   Write file operation is successfully
   2
   Open file successfully
   Hello, this is SPI SPI Nor flash FATFS demo

   3
   Creating new directory succeeded
   Removing new directory succeeded

   4
   Create new file successfully, now writing.....
   Write file operation is successful

   5
   Formatting the SPI Nor flash, depending on the SPI Nor flash capacity, the formatting process may take a long time
   Making file system is successful

Menu item description:

- **1** : Create ``hello.txt`` and write a test string
- **2** : Read the first line of ``hello.txt``
- **3** : Create directory ``hpmicro_spi_nor_flash_test_dir0`` then delete it
- **4** : Create ``big_file.bin`` and write 1 MB of data to test large file write capability
- **5** : Manually format the Flash as a FAT file system
