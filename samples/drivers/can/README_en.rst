.. _can:

CAN
======

Overview
--------

The CAN sample is organized as a shared application core plus one source file per feature area. It provides the following features:

- loopback test for all SoC supported CAN controllers, focusing on the CAN and CAN-FD test

- loopback test for the board supported CAN controllers using interrupt mode

- Dual-board Echo test

- Send out multiple messages for the CAN bus connection check

- CAN Error Detection

- CAN Filter

- CAN timestamp demo

Source Organization
-------------------

The sample follows the same split-demo structure used by the MCAN sample:

- ``src/can_app_common.c`` and ``src/can_app_common.h`` hold the shared entry point, menu, CAN instance table, shared display helpers, and ISR callback dispatch.

- ``src/can_loopback_demo.c`` contains the blocking loopback coverage for all CAN instances and the interrupt-driven loopback demo for the board CAN instance.

- ``src/can_echo_demo.c`` contains the dual-board echo initiator and responder flows.

- ``src/can_txrx_demo.c`` contains the classic CAN and CAN FD bulk transmit demos used for bus connectivity checks.

- ``src/can_error_demo.c`` contains the error-detection demos, including the variant with retransmission disabled.

- ``src/can_filter_demo.c`` contains the acceptance-filter examples.

- ``src/can_timestamp_demo.c`` contains the PTPC-backed transmit/receive timestamp demos for both classic CAN and CAN FD.

Board Setting
-------------

No special settings are required

Running the example
-------------------

For a standalone build from this sample directory, use the bundled preset:

.. code-block:: console

   cmake --preset hpm6750evk2-ram-ninja
   cmake --build --preset hpm6750evk2-ram-ninja

When the example runs successfully, the board shows the following menu:


.. code-block:: console

   *********************************************************************************
   *                                                                               *
   *                         CAN Example Menu                                      *
   *                                                                               *
   * 0 - Run loopback test for all supported CAN controllers (CAN and CANFD)       *
   * 1 - Run loopback test for board supported CAN controller (interrupt mode)     *
   * 2 - Echo test between two boards:initiator                                    *
   * 3 - Echo test between two boards:responder                                    *
   * 4 - Send mulitple messages for transmission check                             *
   * 5 - CAN error test (Need to remove current node from CAN BUS for this test)   *
   * 6 - CAN filter test                                                           *
   * 7 - Send multiple CANFD messages for transmission check                       *
   * 8 - CAN error test with re-transmission disabled                              *
   * 9 - CAN timestamp feature demo                                                *
   *                                                                               *
   *********************************************************************************


Note
~~~~

- For option 0 and option 1, users can start the test by typing the option directly

- For option 2 and 3, users must connect two boards and run this demo, start the responder first, then start the initiator.

- For option 4, users must ensure that the board is attached to a valid CAN bus. If the board is connected to a CAN monitor, users can see multiple CAN messages will be sent out after this option is chosen.

- For option 7, users must ensure that the board is attached to a valid CAN bus and the bus now supports 5Mbits/s CANFD baudrate. If the board is connected

to a CANFD monitor, users will see multiple CANFD messages are sent out after this option is chosen.

- Driver-focused regression and runtime coverage lives in ``unit_test/drivers/can``. Use that target when validating helper APIs, loopback behavior, CAN FD coverage, timestamps, and boundary handling after driver changes.
