.. _segment_led_key:

SEGMENT LED KEY
===============

Overview
------------

`SEGMENT LED KEY` example project shows 5 LEDs and 2x3 key matrix functions. By using the `components/segment_led` component, it is easy to control the blink of any segment of each digital tube.

Running the example
-------------------

- The blinking DP of the LED digital tube indicates that the digital tube is in the selected state. The digital tube can be selected by pressing the left and right buttons; The selected digital tube display value can be operated by pressing the up and down buttons.

- The blink mode of LED digital tube showed in DIG0, representing:

  - 1 - DIG2, DIG3, DIG4 do not blink;

  - 2 - DIG2 blink, DIG3 and DIG4 do not blink;

  - 3 - DIG3 blink, DIG2 and DIG4 do not blink;

  - 4 - DIG4 blink, DIG2 and DIG3 do not blink;

  - 5 - DIG2, DIG3, DIG4 are all blink;

  - 6 - Partial segments of DIG2, DIG3, and DIG4 are blink.

- The LED digital tube DIG1 is blank.

- The LED digital tubes DIG2 - DIG5 display hex values within the range of 0x00 - 0x0F.

- When the program is running, the serial port debugging terminal will first output clock configuration information. When the button is pressed, the serial debugging terminal will print the button status. As shown below:

.. code-block:: console

  segment led key example

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
