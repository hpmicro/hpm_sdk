.. _ppi_async_sram_example:

PPI ASYNC SRAM example
============================================

Overview
--------

This example project shows read and write data in external async SRAM by PPI.

The PPI memory address region is 0xF8000000~0xFFFFFFFF, base address should be aligned by 1MB.

Board Setting
-------------

External async SRAM module is required. The tested async SRAM is IS62WV51216EBLL45TLI. When in AD Mux mode, the address latch chip used in this sample is SN74HC573APWR.

The PPI component supports both AD MUX mode and AD Non-MUx mode.

- When AD Non-Mux mode, `DQ0-DQ15` as data lines, `DQ16-DQ31` as address lines. The high address lines above A16 of IS62WV51216EBLL45TLI must be connected to GND.

- When AD Mux mode, `DQ0-DQ15` as data lines and address lines. The high address lines above A16 of IS62WV51216EBLL45TLI must be connected to GND.

In this example, select the AD mode using the macro `BOARD_PPI_ASYNC_SRAM_AD_MUX_MODE`, select the CS signal using the macro `BOARD_PPI_ASYNC_SRAM_CS_INDEX`, select the ADV signal using the macro `BOARD_PPI_ASYNC_SRAM_ADV_CTRL_PIN`, select the REL signal using the macro `BOARD_PPI_ASYNC_SRAM_OE_CTRL_PIN`, and select the WEL signal using the macro `BOARD_PPI_ASYNC_SRAM_WE_CTRL_PIN`. Users can make modifications based on their actual schematic diagram.

For more information on PPI components, please refer to: :ref:`PPI Components Readme <ppi_component>`

Cache Configuration Notes
----------------------------

- PPI memory address region is `Noncacheable` attribute by default, which can be configured as the `Cacheable` attribute through PMP.

- Note: PPI memory address region cacheable attribute only support: MEM_TYPE_MEM_WB_READ_WRITE_ALLOC - Memory, Write-back, Write-Allocate, Read-Allocate.

Running the example
-------------------

When the project runs correctly, and the serial port terminal will output the following information:

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

