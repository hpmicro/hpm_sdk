.. _sram_example:

SRAM example
========================

概述
------

SRAM示例工程展示了通过FEMC SRAM接口读写内部或外部SRAM存储器的性能。

硬件设置
------------

需外接SRAM模块，本sample测试的SRAM型号为：IS62WV51216EBLL45TLI。

对于支持FEMC异步SRAM的板子，板级配置需要定义宏 ``BOARD_HAS_FEMC_ASYNC_SRAM`` （设为1）以及以下宏：

- ``BOARD_FEMC_ASYNC_SRAM_CS_INDEX``：FEMC片选索引
- ``BOARD_FEMC_ASYNC_SRAM_AD_MUX_MODE``：地址/数据复用模式
- ``BOARD_FEMC_ASYNC_SRAM_SIZE``：SRAM容量（字节）

HPM6300系列在地址线和数据线非复用模式下，只有8根地址线，IS62WV51216EBLL45TLI的高位地址线须接地。

运行现象
------------

当工程在带有SRAM的板子上正确运行后，串口终端会输出如下信息：

.. code-block:: text

   FEMC sram example
   comparison test: from 0x48000000 to 0x48001000
   **  u32 comparison succeeded
   **  u16 comparison succeeded
   **  u8 comparison succeeded

如果板子没有SRAM（未定义宏 ``BOARD_HAS_FEMC_ASYNC_SRAM``），串口终端会输出自定义板卡配置指引：

.. code-block:: text

   FEMC sram example
   No sram on board. If it's your custom board which has sram, please define macros as follows.
   - BOARD_HAS_FEMC_ASYNC_SRAM
   - BOARD_FEMC_ASYNC_SRAM_CS_INDEX
   - BOARD_FEMC_ASYNC_SRAM_AD_MUX_MODE
   - BOARD_FEMC_ASYNC_SRAM_SIZE

温馨提示
------------

当使用SRAM接口访问外部设备（非存储类），例如FPGA，需将SRAM地址空间配置成non-cacheable。可在 ``board_init()`` 中通过 ``board_init_pmp()`` 进行配置，而不必使用 ``l1c_dc_disable()``。