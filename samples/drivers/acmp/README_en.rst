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

When the example runs successfully, ACMP will perform voltage comparison and output level reversal can be observed through the oscilloscope or multimeter, and the following message is displayed in the terminal(dac set value varies with the input analog signal level):


.. code-block:: console

   > acmp example
   > acmp out toggled, the dac set value is 0x8e

