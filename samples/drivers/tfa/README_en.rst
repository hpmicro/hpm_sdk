.. _tfa:

TFA
======

Overview
--------

The example project demonstrates the accuracy and performance of TFA calculation, supporting the calculation of the following 8 functions:

- INV(x)

- SQRT(x)

- SIN(x)

- COS(x)

- POW(2,x)

- log2(x)

- arctan(x)

- INV(SQRT(x))

Hardware Configuration
----------------------

No special configuration is required.

Running the Program
-------------------

By default, FPU is enabled for optimal computing performance; At the same time, a solution is provided to not enable FPU.

If the example runs successfully, the serial output will be as follows:


.. code-block:: console

   *********************************************************************************
   *                                                                               *
   *                         TFA Example Menu                                      *
   *                                                                               *
   * 0 - Run basic test, compare value and time with math lib                      *
   * 1 - Run interrupt test, max test 4 nest interrupt                              *
   *                                                                               *
   *********************************************************************************

Enable FPU:

- Menu options 0~7 tested the basic usage methods of 8 TFA functions, compared the difference between the calculation results and the library function calculation results, and compared the calculation cycle with the library function calculation cycle. The running results are as follows:

.. code-block:: console

   0
   tfa and math diff value:-0.000000, math calculation time:33 ticks, tfa calculation time:62 ticks.
   1
   tfa and math diff value:0.000000, math calculation time:363 ticks, tfa calculation time:63 ticks.
   2
   tfa and math diff value:0.000000, math calculation time:1755 ticks, tfa calculation time:63 ticks.
   3
   tfa and math diff value:0.000000, math calculation time:1826 ticks, tfa calculation time:63 ticks.
   4
   tfa and math diff value:0.000000, math calculation time:3031 ticks, tfa calculation time:63 ticks.
   5
   tfa and math diff value:-0.000000, math calculation time:2875 ticks, tfa calculation time:63 ticks.
   6
   tfa and math diff value:-0.000000, math calculation time:1959 ticks, tfa calculation time:180 ticks.
   7
   tfa and math diff value:-0.000000, math calculation time:552 ticks, tfa calculation time:63 ticks.

- Menu option 8 tested a complex calculation based on the above operation, comparing the difference between the calculation result and the library function calculation result, and comparing the calculation cycle with the library function calculation cycle. The running result is as follows:

.. code-block:: console

   8
   tfa and math diff value:-0.000000, math calculation time:9088 ticks, tfa calculation time:290 ticks.


Do not activate FPU:

- Menu options 0~7 tested the basic usage methods of 8 TFA functions, compared the difference between the calculation results and the library function calculation results, and compared the calculation cycle with the library function calculation cycle. The running results are as follows:

.. code-block:: console

   0
   tfa and math diff value:-0.000000, math calculation time:67 ticks, tfa calculation time:248 ticks.
   1
   tfa and math diff value:0.000000, math calculation time:357 ticks, tfa calculation time:234 ticks.
   2
   tfa and math diff value:0.000000, math calculation time:1703 ticks, tfa calculation time:235 ticks.
   3
   tfa and math diff value:0.000000, math calculation time:1764 ticks, tfa calculation time:237 ticks.
   4
   tfa and math diff value:0.000000, math calculation time:2989 ticks, tfa calculation time:250 ticks.
   5
   tfa and math diff value:-0.000000, math calculation time:2863 ticks, tfa calculation time:227 ticks.
   6
   tfa and math diff value:-0.000000, math calculation time:2045 ticks, tfa calculation time:346 ticks.
   7
   tfa and math diff value:-0.000000, math calculation time:539 ticks, tfa calculation time:228 ticks.

- Menu option 8 tested a complex calculation based on the above operation, comparing the difference between the calculation result and the library function calculation result, and comparing the calculation cycle with the library function calculation cycle. The running result is as follows:

.. code-block:: console

   8
   tfa and math diff value:-0.000000, math calculation time:9178 ticks, tfa calculation time:1263 ticks.
