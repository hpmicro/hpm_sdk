.. _1_wire:

1-Wire
============

Overview
--------

This example shows that accessing a digital temperature sensor DS18B20 via 1-wire bus.

HPM 1-Wire features as follows:

- 1-Wire Master

- Reset and Presence Pulses output and 1-Wire Slave detection

- single or multiple byte read and write

- DMA mode

- SRA mode

- Interrupt

Board Setting
-------------

None

Background
----------

1-Wire(single bus) is a peripheral serial expansion bus technology bus launched by DALLAS company, as the name suggests, it is the use of a signal line for communication, both the transmission of clock signals and data transmission, and can carry out two-way communication, with saving I/O port lines, simple resource structure, low cost, easy to expand and maintain the bus and many other advantages. Commonly used single-bus devices are generally stable sensors, EEPROM, unique serial number chips, such as DS18B20, DS243 and so on.

Running the example
-------------------

- Running log is shown in the serial terminal as follows


  .. code-block:: console

   This is a onewire sample: DS18B20
   OWR Slave Detected!
   Family Code: 28
   Serial Num:67 e2 79 00 00 00
   CRC: 3a
   Thermometer resolution: 12 bit
   Current Temp: 26.94 degree centigrade
   Current Temp: 26.94 degree centigrade
   Current Temp: 26.87 degree centigrade
   Current Temp: 26.94 degree centigrade
   Current Temp: 26.87 degree centigrade
   Current Temp: 26.87 degree centigrade
   Current Temp: 26.94 degree centigrade
   Current Temp: 27.25 degree centigrade
   Current Temp: 27.69 degree centigrade
   Current Temp: 28.06 degree centigrade
   Current Temp: 28.31 degree centigrade
   Current Temp: 28.50 degree centigrade
   Current Temp: 28.62 degree centigrade
   Current Temp: 28.44 degree centigrade
   Current Temp: 28.31 degree centigrade
   Current Temp: 28.19 degree centigrade
   Current Temp: 28.06 degree centigrade

