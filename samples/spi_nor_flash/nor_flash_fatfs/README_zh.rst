.. _spi_nor_flash_fatfs:

SPI NOR FLASH FATFS
================================

概览
------

本示例使用FATFS文件系统操作SPI NOR Flash存储器，通过serial_nor组件提供底层Flash读写，
并结合FatFs实现文件的创建、读写、目录操作等功能。

功能特性：
  - 自动识别Flash的SFDP参数（容量、页大小、扇区大小、块大小等）
  - 首次挂载时自动检测并格式化FAT文件系统
  - 支持文件创建与读写
  - 支持目录创建与删除
  - 支持大文件写入测试（2MB）
  - 支持文件系统格式化
  - 支持裸机及FreeRTOS双模式运行


CMakeLists配置说明
-------------------

组件依赖
^^^^^^^^^^^

.. code-block:: cmake

   set(CONFIG_SPI_NOR_FLASH 1)
   set(CONFIG_FATFS_CUSTOM_PORTABLE 1)
   set(CONFIG_FATFS 1)
   set(CONFIG_DMA_MGR 1)

- ``CONFIG_SPI_NOR_FLASH`` : 使能SPI NOR Flash组件（serial_nor），提供底层Flash操作接口。
- ``CONFIG_FATFS`` : 使能FatFs文件系统组件。
- ``CONFIG_FATFS_CUSTOM_PORTABLE`` : 使能FatFs自定义移植层，允许用户提供自定义的diskio实现（本例为spi_nor_disk）。
- ``CONFIG_DMA_MGR`` : 使能DMA管理器组件，serial_nor的SPI传输借助DMA提升性能。

DMA管理器配置
^^^^^^^^^^^^^^^

- **使能DMA管理器（推荐）**：设置 ``CONFIG_DMA_MGR=1`` 后，DMA管理器自动为serial_nor分配空闲DMA通道，无需用户关心通道号和基地址，避免与其他外设的DMA资源冲突。

- **不使能DMA管理器**：若 ``CONFIG_DMA_MGR=0``，则需要用户在 ``hpm_serial_nor_host_port.c`` 中手动配置以下DMA相关宏：

  .. code-block:: c

     #define PORT_SPI_NOR_DMA           BOARD_APP_DMA0      /* DMA实例基地址 */
     #define PORT_SPI_NOR_DMAMUX        BOARD_APP_DMAMUX     /* DMAMUX实例基地址 */
     #define PORT_SPI_RX_DMA_CH         0                    /* SPI RX DMA通道号 */
     #define PORT_SPI_TX_DMA_CH         1                    /* SPI TX DMA通道号 */

  手动分配时务必确保指定的DMA通道不被系统中其他外设使用，否则可能导致传输冲突或数据错误。

RTOS可选配置
^^^^^^^^^^^^^^^

.. code-block:: cmake

   set(USE_FREERTOS 1)

   if (USE_FREERTOS)
       set(CONFIG_FREERTOS 1)
       set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)
   endif()

- ``USE_FREERTOS`` : 控制是否启用FreeRTOS。设为 ``0`` 则运行裸机模式，设为 ``1`` 则在FreeRTOS任务中运行FATFS测试。
- ``CONFIG_FREERTOS`` : 使能FreeRTOS组件。
- ``CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR`` : 使用GPTMR作为FreeRTOS的系统滴答时钟源。

源码及头文件路径
^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   sdk_inc(../common/port)
   sdk_inc(../common/port/${BOARD})
   sdk_app_src(../common/port/hpm_serial_nor_host_port.c)

   add_subdirectory(fatfs)

   sdk_inc(inc)
   sdk_app_src_glob(src/*.c)

- ``sdk_inc`` 将 ``common/port`` 加入头文件搜索路径，包含SPI NOR Flash板级移植头文件。
- ``sdk_app_src`` 编译 ``hpm_serial_nor_host_port.c`` : SPI NOR Flash的板级移植层。
- ``add_subdirectory(fatfs)`` 引入 ``fatfs/`` 子目录的CMakeLists.txt，包含FatFs的diskio移植层：

fatfs目录CMakeLists
^^^^^^^^^^^^^^^^^^^^^

  .. code-block:: cmake

     sdk_inc(port)
     sdk_app_src(port/diskio.c)
     sdk_app_src(port/spi_nor_disk.c)

  - ``diskio.c`` : FatFs磁盘IO层框架。
  - ``spi_nor_disk.c`` : SPI NOR Flash的diskio实现，封装serial_nor的读写擦除接口为FATFS可调用的扇区读写API。

RTOS编译宏
^^^^^^^^^^^

.. code-block:: cmake

   if (USE_FREERTOS)
       sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
       sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
       sdk_compile_definitions(-DUSE_FREERTOS=1)
       sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
   endif()

- ``USE_FREERTOS`` : 通知代码使用FreeRTOS模式，在任务中运行FATFS逻辑。
- ``NO_BOARD_TIMER_SUPPORT`` : 告知board层不由裸机定时器初始化。

SPI配置宏
^^^^^^^^^^^

.. code-block:: cmake

   sdk_compile_definitions(-DPORT_SPI_CLK_FREQUENCY=80000000)
   sdk_compile_definitions(-DPORT_SPI_IO_MODE=SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE)

- ``PORT_SPI_CLK_FREQUENCY`` : 配置SPI SCLK时钟频率，单位Hz。本例设置为80MHz，可根据目标Flash规格调整。
- ``PORT_SPI_IO_MODE`` : 配置SPI IO模式，可选值：

  .. code-block:: c

     SERIAL_NOR_HOST_SUPPORT_STANDARD_SPI           /* 单线SPI */
     SERIAL_NOR_HOST_SUPPORT_DUAL_IO_MODE           /* 双线SPI */
     SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE           /* 四线SPI */

优化选项
^^^^^^^^^^^

.. code-block:: cmake

   sdk_compile_options("-O3")

- 通过 ``-O3`` 优化提升Flash读写性能。


板级设置
------------

- :ref:`SPI引脚 <board_resource>` 根据板子型号查看具体信息
- SPI引脚需与NOR Flash模块引脚一一对应：

  ============ =============
  SPI信号      NOR Flash引脚
  ============ =============
  SCLK         CLK
  CS           CS#
  MOSI (IO0)   DI (IO0)
  MISO (IO1)   DO (IO1)
  IO2          WP# (IO2)
  IO3          HOLD# (IO3)
  ============ =============

- 使用四线SPI时，IO2/IO3必须连接；单线或双线模式下可省略。


注意事项
------------

- 该实例工程依赖 **serial_nor**、**FatFs**、**DMA管理器** 组件，需在CMakeLists中启用对应配置宏。
- serial_nor组件支持符合 SFDP (JESD216) 标准的NOR Flash，不限定具体型号（W25Q64、W25Q128、W25Q256、MX25L128等）。
- 首次运行时若Flash无FAT文件系统，示例会自动格式化（容量越大耗时越长，请耐心等待）。
- FatFs的diskio移植层（``spi_nor_disk.c``）实现了扇区对齐读写，针对非缓存行对齐的缓冲区自动使用内部对齐缓冲区中转。
- 裸机模式下延时使用 ``board_delay_us()``，FreeRTOS模式下使用 ``vTaskDelay()``。
- 使用 ``console_getchar_nonblocking()`` 实现非阻塞按键读取，避免阻塞菜单循环。


运行示例
------------

当示例成功运行后，会显示Flash参数信息及LOGO：

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

若Flash无FAT文件系统，会自动格式化，务必等待格式化完成：

.. code-block:: console

   Failed to mount SPI Nor flash, cause: There is no valid FAT volume
   Formatting the SPI Nor flash, depending on the SPI Nor flash capacity, the formatting process may take a long time
   Making file system is successful
   SPI Nor flash has been mounted successfully

挂载成功后进入菜单交互：

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

选择测试项：

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

菜单项说明：

- **1** : 创建 ``hello.txt`` 并写入测试字符串
- **2** : 读取 ``hello.txt`` 的第一行内容
- **3** : 创建目录 ``hpmicro_spi_nor_flash_test_dir0`` 后删除
- **4** : 创建 ``big_file.bin`` 并写入1MB数据，测试大文件写入能力
- **5** : 手动格式化Flash为FAT文件系统
