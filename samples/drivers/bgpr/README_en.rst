.. _bpgr_retention:

BPGR Retention
======================

Overview
--------

This example project demonstrates the data retention function of the BPGR register.

Board Setting
-------------

Ensure that the VBAT pin is powered, for example, greater than 3V.

HPM6800EVK has a known issue about VBAT, please refer to :ref:`Board known issue details <board_known_issues>`

Running the example
-------------------

- When the Retention field of BPOR is initialized and set, the serial terminal will output the following information and the LED will not light up.

.. code-block:: console

  bpgr retention example
  bpgr retention enable and data initialized, please keep vbat and power down!

- After power off/on again or press reset. The serial port terminal will output the data retention function verification result. For example, when the verification is successful, the output log is as follows:

.. code-block:: console

  bpgr retention example
  bpgr data retention success!

- In addition, the status of the LED light can also indicate the verification result of the data retention function. If the verification is successful, the LED will light up; Otherwise, it will not light up.
