.. _tamp:

TAMP
========

Overview
--------

- This example demonstrates the functionality of the tamper module, which has active mode and passive mode.

- The active mode has two pins (one pair), one pin outputs a specific waveform sequence, and the other pin receives a specific waveform sequence. When the received data sequence is inconsistent with the output sequence, it represents an intrusion event.

- Passive mode has one pin to detect level signals. In this example, the non-invasive state level is low and the invasive state level is high.

Board Setting
-------------

- Please refer to the readme.md in the board folder for pin configuration

- Connect the two pins of the active mode using a DuPont cable

- Connect the pins of passive mode to GND

Running the example
-------------------

- After the project is running correctly, the serial terminal will output the following information:


.. code-block:: console

   Tamper example

- When the two pins of the active mode are disconnected, the serial terminal will output the following information:


.. code-block:: console

   Tamper active mode trigged!
   Tamper active mode trigged!
   Tamper active mode trigged!
   Tamper active mode trigged!
   Tamper active mode trigged!

- When the pin of the passive mode is connected to a high level, the serial terminal will output the following information:


.. code-block:: console

   Tamper passive mode trigged!
   Tamper passive mode trigged!
   Tamper passive mode trigged!
   Tamper passive mode trigged!
   Tamper passive mode trigged!

