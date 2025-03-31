.. _rtc:

RTC
======

Overview
--------

The RTC demo provides the following features:
- Set RTC time

- Read RTC time

- Set RTC Alarm

- Set RTC Alarm interrupt

- Enable rentention mode for Battery domain

Board Setting
-------------

No special settings are required

Running the example
-------------------

When the example runs successfully, the demo will shoe the menu on the console UART port as below:


.. code-block:: console

   ---------------------------------------------------------------------------
   *                                                                         *
   *                                   RTC Demo                              *
   *                                                                         *
   *       1. Show RTC time                                                  *
   *       2. Demonstrate 3 types of RTC alarm                               *
   *       3. Demonstrate the retention mode of Battery Domain               *
   *       4. Set RTC Time                                                   *
   *       5. Demonstrate RTC interrupt                                      *
   *                                                                         *
   ---------------------------------------------------------------------------


Type one of the listed option to run corresponding example.

NOTE
----

- Example #3 will reset the SoC, so it is better to run it with `flash_xip` or `flash_sdram_xip` related builds for better user experience
