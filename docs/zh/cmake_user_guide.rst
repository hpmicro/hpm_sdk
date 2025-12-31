.. _cmake_user_guide:

SDK CMake 用户指南
######################

本指南提供了关于如何使用 HPM SDK CMake 扩展构建嵌入式应用程序的全面信息。本指南专注于工作流程、最佳实践和实际示例，而不是 API 参考（API 参考请参见 :ref:`cmake_intro`）。

.. contents:: 目录
   :local:

概述
****

SDK CMake 构建系统设计理念
===========================

HPM SDK CMake 构建系统的设计基于一个基本原则：**将用户应用程序与底层 SDK 解耦**。这种设计理念解决了嵌入式开发中的几个关键挑战：

**1. SDK 版本独立性**

- 用户可以通过简单地更改 `HPM_SDK_BASE` 路径在同一大版本下的不同 SDK 版本间切换
- 切换 SDK 版本时无需修改应用程序代码
- 在 SDK 更新过程中保持应用程序兼容性

**2. 团队开发中降低沟通成本**

- 多个开发者可以基于同一个 SDK 版本协作开发应用程序
- 只需要交换应用程序相关的数据信息，而不用传递整个 SDK
- 消除了团队成员之间共享大型 SDK 仓库的需要
- 减少版本同步问题和合并冲突

**3. 通过自定义板子支持实现完全解耦**

- 用户可以定义自定义板卡配置，而无需修改 SDK 核心文件
- 应用程序变得完全独立于 SDK 内部实现
- 支持针对特定硬件需求的快速原型设计和定制
- 通过将应用程序逻辑与 SDK 复杂性隔离来提高开发效率

**4. 模块化架构优势**

- 应用程序和 SDK 层之间关注点清晰分离
- 更容易维护和调试应用程序特定问题
- 简化的依赖管理和构建配置
- 增强跨不同开发环境的可移植性

**5. 跨平台构建系统**

- **Windows、macOS 和 Linux 支持**：在主要操作系统上完全兼容
- **统一构建命令**：相同的 CMake 命令在所有平台上工作一致
- **自动工具链检测**：自动检测和配置平台特定工具链
- **IDE 集成**：为各种 IDE 生成适合平台的项目文件
- **路径处理**：在不同操作系统约定之间自动路径转换
- **依赖解析**：平台无关的依赖管理和解析

**6. SOC 无关的应用程序开发**

- **依赖抽象**：构建系统屏蔽了用户对复杂 SOC 特定依赖细节的了解
- **统一 API 接口**：在不同 HPM SOC 系列上一致的开发体验
- **自动配置**：SOC 特定设置、内存布局和外设配置自动处理
- **专注于应用逻辑**：开发者可以专注于应用程序开发，而无需担心底层 SOC 复杂性
- **无缝 SOC 迁移**：在不同 SOC 变体之间轻松移植应用程序，代码更改最少

HPM SDK 提供了一个全面的基于 CMake 的构建系统，简化了 HPMicro 微控制器嵌入式应用程序的开发。构建系统处理：

架构概述
========

SDK 构建系统创建了一个分层架构，将用户应用程序与 SDK 内部实现分离，同时通过自定义板卡支持保持灵活性：

.. code-block:: text

   ┌─────────────────────────────────────────────────────────────────┐
   │                    User Application Layer                       │
   │  ┌─────────────────┐  ┌─────────────────┐  ┌──────────────────┐ │
   │  │   Application   │  │   Application   │  │   Application    │ │
   │  │   Source Code   │  │   Configuration │  │   Build Files    │ │
   │  │   (main.c, etc) │  │   (app.yaml)    │  │ (CMakeLists.txt) │ │
   │  └─────────────────┘  └─────────────────┘  └──────────────────┘ │
   └─────────────────────────────────────────────────────────────────┘
                                    │
                                    │ 依赖于
                                    ▼
   ┌──────────────────────────────────────────────────────────────────┐
   │                  Custom Board Configuration                      │
   │  ┌─────────────────┐  ┌─────────────────┐  ┌───────────────────┐ │
   │  │   Board YAML    │  │   Custom Board  │  │   Board-Specific  │ │
   │  │  Configuration  │  │   Definitions   │  │   Pin Mappings    │ │
   │  │   (board.yaml)  │  │(board.h/board.c)│  │(pinmux.h/pinmux.c)│ │
   │  └─────────────────┘  └─────────────────┘  └───────────────────┘ │
   └──────────────────────────────────────────────────────────────────┘
                                    │
                                    │ 引用
                                    ▼
   ┌────────────────────────────────────────────────────────────────┐
   │                        SDK Core Layer                          │
   │  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
   │  │      SOC        │  │     Drivers     │  │   Middleware    │ │
   │  │  Definitions    │  │   (GPIO, UART,  │  │  (FreeRTOS,     │ │
   │  │ (registers,     │  │   SPI, I2C,     │  │   USB, etc.)    │ │
   │  │  memory map)    │  │   etc.)         │  │                 │ │
   │  └─────────────────┘  └─────────────────┘  └─────────────────┘ │
   │  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
   │  │    CMake        │  │   Toolchain     │  │     IDE         │ │
   │  │   Build System  │  │  Integration    │  │  Integration    │ │
   │  │  (localization, │  │ (GCC, SEGGER,   │  │ (SES, IAR)      │ │
   │  │   optimization) │  │  Andes, etc.)   │  │                 │ │
   │  └─────────────────┘  └─────────────────┘  └─────────────────┘ │
   └────────────────────────────────────────────────────────────────┘

**SDK 版本灵活性：**

同一个应用程序可以通过简单地更改 HPM_SDK_BASE 路径来与不同的 SDK 版本配合工作：

.. code-block:: text

   应用程序 A (main.c, app.yaml, CMakeLists.txt)
   ├── SDK 版本 1.0.0 (HPM_SDK_BASE=/path/to/sdk_v1.0.0)
   ├── SDK 版本 1.1.0 (HPM_SDK_BASE=/path/to/sdk_v1.1.0)
   ├── SDK 版本 1.2.0 (HPM_SDK_BASE=/path/to/sdk_v1.2.0)
   └── 自定义 SDK 构建 (HPM_SDK_BASE=/path/to/custom_sdk)

   应用程序 B (main.c, app.yaml, CMakeLists.txt)
   ├── SDK 版本 1.0.0 (HPM_SDK_BASE=/path/to/sdk_v1.0.0)
   ├── SDK 版本 1.1.0 (HPM_SDK_BASE=/path/to/sdk_v1.1.0)
   └── SDK 版本 1.2.0 (HPM_SDK_BASE=/path/to/sdk_v1.2.0)

**此架构的主要优势：**

- **关注点分离**：用户应用程序完全独立于 SDK 内部实现
- **自定义板卡支持**：用户可以定义自定义板卡而无需修改 SDK 核心文件
- **版本灵活性**：通过更改 HPM_SDK_BASE 轻松切换 SDK 版本
- **团队协作**：仅共享应用程序和板卡文件，而不是整个 SDK
- **可维护性**：清晰的边界使调试和维护更容易

**构建系统概述：**

SDK 构建系统是一个综合解决方案，集成了多个组件以提供无缝的开发体验。它将基于 CMake 的项目管理与自动化工具链检测、智能依赖解析和跨平台兼容性相结合。该系统旨在处理嵌入式开发的复杂性，同时为应用程序开发者保持简单性。

**核心组件：**

构建系统处理：

- **工具链管理**: 自动检测和配置支持的工具链
- **板卡配置**: 板卡特定设置和外设配置
- **组件集成**: 驱动程序和中间件的模块化组件系统
- **多核支持**: 支持具有链接项目的双核应用程序
- **IDE 集成**: 为各种 IDE 生成项目文件
- **构建优化**: 可配置的构建类型和优化级别

基本工作流程
************

设置项目
========

1. **创建项目结构**

   .. code-block:: bash

      my_project/
      ├── CMakeLists.txt
      ├── app.yaml
      └── src/
         └── main.c

2. **配置 CMakeLists.txt**

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)
      
      # 查找并配置 HPM SDK
      find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
      
      # 定义项目
      project(my_application)
      
      # 添加源文件
      sdk_app_src(src/main.c)
      
      # 生成 IDE 项目（可选）
      generate_ide_projects()

3. **配置 app.yaml**

   .. code-block:: yaml

      debug:
        ses:
          auto_start_gdb_server: true
          gdb_server_port: 3333

构建项目
========

**基本构建命令：**

.. code-block:: bash

   # 配置和构建
   cmake -B build -S . -DBOARD=hpm6750evkmini
   cmake --build build

   # 或者一步完成
   cmake -B build -S . -DBOARD=hpm6750evkmini && cmake --build build

**构建配置：**

SDK 使用两种不同的构建类型概念：

1. **CMAKE_BUILD_TYPE** - 编译优化级别：

   - ``debug``: 调试构建，包含符号和最小优化
   - ``release``: 发布构建，完全优化

2. **HPM_BUILD_TYPE** - 链接器配置，用于内存布局：

   - ``ram``: 为 RAM 执行构建（默认）
   - ``flash_xip``: 为带 XIP 的 flash 执行构建
   - ``flash_sdram_xip``: 为带 SDRAM XIP 的 flash 执行构建
   - ``flash_hybrid_xip``: 为混合执行构建（代码在 ram 映射的 flash 内容中，数据在 RAM 中）
   - ``sec_core_img``: 为次核镜像构建

**设置构建类型：**

1. **编译类型 (CMAKE_BUILD_TYPE)：**

   .. code-block:: bash

      # 调试编译
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=debug

      # 发布编译
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release

2. **链接器类型 (HPM_BUILD_TYPE)：**

   **在 CMakeLists.txt 中（推荐用于项目特定构建）：**

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)
      
      # 为此项目设置链接器配置
      set(HPM_BUILD_TYPE "flash_xip")
      
      find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
      project(my_application)
      
      sdk_app_src(src/main.c)
      generate_ide_projects()

   **通过命令行（覆盖 CMakeLists.txt 设置）：**

   .. code-block:: bash

      # Flash XIP 执行
      cmake -B build -S . -DBOARD=hpm6750evkmini -DHPM_BUILD_TYPE=flash_xip

3. **组合示例：**

   .. code-block:: bash

      # 发布编译，带 flash XIP 执行
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release -DHPM_BUILD_TYPE=flash_xip

高级配置
********

自定义输出文件名
================

**输出文件名 (APP_NAME)：**

`APP_NAME` 变量控制输出文件名。如果未指定，默认为 "demo"。
输出文件名包括：

- `${APP_NAME}.elf` - 可执行文件
- `${APP_NAME}.bin` - 用于烧录的二进制文件
- `${APP_NAME}.map` - 内存映射文件
- `${APP_NAME}.asm` - 汇编列表

**在 CMakeLists.txt 中：**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   # 设置自定义应用程序名称
   set(APP_NAME "my_custom_app")
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   sdk_app_src(src/main.c)
   generate_ide_projects()

**通过命令行：**

.. code-block:: bash

   # 设置自定义应用程序名称
   cmake -B build -S . -DBOARD=hpm6750evkmini -DAPP_NAME=my_custom_app

**输出文件：**

`APP_NAME` 影响以下输出文件：

   - `${APP_NAME}.elf` - 可执行文件
   - `${APP_NAME}.bin` - 用于烧录的二进制文件
   - `${APP_NAME}.map` - 内存映射文件
   - `${APP_NAME}.asm` - 汇编列表

**组合高级示例：**

.. code-block:: bash

   # 发布编译，带 flash XIP 执行和自定义应用程序名称
   cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release -DHPM_BUILD_TYPE=flash_xip -DAPP_NAME=my_custom_app

Segger Embedded Studio (SES) 高级定制化
========================================

SDK 通过 CMake 变量和专门函数为 Segger Embedded Studio 项目提供广泛的定制化选项。

**工具链和编译器变体：**

.. code-block:: cmake

   # 在您的 CMakeLists.txt 文件中：
   
   # 设置工具链变体（Standard, Andes）
   # 注意：如果使用 Andes 扩展或 DSP 功能，必须使用 Andes 工具链
   set(SES_TOOLCHAIN_VARIANT "Andes")
   
   # 使用 Andes 工具链时，以下变体会自动设置：
   # - SES_COMPILER_VARIANT 默认为 "gcc"
   # - SES_ASSEMBLER_VARIANT 默认为 "gcc" 
   # - SES_LINKER_VARIANT 默认为 "gnu"
   
   # Andes 工具链适用于：
   # - Andes RISC-V 扩展（自定义指令）
   # - DSP 功能和优化
   # - Andes 特定编译器优化
   
   # 配置编译器变体（gcc, SEGGER）
   set(SES_COMPILER_VARIANT "gcc")
   
   # 设置汇编器变体（gcc, SEGGER）
   set(SES_ASSEMBLER_VARIANT "gcc")
   
   # 配置链接器变体（gnu, SEGGER）
   set(SES_LINKER_VARIANT "gnu")

**库 I/O 配置：**

.. code-block:: cmake

   # 为 SES 设置库 I/O 类型
   sdk_ses_opt_lib_io_type(STD)  # 选项：STD, SEGGER, Custom
   


**高级编译控制：**

.. code-block:: cmake

   # 设置 SES 特定编译选项
   sdk_ses_compile_options(-O2 -g -Wall)
   
   # 设置 SES 特定编译定义
   sdk_ses_compile_definitions(-DDEBUG -DVERSION=1)
   
   # 链接 SES 特定库
   sdk_ses_link_libraries(libm libc)

**每文件优化控制：**

.. code-block:: cmake

   # 为特定文件设置优化级别
   sdk_ses_set_optimization_level("src/critical.c" "Level 2 for speed")
   sdk_ses_set_optimization_level("src/debug.c" "None")
   
   # 配置特定优化
   sdk_ses_set_optimization_level_debug("src/test.c" "Level 1")
   sdk_ses_set_optimization_level_release("src/test.c" "Level 3 for more speed")

**段放置控制：**

.. code-block:: cmake

   # 控制特定文件的段放置
   sdk_ses_set_code_placement("src/vector.c" "vector_section")
   sdk_ses_set_data_placement("src/constants.c" "const_section")
   sdk_ses_set_isr_placement("src/interrupts.c" "isr_section")
   
   # 配置特定段放置
   sdk_ses_set_code_placement_debug("src/debug.c" "debug_code")
   sdk_ses_set_code_placement_release("src/debug.c" "release_code")

**链接器配置：**

.. code-block:: cmake

   # 设置 SES 特定链接器选项
   sdk_ses_ld_options(-Wl,--start-group -Wl,--end-group)
   
   # 链接 SES 特定输入库
   sdk_ses_ld_lib(custom_lib.a)

**调试和目标配置：**

.. code-block:: cmake

   # 配置调试目标连接
   sdk_ses_opt_debug_connection("GDB Server")  # 选项：GDB Server, J-Link
   
   # 配置 J-Link 速度
   sdk_ses_opt_debug_jlink_speed(4000)

**构建配置：**

.. code-block:: cmake

   # 注意：SES 构建配置由 CMAKE_BUILD_TYPE 自动确定
   # 优化级别使用 sdk_ses_set_optimization_level() 按文件设置

**命令行配置：**

.. code-block:: bash

   # 通过命令行配置 SES
   cmake -B build -S . -DBOARD=hpm6750evkmini \
         -DSES_TOOLCHAIN_VARIANT="Andes" \
         -DSES_COMPILER_VARIANT="gcc" \
         -DSES_ASSEMBLER_VARIANT="gcc" \
         -DSES_LINKER_VARIANT="gnu"

**生成的 SES 项目结构：**

构建后，将生成以下 SES 项目文件：

.. code-block:: text

   build/segger_embedded_studio/
   ├── ${CMAKE_PROJECT_NAME}.emProject          # 主项目文件
   └── ${CMAKE_PROJECT_NAME}.emSession          # 会话配置文件

**与 app.yaml 集成：**

您也可以在 app.yaml 文件中配置一些 SES 调试设置：

.. code-block:: yaml

   # app.yaml
   debug:
     ses:
       auto_start_gdb_server: true      # 自动启动 GDB 服务器（Yes/No）
       gdb_server_port: 3333           # GDB 服务器端口号
       gdb_server_reset_command: "reset halt"  # GDB 服务器重置命令

**注意：** app.yaml 中仅支持这三个 SES 调试设置。对于其他 SES 配置（工具链变体、编译选项、段放置等），必须使用上面描述的 CMake 变量和函数。

**高级 SES 函数参考：**

SDK 为 SES 定制化提供了几个专门函数：

.. code-block:: cmake

   # 设置 SES 项目选项
   sdk_ses_options("LIBRARY_IO_TYPE=STD")
   sdk_ses_options("debug_target_connection=GDB Server")
   sdk_ses_options("speed=4000")

**关于 sdk_ses_options() 的重要说明：**

`sdk_ses_options()` 函数接受的键值对将被保存到 SES 项目文件（`.emProject`）中。要发现可用的配置选项：

1. **使用 SDK 生成 SES 项目**
2. **在 Segger Embedded Studio 中打开项目**
3. **在 SES GUI 界面中修改设置** （项目选项、构建选项等）
4. **保存项目** - 这将更新 `.emProject` 文件
5. **检查 `.emProject` 文件** 以查看保存的键值对

通过这种方式，您可以识别哪些配置选项可用以及它们的有效值，以便在 `sdk_ses_options()` 中使用。

**关键选项的有效值：**

.. code-block:: text

   优化级别：
   - "None", "Level 0", "Level 1", "Level 2 for speed", "Level 2 balanced"
   - "Level 2 for size", "Level 3 for more speed"
   
   段类型：
   - "data", "const", "code", "isr", "bss", "vector"
   
   库 I/O 类型：
   - "STD", "SEGGER", "Custom"
   
   工具链变体：
   - "Standard" - 标准 GNU GCC 工具链
   - "Andes" - Andes RISC-V 工具链（Andes 扩展和 DSP 功能必需）
   
   编译器/汇编器/链接器变体：
   - "gcc", "SEGGER", "gnu"
   
   调试连接类型：
   - "GDB Server", "J-Link"

SDK 本地化
==========

SDK 提供了一个本地化机制，设计用于满足两个特定需求：

1. **SES 工程设置持久化**：使用户能够保存和传递在 CMake 构建系统中无法保留的 SES 工程设置
2. **版本管理工程精简化**：允许用户创建最小化的自包含工程，以便进行高效的版本管理

**本地化过程：**

本地化过程通过复制必要的 SDK 文件来创建自包含工程：

1. **依赖分析**：分析您工程的构建依赖关系
2. **文件收集**：识别所需的 SOC、板卡、中间件和源文件
3. **复制操作**：将必要文件复制到本地目录
4. **路径更新**：更新工程文件以使用本地路径

**使用 Ninja 调用 LOCALIZE_SDK：**

.. code-block:: bash

   # 使用 ninja 本地化 SDK
   ninja -C build localize_sdk
   
   # 取消本地化 SDK（恢复原始路径）
   ninja -C build unlocalize_sdk
   
   # 强制本地化（覆盖现有的本地化文件）
   ninja -C build localize_sdk -- -f

**本地化目录结构：**

本地化后，会创建一个新的目录结构：

.. code-block:: text

   your_project/
   ├── CMakeLists.txt                    # 原始工程文件
   ├── CMakeLists.txt.localized.bak      # 原始文件备份
   ├── src/                              # 您的源文件
   ├── hpm_sdk_localized/                # 本地化 SDK 文件
   │   ├── soc/                          # 所需的 SOC 文件
   │   ├── boards/                       # 所需的板卡文件
   │   ├── middleware/                   # 所需的中间件文件
   │   └── cmake/                        # 所需的 CMake 文件
   └── build/                            # 构建目录

**主要优势：**

- **SES 设置持久化**：保留 CMake 无法维护的 SES 工程配置
- **最小化版本控制**：在您的仓库中仅包含必要的 SDK 文件
- **自包含工程**：无需外部 SDK 依赖即可构建工程
- **更容易分发**：与他人共享工程而无需 SDK 安装要求

**使用场景：**

- **SES 工程定制化**：当您需要保留复杂的 SES 工程设置时
- **版本控制优化**：创建仅包含必要 SDK 组件的最小化仓库
- **工程分发**：与团队成员或外部用户共享自包含工程
- **构建环境一致性**：确保在不同开发环境中一致的构建

多核应用程序
============

对于双核应用程序，SDK 通过反映多核架构的分层目录结构支持链接项目：

1. **多核项目组织**

   **目录结构理念：**
   
   多核项目结构设计用于：

   - **分离核心职责**: 每个核心都有自己的目录，具有独立的构建配置
   - **维护构建独立性**: 每个核心可以独立构建或作为完整系统的一部分
   - **支持不同的构建类型**: Core0 通常运行主应用程序，而 Core1 运行专门任务
   - **支持并行开发**: 团队可以同时在不同核心上工作

   **标准目录布局：**

   .. code-block:: text

      multicore_app/
      ├── core0/                    # 主核心（主应用程序）
      │   ├── CMakeLists.txt       # Core0 构建配置
      │   ├── app.yaml             # Core0 应用程序配置
      │   ├── src/                 # Core0 源文件
      │   │   ├── main.c           # Core0 主入口点
      │   │   ├── app_init.c       # Core0 初始化
      │   │   └── sec_core_img.c   # 生成的 Core1 镜像（自动生成）
      │   └── include/             # Core0 头文件
      │       └── app_config.h
      ├── core1/                    # 次核心（专门任务）
      │   ├── CMakeLists.txt       # Core1 构建配置
      │   ├── src/                 # Core1 源文件
      │   │   ├── demo.c           # Core1 主入口点
      │   │   └── core1_task.c     # Core1 特定任务
      │   └── include/             # Core1 头文件
      │       └── core1_config.h
      ├── shared/                   # 共享资源（可选）
      │   ├── include/             # 共享头文件
      │   │   ├── common.h         # 通用定义
      │   │   └── ipc.h            # 处理器间通信
      │   └── src/                 # 共享源文件
      │       └── ipc_common.c
      └── README.md                 # 项目文档

   **多核应用程序架构：**

   **从镜像角度：**
   
   多核应用程序通过将 Core1 的二进制文件转换为 C 数组并嵌入到 Core0 的项目中来实现：
   
   - **Core1 二进制生成**: Core1 使用 `sec_core_img` 构建类型编译为独立二进制文件
   - **二进制到 C 数组转换**: Core1 二进制文件使用专门工具转换为 C 数组格式
   - **嵌入过程**: C 数组写入 `sec_core_img.c` 并包含在 Core0 的源文件中
   - **单一 ELF 生成**: Core0 与嵌入的 Core1 镜像一起编译，生成包含两个核心的单一 ELF 文件
   
   **从运行时角度：**
   
   在运行时，Core0 管理 Core1 的执行生命周期：
   
   - **镜像加载**: Core0 将 Core1 镜像从嵌入的 C 数组复制到链接器脚本指定的内存位置
   - **入口点更新**: Core0 更新 Core1 的入口点，指向正确的内存位置
   - **Core1 释放**: Core0 释放 Core1 的复位状态，允许其独立执行代码
   - **独立执行**: 两个核心现在独立运行，Core0 管理系统，Core1 处理专门任务

2. **多核程序组织**

   **核心角色和职责：**
   
   - **Core0（主核心）**：

     - 运行主应用程序逻辑
     - 处理系统初始化和配置
     - 管理次核心生命周期
     - 包含生成的次核心镜像
     - 通常使用 `flash_xip` 构建类型
   
   - **Core1（次核心）**：

     - 运行专门任务（实时处理、通信等）
     - 初始化后独立运行
     - 通过 IPC 机制与 Core0 通信
     - 使用 `sec_core_img` 构建类型生成二进制镜像
   
   **构建过程流程：**
   
   1. **Core1 编译**: Core1 首先作为独立二进制文件编译
   2. **二进制到 C 数组**: Core1 二进制文件转换为 C 数组格式
   3. **文件生成**: C 数组写入 Core0 源目录中的 `sec_core_img.c`
   4. **Core0 编译**: Core0 与嵌入的 Core1 镜像一起编译
   5. **最终二进制**: 包含两个核心的单个二进制文件

3. **Core0 配置**

   **Core0 CMakeLists.txt：**

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)

      # 设置链接器配置为 flash XIP（推荐用于主核心）
      set(HPM_BUILD_TYPE "flash_xip")
      
      # 设置自定义应用程序名称（可选）
      set(APP_NAME "multicore_demo")

      find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
      project(multicore_core0)

      # 添加 Core0 源文件
      sdk_app_src(
          src/main.c
          src/app_init.c
          src/sec_core_img.c # 将由构建系统自动生成
      )
      
      # 添加头文件目录
      sdk_app_inc(include)
      
      # 生成 IDE 项目
      generate_ide_projects()

   **Core0 app.yaml：**

   .. code-block:: yaml

      linked_project:
        project_name: multicore_app/core1
        project_path: ../core1
        build_type: sec_core_img

4. **Core1 配置 (CMakeLists.txt)**

   **关键配置元素：**

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)

      # 次核心镜像生成必需
      set(HPM_BUILD_TYPE "sec_core_img")
      
      # 指定生成的 C 数组文件将放置的位置
      # 此文件包含 Core1 的二进制文件作为 C 数组，用于嵌入 Core0
      set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)

      find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
      project(multicore_core1)

      # 添加 Core1 源文件
      sdk_app_src(src/demo.c)
      
      # 生成 IDE 项目（Core1 可选）
      generate_ide_projects()

      **配置说明：**
       
       - **`HPM_BUILD_TYPE="sec_core_img"`**: 告诉构建系统将 Core1 编译为次核心镜像
       - **`SEC_CORE_IMG_C_ARRAY_OUTPUT`**: 指定转换后的 C 数组文件将放置的位置
       - **相对路径**: 使用 `../core0/src/` 将文件放置在 Core0 的源目录中
       - **文件命名**: 约定命名为 `sec_core_img.c` 以便清晰

5. **构建多核应用程序**

   **完整构建过程：**

   .. code-block:: bash

      # 导航到 Core0 目录（主应用程序）
      cd core0
      
      # 构建两个核心（Core1 镜像 + 带嵌入 Core1 的 Core0）
      cmake -B build -S . -DBOARD=hpm6750evkmini
      cmake --build build

   **构建输出结构：**

   .. code-block:: text

      core0/
      ├── src/
      │   ├── main.c                   # Core0 主源文件
      │   └── sec_core_img.c           # Core1 二进制转换为 C 数组（生成）
      └── build/
          ├── output/
          │   ├── ${APP_NAME}.elf      # 包含两个核心的最终二进制文件（默认：demo.elf）
          │   ├── ${APP_NAME}.bin      # 用于烧录的二进制格式（默认：demo.bin）
          │   ├── ${APP_NAME}.map      # 内存映射文件（默认：demo.map）
          │   └── ${APP_NAME}.asm      # 汇编列表（默认：demo.asm）
          ├── segger_embedded_studio/  # Segger IDE 项目文件
          └── iar_embedded_workbench/  # IAR IDE 项目文件

   **构建过程详情：**

   `sec_core_img.c` 文件通过以下过程生成：

   1. **Core1 编译**: Core1 作为独立二进制文件编译
   2. **二进制转换**: Core1 二进制文件转换为 C 数组格式
   3. **文件生成**: C 数组写入 Core0 `src/` 目录中的 `sec_core_img.c`
   4. **Core0 编译**: Core0 与来自 `sec_core_img.c` 的嵌入 Core1 镜像一起编译

   **单独核心开发：**

   .. code-block:: bash

      # 仅构建 Core0（Core1 生成空占位符）
      cmake -B build -S . -DBOARD=hpm6750evkmini -DDISABLE_LINKED_PROJECT_BUILD=ON
      cmake --build build

      # 独立构建 Core1（用于测试）
      cd ../core1
      cmake -B build -S . -DBOARD=hpm6750evkmini -DHPM_BUILD_TYPE=sec_core_img
      cmake --build build

5. **链接项目构建控制**

   `DISABLE_LINKED_PROJECT_BUILD` 选项为多核开发提供灵活性：

   **目的：**

   - **避免构建失败**: 防止链接项目编译失败导致当前项目构建失败
   - **Windows 路径长度问题**: 解决由 Windows 路径长度限制（260 字符限制）导致的编译失败，该限制影响 GNU GCC 工具
   - **开发工作流程**: 允许开发人员在一个核心仍在开发时专注于另一个核心
   - **增量开发**: 支持独立构建和测试各个核心

   **工作原理：**
   
   当 `DISABLE_LINKED_PROJECT_BUILD=ON` 时，构建系统：
   
   - 跳过链接项目的实际编译
   - 从链接项目的 `CMakeLists.txt` 中提取输出文件路径
   - 在该位置生成空占位符文件
   - 继续当前项目构建

   **使用场景：**
   
   - **核心开发**: 开发 core1 时，可以在 core1 完成之前构建 core0
   - **Windows 路径长度问题**: 当项目路径超过 Windows 的 260 字符限制时，导致 GNU GCC 工具在编译过程中失败
   - **调试**: 在另一个核心暂时损坏时专注于调试一个核心
   - **CI/CD**: 在链接项目可能不可用的自动化构建中
   - **测试**: 独立测试各个核心功能

   **Windows 路径长度问题详情：**
   
   Windows 路径长度限制（260 字符）通常影响多核构建，原因如下：
   
   - **嵌套目录结构**: 多核项目通常具有更深的目录层次结构（core0/core1/shared）
   - **长 SDK 路径**: HPM SDK 安装路径可能很长
   - **构建目录嵌套**: CMake 构建目录增加了额外的路径长度
   - **工具链路径**: Windows 上的 GNU GCC 工具链路径可能非常长
   - **生成的文件路径**: 中间构建文件创建更长的绝对路径
   
   当总路径长度超过 260 字符时，Windows 工具（包括 GNU GCC）可能会失败，出现以下错误：

   - "系统找不到指定的文件"
   - "路径太长" 错误
   - 链接项目构建期间的编译失败

   **示例工作流程：**

   .. code-block:: bash

      # 正常构建（两个核心）
      cmake -B build -S . -DBOARD=hpm6750evkmini
      cmake --build build

      # 仅构建 core0（core1 生成空文件）
      cmake -B build -S . -DBOARD=hpm6750evkmini -DDISABLE_LINKED_PROJECT_BUILD=ON
      cmake --build build

      # 稍后，当 core1 准备好时，正常构建
      cmake -B build -S . -DBOARD=hpm6750evkmini
      cmake --build build

自定义板卡配置
==============

对于自定义板卡或开发套件：

1. **创建板卡目录**
   
   .. code-block:: text

      boards/
      └── my_custom_board/
         ├── my_custom_board.yaml
         └── my_custom_board.cfg

   **重要提示：** 目录名称、YAML 文件名称和 CFG 文件名称必须完全相同（例如，`my_custom_board`）。任何不匹配都会导致构建系统无法找到您的自定义板卡配置。

2. **板卡 YAML 配置**

   板卡 YAML 文件定义硬件配置和构建设置：

   .. code-block:: yaml

      board:
          soc: HPM6750                    # the name of SOC used on this board
          device: HPM6750xVMx             # device name used for jlink connection
          openocd-soc: hpm6750-dual-core  # used for openocd soc config file name: hpm6750-dual-core.cfg
          openocd-probe: ft2232           # used for openocd probe config file name: ft2232.cfg
          on-board-ram:
            type: sdram
            size: 16M                     # the size of on-board ram will be used in linker script as external ram size
            width: 16bit
          on-board-flash:
            type: qspi-nor-flash
            size: 8M                      # the size of on-board flash which will be used in linker script
          # 可选：SDK 依赖关系维护（自定义板卡不需要）
          feature:
            - board_audio_in # PDM
            - board_audio_out # DAO
            - board_gpt_pin
            - board_motor_control
            - board_gpio_led
            - board_gpio_key
            - board_pwm_rgb_led
            - board_puart_pin
          # 可选：SDK 依赖关系维护（自定义板卡不需要）
          excluded_samples:
            - samples/motor_ctrl/bldc_hfi
            - samples/trace_recorder/rtthread-nano

   **注意：** `feature` 和 `excluded_samples` 字段都用于 SDK 依赖关系维护，自定义板卡配置不需要这些字段。`feature` 字段指定板卡特定的硬件功能以支持示例应用程序兼容性，而 `excluded_samples` 指定了由于硬件限制或兼容性问题而不应为此板卡构建的示例应用程序。

3. **使用自定义板卡**

   使用自定义板卡时，需要指定 `BOARD_SEARCH_PATH` 来告诉 CMake 在哪里找到您的自定义板卡配置：

   .. code-block:: bash

      # 指定自定义板卡目录的路径
      cmake -B build -S . -DBOARD=my_custom_board -DBOARD_SEARCH_PATH=/path/to/your/boards

   **示例：**
   
   如果您的自定义板卡位于 `/home/user/my_boards/my_custom_board/`，请使用：

   .. code-block:: bash

      cmake -B build -S . -DBOARD=my_custom_board -DBOARD_SEARCH_PATH=/home/user/my_boards

组件管理
========

SDK 使用基于组件的架构：

**可用组件：**

- **驱动程序**: gpio, uart, spi, i2c, adc, pwm 等
- **中间件**: freertos, lwip, fatfs 等
- **组件**: ppi, plb, i2c, spi, i2s_over_spi, pmbus, smbus, serial_nor, uart_lin, dma_mgr, tsw_phy, touch, sccb, segment_led, panel, log, jpeg, ipc_event_mgr, enet_phy, eeprom_emulation, debug_console, codec, camera, usb 等
- **实用工具**: sbrk, ffssi, swap, crc32, size_utils 等

**添加组件：**

组件是可重用的软件模块，提供特定功能。它们位于 `components/` 目录中，包括：

- **通信**: i2c, spi, uart_lin, smbus, pmbus, serial_nor
- **显示和接口**: panel, segment_led, camera, codec
- **系统服务**: dma_mgr, ipc_event_mgr, log, debug_console
- **专用硬件**: tsw_phy, enet_phy, eeprom_emulation
- **协议支持**: ppi, plb, i2s_over_spi, sccb, touch, jpeg

**实用工具：**

`utils/` 目录包含实用函数和工具：

- **内存管理**: sbrk（动态内存分配）
- **数据处理**: swap（字节序转换）, crc32（校验和计算）
- **文件系统**: ffssi（闪存文件系统）
- **构建工具**: size_utils（二进制大小分析）

这些实用工具在使用 SDK 时自动可用，无需显式添加到依赖项中。

构建优化
========

**优化级别：**

.. code-block:: bash

   # 调试构建（默认）
   cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=debug

   # 发布构建
   cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release

**自定义编译器标志：**

.. code-block:: cmake

   # 使用 SDK CMake 扩展
   sdk_compile_options("-O2")
   sdk_compile_definitions("NDEBUG=1")

编译器标志配置
==============

SDK 提供 CMake 函数以跨平台方式设置编译器标志：

**设置编译器选项：**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   # 为所有工具链添加编译器选项
   sdk_compile_options("-O2")
   sdk_compile_options("-DNDEBUG")
   sdk_compile_options("-Wall")
   
   # 添加条件编译器选项
   sdk_compile_options_ifdef(CONFIG_DEBUG "-g")
   sdk_compile_options_ifdef(CONFIG_RELEASE "-O3")
   
   sdk_app_src(src/main.c)
   generate_ide_projects()

**设置编译器定义：**

.. code-block:: cmake

   # 为所有工具链添加编译器定义
   sdk_compile_definitions("DEBUG_MODE=1")
   sdk_compile_definitions("VERSION_MAJOR=1")
   sdk_compile_definitions("VERSION_MINOR=0")
   
   # 添加条件编译器定义
   sdk_compile_definitions_ifdef(CONFIG_FEATURE_X "FEATURE_X_ENABLED=1")
   sdk_compile_definitions_ifndef(CONFIG_FEATURE_Y "FEATURE_Y_DISABLED=1")

**可用函数：**

- **sdk_compile_options(OPTIONS...)**: 为所有工具链添加编译器选项
- **sdk_compile_options_ifdef(CONDITION OPTIONS...)**: 如果条件已定义，则添加编译器选项
- **sdk_compile_options_ifndef(CONDITION OPTIONS...)**: 如果条件未定义，则添加编译器选项
- **sdk_compile_definitions(DEFINITIONS...)**: 为所有工具链添加编译器定义
- **sdk_compile_definitions_ifdef(CONDITION DEFINITIONS...)**: 如果条件已定义，则添加编译器定义
- **sdk_compile_definitions_ifndef(CONDITION DEFINITIONS...)**: 如果条件未定义，则添加编译器定义

**条件编译示例：**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   # 根据构建类型设置优化级别
   if(CMAKE_BUILD_TYPE STREQUAL "release")
       sdk_compile_options("-O3")
       sdk_compile_definitions("NDEBUG=1")
   else()
       sdk_compile_options("-O0", "-g")
       sdk_compile_definitions("DEBUG=1")
   endif()
   
   # 添加功能特定标志
   sdk_compile_definitions_ifdef(CONFIG_USB "USB_ENABLED=1")
   sdk_compile_definitions_ifdef(CONFIG_NETWORK "NETWORK_ENABLED=1")

**使用 EXTRA 标志：**

SDK 还通过 CMake 变量支持额外的编译器、链接器和汇编器标志：

.. code-block:: cmake

   # 设置额外的 C 编译器标志
   set(EXTRA_C_FLAGS "-fstack-protector-strong -fno-strict-aliasing")
   
   # 设置额外的链接器标志
   set(EXTRA_LD_FLAGS "-Wl,--gc-sections -Wl,--as-needed")
   
   # 设置额外的汇编器标志
   set(EXTRA_AS_FLAGS "-Wa,--no-warn")
   
   # 设置额外的链接器符号
   set(EXTRA_LD_SYMBOLS "--defsym=__heap_size__=0x8000")

**命令行用法：**

.. code-block:: bash

   # 通过命令行传递额外标志
   cmake -B build -S . -DBOARD=hpm6750evkmini -DEXTRA_C_FLAGS="-fstack-protector-strong" -DEXTRA_LD_FLAGS="-Wl,--gc-sections" -DEXTRA_AS_FLAGS="-Wa,--no-warn" -DEXTRA_LD_SYMBOLS="--defsym=__heap_size__=0x8000"
   
   # 通过命令行配置内存大小
   cmake -B build -S . -DBOARD=hpm6750evkmini -DHEAP_SIZE=0x8000 -DSTACK_SIZE=0x2000
   
   # 通过命令行配置 RISC-V 架构和 ABI
   cmake -B build -S . -DBOARD=hpm6750evkmini -DRV_ARCH="rv32imac_zicsr_zifencei" -DRV_ABI="ilp32"

**Nano Specs 配置：**

`HPM_SDK_LD_NO_NANO_SPECS` 变量控制是否使用 nano.specs 链接器规范文件，该文件为嵌入式系统提供优化的标准库函数实现。

**缺省行为：**

缺省情况下，SDK 会自动为 GCC 工具链构建包含 nano.specs，它提供：

- **优化的 printf/scanf**：更小、更快的 printf 和 scanf 函数实现
- **减少内存占用**：与完整标准库相比使用更少的 RAM 和 Flash
- **浮点支持**：为 printf 和 scanf 操作启用浮点支持

**禁用 Nano Specs：**

要禁用 nano.specs 并使用完整标准库实现：

.. code-block:: cmake

   # 在您的 CMakeLists.txt 文件中：
   
   # 禁用 nano.specs（使用完整标准库）
   set(HPM_SDK_LD_NO_NANO_SPECS 1)

**何时禁用 Nano Specs：**

- **完整 printf/scanf 功能**：当您需要 nano.specs 中不可用的完整 printf/scanf 功能时
- **自定义库实现**：当使用自定义或第三方标准库实现时
- **调试目的**：当您需要完整标准库调试功能时
- **专用应用程序**：对于需要特定标准库行为的应用程序

**使用示例：**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   # 为此项目禁用 nano.specs
   set(HPM_SDK_LD_NO_NANO_SPECS 1)
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   sdk_app_src(src/main.c)
   generate_ide_projects()

**命令行用法：**

.. code-block:: bash

   # 通过命令行禁用 nano.specs
   cmake -B build -S . -DBOARD=hpm6750evkmini -DHPM_SDK_LD_NO_NANO_SPECS=1

**自定义目标三元组配置：**

`CUSTOM_TARGET_TRIPLET` 变量允许您覆盖 SDK 交叉编译工具链使用的缺省目标三元组。当您有与 SDK 缺省命名约定不同的工具链时，这很有用。

**缺省行为：**

缺省情况下，SDK 使用 `riscv32-unknown-elf` 作为目标三元组，这意味着：

- **架构**：RISC-V 32位
- **供应商**：unknown（通用）
- **操作系统**：elf（裸机/嵌入式）

**使用自定义目标三元组：**

要使用不同的目标三元组（例如，用于不同的工具链安装）：

.. code-block:: cmake

   # 在您的 CMakeLists.txt 文件中：
   
   # 设置自定义目标三元组
   set(CUSTOM_TARGET_TRIPLET "riscv32-elf")

**常用目标三元组格式：**

.. code-block:: cmake

   # 标准 RISC-V 裸机（缺省）
   set(CUSTOM_TARGET_TRIPLET "riscv32-unknown-elf")

**工作原理：**

当您设置 `CUSTOM_TARGET_TRIPLET` 时，SDK 使用此值来构建交叉编译工具链前缀，而不是缺省的 `riscv32-unknown-elf`：

- **编译器**：`${CUSTOM_TARGET_TRIPLET}-gcc`
- **链接器**：`${CUSTOM_TARGET_TRIPLET}-ld`
- **二进制工具**：`${CUSTOM_TARGET_TRIPLET}-objcopy`、`${CUSTOM_TARGET_TRIPLET}-objdump` 等

**使用示例：**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   # 为特定工具链设置自定义目标三元组
   set(CUSTOM_TARGET_TRIPLET "riscv32-elf")
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   sdk_app_src(src/main.c)
   generate_ide_projects()

**命令行用法：**

.. code-block:: bash

   # 通过命令行设置自定义目标三元组
   cmake -B build -S . -DBOARD=hpm6750evkmini -DCUSTOM_TARGET_TRIPLET="riscv32-elf"

**工具链兼容性：**

目标三元组必须与您安装的工具链匹配。常见场景：

- **标准 RISC-V GCC**：使用 `riscv32-unknown-elf`
- **供应商特定工具链**：使用供应商特定三元组
- **自定义工具链**：使用您自定义工具链的前缀格式

**验证：**

要验证您的工具链使用正确的目标三元组：

.. code-block:: bash

   # 检查工具链是否存在
   ls $GNURISCV_TOOLCHAIN_PATH/bin/
   
   # 查找与您的 CUSTOM_TARGET_TRIPLET 匹配的文件
   # 示例：如果 CUSTOM_TARGET_TRIPLET="riscv32-elf"
   # 查找：riscv32-elf-gcc、riscv32-elf-ld 等

内存配置
========

SDK 提供了通过 CMake 变量配置堆和栈大小的内置支持，这些变量会自动控制链接脚本中对应的段。

**堆和栈大小配置：**

通过在项目的 CMakeLists.txt 文件中设置变量来配置堆和栈大小：

.. code-block:: cmake

   # 在您项目的 CMakeLists.txt 文件中：
   
   # 设置堆大小（默认：0x4000 = 16KB）
   set(HEAP_SIZE 0x8000)  # 32KB 堆
   
   # 设置栈大小（默认：0x4000 = 16KB）
   set(STACK_SIZE 0x2000)  # 8KB 栈

**缺省值和工作原理：**

如果未指定，SDK 会自动设置这些变量：

- **HEAP_SIZE**: 缺省值为 `0x4000` (16KB) - Segger 缺省堆大小
- **STACK_SIZE**: 缺省值为 `0x4000` (16KB) - Segger 缺省栈大小

SDK 自动将这些 CMake 变量转换为链接器符号：
- ``HEAP_SIZE`` → 链接脚本中的 ``_heap_size`` 符号
- ``STACK_SIZE`` → 链接脚本中的 ``_stack_size`` 符号

**大小格式支持：**

您可以用不同格式指定大小：

.. code-block:: cmake

   # 十六进制（推荐）
   set(HEAP_SIZE 0x8000)
   
   # 十进制
   set(HEAP_SIZE 32768)
   
   # 带 K/M 后缀
   set(HEAP_SIZE 32K)
   set(HEAP_SIZE 1M)

**RISC-V 架构配置：**

配置 RISC-V 架构和 ABI 设置：

.. code-block:: cmake

   # 在您项目的 CMakeLists.txt 文件中：
   
   # 设置 RISC-V 架构（缺省值：rv32imac）
   set(RV_ARCH "rv32imac_zicsr_zifencei")
   
   # 设置 RISC-V ABI（缺省值：ilp32）
   set(RV_ABI "ilp32")

**缺省值和工作原理：**

如果未指定，SDK 会自动设置这些变量：

- **RV_ARCH**: 缺省值为 `rv32imac` （32位 RISC-V，支持整数乘除、原子操作和压缩指令）
- **RV_ABI**: 缺省值为 `ilp32` （32位整数、长整型和指针）

SDK 自动添加必需的扩展：

- **CSR 支持**: 如果不存在且需要，自动添加 `_zicsr`
- **Fence.i 支持**: 如果不存在且需要，自动添加 `_zifencei`

**常用 RV_ARCH 值：**

.. code-block:: cmake

   # 基本 32位 RISC-V
   set(RV_ARCH "rv32imac")
   
   # 支持浮点运算
   set(RV_ARCH "rv32imafc")
   
   # 支持向量运算
   set(RV_ARCH "rv32imafdcv")
   
   # 自定义扩展
   set(RV_ARCH "rv32imac_zicsr_zifencei_zba_zbb")

**常用 RV_ABI 值：**

.. code-block:: cmake

   # 32位整数、长整型和指针（最常用）
   set(RV_ABI "ilp32")
   
   # 32位整数、64位长整型、32位指针
   set(RV_ABI "ilp32f")
   
   # 32位整数、64位长整型、32位指针，支持浮点
   set(RV_ABI "ilp32d")

**命令行用法：**

.. code-block:: bash

   # 通过命令行配置堆和栈大小
   cmake -B build -S . -DBOARD=hpm6750evkmini -DHEAP_SIZE=0x8000 -DSTACK_SIZE=0x2000

**工作原理：**

SDK 自动将这些 CMake 变量转换为链接器符号：

- ``HEAP_SIZE`` → 链接脚本中的 ``_heap_size`` 符号
- ``STACK_SIZE`` → 链接脚本中的 ``_stack_size`` 符号

链接脚本使用这些符号来定义堆和栈段：

.. code-block:: text

   # 在链接脚本（*.ld 文件）中：
   HEAP_SIZE = _heap_size;
   STACK_SIZE = _stack_size;
   
   .heap (NOLOAD) : {
       . = ALIGN(8);
       __heap_start__ = .;
       . += HEAP_SIZE;  # 使用配置的堆大小
       __heap_end__ = .;
   } > SDRAM
   
   .stack (NOLOAD) : {
       . = ALIGN(16);
       __stack_base__ = .;
       . += STACK_SIZE;  # 使用配置的栈大小
       . = ALIGN(16);
       PROVIDE (_stack = .);
   } > DLM

**大小格式支持：**

您可以用不同格式指定大小：

.. code-block:: cmake

   # 十六进制（推荐）
   set(HEAP_SIZE 0x8000)
   
   # 十进制
   set(HEAP_SIZE 32768)
   
   # 带 K/M 后缀
   set(HEAP_SIZE 32K)
   set(HEAP_SIZE 1M)

IDE 集成
========

SDK 通过 CMakeLists.txt 中的 `generate_ide_projects()` 函数支持多个 IDE：

**CMakeLists.txt 配置：**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   sdk_app_src(src/main.c)
   
   # 为所有支持的 IDE 生成项目
   generate_ide_projects()

**构建和生成项目：**

.. code-block:: bash

   # 配置和构建项目
   cmake -B build -S . -DBOARD=hpm6750evkmini
   cmake --build build

**生成的 IDE 项目：**

构建后，以下 IDE 项目文件将在 `build` 目录中生成：

- **Segger Embedded Studio**: `build/segger_embedded_studio/`
- **IAR Embedded Workbench**: `build/iar_embedded_workbench/`

**在 IDE 中打开：**

- **Segger Embedded Studio**: 打开 segger 目录中的 `.emProject` 文件
- **IAR Embedded Workbench**: 打开 iar 目录中的 `.ewp` 文件

最佳实践
********

项目组织
========

1. **使用清晰的目录结构**
   
   .. code-block:: text

      project/
      ├── CMakeLists.txt          # 主构建配置
      ├── app.yaml               # 应用程序配置
      ├── src/                   # 源文件
      │   ├── main.c
      │   └── app/
      ├── include/               # 头文件
      │   └── app/
      ├── config/                # 配置文件
      └── docs/                  # 文档

2. **模块化源文件组织**

   .. code-block:: cmake

         # CMakeLists.txt
      sdk_app_src(
         src/main.c
         src/app/gpio_handler.c
         src/app/uart_handler.c
      )

      sdk_app_inc(include/app)

配置管理
========

**环境特定配置**

   .. code-block:: bash

      # 开发
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=debug

      # 生产
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release

故障排除
********

常见问题和解决方案
==================

**构建失败，提示"Board not found"**

首先，检查这些基本问题：

.. code-block:: bash

   # 1. 验证板卡路径是否正确
   ls boards/ | grep your_board_name

   # 2. 验证板卡 YAML 文件是否存在且名称正确
   ls boards/your_board_name/your_board_name.yaml

   # 3. 检查板卡名称是否完全匹配（区分大小写）
   cmake -B build -S . -DBOARD=your_board_name

有两种情况需要考虑：

**情况 1：使用 SDK 标准板卡**

.. code-block:: bash

   # 检查 SDK 中可用的板卡
   ls boards/

   # 使用 SDK 中正确的板卡名称
   cmake -B build -S . -DBOARD=hpm6750evkmini

**情况 2：使用自定义板卡**

.. code-block:: bash

   # 确保自定义板卡目录结构正确
   # 目录名称必须与 YAML 和 CFG 文件名称完全匹配
   ls /path/to/your/custom/boards/

   # 指定 BOARD_SEARCH_PATH 来定位自定义板卡
   cmake -B build -S . -DBOARD=my_custom_board -DBOARD_SEARCH_PATH=/path/to/your/custom/boards

**缺少工具链**

.. code-block:: bash

   # 设置工具链路径
   export PATH=$PATH:/path/to/toolchain/bin

   # 或在 CMake 中指定
   cmake -B build -S . -DBOARD=hpm6750evkmini -DTOOLCHAIN_PATH=/path/to/toolchain

**多核构建问题**

缺省情况下，多核 Core0 工程构建依赖使用 GNU GCC 顺利编译 Core1 工程。但是 GNU GCC 在 Windows 上无法处理长路径，从而导致 Core1 工程无法编译，进而导致 Core0 构建失败。此时可以使用 `DISABLE_LINKED_PROJECT_BUILD` 绕过 Core1 编译。

.. code-block:: bash

   # 检查链接项目配置
   # 确保 SEC_CORE_IMG_C_ARRAY_OUTPUT 设置正确
   # 使用 DISABLE_LINKED_PROJECT_BUILD 在 Windows 上绕过 Core1 编译
   cmake -B build -S . -DBOARD=hpm6750evkmini -DDISABLE_LINKED_PROJECT_BUILD=ON

**IDE 项目生成失败**

.. code-block:: bash

   # 使用支持的生成器
   cmake -B build -S . -DBOARD=hpm6750evkmini -G "Unix Makefiles"

调试构建问题
============

1. **启用详细输出**

   .. code-block:: bash

      cmake -B build -S . -DBOARD=hpm6750evkmini --debug-output
      cmake --build build --verbose

2. **检查 CMake 缓存**

   .. code-block:: bash

      cat build/CMakeCache.txt | grep -i board
      cat build/CMakeCache.txt | grep -i toolchain

3. **验证配置**

   .. code-block:: bash

      cmake -B build -S . -DBOARD=hpm6750evkmini -L

示例
****

基本 GPIO 应用程序
==================

**项目结构：**

.. code-block:: text

   gpio_example/
   ├── CMakeLists.txt
   └── src/
      └── main.c

**CMakeLists.txt：**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(gpio_example)

   sdk_app_src(src/main.c)
   generate_ide_projects()

**main.c：**

.. code-block:: c

   #include "board.h"
   #include "hpm_gpio_drv.h"

   int main(void)
   {
      /* 初始化 GPIO */
      gpio_set_pin_output(HPM_GPIO0, GPIO_DI_GPIOA, 0);
      
      while (1) {
         /* 切换 LED */
         gpio_toggle_pin(HPM_GPIO0, GPIO_DI_GPIOA, 0);
         board_delay_ms(500);
      }
   }

**构建命令：**

.. code-block:: bash

   cmake -B build -S . -DBOARD=hpm6750evkmini
   cmake --build build

UART 通信示例
=============

**main.c：**

.. code-block:: c

   #include "board.h"
   #include "hpm_uart_drv.h"

   int main(void)
   {
      uart_config_t config;
      uart_default_config(HPM_UART0, &config);
      config.baudrate = 115200;
      uart_init(HPM_UART0, &config);
      
      while (1) {
         uart_send_byte(HPM_UART0, 'H');
         board_delay_ms(1000);
      }
   }

多核示例
========

**Core0 (app.yaml)：**

.. code-block:: yaml

   linked_project:
      project_name: multicore_example/core1
      project_path: ../core1
      build_type: sec_core_img

**Core1 (CMakeLists.txt)：**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   set(HPM_BUILD_TYPE "sec_core_img")
   set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(multicore_core1)

   sdk_app_src(src/core1_main.c)

**构建命令：**

.. code-block:: bash

   cd core0
   cmake -B build -S . -DBOARD=hpm6750evkmini
   cmake --build build

结论
****

HPM SDK CMake 构建系统为嵌入式应用程序开发提供了强大而灵活的基础。通过遵循本指南中概述的工作流程和最佳实践，您可以：

- **加速开发**: 使用预配置的组件和板卡支持
- **确保一致性**: 跨项目的标准化构建过程
- **简化调试**: 集成的 IDE 支持和调试工具
- **优化性能**: 可配置的构建类型和优化级别
- **支持多核**: 无缝的双核应用程序开发

有关详细的 API 参考，请参见 :ref:`cmake_intro`。有关其他示例和高级用法，请参考 SDK 中的 samples 目录。

.. _cmake_intro: ../cmake_intro.html
