.. _eui:

EUI
======

Overview
--------

EUI example project shows 5 LEDs and 2x3 key matrix functions

Running the example
-------------------

- The LEDs will blink and loop display 3 sets of data, with each set blinking 10 times.

- The button interrupt is triggered once when KEY is pressed, and then again when released.

- The serial port debugging console will first print clock configuration information, and then print the status information of key presses (no status information will be printed when the key is released). The printf information demo as follows:

.. code-block:: console

  eui example

  clko_tm_us:10, slot_tm_us:320, hold_tm_us:0, disp_tm_us:2560, filter_tm_us:51200

   esc: 1, up: 0, enter: 0
  left: 0, dn: 0, right: 0

   esc: 0, up: 1, enter: 0
  left: 0, dn: 0, right: 0

   esc: 0, up: 0, enter: 1
  left: 0, dn: 0, right: 0

   esc: 0, up: 0, enter: 0
  left: 0, dn: 0, right: 1

   esc: 0, up: 0, enter: 0
  left: 0, dn: 1, right: 0

   esc: 0, up: 0, enter: 0
  left: 1, dn: 0, right: 0

   esc: 0, up: 1, enter: 0
  left: 0, dn: 0, right: 0

   esc: 1, up: 0, enter: 0
  left: 0, dn: 0, right: 0

   esc: 0, up: 0, enter: 1
  left: 0, dn: 0, right: 0

Others
------

Advanced digital tubes blink control can be achieved by using the `components/segment_led` component, the sample is `samples/segment_led_key`.
