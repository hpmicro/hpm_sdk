.. _adc16_temperature:

ADC16 Temperature
==================================

Overview
--------

This example shows the soc temperature measured by ADC16

Board Setting
-------------

- If necessary, connect a jumper for VREF pin according to the HW design.（Please refer to   :ref:`Pin Description <board_resource>` ）

Running the example
-------------------

- Running log is shown in the serial terminal as follows


  .. code-block:: console

   This is an ADC16 temperature acquisition demo:
   ADC16 clock: input=200000000 Hz, div=4, conv=50000000 Hz
   ADC16 sample: cycle=511, convert=21, fs=93984 Hz
   Current Soc Temp: 26℃
   Current Soc Temp: 26℃
   Current Soc Temp: 26℃
   Current Soc Temp: 28℃
   Current Soc Temp: 27℃
   Current Soc Temp: 26℃
   Current Soc Temp: 27℃
   Current Soc Temp: 28℃
   Current Soc Temp: 29℃
   Current Soc Temp: 27℃
   Current Soc Temp: 30℃
   Current Soc Temp: 28℃
   Current Soc Temp: 28℃
   Current Soc Temp: 26℃
   Current Soc Temp: 28℃
   Current Soc Temp: 26℃
   Current Soc Temp: 28℃
   Current Soc Temp: 28℃

- The clock lines above are from ``hpm6750evk2`` (AHB 200 MHz). This sample requires ADC16 temperature sensor support.

