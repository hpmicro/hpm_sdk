.. _ewdg:

EWDG
========

Overview
--------

This example demonstrate the typical use case of EWDG IP.

Board Setting
-------------

No special settings

Running the example
-------------------

When the project runs correctly, and the serial port terminal will output the following information:

.. code-block:: console

   ****************************************************************
   *                                                              *
   *  WDG Example Menu                                            *
   *  1. Test EWDG service                                        *
   *  2. Test EWDG reset                                          *
   *  3. Test EWDG Window Mode                                    *
   ****************************************************************


Select each function by selecting the corresponding option.

NOTE
----

Case 2 and Case 3 trigger reset during test, it is recommended to test it with `flash_xip` related build targets
