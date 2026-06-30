.. _cmake_quick_start:

SDK CMake Quick Start
##########################

本指南帮助你在 5 分钟内学会编写 HPM SDK 示例程序的 CMakeLists.txt。
详细说明请参考 :ref:`cmake_user_guide`，完整 API 列表请参考 :ref:`cmake_intro`。

.. contents:: 目录
   :local:

最小模板
********

下面是一个 HPM SDK 应用程序的最小 CMakeLists.txt，可以直接复制使用：

.. code-block:: cmake

   # Copyright (c) 2026 HPMicro
   # SPDX-License-Identifier: BSD-3-Clause

   cmake_minimum_required(VERSION 3.13)

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

   project(my_app)

   sdk_app_src(src/main.c)
   generate_ide_projects()

逐步说明
********

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)

指定最低 CMake 版本。HPM SDK 要求 3.13 或更高。

.. code-block:: cmake

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

这是最关键的一行。它加载整个 SDK 构建系统，包括：

- 工具链配置（RISC-V GCC / Andes NDS / ZCC）
- Board 选择（通过 ``-DBOARD=<name>`` 在命令行指定）
- 构建类型配置（``-DHPM_BUILD_TYPE=<type>``，详见 :ref:`cmake_user_guide`\）
- SDK 核心库（驱动、组件、中间件）

执行该命令前需要设置 ``HPM_SDK_BASE`` 环境变量，指向 SDK 根目录：

.. code-block:: bash

   source hpm_sdk/env.sh  # 自动设置 HPM_SDK_BASE

.. important::

   所有 SDK 配置变量**必须**在 ``find_package(hpm-sdk)`` 之前设置，包括：

    - ``CONFIG_*`` — 功能开关（如 ``CONFIG_FREERTOS``、``CONFIG_LWIP``）
    - ``HEAP_SIZE`` / ``STACK_SIZE`` — 堆栈大小
    - ``HPM_BUILD_TYPE`` — 构建类型
    - ``CUSTOM_GCC_LINKER_FILE`` 等自定义变量

   在 ``find_package`` 之后设置这些变量将**不会生效**。

.. code-block:: cmake

   project(my_app)

定义项目名称。输出文件将以项目名命名（例如 ``my_app.elf``、``my_app.bin``）。

.. code-block:: cmake

   sdk_app_src(src/main.c)

将源文件添加到应用程序中。可一次添加多个文件：

.. code-block:: cmake

   sdk_app_src(src/main.c src/utils.c src/config.c)

API 详情见 :ref:`cmake_intro`。

.. code-block:: cmake

   generate_ide_projects()

生成 IAR EWARM 和 SEGGER Embedded Studio 的项目文件。每个 CMakeLists.txt 必须以该命令结尾。详见 :ref:`cmake_user_guide`。


常用 CMake 扩展速查
********************

HPM SDK 提供了丰富的 ``sdk_*`` CMake 函数，分为应用层和 SDK 层两类。完整 API 列表见 :ref:`cmake_intro`。

应用层函数（操作你的应用程序）
===============================

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 函数
     - 说明
   * - ``sdk_app_src(file ...)``
     - 添加应用程序源文件。相对路径相对于当前 CMakeLists.txt 所在目录。
   * - ``sdk_app_inc(dir ...)``
     - 添加应用程序头文件搜索路径。
   * - ``sdk_app_src_glob(pattern ...)``
     - 通过 glob 模式批量添加源文件，例如 ``sdk_app_src_glob(src/*.c)``。

SDK 层函数（配置 SDK 核心库）
=============================

编译与定义
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 函数
     - 说明
   * - ``sdk_compile_options(opt ...)``
     - 添加编译器选项，例如 ``-Wall``、``-O2``。
   * - ``sdk_compile_definitions(def ...)``
     - 添加预处理器宏，例如 ``DEBUG=1``、``ENABLE_FEATURE_X``。

源文件与头文件
--------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 函数
     - 说明
   * - ``sdk_src(file ...)``
     - 向 SDK 核心库添加源文件（用于中间件、组件开发）。
   * - ``sdk_inc(dir ...)``
     - 向 SDK 核心库添加头文件搜索路径（INTERFACE 可见性）。
   * - ``sdk_sys_inc(dir ...)``
     - 添加系统头文件路径（SYSTEM 可见性，抑制第三方库警告）。

链接器
------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 函数
     - 说明
   * - ``sdk_link_libraries(lib ...)``
     - 链接库，例如 ``m``（数学库）、``libcustom.a``。
   * - ``sdk_ld_options(opt ...)``
     - 设置链接器选项，例如 ``-Wl,--start-group``。
   * - ``sdk_linker_global_symbols(sym ...)``
     - 定义全局链接符号（``--defsym``）。

条件控制函数（基于 ``CONFIG_*`` 变量）
=======================================

这些函数仅在对应的 ``CONFIG_*`` 变量启用时生效：

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - 函数
     - 说明
   * - ``sdk_src_ifdef(FEATURE file ...)``
     - ``FEATURE`` 启用时才添加源文件。
   * - ``sdk_src_ifndef(FEATURE file ...)``
     - ``FEATURE`` 未启用时才添加源文件。
   * - ``sdk_inc_ifdef(FEATURE dir ...)``
     - ``FEATURE`` 启用时才添加头文件路径。
   * - ``sdk_inc_ifndef(FEATURE dir ...)``
     - ``FEATURE`` 未启用时才添加头文件路径。
   * - ``sdk_compile_definitions_ifdef(FEATURE def ...)``
     - ``FEATURE`` 启用时才添加预处理器宏。
   * - ``sdk_link_libraries_ifdef(FEATURE lib ...)``
     - ``FEATURE`` 启用时才链接库。
   * - ``sdk_ld_options_ifdef(FEATURE opt ...)``
     - ``FEATURE`` 启用时才设置链接器选项。
   * - ``sdk_src_glob_ifdef(FEATURE pattern ...)``
     - ``FEATURE`` 启用时才通过 glob 添加源文件。


典型场景
********

场景 1：单文件驱动示例（最简）
===============================

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(gpio_example)
   sdk_app_src(src/gpio.c)
   generate_ide_projects()

场景 2：多文件 + 头文件目录
===============================

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(uart_demo)
   sdk_inc(src)
   sdk_app_src(src/uart.c src/protocol.c src/ringbuffer.c)
   generate_ide_projects()

场景 3：启用 SDK 中间件
===============================

在 ``find_package(hpm-sdk)`` 之前通过 ``set(CONFIG_* 1)`` 启用所需功能。可用的 ``CONFIG_*`` 变量和功能模块详细介绍见 :ref:`cmake_user_guide`：

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)

   set(CONFIG_FREERTOS 1)    # 启用 FreeRTOS
   set(CONFIG_LWIP 1)        # 启用 LwIP 网络协议栈
   set(CONFIG_SDMMC 1)       # 启用 SD/eMMC 驱动

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(network_app)
   sdk_app_src(src/main.c src/tcp_server.c)
   generate_ide_projects()

支持的 ``CONFIG_*`` 变量取决于你使用的 SDK 版本和 SoC。常用值：

- ``CONFIG_FREERTOS`` — FreeRTOS
- ``CONFIG_LWIP`` — LwIP 网络协议栈
- ``CONFIG_FATFS`` — FatFs 文件系统
- ``CONFIG_USB_DEVICE`` / ``CONFIG_USB_HOST`` — USB 协议栈
- ``CONFIG_LVGL`` — LVGL 图形库
- ``CONFIG_SDMMC`` — SD/eMMC 驱动
- ``CONFIG_ENET_PHY`` — 以太网 PHY
- ``CONFIG_LIBJPEG`` — libjpeg-turbo

场景 4：编译宏定义
===================

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(plic_example)
   sdk_compile_definitions(-DUSE_S_MODE_IRQ=1)
   sdk_app_src(src/plic.c)
   generate_ide_projects()

场景 5：调优堆栈大小
=====================

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(mem_intensive_app)
   set(HEAP_SIZE 0x200000)   # 2MB 堆
   set(STACK_SIZE 0x20000)   # 128KB 栈
   sdk_app_src(src/main.c)
   generate_ide_projects()

场景 6：多核应用（Core 0 + Core 1）
=====================================

多核构建的完整说明见 :ref:`cmake_user_guide`。以下是典型配置：

**Core 0** — 正常编译，构建类型为 ``flash_xip``，需要包含 core 1 的固件镜像：

.. code-block:: cmake

   if("${HPM_BUILD_TYPE}" STREQUAL "")
       SET(HPM_BUILD_TYPE flash_xip)
   endif()
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(multicore_core0)
   sdk_app_src(../../common/multicore_common.c)
   sdk_app_src(src/demo.c src/sec_core_img.c)
   generate_ide_projects()

**Core 1** — 构建类型为 ``sec_core_img``，输出会被转换为 C 数组供 Core 0 链接：

.. code-block:: cmake

   set(HPM_BUILD_TYPE "sec_core_img")
   set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(multicore_core1)
   sdk_app_src(src/demo.c)
   generate_ide_projects()

场景 7：自定义链接脚本
=======================

自定义链接脚本的详细配置见 :ref:`cmake_user_guide`：

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(custom_memory_map)
   set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/custom_layout.ld)
   set(CUSTOM_SES_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/custom_layout.icf)
   sdk_app_src(src/main.c)
   generate_ide_projects()


关键变量
********

构建系统变量（命令行）
=======================

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 变量
     - 说明
   * - ``BOARD``
     - **必须。** 目标板名称，例如 ``hpm6200evk``。通过 ``-DBOARD=hpm6200evk`` 设置。可用板子列表见 :doc:`boards/index_zh`。
   * - ``HPM_BUILD_TYPE``
     - 构建类型。常用值：
       ``ram`` (默认，在 RAM 中调试运行),
       ``flash_xip`` (Flash XIP 运行),
       ``flash_sdram_xip`` (Flash 运行 + SDRAM),
       ``sec_core_img`` (多核从核镜像),
       ``ram_debug`` (RAM 调试)。
       详见 :ref:`cmake_user_guide`。
   * - ``GNURISCV_TOOLCHAIN_PATH``
     - RISC-V GCC 工具链路径。也可以通过环境变量设置。

应用程序变量
=============

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - 变量
     - 说明
   * - ``CONFIG_*``
     - 启用/禁用 SDK 功能模块。在 ``find_package(hpm-sdk)`` 之前设置。详见 :ref:`cmake_user_guide`。
   * - ``HEAP_SIZE``
     - 堆大小（字节）。默认值取决于 SDK 配置。
   * - ``STACK_SIZE``
     - 栈大小（字节）。默认值取决于 SDK 配置。
   * - ``CUSTOM_GCC_LINKER_FILE``
     - GCC 自定义链接脚本路径。
   * - ``CUSTOM_SES_LINKER_FILE``
     - SEGGER Embedded Studio 自定义链接配置文件（``.icf``）。


构建与运行
**********

.. code-block:: bash

   # 设置环境
   source hpm_sdk/env.sh
   export GNURISCV_TOOLCHAIN_PATH=/path/to/riscv32-unknown-elf

   # 构建
   cd samples/my_app
   mkdir build && cd build
   cmake -GNinja -DBOARD=hpm6200evk ..
   ninja

   # 输出文件在 build/output/ 目录下：
   #   demo.elf  — ELF 可执行文件
   #   demo.bin  — 二进制固件
   #   demo.map  — 内存映射文件

关于 IDE 项目生成、Flash 烧写、调试配置等更多内容，请参考 :ref:`cmake_user_guide`。


下一步
******

- :ref:`cmake_user_guide` — 完整 CMake 用户指南（工作流、多核、IDE 集成、故障排除）
- :ref:`cmake_intro` — 全部 SDK CMake 函数 API 参考
- :ref:`getting_started` — HPM SDK 入门指南
