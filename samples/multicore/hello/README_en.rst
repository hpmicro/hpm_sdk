.. _hello_demo:

Hello demo
====================

Overview
--------

The multi-core example project runs a serial port echo example on Core0 and a GPIO toggles RGB LEDs in GPTMR interrupt example on Core1.

In this project:
 - The serial port outputs "hello world"; Manually input the stirng information through the keyboard and print it through the serial port

 - The RGB leds are switching among RED, GREEN, BLUE respectively.

Board Setting
-------------

  BOOT_PIN should be configured to 0-OFF, 1-OFF

Generate, Build and Debug Multi-core projects
---------------------------------------------

Please reference :ref:`Multicore General Description <multicore_general_description>`

Running Results
---------------

- Taking HPM6750EVKMINI as an example, when the project runs successfully, the serial port terminal will output the following information:


.. code-block:: console

           Copying secondary core image to destination memory: 0x1180000
           Hello world for multicore! Secondary core control RGB leds blinking...


- The RGB leds are switching among RED, GREEN, BLUE respectively.
