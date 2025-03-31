.. _dac:

DAC
======

Overview
--------

This example shows DAC conversions and waveform display in three working modes.

Board Setting
-------------

- Output voltage at the specified pin. （Please refer to   :ref:`Pin Description <board_resource>` ）

- If necessary, connect a jumper for VREF pin according to the HW design. （Please refer to   :ref:`Pin Description <board_resource>` ）

Running the example
-------------------

- Running log is shown in the serial terminal as follows


  .. code-block:: console

   This is a DAC demo:
   1. Direct mode
   2. Step   mode
   3. Buffer mode
   Please enter the DAC mode code:


- Select one of DAC working modes to start DAC conversion,  and then observe the waveform through an oscilloscope

  - Direct mode


    .. code-block:: console

     Please enter the DAC mode code: 1
     Set DAC to output data in direct mode
     DAC is outputting a triangle waveform in direct mode



    .. image:: doc/dac_direct_mode.png
       :alt:

  - Step mode


    .. code-block:: console

     Please enter the DAC mode code: 2
     Set DAC to output data in step mode
     DAC is outputting a saw tooth waveform in step mode



    .. image:: doc/dac_step_mode.png
       :alt:

  - Buffer mode


    .. code-block:: console

     Please enter the DAC mode code: 3
     Set DAC to output data in buffer mode
     DAC is outputting a sine waveform in buffer mode



    .. image:: doc/dac_buffer_mode.png
       :alt:

Note
----

- Exception exit

  1. In step mode or in buffer mode, press the "space" key to abort the test, and then a test mode can be reselected.
  2. In direct mode, press the "space" key to abort the loop after the test is finished, and then a test mode can be reselected.
