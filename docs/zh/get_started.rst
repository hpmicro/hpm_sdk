.. _getting_started:

================
快速入门
================

依赖项的最低版本要求
-----------------------------------------

.. list-table::
   :header-rows: 1

   * - 名称
     - 版本
   * - CMake
     - 3.13
   * - Python
     - 3.8

.. _install_dependencies:

安装依赖项
--------------------

.. tabs::

   .. group-tab:: Linux

      以 Ubuntu 为例

      1. **安装工具**

         .. code-block:: shell

            sudo apt install build-essential cmake ninja-build libc6-i386 libc6-i386-cross libstdc++6-i386-cross

      2. **安装 Python 3（最低 3.8.5）和 pip**

         .. code-block:: shell

            sudo apt install python3 python3-pip

   .. group-tab:: Windows

      1. **安装 Chocolatey**

         - Chocolatey 是一个 Windows 的包管理器，可以简化本地 Windows 依赖项的安装。
         - 按照 `Chocolatey 安装 <https://chocolatey.org/install>`_ 页面上的说明安装 Chocolatey。
         - 以"管理员"身份打开"cmd.exe"。
         - 禁用全局确认，以避免在安装单个程序时需要确认：

           .. code-block:: batch

              choco feature enable -n allowGlobalConfirmation

      2. **安装 CMake**

         .. code-block:: batch

            choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'

      3. **安装其他工具**

         .. code-block:: batch

            choco install git python ninja

      4. **关闭管理员命令提示符窗口**

.. _prepare_toolchain:

准备工具链和环境变量
-----------------------------------------

支持的工具链
^^^^^^^^^^^^^^^^^^^^

- gnu-gcc（默认工具链）
- nds-gcc
- zcc

工具链设置
^^^^^^^^^^^^^^^^

.. tabs::

   .. tab:: gnu-gcc

      1. **下载并解压工具链**

         - 将工具链包解压到特定路径（例如，``TOOLCHAIN_PATH``）。可执行文件 ``riscv32-unknown-elf-gcc`` 应位于 ``TOOLCHAIN_PATH/bin``。

      2. **设置环境变量**

         .. tabs::

            .. group-tab:: Linux

               假设使用 zsh

               .. code-block:: shell

                  export GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  export HPM_SDK_TOOLCHAIN_VARIANT=

            .. group-tab:: Windows

               .. code-block:: batch

                  set GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  set HPM_SDK_TOOLCHAIN_VARIANT=

   .. tab:: nds-gcc

      1. **下载并解压工具链**

         - 将工具链包解压到特定路径（例如，``TOOLCHAIN_PATH``）。可执行文件 ``riscv32-elf-gcc`` 应位于 ``TOOLCHAIN_PATH/bin``。

      2. **设置环境变量**

         .. tabs::

            .. group-tab:: Linux

               假设使用 zsh

               .. code-block:: shell

                  export GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  export HPM_SDK_TOOLCHAIN_VARIANT=nds-gcc

            .. group-tab:: Windows

               .. code-block:: batch

                  set GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  set HPM_SDK_TOOLCHAIN_VARIANT=nds-gcc

               .. note::

                  对于 Windows，Andes 编译器需要以下库：
                  - ``cygwin1.dll``
                  - ``cygncursesw-10.dll``
                  确保它们的路径已添加到系统环境变量 ``PATH`` 中。

   .. tab:: zcc

      1. **下载并解压工具链**

         - 将工具链包解压到特定路径（例如，``TOOLCHAIN_PATH``）。

      2. **设置环境变量**

         .. tabs::

            .. group-tab:: Linux

               .. code-block:: shell

                  export GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  export HPM_SDK_TOOLCHAIN_VARIANT=zcc

            .. group-tab:: Windows

               .. code-block:: batch

                  set GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  set HPM_SDK_TOOLCHAIN_VARIANT=zcc

.. _environment_variables:

环境变量
---------------------

.. tabs::

   .. tab:: 使用提供的脚本

      .. tabs::

         .. group-tab:: Linux

            .. code-block:: shell

               source env.sh

         .. group-tab:: Windows

            .. code-block:: batch

               env.cmd

   .. tab:: 手动声明

      手动声明一个环境变量 ``HPM_SDK_BASE``，指向 SDK 根路径。

      .. tabs::
         .. group-tab:: Linux
            使用 zsh，假设 SDK 位于 ``$HOME/hpm_sdk``

            .. code-block:: shell

               export HPM_SDK_BASE=$HOME/hpm_sdk

         .. group-tab:: Windows
            假设 SDK 位于 ``c:\hpm_sdk``

            .. code-block:: batch

               set HPM_SDK_BASE=c:\hpm_sdk

.. _install_python_dependencies:

安装 Python 依赖项
---------------------------

.. tabs::

   .. group-tab:: Linux

      .. code-block:: shell

         pip3 install --user -r "$HPM_SDK_BASE/scripts/requirements.txt"

   .. group-tab:: Windows

      默认情况下，安装 Python 3.x 后，``python3/pip3`` 在 Windows 上不可用。只有 ``python/pip`` 可用。

      .. code-block:: batch

         pip install --user -r "%HPM_SDK_BASE%/scripts/requirements.txt"

.. _build_application_gnu_gcc:

使用 GNU GCC 工具链构建应用程序
-------------------------------------------

完成上述步骤后，可以生成和构建 SDK 项目。以下步骤描述如何构建一个示例（例如，``hello_world``）：

1. **导航到应用程序目录**

 .. code-block:: shell

    cd samples/hello_world

2. **创建构建目录**

 .. tabs::

    .. group-tab:: Linux

       .. code-block:: shell

          mkdir build

    .. group-tab:: Windows

       .. code-block:: batch

          md build

3. **切换到"build"目录**

 .. code-block:: shell

    cd build

4. **为 Ninja 生成构建文件**

 .. code-block:: shell

    cmake -GNinja -DBOARD=hpm6750evkmini ..

 .. note::

    如果提示"CMAKE_MAKE_PROGRAM is not set"，请在前一个命令中附加 ``-DCMAKE_MAKE_PROGRAM=YOUR_MAKE_EXECUTABLE_PATH``。（将 ``NINJA_PATH`` 替换为 ``ninja`` 所在的文件夹）：

    .. code-block:: shell

       cmake -GNinja -DBOARD=hpm6750evkmini -DCMAKE_MAKE_PROGRAM=NINJA_PATH/ninja ..

5. **构建项目**

 .. code-block:: shell

    ninja

 构建完成后，ELF 文件和其他应用程序相关文件（例如，映射文件、汇编源文件或二进制文件）可以在 ``output`` 目录中找到。

.. _run_debug_application:

快速指南：运行/调试应用程序（hello_world）
----------------------------------------------------

1. **连接开发板**

 - 包括调试器（默认支持 JLink）和串口。

2. **为开发板供电**

3. **打开控制台**

 - 连接到调试串口波特率为115200。

4. **安装 OpenOCD**

 - 可以通过包管理系统安装，或从 SourceForge 或 GitHub 下载。确保其版本 > 0.11。

5. **设置环境变量**

 .. tabs::

    .. group-tab:: Linux

       .. code-block:: shell

          source env.sh

    .. group-tab:: Windows

       .. code-block:: batch

          env.cmd

       - 或者，手动设置 ``OPENOCD_SCRIPTS`` 环境变量：

         .. code-block:: batch

            set OPENOCD_SCRIPTS=%HPM_SDK_BASE%\boards\openocd

6. **启动 OpenOCD**

 - 使用探针类型、核心类型和板类型的多个配置文件。例如，以下命令为 HPM6750EVKMini 上的单核设置 OpenOCD GDB 服务器，使用 FT2232 探针：

   .. code-block:: shell

      openocd -f probes/ft2232.cfg -f soc/hpm6750-single-core.cfg -f boards/hpm6750evkmini.cfg

 .. note::

    如果使用 FTDI 调试器并遇到错误 ``Error: libusb_open() failed with LIBUSB_ERROR_NOT_FOUND``，请检查 FTDI USB 驱动程序。如果未正确安装，请使用 `zadig <https://github.com/pbatard/libwdi/releases/download/b730/zadig-2.5.exe>`_ 更新：
      - 打开 zadig，点击 ``Options -> List All Devices``。
      - 选择 ``Dual RS232-HS (Interface 0)``。
      - 点击 ``Install Driver`` 或 ``Replace Driver``。

7. **导航到 hello_world 目录**

 .. code-block:: shell

    cd samples/hello_world

8. **在另一个终端中启动 GDB 客户端**

 .. tabs::

    .. tab:: gnu-gcc

       .. code-block:: shell

          TOOLCHAIN_PATH/bin/riscv32-unknown-elf-gdb

    .. tab:: nds-gcc

       .. code-block:: shell

          TOOLCHAIN_PATH/bin/riscv32-elf-gdb

9. **将 GDB 客户端连接到 GDB 服务器**

 .. code-block:: shell

    gdb> file build/output/demo.elf
    gdb> target remote localhost:3333
    gdb> load
    gdb> b main
    gdb> c

10. **验证输出**

 - 在调试控制台上，应打印"hello_world"。

.. _build_application_segger:

使用 Segger Embedded Studio 构建应用程序
------------------------------------------------

1. **下载 Segger Embedded Studio for RISC-V**

   - 可以从 `Segger <https://www.segger.com/downloads/embedded-studio/>`_ 下载。

2. **生成项目文件**

   - 在为 Ninja 生成构建文件时（如 :ref:`build_application_gnu_gcc` -> "4. 为 Ninja 生成构建文件" 中所述），将生成 Segger Embedded Studio 的项目文件。
   - 项目文件 (.emProject) 可以在 ``build/segger_embedded_studio/`` 中找到。

   .. note::

      ``openocd`` 可执行文件需要在当前控制台的 ``PATH`` 变量中找到。否则，项目文件中的调试配置将不会生成，需要在 Segger Embedded Studio 中手动配置。
