.. _cmake_user_guide:

SDK CMake User Guide
####################

This guide provides comprehensive information on how to use the HPM SDK's CMake extensions for building embedded applications. It focuses on workflow, best practices, and practical examples rather than API reference (which is covered in :ref:`cmake_intro`).

.. contents:: Table of Contents
   :local:

Overview
********

This section provides a high-level introduction to the HPM SDK CMake build system and its key concepts.

Design Philosophy and Architecture
===================================

**Design Philosophy of SDK CMake Build System**

The HPM SDK CMake build system is designed with a fundamental principle: **decoupling user applications from the underlying SDK**. This design philosophy addresses several key challenges in embedded development:

**1. SDK Version Independence**

- Users can switch between different SDK versions within the same major version by simply changing the `SDK_BASE` path
- No need to modify application code when switching SDK versions
- Maintains application compatibility across SDK updates

**2. Reduced Communication Overhead in Team Development**

- Multiple developers can collaborate on applications based on the same SDK version
- Only application-related data needs to be exchanged, not the entire SDK
- Eliminates the need to share large SDK repositories between team members
- Reduces version synchronization issues and merge conflicts

**3. Complete Decoupling Through Custom Board Support**

- Users can define custom board configurations without modifying SDK core files
- Applications become completely independent of SDK internals
- Enables rapid prototyping and customization for specific hardware requirements
- Improves development efficiency by isolating application logic from SDK complexity

**4. Modular Architecture Benefits**

- Clear separation of concerns between application and SDK layers
- Easier maintenance and debugging of application-specific issues
- Simplified dependency management and build configuration
- Enhanced portability across different development environments


**5. Cross-Platform Build System**

- **Windows, macOS, and Linux Support**: Full compatibility across major operating systems
- **Unified Build Commands**: Same CMake commands work identically on all platforms
- **Automatic Toolchain Detection**: Automatically detects and configures platform-specific toolchains
- **IDE Integration**: Generates platform-appropriate project files for various IDEs
- **Path Handling**: Automatic path conversion between different operating system conventions
- **Dependency Resolution**: Platform-agnostic dependency management and resolution

**6. SOC-Agnostic Application Development**

- **Dependency Abstraction**: Build system shields users from complex SOC-specific dependency details
- **Unified API Interface**: Consistent development experience across different HPM SOC families
- **Automatic Configuration**: SOC-specific settings, memory layouts, and peripheral configurations are handled automatically
- **Focus on Application Logic**: Developers can concentrate on application development without worrying about underlying SOC complexities

Architecture Overview
=======================

hpm_sdk's build system creates a layered architecture that separates user applicat with rom SDK internals while maintaincodeflexibility through custom board support:

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
                                    │ depends on
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
                                    │ references
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

**SDK Version Flexibility:**

The same application can work with different SDK versions by simply changing the HPM_SDK_BASE path:

.. code-block:: text

   Application A (main.c, app.yaml, CMakeLists.txt)
   ├── SDK Version 1.0.0 (HPM_SDK_BASE=/path/to/sdk_v1.0.0)
   ├── SDK Version 1.1.0 (HPM_SDK_BASE=/path/to/sdk_v1.1.0)
   ├── SDK Version 1.2.0 (HPM_SDK_BASE=/path/to/sdk_v1.2.0)
   └── Custom SDK Build  (HPM_SDK_BASE=/path/to/custom_sdk)

   Application B (main.c, app.yaml, CMakeLists.txt)
   ├── SDK Version 1.0.0 (HPM_SDK_BASE=/path/to/sdk_v1.0.0)
   ├── SDK Version 1.1.0 (HPM_SDK_BASE=/path/to/sdk_v1.1.0)
   └── SDK Version 1.2.0 (HPM_SDK_BASE=/path/to/sdk_v1.2.0)

**Key Benefits of This Architecture:**

- **Separation of Concerns**: User applications are completely independent of SDK internals
- **Custom Board Support**: Users can define custom boards without modifying SDK core files
- **Version Flexibility**: Easy switching between SDK versions by changing HPM_SDK_BASE
- **Team Collaboration**: Share only application and board files, not the entire SDK
- **Maintainability**: Clear boundaries make debugging and maintenance easier

**Build System Overview:**

The SDK build system is a comprehensive solution that integrates multiple components to provide a seamless development experience. It combines CMake-based project management with automated toolchain detection, intelligent dependency resolution, and cross-platform compatibility. The system is designed to handle the complexity of embedded development while maintaining simplicity for application developers.

**Core Components:**

The build system handles:

- **Toolchain Management**: Automatic detection and configuration of supported toolchains
- **Board Configuration**: Board-specific settings and peripheral configurations
- **Component Integration**: Modular component system for drivers and middleware
- **Multi-core Support**: Support for dual-core applications with linked projects
- **IDE Integration**: Generation of project files for various IDEs
- **Build Optimization**: Configurable build types and optimization levels

Basic Workflow
**************

Setting Up Your Project
=======================

1. **Create Project Structure**

   .. code-block:: bash

      my_project/
      ├── CMakeLists.txt
      ├── app.yaml
      └── src/
          └── main.c

2. **Configure CMakeLists.txt**

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)

      # Find and configure the HPM SDK
      find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

      # Define your project
      project(my_application)

      # Add source files
      sdk_app_src(src/main.c)

      # Generate IDE projects (optional)
      generate_ide_projects()

3. **Configure app.yaml**

   .. code-block:: yaml

      debug:
        ses:
          auto_start_gdb_server: true
          gdb_server_port: 3333

Building Your Project
=====================

**Basic Build Commands:**

.. code-block:: bash

   # Configure and build
   cmake -B build -S . -DBOARD=hpm6750evkmini
   cmake --build build

   # Or in one step
   cmake -B build -S . -DBOARD=hpm6750evkmini && cmake --build build

**Build Configuration:**

The SDK uses two different build type concepts:

1. **CMAKE_BUILD_TYPE** - Compilation optimization level:

   - ``debug``: Debug build with symbols and minimal optimization
   - ``release``: Release build with full optimization

2. **HPM_BUILD_TYPE** - Linker configuration for memory layout:

   - ``ram``: Build for RAM execution (default)
   - ``flash_xip``: Build for flash execution with XIP
   - ``flash_sdram_xip``: Build for flash execution with SDRAM XIP
   - ``flash_hybrid_xip``: Build for hybrid execution (code in ram-mapped flash content, data in RAM)
   - ``sec_core_img``: Build for secondary core image

**Setting Build Types:**

1. **Compilation Type (CMAKE_BUILD_TYPE):**

   .. code-block:: bash

      # Debug compilation
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=debug

      # Release compilation
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release

2. **Linker Type (HPM_BUILD_TYPE):**

   **In CMakeLists.txt (Recommended for project-specific builds):**

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)
      
      # Set linker configuration for this project
      set(HPM_BUILD_TYPE "flash_xip")
      
      find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
      project(my_application)
      
      sdk_app_src(src/main.c)
      generate_ide_projects()

   **Via Command Line (Override CMakeLists.txt setting):**

   .. code-block:: bash

      # Flash XIP execution
      cmake -B build -S . -DBOARD=hpm6750evkmini -DHPM_BUILD_TYPE=flash_xip

3. **Combined Example:**

   .. code-block:: bash

      # Release compilation with flash XIP execution
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release -DHPM_BUILD_TYPE=flash_xip

Advanced Configuration
**********************

Customize Output File Names
===========================

**Output File Name (APP_NAME):**

The `APP_NAME` variable controls the output file names. If not specified, it defaults to "demo".
The output file names are:

- `${APP_NAME}.elf` - Executable file
- `${APP_NAME}.bin` - Binary file for flashing
- `${APP_NAME}.map` - Memory map file
- `${APP_NAME}.asm` - Assembly listing

**In CMakeLists.txt:**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   # Set custom application name
   set(APP_NAME "my_custom_app")
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   sdk_app_src(src/main.c)
   generate_ide_projects()

**Via Command Line:**

.. code-block:: bash

   # Set custom application name
   cmake -B build -S . -DBOARD=hpm6750evkmini -DAPP_NAME=my_custom_app

**Output Files:**

The `APP_NAME` affects the following output files:

   - `${APP_NAME}.elf` - Executable file
   - `${APP_NAME}.bin` - Binary file for flashing
   - `${APP_NAME}.map` - Memory map file
   - `${APP_NAME}.asm` - Assembly listing

**Combined Advanced Example:**

.. code-block:: bash

   # Release compilation with flash XIP execution and custom app name
   cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release -DHPM_BUILD_TYPE=flash_xip -DAPP_NAME=my_custom_app

Segger Embedded Studio (SES) Advanced Customization
====================================================

The SDK provides extensive customization options for Segger Embedded Studio projects through CMake variables and specialized functions.

**Toolchain and Compiler Variants:**

.. code-block:: cmake

   # In your CMakeLists.txt file:
   
   # Set toolchain variant (Standard, Andes)
   set(SES_TOOLCHAIN_VARIANT "Standard")
   
   # Configure compiler variant (gcc, SEGGER)
   set(SES_COMPILER_VARIANT "gcc")
   
   # Set assembler variant (gcc, SEGGER)
   set(SES_ASSEMBLER_VARIANT "SEGGER")
   
   # Configure linker variant (gnu, SEGGER)
   set(SES_LINKER_VARIANT "SEGGER")

**Library I/O Configuration:**

.. code-block:: cmake

   # Set library I/O type for SES
   sdk_ses_opt_lib_io_type(STD)  # Options: STD, SEGGER, Custom


**Advanced Compilation Control:**

.. code-block:: cmake

   # Set SES-specific compile options
   sdk_ses_compile_options(-O2 -g -Wall)
   
   # Set SES-specific compile definitions
   sdk_ses_compile_definitions(-DDEBUG -DVERSION=1)
   
   # Link SES-specific libraries
   sdk_ses_link_libraries(libm libc)

**Per-File Optimization Control:**

.. code-block:: cmake

   # Set optimization level for specific files
   sdk_ses_set_optimization_level("src/critical.c" "Level 2 for speed")
   sdk_ses_set_optimization_level("src/debug.c" "None")
   
   # Configuration-specific optimization
   sdk_ses_set_optimization_level_debug("src/test.c" "Level 1")
   sdk_ses_set_optimization_level_release("src/test.c" "Level 3 for more speed")

**Section Placement Control:**

.. code-block:: cmake

   # Control section placement for specific files
   sdk_ses_set_code_placement("src/vector.c" "vector_section")
   sdk_ses_set_data_placement("src/constants.c" "const_section")
   sdk_ses_set_isr_placement("src/interrupts.c" "isr_section")
   
   # Configuration-specific section placement
   sdk_ses_set_code_placement_debug("src/debug.c" "debug_code")
   sdk_ses_set_code_placement_release("src/debug.c" "release_code")

**Linker and Memory Configuration:**

.. code-block:: cmake

   # Set SES-specific linker options
   sdk_ses_ld_options(-Wl,--start-group -Wl,--end-group)
   
   # Link SES-specific input libraries
   sdk_ses_ld_lib(custom_lib.a)
   
   # Configure memory layout
   set(SES_MEMORY_LAYOUT "Custom")

**Debug and Target Configuration:**

.. code-block:: cmake

   # Configure debug target connection
   set(SES_DEBUG_TARGET_CONNECTION "GDB Server")  # Options: GDB Server, J-Link
   
   # GDB Server configuration
   set(SES_GDB_SERVER_TYPE "Custom")
   set(SES_GDB_SERVER_COMMAND "openocd")
   set(SES_GDB_SERVER_ARGS "-f board/hpm6750evkmini.cfg")
   set(SES_GDB_SERVER_PORT "3333")
   
   # J-Link configuration
   set(SES_JLINK_SPEED "4000")
   set(SES_JLINK_DEVICE "HPM6750xVMx")

**Build Configuration:**

.. code-block:: cmake

   # Set SES build configuration
   set(SES_BUILD_CONFIGURATION "Release")  # Options: Debug, Release, RelWithDebInfo
   
   # Configure optimization level
   set(SES_OPTIMIZATION_LEVEL "2")  # 0=None, 1=Basic, 2=Full, 3=Size
   
   # Set warning level
   set(SES_WARNING_LEVEL "All")  # Options: None, Basic, All, Extra

**Command Line Configuration:**

.. code-block:: bash

   # Configure SES via command line
   cmake -B build -S . -DBOARD=hpm6750evkmini \
         -DSES_TOOLCHAIN_VARIANT="Standard" \
         -DSES_COMPILER_VARIANT="gcc" \
         -DSES_ASSEMBLER_VARIANT="SEGGER" \
         -DSES_LINKER_VARIANT="SEGGER" \
         -DSES_DEBUG_TARGET_CONNECTION="GDB Server" \
         -DSES_GDB_SERVER_COMMAND="openocd" \
         -DSES_GDB_SERVER_ARGS="-f board/hpm6750evkmini.cfg"

**Generated SES Project Structure:**

After building, the following SES project files will be generated:

.. code-block:: text

   build/segger_embedded_studio/
   ├── ${CMAKE_PROJECT_NAME}.emProject          # Main project file
   └── ${CMAKE_PROJECT_NAME}.emSession          # Session configuration

**Integration with app.yaml:**

You can also configure some SES debug settings in your app.yaml file:

.. code-block:: yaml

   # app.yaml
   debug:
     ses:
       auto_start_gdb_server: true      # Auto-start GDB server (Yes/No)
       gdb_server_port: 3333           # GDB server port number
       gdb_server_reset_command: "reset halt"  # GDB server reset command

**Note:** Only these three SES debug settings are supported in app.yaml. For other SES configurations (toolchain variants, compiler options, section placement, etc.), you must use CMake variables and functions as described above.

**Advanced SES Functions Reference:**

The SDK provides several specialized functions for SES customization:

.. code-block:: cmake

   # Set SES project options
   sdk_ses_options("LIBRARY_IO_TYPE=STD")
   sdk_ses_options("debug_target_connection=GDB Server")
   sdk_ses_options("linker_printf_fp_enabled=1")
   sdk_ses_options("linker_scanf_fp_enabled=1")
   sdk_ses_options("linker_printf_fmt_level=F4000"
   
   Debug Connection Types:
   - "GDB Server", "J-Link# Note: If using Andes extensions or DSP features, you MUST use Andes toolchain
   Andes")
   
   # When using Andes toolchain, the following variants are automatically set:
   # - ES_COMPILER_VARIANT defauls to "gcc"
   # - SES_ASSEMBLER_VARIANT defults to "gcc" 
   # - SES_LINKER_VARIANT defaults to "gnu"
   
   # Aes toolchain is required for:
   # - Andes RISC-V extensions (custom instructions)
   # - DSP features and optimizations
   # - Andes-specific compiler gnu - Standard GNU GCC toolchain
   - - Andes RISC-V toolchain (required for Andes extensions and DSP features))Important Note about sdk_ses_options():**

The `sdk_ses_options()` function accepts key-value pairs that will be saved to the SES project file (`.emProject`). To discover available configuration options:

1. **Generate the SES project** using the SDK
2. **Open the project in Segger Embedded Studio**
3. **Modify settings** in the SES GUI interface (Project Options, Build Options, etc.)
4. **Save the project** - this will update the `.emProject` file
5. **Examine the `.emProject` file** to see the key-value pairs that were saved

This way you can identify which configuration options are available and their valid values for use with `sdk_ses_options()`.

SDK Localization
================

The SDK provides a localization mechanism designed to address two specific needs:

1. **SES Project Settings Persistence**: Enable users to save and transfer SES project settings that cannot be preserved in the CMake build system
2. **Project Simplification for Version Control**: Allow users to create minimal, self-contained projects for efficient version management

**Localization Process:**

The localization process creates a self-contained project by copying only the necessary SDK files:

1. **Dependency Analysis**: Analyzes your project's build dependencies
2. **File Collection**: Identifies required SOC, board, middleware, and source files
3. **Copy Operation**: Copies necessary files to a local directory
4. **Path Updates**: Updates project files to use local paths

**Using LOCALIZE_SDK with Ninja:**

.. code-block:: bash

   # Localize the SDK using ninja
   ninja -C build localize_sdk
   
   # Unlocalize the SDK (restore original paths)
   ninja -C build unlocalize_sdk
   
   # Force localization (overwrite existing localized files)
   ninja -C build localize_sdk -- -f

**Localization Directory Structure:**

After localization, a new directory structure is created:

.. code-block:: text

   your_project/
   ├── CMakeLists.txt                    # Original project file
   ├── CMakeLists.txt.localized.bak      # Backup of original file
   ├── src/                              # Your source files
   ├── hpm_sdk_localized/                # Localized SDK files
   │   ├── soc/                          # Required SOC files
   │   ├── boards/                       # Required board files
   │   ├── middleware/                   # Required middleware files
   │   └── cmake/                        # Required CMake files
   └── build/                            # Build directory

**Key Benefits:**

- **SES Settings Persistence**: Preserve SES project configurations that CMake cannot maintain
- **Minimal Version Control**: Include only essential SDK files in your repository
- **Self-contained Projects**: Build projects without external SDK dependencies
- **Easier Distribution**: Share projects with others without SDK installation requirements

**Use Cases:**

- **SES Project Customization**: When you need to preserve complex SES project settings
- **Version Control Optimization**: Create minimal repositories with only necessary SDK components
- **Project Distribution**: Share self-contained projects with team members or external users
- **Build Environment Consistency**: Ensure consistent builds across different development environments

Multi-core Applications
=======================

For dual-core applications, the SDK supports linked projects through a hierarchical directory structure that reflects the multi-core architecture:

1. **Multi-core Project Organization**

   **Directory Structure Philosophy:**
   
   The multi-core project structure is designed to:

   - **Separate Core Responsibilities**: Each core has its own directory with independent build configuration
   - **Maintain Build Independence**: Each core can be built independently or as part of the complete system
   - **Support Different Build Types**: Core0 typically runs the main application, while Core1 runs specialized tasks
   - **Enable Parallel Development**: Teams can work on different cores simultaneously

   **Standard Directory Layout:**

   .. code-block:: text

      multicore_app/
      ├── core0/                    # Primary core (main application)
      │   ├── CMakeLists.txt       # Core0 build configuration
      │   ├── app.yaml             # Core0 application configuration
      │   ├── src/                 # Core0 source files
      │   │   ├── main.c           # Core0 main entry point
      │   │   ├── app_init.c       # Core0 initialization
      │   │   └── sec_core_img.c   # Generated Core1 image (auto-generated)
      │   └── include/             # Core0 header files
      │       └── app_config.h
      ├── core1/                    # Secondary core (specialized tasks)
      │   ├── CMakeLists.txt       # Core1 build configuration
      │   ├── src/                 # Core1 source files
      │   │   ├── demo.c           # Core1 main entry point
      │   │   └── core1_task.c     # Core1 specific tasks
      │   └── include/             # Core1 header files
      │       └── core1_config.h
      ├── shared/                   # Shared resources (optional)
      │   ├── include/             # Shared header files
      │   │   ├── common.h         # Common definitions
      │   │   └── ipc.h            # Inter-processor communication
      │   └── src/                 # Shared source files
      │       └── ipc_common.c
      └── README.md                 # Project documentation

   **Multi-core Application Architecture:**

   **From Image Perspective:**
   
   The multi-core application works by converting Core1's binary into a C array that is embedded within Core0's project:
   
   - **Core1 Binary Generation**: Core1 is compiled as a standalone binary with `sec_core_img` build type
   - **Binary to C Array Conversion**: The Core1 binary is converted to a C array format using specialized tools
   - **Embedding Process**: The C array is written to `sec_core_img.c` and included in Core0's source files
   - **Single ELF Generation**: Core0 is compiled with the embedded Core1 image, resulting in a single ELF file containing both cores
   
   **From Runtime Perspective:**
   
   At runtime, Core0 manages the Core1 execution lifecycle:
   
   - **Image Loading**: Core0 copies the Core1 image from the embedded C array to the memory location specified in the linker script
   - **Entry Point Update**: Core0 updates Core1's entry point to point to the correct memory location
   - **Core1 Release**: Core0 releases Core1 from reset, allowing it to execute its code independently
   - **Independent Execution**: Both cores now run independently, with Core0 managing the system and Core1 handling specialized tasks

2. **Multi-core Program Organization**

   **Core Roles and Responsibilities:**
   
   - **Core0 (Primary Core)**:

     - Runs the main application logic
     - Handles system initialization and configuration
     - Manages the secondary core lifecycle
     - Contains the generated secondary core image
     - Typically uses `flash_xip` build type
   
   - **Core1 (Secondary Core)**:

     - Runs specialized tasks (real-time processing, communication, etc.)
     - Operates independently after initialization
     - Communicates with Core0 through IPC mechanisms
     - Uses `sec_core_img` build type to generate a binary image
   
   **Build Process Flow:**
   
   1. **Core1 Compilation**: Core1 is compiled first as a standalone binary
   2. **Binary to C Array**: Core1 binary is converted to a C array format
   3. **File Generation**: The C array is written to `sec_core_img.c` in Core0's source directory
   4. **Core0 Compilation**: Core0 is compiled with the embedded Core1 image
   5. **Final Binary**: Single binary containing both cores

3. **Core0 Configuration**

   **Core0 CMakeLists.txt:**

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)

      # Set linker configuration for flash XIP (recommended for primary core)
      set(HPM_BUILD_TYPE "flash_xip")
      
      # Set custom application name (optional)
      set(APP_NAME "multicore_demo")

      find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
      project(multicore_core0)

      # Add Core0 source files
      sdk_app_src(
          src/main.c
          src/app_init.c
          src/sec_core_img.c # it will be auto-generated by build system
      )
      
      # Add header file directories
      sdk_app_inc(include)
      
      # Generate IDE projects
      generate_ide_projects()

   **Core0 app.yaml:**

   .. code-block:: yaml

      linked_project:
        project_name: multicore_app/core1
        project_path: ../core1
        build_type: sec_core_img

4. **Core1 Configuration (CMakeLists.txt)**

   **Key Configuration Elements:**

   .. code-block:: cmake

      cmake_minimum_required(VERSION 3.13)

      # Essential for secondary core image generation
      set(HPM_BUILD_TYPE "sec_core_img")
      
      # Specify where the generated C array file will be placed
      # This file contains Core1's binary as a C array for embedding in Core0
      set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)

      find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
      project(multicore_core1)

      # Add Core1 source files
      sdk_app_src(src/demo.c)
      
      # Generate IDE projects (optional for Core1)
      generate_ide_projects()

   **Configuration Explanation:**
      
      - **`HPM_BUILD_TYPE="sec_core_img"`**: Tells the build system to compile Core1 as a secondary core image
      - **`SEC_CORE_IMG_C_ARRAY_OUTPUT`**: Specifies where the converted C array file will be placed
      - **Relative Path**: Uses `../core0/src/` to place the file in Core0's source directory
      - **File Naming**: Conventionally named `sec_core_img.c` for clarity

5. **Building Multi-core Applications**

   **Complete Build Process:**

   .. code-block:: bash

      # Navigate to Core0 directory (main application)
      cd core0
      
      # Build both cores (Core1 image + Core0 with embedded Core1)
      cmake -B build -S . -DBOARD=hpm6750evkmini
      cmake --build build

   **Build Output Structure:**

   .. code-block:: text

      core0/
      ├── src/
      │   ├── main.c                   # Core0 main source
      │   └── sec_core_img.c           # Core1 binary converted to C array (generated)
      └── build/
          ├── output/
          │   ├── ${APP_NAME}.elf      # Final binary with both cores (default: demo.elf)
          │   ├── ${APP_NAME}.bin      # Binary format for flashing (default: demo.bin)
          │   ├── ${APP_NAME}.map      # Memory map file (default: demo.map)
          │   └── ${APP_NAME}.asm      # Assembly listing (default: demo.asm)
          ├── segger_embedded_studio/  # Segger IDE project files
          └── iar_embedded_workbench/  # IAR IDE project files

   **Build Process Details:**

   The `sec_core_img.c` file is generated through the following process:

   1. **Core1 Compilation**: Core1 is compiled as a standalone binary
   2. **Binary Conversion**: The Core1 binary is converted to a C array format
   3. **File Generation**: The C array is written to `sec_core_img.c` in Core0's `src/` directory
   4. **Core0 Compilation**: Core0 is compiled with the embedded Core1 image from `sec_core_img.c`

   **Individual Core Development:**

   .. code-block:: bash

      # Build Core0 only (Core1 generates empty placeholder)
      cmake -B build -S . -DBOARD=hpm6750evkmini -DDISABLE_LINKED_PROJECT_BUILD=ON
      cmake --build build

      # Build Core1 standalone (for testing)
      cd ../core1
      cmake -B build -S . -DBOARD=hpm6750evkmini -DHPM_BUILD_TYPE=sec_core_img
      cmake --build build

5. **Linked Project Build Control**

   The `DISABLE_LINKED_PROJECT_BUILD` option provides flexibility in multi-core development:

   **Purpose:**

   - **Avoid Build Failures**: Prevents linked project compilation failures from causing the current project build to fail
   - **Development Workflow**: Allows developers to focus on one core while the other core is still under development
   - **Incremental Development**: Enables building and testing individual cores independently

   **How it Works:**
   
   When `DISABLE_LINKED_PROJECT_BUILD=ON`, the build system:
   
   - Skips the actual compilation of the linked project
   - Extracts the output file path from the linked project's `CMakeLists.txt`
   - Generates a placeholder file with C error macro at that location
   - Continues with the current project build

   **Placeholder File Details:**
   
   The generated placeholder file contains:
   
   - A warning comment explaining that this is an automatically generated file
   - A C preprocessor `#error` directive that will cause compilation to fail
   - Clear instructions on how to obtain the actual file
   
   This ensures that:
   
   - Users cannot accidentally use the placeholder file in their builds
   - Clear error messages guide users to complete the linked project build
   - The build system can continue without the actual linked project output
   
   **Use Cases:**
   
   - **Core Development**: When developing core1, you can build core0 without waiting for core1 to be complete
   - **Debugging**: Focus on debugging one core while the other is temporarily broken
   - **CI/CD**: In automated builds where linked projects might not be available
   - **Testing**: Test individual core functionality independently

   **Example Workflow:**

   .. code-block:: bash

      # Normal build (both cores)
      cmake -B build -S . -DBOARD=hpm6750evkmini
      cmake --build build

      # Build core0 only (core1 generates placeholder file with error macro)
      cmake -B build -S . -DBOARD=hpm6750evkmini -DDISABLE_LINKED_PROJECT_BUILD=ON
      cmake --build build

      # Later, when core1 is ready, build normally
      cmake -B build -S . -DBOARD=hpm6750evkmini
      cmake --build build

   **Example Placeholder File Content:**

   When `DISABLE_LINKED_PROJECT_BUILD=ON` is used, the generated placeholder file will contain:

   .. code-block:: c

      /*
       * WARNING: This file is automatically generated by build_linked_project.py
       * This is NOT the actual SEC_CORE_IMG file!
       * 
       * To get the actual SEC_CORE_IMG, you need to complete the compilation
       * of the linked project by setting build_linked=true
       */

      #error "SEC_CORE_IMG not available. Please compile the linked project to generate the actual SEC_CORE_IMG file."

Custom Board Configuration
==========================

For custom boards or development kits:

1. **Create Board Directory**
   
   .. code-block:: text

      boards/
      └── my_custom_board/
         ├── my_custom_board.yaml
         └── my_custom_board.cfg

   **Important:** The directory name, YAML file name, and CFG file name must all be exactly the same (e.g., `my_custom_board`). Any mismatch will cause the build system to fail to locate your custom board configuration.

2. **Board YAML Configuration**

   The board YAML file defines the hardware configuration and build settings:

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
          # Optional: SDK dependency maintenance (not required for custom boards)
          feature:
            - board_audio_in # PDM
            - board_audio_out # DAO
            - board_gpt_pin
            - board_motor_control
            - board_gpio_led
            - board_gpio_key
            - board_pwm_rgb_led
            - board_puart_pin
          # Optional: SDK dependency maintenance (not required for custom boards)
          excluded_samples:
            - samples/motor_ctrl/bldc_hfi
            - samples/trace_recorder/rtthread-nano

   **Note:** Both `feature` and `excluded_samples` fields are used by the SDK for dependency maintenance and are not required for custom board configurations. The `feature` field specifies board-specific hardware features for sample application compatibility, while `excluded_samples` specifies which sample applications should not be built for this board due to hardware limitations or compatibility issues.

3. **Using Custom Board**

   When using a custom board, you need to specify the `BOARD_SEARCH_PATH` to tell CMake where to find your custom board configuration:

   .. code-block:: bash

      # Specify the path to your custom board directory
      cmake -B build -S . -DBOARD=my_custom_board -DBOARD_SEARCH_PATH=/path/to/your/boards

   **Example:**
   
   If your custom board is located at `/home/user/my_boards/my_custom_board/`, use:

   .. code-block:: bash

      cmake -B build -S . -DBOARD=my_custom_board -DBOARD_SEARCH_PATH=/home/user/my_boards

Component Management
====================

The SDK uses a component-based architecture:

**Available Components:**

- **Drivers**: gpio, uart, spi, i2c, adc, pwm, etc.
- **Middleware**: freertos, lwip, fatfs, etc.
- **Components**: ppi, plb, i2c, spi, i2s_over_spi, pmbus, smbus, serial_nor, uart_lin, dma_mgr, tsw_phy, touch, sccb, segment_led, panel, log, jpeg, ipc_event_mgr, enet_phy, eeprom_emulation, debug_console, codec, camera, usb, etc.
- **Utilities**: sbrk, ffssi, swap, crc32, size_utils, etc.

**Adding Components:**

Components are reusable software modules that provide specific functionality. They are located in the `components/` directory and include:

- **Communication**: i2c, spi, uart_lin, smbus, pmbus, serial_nor
- **Display & Interface**: panel, segment_led, camera, codec
- **System Services**: dma_mgr, ipc_event_mgr, log, debug_console
- **Specialized Hardware**: tsw_phy, enet_phy, eeprom_emulation
- **Protocol Support**: ppi, plb, i2s_over_spi, sccb, touch, jpeg

**Utilities:**

The `utils/` directory contains utility functions and tools:

- **Memory Management**: sbrk (dynamic memory allocation)
- **Data Processing**: swap (byte order conversion), crc32 (checksum calculation)
- **File System**: ffssi (flash file system)
- **Build Tools**: size_utils (binary size analysis)

These utilities are automatically available when using the SDK and don't need to be explicitly added to dependencies.

Build Optimization
==================

**Optimization Levels:**

.. code-block:: bash

   # Debug build (default)
   cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=debug

   # Release build
   cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release

**Custom Compiler Flags:**

.. code-block:: cmake

   # Using SDK CMake extensions
   sdk_compile_options("-O2")
   sdk_compile_definitions("NDEBUG=1")

Compiler Flags Configuration
============================

The SDK provides CMake functions to set compiler flags in a cross-platform way:

**Setting Compiler Options:**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   # Add compiler options for all toolchains
   sdk_compile_options("-O2")
   sdk_compile_options("-DNDEBUG")
   sdk_compile_options("-Wall")
   
   # Add conditional compiler options
   sdk_compile_options_ifdef(CONFIG_DEBUG "-g")
   sdk_compile_options_ifdef(CONFIG_RELEASE "-O3")
   
   sdk_app_src(src/main.c)
   generate_ide_projects()

**Setting Compiler Definitions:**

.. code-block:: cmake

   # Add compiler definitions for all toolchains
   sdk_compile_definitions("DEBUG_MODE=1")
   sdk_compile_definitions("VERSION_MAJOR=1")
   sdk_compile_definitions("VERSION_MINOR=0")
   
   # Add conditional compiler definitions
   sdk_compile_definitions_ifdef(CONFIG_FEATURE_X "FEATURE_X_ENABLED=1")
   sdk_compile_definitions_ifndef(CONFIG_FEATURE_Y "FEATURE_Y_DISABLED=1")

**Available Functions:**

- **sdk_compile_options(OPTIONS...)**: Add compiler options for all toolchains
- **sdk_compile_options_ifdef(CONDITION OPTIONS...)**: Add compiler options if condition is defined
- **sdk_compile_options_ifndef(CONDITION OPTIONS...)**: Add compiler options if condition is not defined
- **sdk_compile_definitions(DEFINITIONS...)**: Add compiler definitions for all toolchains
- **sdk_compile_definitions_ifdef(CONDITION DEFINITIONS...)**: Add compiler definitions if condition is defined
- **sdk_compile_definitions_ifndef(CONDITION DEFINITIONS...)**: Add compiler definitions if condition is not defined

**Example with Conditional Compilation:**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   # Set optimization level based on build type
   if(CMAKE_BUILD_TYPE STREQUAL "release")
       sdk_compile_options("-O3")
       sdk_compile_definitions("NDEBUG=1")
   else()
       sdk_compile_options("-O0", "-g")
       sdk_compile_definitions("DEBUG=1")
   endif()
   
   # Add feature-specific flags
   sdk_compile_definitions_ifdef(CONFIG_USB "USB_ENABLED=1")
   sdk_compile_definitions_ifdef(CONFIG_NETWORK "NETWORK_ENABLED=1")

**Using EXTRA Flags:**

The SDK also supports additional compiler, linker, and assembler flags through CMake variables:

.. code-block:: cmake

   # Set extra C compiler flags
   set(EXTRA_C_FLAGS "-fstack-protector-strong -fno-strict-aliasing")
   
   # Set extra linker flags
   set(EXTRA_LD_FLAGS "-Wl,--gc-sections -Wl,--as-needed")
   
   # Set extra assembler flags
   set(EXTRA_AS_FLAGS "-Wa,--no-warn")
   
   # Set extra linker symbols
   set(EXTRA_LD_SYMBOLS "--defsym=__heap_size__=0x8000")

**Memory Configuration:**

The SDK provides built-in support for configuring heap and stack sizes through CMake variables:

.. code-block:: cmake

   # In your project's CMakeLists.txt file:
   
   # Set heap size (default: 0x4000 = 16KB)
   set(HEAP_SIZE 0x8000)  # 32KB heap
   
   # Set stack size (default: 0x4000 = 16KB)
   set(STACK_SIZE 0x2000)  # 8KB stack

**Default Values and How It Works:**

The SDK automatically sets these variables if not specified:

- **HEAP_SIZE**: Defaults to `0x4000` (16KB) - Segger default heap size
- **STACK_SIZE**: Defaults to `0x4000` (16KB) - Segger default stack size

The SDK automatically converts these CMake variables into linker symbols:
- ``HEAP_SIZE`` → ``_heap_size`` symbol in linker script
- ``STACK_SIZE`` → ``_stack_size`` symbol in linker script

**Size Format Support:**

You can specify sizes in different formats:

.. code-block:: cmake

   # Hexadecimal (recommended)
   set(HEAP_SIZE 0x8000)
   
   # Decimal
   set(HEAP_SIZE 32768)
   
   # With K/M suffix
   set(HEAP_SIZE 32K)
   set(HEAP_SIZE 1M)

**RISC-V Architecture Configuration:**

Configure RISC-V architecture and ABI settings:

.. code-block:: cmake

   # In your project's CMakeLists.txt file:
   
   # Set RISC-V architecture (default: rv32imac)
   set(RV_ARCH "rv32imac_zicsr_zifencei")
   
   # Set RISC-V ABI (default: ilp32)
   set(RV_ABI "ilp32")

**Default Values and How It Works:**

The SDK automatically sets these variables if not specified:

- **RV_ARCH**: Defaults to `rv32imac` (32-bit RISC-V with integer multiply/divide, atomic, and compressed instructions)
- **RV_ABI**: Defaults to `ilp32` (32-bit integer, long, and pointer)

The SDK automatically appends required extensions:

- **CSR Support**: Automatically adds `_zicsr` if not present and needed
- **Fence.i Support**: Automatically adds `_zifencei` if not present and needed

**Common RV_ARCH Values:**

.. code-block:: cmake

   # Basic 32-bit RISC-V
   set(RV_ARCH "rv32imac")
   
   # With floating point support
   set(RV_ARCH "rv32imafc")
   
   # With vector support
   set(RV_ARCH "rv32imafdcv")
   
   # Custom extensions
   set(RV_ARCH "rv32imac_zicsr_zifencei_zba_zbb")

**Common RV_ABI Values:**

.. code-block:: cmake

   # 32-bit integer, long, and pointer (most common)
   set(RV_ABI "ilp32")
   
   # 32-bit integer, 64-bit long, 32-bit pointer
   set(RV_ABI "ilp32f")
   
   # 32-bit integer, 64-bit long, 32-bit pointer with float
   set(RV_ABI "ilp32d")

**Command Line Usage:**

.. code-block:: bash

   # Pass extra flags via command line
   cmake -B build -S . -DBOARD=hpm6750evkmini -DEXTRA_C_FLAGS="-fstack-protector-strong" -DEXTRA_LD_FLAGS="-Wl,--gc-sections" -DEXTRA_AS_FLAGS="-Wa,--no-warn" -DEXTRA_LD_SYMBOLS="--defsym=__heap_size__=0x8000"
   
   # Configure memory sizes via command line
   cmake -B build -S . -DBOARD=hpm6750evkmini -DHEAP_SIZE=0x8000 -DSTACK_SIZE=0x2000
   
   # Configure RISC-V architecture and ABI via command line
   cmake -B build -S . -DBOARD=hpm6750evkmini -DRV_ARCH="rv32imac_zicsr_zifencei" -DRV_ABI="ilp32"

**Nano Specs Configuration:**

The `HPM_SDK_LD_NO_NANO_SPECS` variable controls whether to use the nano.specs linker specification file, which provides optimized implementations of standard library functions for embedded systems.

**Default Behavior:**

By default, the SDK automatically includes nano.specs for GCC toolchain builds, which provides:

- **Optimized printf/scanf**: Smaller, faster implementations of printf and scanf functions
- **Reduced memory footprint**: Uses less RAM and flash compared to full standard library
- **Float support**: Enables floating-point support for printf and scanf operations

**Disabling Nano Specs:**

To disable nano.specs and use the full standard library implementations:

.. code-block:: cmake

   # In your CMakeLists.txt file:
   
   # Disable nano.specs (use full standard library)
   set(HPM_SDK_LD_NO_NANO_SPECS 1)

**When to Disable Nano Specs:**

- **Full printf/scanf functionality**: When you need complete printf/scanf features not available in nano.specs
- **Custom library implementations**: When using custom or third-party standard library implementations
- **Debugging purposes**: When you need full standard library debugging capabilities
- **Specialized applications**: For applications that require specific standard library behavior

**Example Usage:**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   # Disable nano.specs for this project
   set(HPM_SDK_LD_NO_NANO_SPECS 1)
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   sdk_app_src(src/main.c)
   generate_ide_projects()

**Command Line Usage:**

.. code-block:: bash

   # Disable nano.specs via command line
   cmake -B build -S . -DBOARD=hpm6750evkmini -DHPM_SDK_LD_NO_NANO_SPECS=1

**Custom Target Triplet Configuration:**

The `CUSTOM_TARGET_TRIPLET` variable allows you to override the default target triplet used by the SDK's cross-compilation toolchain. This is useful when you have a toolchain with a different naming convention than the SDK's default.

**Default Behavior:**

By default, the SDK uses `riscv32-unknown-elf` as the target triplet, which means:

- **Architecture**: RISC-V 32-bit
- **Vendor**: unknown (generic)
- **Operating System**: elf (bare metal/embedded)

**Using Custom Target Triplet:**

To use a different target triplet (e.g., for different toolchain installations):

.. code-block:: cmake

   # In your CMakeLists.txt file:
   
   # Set custom target triplet
   set(CUSTOM_TARGET_TRIPLET "riscv32-elf")

**Common Target Triplet Formats:**

.. code-block:: cmake

   # Standard RISC-V bare metal (default)
   set(CUSTOM_TARGET_TRIPLET "riscv32-unknown-elf")
  
**How It Works:**

When you set `CUSTOM_TARGET_TRIPLET`, the SDK uses this value to construct the cross-compilation toolchain prefix instead of the default `riscv32-unknown-elf`:

- **Compiler**: `${CUSTOM_TARGET_TRIPLET}-gcc`
- **Linker**: `${CUSTOM_TARGET_TRIPLET}-ld`
- **Binutils**: `${CUSTOM_TARGET_TRIPLET}-objcopy`, `${CUSTOM_TARGET_TRIPLET}-objdump`, etc.

**Example Usage:**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   # Set custom target triplet for specific toolchain
   set(CUSTOM_TARGET_TRIPLET "riscv32-elf")
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   sdk_app_src(src/main.c)
   generate_ide_projects()

**Command Line Usage:**

.. code-block:: bash

   # Set custom target triplet via command line
   cmake -B build -S . -DBOARD=hpm6750evkmini -DCUSTOM_TARGET_TRIPLET="riscv32-hpmicro-elf"

**Toolchain Compatibility:**

The target triplet must match your installed toolchain. Common scenarios:

- **Standard RISC-V GCC**: Use `riscv32-unknown-elf`
- **Vendor-specific toolchain**: Use vendor-specific triplet
- **Custom toolchain**: Use the prefix format of your custom toolchain

**Verification:**

To verify your toolchain uses the correct target triplet:

.. code-block:: bash

   # Check if the toolchain exists
   ls $GNURISCV_TOOLCHAIN_PATH/bin/
   
   # Look for files matching your CUSTOM_TARGET_TRIPLET
   # Example: if CUSTOM_TARGET_TRIPLET="riscv32-elf"
   # Look for: riscv32-elf-gcc, riscv32-elf-ld, etc.

IDE Integration
===============

The SDK supports multiple IDEs through the `generate_ide_projects()` function in CMakeLists.txt:

**CMakeLists.txt Configuration:**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   
   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(my_application)
   
   sdk_app_src(src/main.c)
   
   # Generate IDE projects for all supported IDEs
   generate_ide_projects()

**Build and Generate Projects:**

.. code-block:: bash

   # Configure and build the project
   cmake -B build -S . -DBOARD=hpm6750evkmini
   cmake --build build

**Generated IDE Projects:**

After building, the following IDE project files will be generated in the `build` directory:

- **Segger Embedded Studio**: `build/segger_embedded_studio/`
- **IAR Embedded Workbench**: `build/iar_embedded_workbench/`

**Opening in IDEs:**

- **Segger Embedded Studio**: Open the `.emProject` file in the segger directory
- **IAR Embedded Workbench**: Open the `.ewp` file in the iar directory

Best Practices
**************

Project Organization
====================

1. **Use Clear Directory Structure**
   
   .. code-block:: text

      project/
      ├── CMakeLists.txt          # Main build configuration
      ├── app.yaml               # Application configuration
      ├── src/                   # Source files
      │   ├── main.c
      │   └── app/
      ├── include/               # Header files
      │   └── app/
      ├── config/                # Configuration files
      └── docs/                  # Documentation

2. **Modular Source Organization**

   .. code-block:: cmake

         # CMakeLists.txt
      sdk_app_src(
         src/main.c
         src/app/gpio_handler.c
         src/app/uart_handler.c
      )

      sdk_app_inc(include/app)

Configuration Management
========================

**Environment-specific Configurations**

   .. code-block:: bash

      # Development
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=debug

      # Production
      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=release


Debugging
=========

1. **Enable Debug Information**

   .. code-block:: bash

      cmake -B build -S . -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=debug

2. **Use IDE Debugging**

   - Open generated IDE project
   - Set breakpoints
   - Start debugging session

Performance Optimization
========================

**Choose Appropriate Build Type**

   .. code-block:: bash

      # For development
      cmake -B build -S . -DCMAKE_BUILD_TYPE=debug

      # For production
      cmake -B build -S . -DCMAKE_BUILD_TYPE=release

Troubleshooting
***************

Common Issues and Solutions
===========================

**Build Fails with "Board not found"**

First, check these fundamental issues:

.. code-block:: bash

   # 1. Verify board path is correct
   ls boards/ | grep your_board_name

   # 2. Verify board YAML file exists and name is correct
   ls boards/your_board_name/your_board_name.yaml

   # 3. Check if board name matches exactly (case-sensitive)
   cmake -B build -S . -DBOARD=your_board_name

There are two scenarios to consider:

**Scenario 1: Using Standard SDK Boards**

.. code-block:: bash

   # Check available boards in SDK
   ls boards/

   # Use correct board name from SDK
   cmake -B build -S . -DBOARD=hpm6750evkmini

**Scenario 2: Using Custom Boards**

.. code-block:: bash

   # Ensure custom board directory structure is correct
   # Directory name must match YAML and CFG file names exactly
   ls /path/to/your/custom/boards/

   # Specify BOARD_SEARCH_PATH to locate custom board
   cmake -B build -S . -DBOARD=my_custom_board -DBOARD_SEARCH_PATH=/path/to/your/custom/boards

**Missing Toolchain**

.. code-block:: bash

   # Set toolchain path
   export PATH=$PATH:/path/to/toolchain/bin

   # Or specify in CMake
   cmake -B build -S . -DBOARD=hpm6750evkmini -DTOOLCHAIN_PATH=/path/to/toolchain

**Multi-core Build Issues**

By default, multi-core Core0 projects depend on successful compilation of Core1 projects using GNU GCC. However, GNU GCC on Windows cannot handle long file paths, which may cause Core1 project compilation to fail, leading to Core0 build failure.

.. code-block:: bash

   # Check linked project configuration
   # Ensure SEC_CORE_IMG_C_ARRAY_OUTPUT is set correctly
   # Use DISABLE_LINKED_PROJECT_BUILD to bypass Core1 compilation on Windows
   cmake -B build -S . -DBOARD=hpm6750evkmini -DDISABLE_LINKED_PROJECT_BUILD=ON

**IDE Project Generation Fails**

.. code-block:: bash

   # Use supported generator
   cmake -B build -S . -DBOARD=hpm6750evkmini -G "Unix Makefiles"

Debugging Build Issues
======================

1. **Enable Verbose Output**

   .. code-block:: bash

      cmake -B build -S . -DBOARD=hpm6750evkmini --debug-output
      cmake --build build --verbose

2. **Check CMake Cache**

   .. code-block:: bash

      cat build/CMakeCache.txt | grep -i board
      cat build/CMakeCache.txt | grep -i toolchain

3. **Validate Configuration**

   .. code-block:: bash

      cmake -B build -S . -DBOARD=hpm6750evkmini -L

Examples
********

Basic GPIO Application
======================

**Project Structure:**

.. code-block:: text

   gpio_example/
   ├── CMakeLists.txt
   └── src/
      └── main.c

**CMakeLists.txt:**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(gpio_example)

   sdk_app_src(src/main.c)
   generate_ide_projects()

**main.c:**

.. code-block:: c

   #include "board.h"
   #include "hpm_gpio_drv.h"

   int main(void)
   {
      /* Initialize GPIO */
      gpio_set_pin_output(HPM_GPIO0, GPIO_DI_GPIOA, 0);
      
      while (1) {
         /* Toggle LED */
         gpio_toggle_pin(HPM_GPIO0, GPIO_DI_GPIOA, 0);
         board_delay_ms(500);
      }
   }

**Build Command:**

.. code-block:: bash

   cmake -B build -S . -DBOARD=hpm6750evkmini
   cmake --build build

UART Communication Example
==========================

**main.c:**

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

Multi-core Example
==================

**Core0 (app.yaml):**

.. code-block:: yaml

   linked_project:
      project_name: multicore_example/core1
      project_path: ../core1
      build_type: sec_core_img

**Core1 (CMakeLists.txt):**

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.13)
   set(HPM_BUILD_TYPE "sec_core_img")
   set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)

   find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
   project(multicore_core1)

   sdk_app_src(src/core1_main.c)

**Build Command:**

.. code-block:: bash

   cd core0
   cmake -B build -S . -DBOARD=hpm6750evkmini
   cmake --build build

Conclusion
**********

The HPM SDK CMake build system provides a powerful and flexible foundation for embedded application development. By following the workflows and best practices outlined in this guide, you can:

- **Accelerate Development**: Use pre-configured components and board support
- **Ensure Consistency**: Standardized build process across projects
- **Simplify Debugging**: Integrated IDE support and debugging tools
- **Optimize Performance**: Configurable build types and optimization levels
- **Support Multi-core**: Seamless dual-core application development

For detailed API reference, see :ref:`cmake_intro`. For additional examples and advanced usage, refer to the samples directory in the SDK.

