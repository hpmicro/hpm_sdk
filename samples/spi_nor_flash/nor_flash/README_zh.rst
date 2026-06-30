.. _spi_nor_flash:

SPI NOR FLASH
================================

概览
------

本示例使用SPI接口操作NOR Flash存储器，通过serial_nor组件实现对符合SFDP标准的NOR Flash的读写擦除操作。

功能特性：
  - 自动识别Flash的SFDP参数（容量、页大小、扇区大小、块大小等）
  - 支持阻塞/非阻塞擦除（全片擦除、扇区擦除）
  - 支持阻塞编程和非阻塞页编程
  - 支持数据回读校验
  - 统计读写速度


CMakeLists配置说明
-------------------

组件依赖
^^^^^^^^^^^

.. code-block:: cmake

   set(CONFIG_SPI_NOR_FLASH 1)
   set(CONFIG_DMA_MGR 1)

- ``CONFIG_SPI_NOR_FLASH`` : 使能SPI NOR Flash组件（serial_nor），提供SFDP解析及Flash操作接口。
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

源码及头文件路径
^^^^^^^^^^^^^^^^^

.. code-block:: cmake

   sdk_inc(../common/port)
   sdk_app_src(../common/port/hpm_serial_nor_host_port.c)
   sdk_app_src(src/main.c)

- ``sdk_inc`` 将 ``common/port`` 加入头文件搜索路径，用于包含 ``hpm_serial_nor_host_port.h``。
- ``sdk_app_src`` 指定参与编译的源文件：

  - ``hpm_serial_nor_host_port.c`` : SPI NOR Flash的板级移植层，封装SPI引脚初始化及时钟配置。
  - ``main.c`` : 应用主逻辑。

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

  使用四线模式时，需确保硬件引出SPI的IO2、IO3引脚并完成相关初始化。

优化选项
^^^^^^^^^^^

.. code-block:: cmake

   sdk_compile_options("-O3")

- 通过 ``-O3`` 优化提升Flash读写性能，尤其是在大块数据传输场景下效果明显。


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
- ``serial_nor_get_board_host()`` 接口会根据板级配置自动获取SPI基地址及DMA资源。


注意事项
------------

- 该实例工程依赖 **serial_nor** 组件和 **DMA管理器** 组件，需在CMakeLists中启用对应配置宏。
- serial_nor组件支持符合 SFDP (JESD216) 标准的NOR Flash，不限定具体型号（W25Q64、W25Q128、W25Q256、MX25L128等）。
- 不同Flash的SCLK频率上限不同，修改 ``PORT_SPI_CLK_FREQUENCY`` 时请参考目标Flash的数据手册。
- 四线模式下，部分Flash需要先通过指令使能QPI模式，serial_nor组件已内置处理。
- SPI传输基于DMA管理器进行，无需用户手动管理DMA通道。


运行示例
------------

当示例成功运行后，串口终端会输出如下信息：

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

输出信息解读：

- **sfdp version** : Flash的SFDP标准版本号
- **flash size** : Flash总容量
- **page_size** : 页编程大小（通常256字节）
- **sector_size / block_size** : 最小擦除单元（扇区/块）
- **sector_erase_cmd / block_erase_cmd** : 擦除命令码
- **write_speed / read_speed** : 写入和读取速度（KB/s），速度受SPI时钟频率和IO模式影响
- **page program** : 非阻塞页编程校验结果
