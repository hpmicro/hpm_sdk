.. _cmake_quick_start:

SDK CMake Quick Start
##########################

This guide teaches you how to write CMakeLists.txt for HPM SDK applications in 5 minutes.
For detailed explanations, see :ref:`cmake_user_guide`. For the complete API reference, see :ref:`cmake_intro`.

.. contents:: Contents
   :local:

Minimal Template
****************

Below is the minimal CMakeLists.txt for an HPM SDK application. Copy and use it directly:

.. code-block:: cmake

   # Copyright (c) 2026 HPMicro
   # SPDX-License-Identifier: BSD-3-Clause

   cmake_minimum_required(VERSION 3.13)

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

   project(my_app)

   sdk_app_src(src/main.c)
   generate_ide_projects()

Step by Step
************

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)

Specifies the minimum CMake version. HPM SDK requires 3.13 or higher.

.. code-block:: cmake

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

This is the most critical line. It loads the entire SDK build system, including:

- Toolchain configuration (RISC-V GCC / Andes NDS / ZCC)
- Board selection (specified via ``-DBOARD=<name>`` on the command line)
- Build type configuration (``-DHPM_BUILD_TYPE=<type>``, see :ref:`cmake_user_guide`\ )
- SDK core library (drivers, components, middleware)

Set the ``HPM_SDK_BASE`` environment variable before running cmake:

.. code-block:: bash

   source hpm_sdk/env.sh  # Automatically sets HPM_SDK_BASE

.. important::

   All SDK configuration variables **must** be set before ``find_package(hpm-sdk)``, including:

    - ``CONFIG_*`` — feature switches (e.g. ``CONFIG_FREERTOS``, ``CONFIG_LWIP``)
    - ``HEAP_SIZE`` / ``STACK_SIZE`` — heap and stack sizes
    - ``HPM_BUILD_TYPE`` — build type
    - ``CUSTOM_GCC_LINKER_FILE`` and similar custom variables

   Setting these variables **after** ``find_package`` will have **no effect**.

.. code-block:: cmake

   project(my_app)

Defines the project name. Output files will be named accordingly (e.g., ``my_app.elf``, ``my_app.bin``).

.. code-block:: cmake

   sdk_app_src(src/main.c)

Adds source files to the application. Multiple files can be added at once:

.. code-block:: cmake

   sdk_app_src(src/main.c src/utils.c src/config.c)

API details: :ref:`cmake_intro`.

.. code-block:: cmake

   generate_ide_projects()

Generates IAR EWARM and SEGGER Embedded Studio project files.
**Must** be the last line of every CMakeLists.txt. See :ref:`cmake_user_guide` for details.


CMake Extension Cheatsheet
**************************

HPM SDK provides ``sdk_*`` CMake functions in two categories: application-level and SDK core-level.
Full API list: :ref:`cmake_intro`.

Application-Level Functions
============================

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Function
     - Description
   * - ``sdk_app_src(file ...)``
     - Add source files to the application. Relative paths are resolved against the current CMakeLists.txt directory.
   * - ``sdk_app_inc(dir ...)``
     - Add include directories to the application.
   * - ``sdk_app_src_glob(pattern ...)``
     - Add source files via glob patterns, e.g. ``sdk_app_src_glob(src/*.c)``.

SDK Core-Level Functions
=========================

Compile Options & Definitions
-----------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Function
     - Description
   * - ``sdk_compile_options(opt ...)``
     - Add compiler options, e.g. ``-Wall``, ``-O2``.
   * - ``sdk_compile_definitions(def ...)``
     - Add preprocessor definitions, e.g. ``DEBUG=1``, ``ENABLE_FEATURE_X``.

Source Files & Include Directories
----------------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Function
     - Description
   * - ``sdk_src(file ...)``
     - Add source files to the SDK core library (use for middleware/component development).
   * - ``sdk_inc(dir ...)``
     - Add include directories to the SDK core library (INTERFACE visibility).
   * - ``sdk_sys_inc(dir ...)``
     - Add system include directories (SYSTEM visibility, suppresses third-party warnings).

Linker
------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Function
     - Description
   * - ``sdk_link_libraries(lib ...)``
     - Link libraries, e.g. ``m`` (math library), ``libcustom.a``.
   * - ``sdk_ld_options(opt ...)``
     - Set linker options, e.g. ``-Wl,--start-group``.
   * - ``sdk_linker_global_symbols(sym ...)``
     - Define global linker symbols (``--defsym``).

Feature-Gated Functions (``CONFIG_*``)
======================================

These functions take effect only when the corresponding ``CONFIG_*`` variable is enabled:

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - Function
     - Description
   * - ``sdk_src_ifdef(FEATURE file ...)``
     - Add source files only if ``FEATURE`` is enabled.
   * - ``sdk_src_ifndef(FEATURE file ...)``
     - Add source files only if ``FEATURE`` is disabled.
   * - ``sdk_inc_ifdef(FEATURE dir ...)``
     - Add include dir only if ``FEATURE`` is enabled.
   * - ``sdk_inc_ifndef(FEATURE dir ...)``
     - Add include dir only if ``FEATURE`` is disabled.
   * - ``sdk_compile_definitions_ifdef(FEATURE def ...)``
     - Add preprocessor definitions only if ``FEATURE`` is enabled.
   * - ``sdk_link_libraries_ifdef(FEATURE lib ...)``
     - Link libraries only if ``FEATURE`` is enabled.
   * - ``sdk_ld_options_ifdef(FEATURE opt ...)``
     - Set linker options only if ``FEATURE`` is enabled.
   * - ``sdk_src_glob_ifdef(FEATURE pattern ...)``
     - Glob source files only if ``FEATURE`` is enabled.


Common Patterns
***************

Pattern 1: Single-File Driver Sample
====================================

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(gpio_example)
   sdk_app_src(src/gpio.c)
   generate_ide_projects()

Pattern 2: Multiple Files + Include Directory
=============================================

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(uart_demo)
   sdk_inc(src)
   sdk_app_src(src/uart.c src/protocol.c src/ringbuffer.c)
   generate_ide_projects()

Pattern 3: Enabling SDK Middleware
==================================

Set ``CONFIG_*`` variables **before** ``find_package(hpm-sdk)``. See :ref:`cmake_user_guide` for the full list of available ``CONFIG_*`` variables and feature modules:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)

   set(CONFIG_FREERTOS 1)    # Enable FreeRTOS
   set(CONFIG_LWIP 1)        # Enable LwIP
   set(CONFIG_SDMMC 1)       # Enable SD/eMMC driver

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(network_app)
   sdk_app_src(src/main.c src/tcp_server.c)
   generate_ide_projects()

Common ``CONFIG_*`` variables:
  - ``CONFIG_FREERTOS`` — FreeRTOS
  - ``CONFIG_LWIP`` — LwIP networking stack
  - ``CONFIG_FATFS`` — FatFs filesystem
  - ``CONFIG_USB_DEVICE`` / ``CONFIG_USB_HOST`` — USB stack
  - ``CONFIG_LVGL`` — LVGL graphics library
  - ``CONFIG_SDMMC`` — SD/eMMC driver
  - ``CONFIG_LIBJPEG`` — libjpeg-turbo

Pattern 4: Compile Definitions
==============================

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(plic_example)
   sdk_compile_definitions(-DUSE_S_MODE_IRQ=1)
   sdk_app_src(src/plic.c)
   generate_ide_projects()

Pattern 5: Heap/Stack Tuning
=============================

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(mem_intensive_app)
   set(HEAP_SIZE 0x200000)   # 2MB heap
   set(STACK_SIZE 0x20000)   # 128KB stack
   sdk_app_src(src/main.c)
   generate_ide_projects()

Pattern 6: Multicore (Core 0 + Core 1)
======================================

See :ref:`cmake_user_guide` for full multicore build instructions. Typical configuration:

**Core 0** — builds normally with ``flash_xip``, includes core 1's firmware image:

.. code-block:: cmake

   if("${HPM_BUILD_TYPE}" STREQUAL "")
       SET(HPM_BUILD_TYPE flash_xip)
   endif()
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(multicore_core0)
   sdk_app_src(../../common/multicore_common.c)
   sdk_app_src(src/demo.c src/sec_core_img.c)
   generate_ide_projects()

**Core 1** — builds as ``sec_core_img``, output is converted to a C array for core 0:

.. code-block:: cmake

   set(HPM_BUILD_TYPE "sec_core_img")
   set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(multicore_core1)
   sdk_app_src(src/demo.c)
   generate_ide_projects()

Pattern 7: Custom Linker Script
===============================

See :ref:`cmake_user_guide` for detailed custom linker script configuration:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(custom_memory_map)
   set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/custom_layout.ld)
   set(CUSTOM_SES_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/custom_layout.icf)
   sdk_app_src(src/main.c)
   generate_ide_projects()


Key Variables
*************

Build System Variables (Command Line)
=====================================

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Variable
     - Description
   * - ``BOARD``
     - **Required.** Target board name, e.g. ``hpm6200evk``. Set via ``-DBOARD=hpm6200evk``. See :doc:`boards/index_en` for available boards.
   * - ``HPM_BUILD_TYPE``
     - Build type. Common values:
       ``ram`` (default, run in RAM),
       ``flash_xip`` (execute-in-place from Flash),
       ``flash_sdram_xip`` (Flash XIP + SDRAM),
       ``sec_core_img`` (secondary core image),
       ``ram_debug`` (RAM debug).
       See :ref:`cmake_user_guide` for details.
   * - ``GNURISCV_TOOLCHAIN_PATH``
     - Path to RISC-V GCC toolchain. Can also be set as an environment variable.

Application Variables
=====================

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Variable
     - Description
   * - ``CONFIG_*``
     - Enable/disable SDK feature modules. Set before ``find_package(hpm-sdk)``. See :ref:`cmake_user_guide` for details.
   * - ``HEAP_SIZE``
     - Heap size in bytes. Default depends on SDK configuration.
   * - ``STACK_SIZE``
     - Stack size in bytes. Default depends on SDK configuration.
   * - ``CUSTOM_GCC_LINKER_FILE``
     - Path to custom GCC linker script.
   * - ``CUSTOM_SES_LINKER_FILE``
     - Path to custom SEGGER Embedded Studio linker config (``.icf``).


Build & Run
***********

.. code-block:: bash

   # Set up environment
   source hpm_sdk/env.sh
   export GNURISCV_TOOLCHAIN_PATH=/path/to/riscv32-unknown-elf

   # Build
   cd samples/my_app
   mkdir build && cd build
   cmake -GNinja -DBOARD=hpm6200evk ..
   ninja

   # Output files in build/output/:
   #   demo.elf  — ELF executable
   #   demo.bin  — Binary firmware
   #   demo.map  — Memory map file

For IDE project generation, flash programming, and debug configuration, see :ref:`cmake_user_guide`.


Next Steps
**********

- :ref:`cmake_user_guide` — Full CMake user guide (workflows, multicore, IDE integration, troubleshooting)
- :ref:`cmake_intro` — Complete SDK CMake function API reference
- :ref:`getting_started` — HPM SDK getting started guide
