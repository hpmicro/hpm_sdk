.. _lobs:

LOBS
========

Overview
--------

The LOBS example project demonstrates the trace function.

In this example, the Trace Buffer setting can record 1024 records of data, each containing 16 bytes. The design is to capture 64 data before the trigger point, and after the trigger point, capture (1024-64)=960 data.

Trigger signal pins can be found in the :ref:`LOBS Trigger Pins <board_resource>`, pin0 trigger by high level, pin1 trigger by low level, lobs will be triggered when pin0 and pin1 meet the triggering conditions simultaneously.

Users can modify the macro definition LOBS_USE_TWO_GROUP in the CMakeLists.txt file to select the group working mode.

Board Setting
-------------

No special settings are required

Running the example
-------------------

- Connect the trigger signal pin0 to GND and pin1 to 3.3V, make it not meet the triggering conditions.

- Download the program to the development board and run it.

- Connect the trigger signal pin0 to 3.3V and pin1 to GND, make it meet the triggering conditions.

- After completing the trigger, the serial port will print `trace finish`.

Note: The Trace data is stored in AHB RAM, and users can view the memory through the debugger.


.. code-block:: console

    lobs example

    lobs trace finish! isr info.
    lobs trace finish!
    trace memory base addr: 0xf0200000, trace data final addr: 0xf020154c

    trig addr1: 0xf020193c, trig addr2: 0x0, trig addr3: 0x0, trig addr4: 0x0

