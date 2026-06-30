.. _mcan:

MCAN
========

Overview
--------

This sample demonstrates a comprehensive set of MCAN (Bosch M_CAN) peripheral
features on HPMicro SoCs.  All demos share a common infrastructure
(``mcan_app_common.c/.h``) and are presented through an interactive serial
menu.  Each menu option is self-contained: it initialises the CAN peripheral,
runs the test, prints the result, and returns to the menu.

Source Organisation
-------------------

The sample is split into one file per feature area so that each demo can be
read independently.

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Source file
     - Description
   * - ``mcan_app_common.c/.h``
     - Shared infrastructure: ``main()``, interactive menu, unified ISR
       dispatcher (``board_can_isr``), CAN instance table (``s_can_info[]``),
       AHB-RAM message-buffer helpers (``mcan_setup_msg_buf``), and common
       utility functions (``show_received_can_message``,
       ``get_timestamp_hex_string``, ``handle_can_error``).
   * - ``mcan_loopback_demo.c``
     - Internal-loopback tests: polling loopback for every SoC CAN instance
       (CAN 2.0 and CAN-FD), and interrupt-driven loopback for the board CAN.
   * - ``mcan_echo_demo.c``
     - Two-board echo test.  One board acts as initiator (sends a frame), the
       other as responder (echoes the payload back with a different ID).
   * - ``mcan_txrx_demo.c``
     - Bulk transmission: 2048 classic CAN 2.0 frames, then 2048 CAN-FD frames
       with manually specified bit-timing and optional Transmitter Delay
       Compensation (TDC).
   * - ``mcan_filter_demo.c``
     - Six filter test cases covering all MCAN filter types: classic filter
       (STD and EXT ID), range filter, and dual-ID (specified-ID) filter.
       Each case transmits 2048 frames and counts accepted frames.
   * - ``mcan_error_demo.c``
     - Error handling: triggers a no-ACK condition (board disconnected from
       bus) for both CAN 2.0B and CAN-FD, then decodes and prints the PSR /
       ECR registers via ``handle_can_error()``.
   * - ``mcan_rxbuf_demo.c``
     - Dedicated RX buffer (RXBUF) demo.  A classic filter routes only frames
       with ID 0x123 into RXBUF index 1; all other IDs are rejected.  TX is
       done via a dedicated TX buffer in non-blocking mode.
   * - ``mcan_tx_event_demo.c``
     - TX Event FIFO demo.  Each transmitted frame carries an 8-bit message
       marker; after transmission the hardware stores a TX-event element that
       echoes the marker back, allowing the application to correlate every
       sent frame with its transmission result.
   * - ``mcan_timestamp_demo.c``
     - Three timestamp capture modes: 32-bit TSU (Timestamp Unit), 64-bit TSU
       (extended precision), and the MCAN internal 16-bit counter.  For each
       mode both the TX-event timestamp and the RX-message timestamp are
       printed for comparison.
   * - ``mcan_timeout_demo.c``
     - Timeout counter feature: four trigger modes are exercised in sequence —
       continuous (free-running), TX-Event-FIFO-triggered, RX-FIFO0-triggered,
       and RX-FIFO1-triggered.  Each case is fully self-contained.
   * - ``mcan_txcancel_demo.c``
     - TX cancellation: demonstrates selective cancellation of pending TX FIFO
       entries and dedicated TX buffer entries.  Slot 0 is left to complete
       normally; slots 1-3 are cancelled and the cancellation-finished flags
       are verified.
   * - ``mcan_txqueue_demo.c``
     - TX Queue mode (interrupt-driven, fully asynchronous).  The CPU fills
       the hardware queue as fast as possible with 2048 frames (IDs
       0x000-0x7FF); when the queue is momentarily full the submission
       busy-spins until the hardware frees a slot.  Received loopback echoes
       are accumulated by the ISR concurrently.  The hardware schedules
       transmission by CAN priority (lowest ID first), unlike TX FIFO which
       preserves submission order.

Board Setting
-------------

No special hardware settings are required for self-contained tests (options
0, 1, 6-d).  Exceptions:

- **Options 2 / 3 (Echo test):** connect two boards to the same CAN bus.
- **Options 4 / 5 (Bulk TX):** connect the board to a CAN bus or CAN analyser
  so the transmitted frames can be observed.
- **Option 6 (Error test):** *remove* the board from the CAN bus (disconnect
  the CAN transceiver) so the no-ACK condition can be triggered.

Running the example
-------------------

When the example runs successfully, the following menu is printed on the
serial terminal:

.. code-block:: console

   *******************************************************************************
   *                                                                             *
   *                         CAN Example Menu                                    *
   *                                                                             *
   * 0 - Run loopback test for all supported CAN controllers (CAN and CANFD)     *
   * 1 - Run loopback test for board supported CAN controller (interrupt mode)   *
   * 2 - Echo test between two board:initiator                                   *
   * 3 - Echo test between two board:responder                                   *
   * 4 - Send multiple classic CAN messages for transmission check               *
   * 5 - Send multiple CANFD messages for transmission check                     *
   * 6 - CAN error test (Need to remove current node from CAN BUS for this test) *
   * 7 - CAN filter test                                                         *
   * 8 - CAN RXBUF test                                                          *
   * 9 - CAN TX EVENT FIFO test                                                  *
   * a - CAN Timestamp Test                                                      *
   * b - CAN timeout counter Test                                                *
   * c - CAN TXBUF cancellation Test                                             *
   * d - CAN TX Queue test                                                       *
   *                                                                             *
   *******************************************************************************

Menu option details
~~~~~~~~~~~~~~~~~~~

.. list-table::
   :widths: 8 15 77
   :header-rows: 1

   * - Key
     - Name
     - What it does / what to expect
   * - ``0``
     - Loopback (all CAN)
     - Iterates every MCAN instance on the SoC.  For each instance runs one
       CAN 2.0 loopback pass and one CAN-FD loopback pass in polling mode.
       Prints PASSED / FAILED per instance.  No external connection needed.
   * - ``1``
     - Loopback (interrupt)
     - Runs 2048 loopback frames on ``BOARD_APP_CAN_BASE`` using interrupts.
       Prints the received ID for every frame.  No external connection needed.
   * - ``2``
     - Echo initiator
     - Initialises CAN at 500 kbps in normal mode, sends one frame (STD ID
       0x123, DLC=8), and waits for the echo reply from the responder board.
       Prints the echoed payload.
   * - ``3``
     - Echo responder
     - Initialises CAN at 500 kbps in normal mode, waits for a frame from
       the initiator, prints it, then echoes the payload back with STD ID
       0x321.
   * - ``4``
     - Bulk TX classic
     - Transmits 2048 CAN 2.0 frames (IDs 0x000-0x7FF) at 1 Mbps in normal
       mode.  Board must be connected to a CAN bus or analyser.
   * - ``5``
     - Bulk TX CAN-FD
     - Transmits 2048 CAN-FD frames (DLC=15, 64 bytes) with manually
       configured bit-timing (1 Mbps arbitration / 2 Mbps data).  Board must
       be connected to a CAN bus or analyser.
   * - ``6``
     - Error test
     - Attempts transmission with no other node on the bus to trigger a
       no-ACK error.  Runs both CAN 2.0B (500 kbps, DLC=8) and CAN-FD
       (500 kbps / 2 Mbps, DLC=15) cases, then prints the PSR and ECR
       register state.  **Board must be disconnected from the CAN bus.**
   * - ``7``
     - Filter test
     - Runs six sub-cases in sequence, each reconfiguring the peripheral with
       a different filter type and transmitting 2048 frames to verify the
       expected accept count:

       - Case 0: Classic filter, STD ID, mask bit[2:0] → expect 256 accepted
       - Case 1: Classic filter, exact STD ID (16 IDs) → expect 16 accepted
       - Case 2: Range filter, STD ID 0x000-0x00F → expect 16 accepted
       - Case 3: Dual-ID filter, STD ID 0x22 & 0x33 → expect 2 accepted
       - Case 4: Dual-ID filter, same IDs, repeat run → expect 2 accepted
       - Case 5: Classic filter, EXT ID, mask bit[2:0] → expect 256 accepted
   * - ``8``
     - RXBUF test
     - Configures a classic filter to route STD ID 0x123 into dedicated RX
       buffer index 1.  Transmits 2048 frames; only the frame with ID 0x123
       should be stored.  Prints the received ID and PASSED / FAILED.
   * - ``9``
     - TX Event FIFO
     - Transmits 256 frames via TX FIFO with ``event_fifo_control=1`` and an
       8-bit message marker.  After each transmission the TX-event element is
       read back and the marker is printed to confirm round-trip correlation.
   * - ``a``
     - Timestamp
     - Runs three timestamp modes (32-bit TSU, 64-bit TSU, internal 16-bit
       counter).  For each mode sends one loopback frame and prints both the
       TX-event timestamp and the RX-message timestamp for comparison.
   * - ``b``
     - Timeout counter
     - Runs four timeout trigger modes in sequence: continuous, TX-Event-FIFO
       triggered, RX-FIFO0 triggered, RX-FIFO1 triggered.  Prints PASSED /
       FAILED for each mode.
   * - ``c``
     - TX cancellation
     - Submits four frames to TX FIFO slots 0-3 and immediately cancels slots
       1-3.  Then repeats using dedicated TX buffers 0-3.  Prints PASSED if
       all three cancellations completed for each scenario.
   * - ``d``
     - TX Queue
     - Enables TX Queue mode (hardware priority-ordered scheduling).  Fills
       the queue with 2048 frames (IDs 0x000-0x7FF) as fast as possible;
       loopback echoes are received asynchronously by the ISR.  Verifies that
       all 2048 IDs were echoed back and prints PASSED / FAILED.
