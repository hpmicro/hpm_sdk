.. _pllctl_spreadspectrum:

PLLCTL SPREAD SPECTRUM
========================

Overview
--------

This example demonstrates the configuration spread spectrum of the Phase-Locked Loop (PLL) controller.

**Spread Spectrum Capabilities**

Implements spread spectrum modulation for EMI (Electromagnetic Interference) reduction with configurable parameters:

- **Spread Range**
    - Default: **0.5%**
    - Configurable through **SS_RANGE** macro
    - Adjustable range: **0.1% to 3.1%**

- **Modulation Frequency**
    - Default: **30 kHz**
    - Customizable via **MODULATION_FREQ** macro

- **Spread Spectrum Type**
    - Default: Downspread modulation
    - Configurable through **PLLCTL_SS_TYPE** macro

When the spread spectrum is enabled, on oscilloscope, similar FFT result is expected to be observed as following:

.. image:: ./doc/pllctl_ss.png

For comparison, the FFT result observed on the oscilloscope without spread spectrum enabled is shown below:

.. image:: ./doc/pllctl_no_ss.png

Board Setting
-------------

- The configured PLL frequency can be observed via clk_ref* pin by the oscilloscope, please refer to :ref:`Pin Description <board_resource>` for pin information of specific board.

Running the example
-------------------

When the project runs correctly, the serial port terminal will output the following information:

.. code-block:: console

   PLLCTL spread spectrum example
   PLL0CLK0 @ 80000000Mhz ss has been enabled (downspread) with range 0.5% @ 30000Hz, which can be observed on "P2[16]" pin by the oscilloscope

   PLLCTL example finished

