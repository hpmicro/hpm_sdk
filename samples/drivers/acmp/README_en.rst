.. _acmp:

ACMP
========

Overview
--------

The ACMP example shows how to use ACMP  to compare one analog input voltage with internal DAC output voltage.
In this example, ACMP's MIN signal connects analog input pin and ACMP's PIN signal connects to internl DAC output. The program gradually increases the DAC output voltage until it is higher than MIN signal voltage, the ACMP output level will toggle.

Note:

After ACMP output toggles,  MCU will stop adjusting the internal DAC output. At this time, the analog input voltage is very close to the internal DAC output voltage, but ACMP output level will toggle randomly due to circuit noise.

Board Setting
-------------

Please refer to  :ref:`Pin Description <board_resource>`  for specific board

1. Connect analog input signal to ACMP MIN pin
2. Connect ACMP output pin to oscilloscope or multimeter

Note:

1. The input voltage range at ACMP input pins: 0~VCC
2. Please make sure that no any extension board without power supply connects to the development board

Running the example
-------------------

When the example runs successfully, the program will adjust the output of the built-in DAC at certain intervals,
traversing from the minimum value to the maximum value. Meanwhile, the ACMP will conduct real-time voltage comparison.
Through an oscilloscope or multimeter, the level flip on the output pin of the analog comparator can be observed,
and the serial terminal will output the following information.


.. code-block:: console

   > acmp example
   > acmp out toggled, the dac set value is 0x8e

