.. _brownout:

Brownout Interrupt
======================

Overview
-----------

Brownout interrupt example project demonstrates the function of triggering brownout interrupt when VPMC undervoltage occurs. In the brownout interrupt service function, the corresponding GPIO pin will be pulled high, and the triggering of the brownout interrupt can be observed through a logic analyzer or oscilloscope.

Hardware Setting
-------------------

Refer to the BROWNOUT interrupt indicator pin section in the development board readme, and connect a logic analyzer or oscilloscope.

Running Phenomenon
---------------------

When the project runs correctly, disconnect the development board power supply, and the waveform of the brownout interrupt indicator pin being pulled high can be observed on the logic analyzer or oscilloscope, indicating that the brownout interrupt has been triggered.
