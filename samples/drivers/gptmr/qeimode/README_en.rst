.. _gptmr_qei_mode:

GPTMR QEI MODE
============================

Overview
--------

This example project demonstrates the QEI mode function of the timer, supporting QEI UD mode, AB mode, and PD mode.

Board Setting
-------------

- Use two capture channels of GPTMR, with ch0 and ch1 in one group and ch2 and ch3 in the other group.

- Connect the encoded signal to the capture pin of GPTMR, with 2 channels, please ref to :ref:`Pin Description <board_resource>`.

Running the example
-------------------

- Run the project and execute the timer QEI mode function. The serial terminal outputs the following information, and the user inputs 0-2 to select the QEI working mode.


.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                       GPTMR QEI DEMO                        *
   *                                                             *
   * 0 - ud mode                                                 *
   * 1 - ab mode                                                 *
   * 2 - pd mode                                                 *
   *-------------------------------------------------------------*


- After selecting the mode, output phcnt data every 1 second


.. code-block:: console

   * 1 - ab mode                                                 *
   qei_phcnt: 0
   qei_phcnt: 0
   qei_phcnt: 8
   qei_phcnt: 16
   qei_phcnt: 40
   qei_phcnt: 56
   qei_phcnt: 80

