.. _ppi_async_sram_example:

PPI ASYNC SRAM example
============================================

概述
------

本示例工程展示了使用PPI外设读写外部异步SRAM存储器。

PPI外设的地址空间为：0xF8000000~0xFFFFFFFF，基地址需1MB对齐。

硬件设置
------------

需外接异步SRAM模块，本sample测试的异步SRAM型号为：IS62WV51216EBLL45TLI。AD复用模式时，本sample使用的地址锁存器型号为：SN74HC573APWR。

PPI组件支持AD复用模式和AD非复用模式。

- 使用地址和数据非复用模式时，`DQ0-DQ15` 作为数据线，`DQ16-DQ31` 作为地址线。IS62WV51216EBLL45TLI的高位地址A16以上的须接地。

- 使用地址和数据复用模式时，`DQ0-DQ15` 作为数据线和地址线。IS62WV51216EBLL45TLI的高位地址A16以上的须接地。

本示例中，使用宏 `BOARD_PPI_ASYNC_SRAM_AD_MUX_MODE` 选择AD的工作模式，使用宏 `BOARD_PPI_ASYNC_SRAM_CS_INDEX` 选择CS信号，使用宏 `BOARD_PPI_ASYNC_SRAM_ADV_CTRL_PIN` 选择ADV信号，使用宏 `BOARD_PPI_ASYNC_SRAM_OE_CTRL_PIN` 选择REL信号，使用宏 `BOARD_PPI_ASYNC_SRAM_WE_CTRL_PIN` 选择WEL信号。用户可根据自己实际原理图进行修改。

PPI组件的更多信息，请参考：:ref:`PPI组件Readme <ppi_component>`

Cache配置说明
-----------------

- PPI 内存地址空间默认为 `Noncacheable` 属性，可通过PMP配置为 `Cacheable` 属性。

- 需要注意的是，PPI内存地址空间的Cacheable属性仅支持： MEM_TYPE_MEM_WB_READ_WRITE_ALLOC - Memory, Write-back, Write-Allocate, Read-Allocate。

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: text

   ppi sram example

   Noncacheable Test
   comparison test: from 0xf8000000 to 0xf8100000
   write u32 elapsed tick: 1950364
   read u32 elapsed tick: 2107645
   **  u32 comparison succeeded
   write u16 elapsed tick: 2202048
   read u16 elapsed tick: 2579520
   **  u16 comparison succeeded
   write u8 elapsed tick: 4404064
   read u8 elapsed tick: 5033188
   **  u8 comparison succeeded

   Cacheable Test
   comparison test: from 0xf8000000 to 0xf8100000
   write u32 elapsed tick: 1696406
   read u32 elapsed tick: 1881572
   **  u32 comparison succeeded
   write u16 elapsed tick: 1652905
   read u16 elapsed tick: 2141575
   **  u16 comparison succeeded
   write u8 elapsed tick: 1658193
   read u8 elapsed tick: 2406436
   **  u8 comparison succeeded

