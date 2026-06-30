.. _sram_example:

SRAM example
========================

Overview
--------

SRAM example project shows performance of reading and writing data in internal or external SRAM memory via FEMC SRAM interface.

Board Setting
-------------

External SRAM module is required. The tested SRAM is IS62WV51216EBLL45TLI.

For boards that support FEMC async SRAM, the board configuration should define the macro ``BOARD_HAS_FEMC_ASYNC_SRAM`` (set to 1) along with the following macros:

- ``BOARD_FEMC_ASYNC_SRAM_CS_INDEX``: FEMC chip select index
- ``BOARD_FEMC_ASYNC_SRAM_AD_MUX_MODE``: address/data multiplexing mode
- ``BOARD_FEMC_ASYNC_SRAM_SIZE``: SRAM size in bytes

HPM6300 series has only 8 address lines in non multiplexing mode of address lines and data lines, so the high address lines of IS62WV51216EBLL45TLI must be connected to GND.

Running the example
-------------------

When the project runs correctly on a board with SRAM, the serial port terminal will output the following information:

.. code-block:: text

   FEMC sram example
   comparison test: from 0x48000000 to 0x48001000
   **  u32 comparison succeeded
   **  u16 comparison succeeded
   **  u8 comparison succeeded

If the board does not have SRAM (macro ``BOARD_HAS_FEMC_ASYNC_SRAM`` is not defined), the serial port terminal will output guidance for custom board configuration:

.. code-block:: text

   FEMC sram example
   No sram on board. If it's your custom board which has sram, please define macros as follows.
   - BOARD_HAS_FEMC_ASYNC_SRAM
   - BOARD_FEMC_ASYNC_SRAM_CS_INDEX
   - BOARD_FEMC_ASYNC_SRAM_AD_MUX_MODE
   - BOARD_FEMC_ASYNC_SRAM_SIZE

Hint
----

When SRAM interface is used to access external devices (non storage class), such as FPGA, the SRAM address space should be configured as non-cacheable. This can be done in ``board_init()`` via ``board_init_pmp()`` instead of using ``l1c_dc_disable()``.