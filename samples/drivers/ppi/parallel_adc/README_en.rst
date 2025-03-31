.. _ppi_parallel_adc_example:

PPI Parallel ADC example
================================================

Overview
--------

This example project shows read data from external parallel ADC by PPI, using asynchronous SRAM interface to access.

The PPI memory address region is 0xF8000000~0xFFFFFFFF, base address should be aligned by 1MB.

Board Setting
-------------

External parallel ADC module is required. The tested parallel ADC is MS9280.

Running the example
-------------------

When the project runs correctly, and the serial port terminal will output the following information:

.. code-block:: text

   ppi parallel adc example
   adc data: 0xc3
   adc data: 0xc2
   adc data: 0xc3
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2

