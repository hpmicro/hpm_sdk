.. _getting_started:

================
Getting Started
================

Minimum Required Versions of Dependencies
-----------------------------------------

.. list-table::
   :header-rows: 1

   * - Name
     - Version
   * - CMake
     - 3.13
   * - Python
     - 3.8

.. _install_dependencies:

Install Dependencies
--------------------

.. tabs::

   .. group-tab:: Linux

      Take Ubuntu as example

      1. **Install Tools**

         .. code-block:: shell

            sudo apt install build-essential cmake ninja-build libc6-i386 libc6-i386-cross libstdc++6-i386-cross

      2. **Install Python 3 (3.8.5 minimum) and pip**

         .. code-block:: shell

            sudo apt install python3 python3-pip

   .. group-tab:: Windows

      1. **Install Chocolatey**

         - Chocolatey is a package manager for Windows that simplifies the installation of native Windows dependencies.
         - Install Chocolatey by following the instructions on the `Chocolatey Install <https://chocolatey.org/install>`_ page.
         - Open "cmd.exe" as "Administrator".
         - Disable global confirmation to avoid having to confirm the installation of individual programs:

           .. code-block:: batch

              choco feature enable -n allowGlobalConfirmation

      2. **Install CMake**

         .. code-block:: batch

            choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'

      3. **Install Other Tools**

         .. code-block:: batch

            choco install git python ninja

      4. **Close the Administrator Command Prompt Window**

.. _prepare_toolchain:

Prepare Toolchain & Environment Variables
-----------------------------------------

Supported Toolchains
^^^^^^^^^^^^^^^^^^^^

- gnu-gcc (default toolchain)
- nds-gcc
- zcc

Toolchain Setup
^^^^^^^^^^^^^^^^

.. tabs::

   .. tab:: gnu-gcc

      1. **Download and Unzip the Toolchain**

         - Unzip the toolchain package to a specific path (e.g., ``TOOLCHAIN_PATH``). The executable ``riscv32-unknown-elf-gcc`` should be located in ``TOOLCHAIN_PATH/bin``.

      2. **Set Environment Variables**

         .. tabs::

            .. group-tab:: Linux

               Assume using zsh

               .. code-block:: shell

                  export GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  export HPM_SDK_TOOLCHAIN_VARIANT=

            .. group-tab:: Windows

               .. code-block:: batch

                  set GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  set HPM_SDK_TOOLCHAIN_VARIANT=

   .. tab:: nds-gcc

      1. **Download and Unzip the Toolchain**

         - Unzip the toolchain package to a specific path (e.g., ``TOOLCHAIN_PATH``). The executable ``riscv32-elf-gcc`` should be located in ``TOOLCHAIN_PATH/bin``.

      2. **Set Environment Variables**

         .. tabs::

            .. group-tab:: Linux

               Assume using zsh

               .. code-block:: shell

                  export GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  export HPM_SDK_TOOLCHAIN_VARIANT=nds-gcc

            .. group-tab:: Windows

               .. code-block:: batch

                  set GNURISCV_TOOLCHAIN_PATH=TOOLCHAIN_PATH
                  set HPM_SDK_TOOLCHAIN_VARIANT=nds-gcc

               .. note::

                  For Windows, the Andes compiler requires the following libraries:
                  - ``cygwin1.dll``
                  - ``cygncursesw-10.dll``
                  Ensure their paths are appended to the system environment variable ``PATH``.

   .. tab:: zcc

      1. **Download and Unzip the Toolchain**

         - Unzip the toolchain package to a specific path (e.g., ``TOOLCHAIN_PATH``).

      2. **Set Environment Variables**

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

Environment Variables
---------------------

.. tabs::

   .. tab:: Using Provided Scripts

      .. tabs::

         .. group-tab:: Linux

            .. code-block:: shell

               source env.sh

         .. group-tab:: Windows

            .. code-block:: batch

               env.cmd

   .. tab:: Manual Declaration

      Manually declare an environment variable ``HPM_SDK_BASE`` pointing to the SDK root path.

      .. tabs::
         .. group-tab:: Linux
            using zsh, assuming SDK is located at ``$HOME/hpm_sdk``

            .. code-block:: shell

               export HPM_SDK_BASE=$HOME/hpm_sdk

         .. group-tab:: Windows
            assuming SDK is located at ``c:\hpm_sdk``

            .. code-block:: batch

               set HPM_SDK_BASE=c:\hpm_sdk

.. _install_python_dependencies:

Install Python Dependencies
---------------------------

.. tabs::

   .. group-tab:: Linux

      .. code-block:: shell

         pip3 install --user -r "$HPM_SDK_BASE/scripts/requirements.txt"

   .. group-tab:: Windows

      By default, ``python3/pip3`` is not available after installing Python 3.x on Windows. Only ``python/pip`` is available.

      .. code-block:: batch

         pip install --user -r "%HPM_SDK_BASE%/scripts/requirements.txt"

.. _build_application_gnu_gcc:

Build an Application with GNU GCC Toolchain
-------------------------------------------

After completing the steps above, SDK projects can be generated and built. The following steps describe how to build a demo (e.g., ``hello_world``):

1. **Navigate to the Application Directory**

 .. code-block:: shell

    cd samples/hello_world

2. **Create a Build Directory**

 .. tabs::

    .. group-tab:: Linux

       .. code-block:: shell

          mkdir build

    .. group-tab:: Windows

       .. code-block:: batch

          md build

3. **Change Directory to "build"**

 .. code-block:: shell

    cd build

4. **Generate Build Files for Ninja**

 .. code-block:: shell

    cmake -GNinja -DBOARD=hpm6750evkmini ..

 .. note::

    If it complains about "CMAKE_MAKE_PROGRAM is not set," append ``-DCMAKE_MAKE_PROGRAM=YOUR_MAKE_EXECUTABLE_PATH`` to the previous command. (Replace ``NINJA_PATH`` with the folder where ``ninja`` is located):

    .. code-block:: shell

       cmake -GNinja -DBOARD=hpm6750evkmini -DCMAKE_MAKE_PROGRAM=NINJA_PATH/ninja ..

5. **Build the Project**

 .. code-block:: shell

    ninja

 When the build is complete, the ELF file and other application-related files (e.g., map file, assembly source, or binary file) can be found in the ``output`` directory.

.. _run_debug_application:

Quick Guide to Run/Debug an Application (hello_world)
---------------------------------------------------------

1. **Wire Up the Board**

 - Include the debug probe (by default, it supports JLink) and serial port.

2. **Power Up the Board**

3. **Open a Console**

 - Connect to the debug console (target serial port) with a baud rate of 115200.

4. **Install OpenOCD**

 - It can be installed via a package management system or downloaded from SourceForge or GitHub. Ensure its revision is > 0.11.

5. **Set Environment Variables**

 .. tabs::

    .. group-tab:: Linux

       .. code-block:: shell

          source env.sh

    .. group-tab:: Windows

       .. code-block:: batch

          env.cmd

       - Alternatively, set the ``OPENOCD_SCRIPTS`` environment variable manually:

         .. code-block:: batch

            set OPENOCD_SCRIPTS=%HPM_SDK_BASE%\boards\openocd

6. **Start OpenOCD**

 - Use several configuration files in the order of probe type, core type, and board type. For example, the following command sets up an OpenOCD GDB server with an FT2232 probe for a single core on the HPM6750EVKMini:

   .. code-block:: shell

      openocd -f probes/ft2232.cfg -f soc/hpm6750-single-core.cfg -f boards/hpm6750evkmini.cfg

 .. note::

    If using an FTDI debugger and encountering the error ``Error: libusb_open() failed with LIBUSB_ERROR_NOT_FOUND``, check the FTDI USB driver. If it is not installed correctly, use `zadig <https://github.com/pbatard/libwdi/releases/download/b730/zadig-2.5.exe>`_ to update:
      - Open zadig, click ``Options -> List All Devices``.
      - Select ``Dual RS232-HS (Interface 0)``.
      - Click ``Install Driver`` or ``Replace Driver``.

7. **Navigate to the hello_world Directory**

 .. code-block:: shell

    cd samples/hello_world

8. **Start a GDB Client in Another Terminal**

 .. tabs::

    .. tab:: gnu-gcc

       .. code-block:: shell

          TOOLCHAIN_PATH/bin/riscv32-unknown-elf-gdb

    .. tab:: nds-gcc

       .. code-block:: shell

          TOOLCHAIN_PATH/bin/riscv32-elf-gdb

9. **Connect the GDB Client to the GDB Server**

 .. code-block:: shell

    gdb> file build/output/demo.elf
    gdb> target remote localhost:3333
    gdb> load
    gdb> b main
    gdb> c

10. **Verify the Output**

 - On the debug console, "hello_world" should be printed.

.. _build_application_segger:

Build an Application with Segger Embedded Studio
------------------------------------------------

1. **Download Segger Embedded Studio for RISC-V**

   - It can be downloaded from `Segger <https://www.segger.com/downloads/embedded-studio/>`_.

2. **Generate Project File**

   - The project file for Segger Embedded Studio will be generated while generating build files for Ninja (as mentioned in :ref:`build_application_gnu_gcc` -> "4. Generate Build Files for Ninja").
   - The project file (.emProject) can be found at ``build/segger_embedded_studio/``.

   .. note::

      The ``openocd`` executable needs to be found in the ``PATH`` variable of the current console. Otherwise, the debug configuration will not be generated in the project file and needs to be configured manually in Segger Embedded Studio later.
