.. _uart_rxline_status:

UART_RXLINE_STATUS
====================================

Overview
--------

This sample show uart rxline status detection function, included Overrun errors, parity errors, framing errors, and line breaks

Board Setting
-------------

- Except for the break tes, all others Use the usb2ttl module for testing,connect the tx pin of the module to rx pin of the board to be tested, connect the rx pin of the module to tx pin of the board to be tested,and connect th gnd pin of the module to gnd pin of the board to be tested

- Please refer to  :ref:`Pin Description <board_resource>`  for specific board

Notice
------

- According to each detection test, the uart configuration is different and needs to be configured according to the serial port configuration information output by the terminal.

Run steps
---------

When the project runs correctly, the serial port terminal will output the information like：

.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                   uart rxline status demo                   *
   *                                                             *
   *        1 - overrun error test for rx line status            *
   *        2 - parity error test for rx line status             *
   *        3 - framing error test for rx line status            *
   *        4 - line break test for rx line status               *
   *-------------------------------------------------------------*


overrun error test
~~~~~~~~~~~~~~~~~~

- Manually input the '1' character through the serial port, it will prompt that the uart of the sending end needs to be configured as follows in advance

- Serial port baud rate is set to ``115200bps`` , ``8 bits data length``  with ``one stop bit``  and ``no parity bit``

- The data sent needs to be greater than the RXFIFO length, such as the 16 bytes shown in the following log.

- Waiting for the corresponding error status to be detected


.. code-block:: console

   1
   overrun error test start.......
   ##############################################################
   step.1......
   For the sender, please set the following uart configuration:
   baudrate:115200
   data bits:8
   parity bits:none
   stop bits:1bits
   ##############################################################
   step.2......
   Please send length more than 16 bytes
   ##############################################################
   step.3......
   waiting the rxline status detection......
   overrun error status: true
   ##############################################################


parity error test
~~~~~~~~~~~~~~~~~

- Manually input the '2' character through the serial port, it will prompt that the uart of the sending end needs to be configured as follows in advance

- Serial port baud rate is set to ``115200bps`` , ``8 bits data length``  with ``one stop bit``  and ``even parity bit``

- The sender sends some bytes

- Waiting for the corresponding error status to be detected


.. code-block:: console

   2
   parity error test start.......
   ##############################################################
   step.1......
   For the sender, please set the following uart configuration:
   baudrate:115200
   data bits:8
   parity bits:even
   stop bits:1bits
   ##############################################################
   step.2......
   Please send some bytes
   ##############################################################
   step.3......
   waiting the rxline status detection......
   parity error status: true
   ##############################################################


framing error test
~~~~~~~~~~~~~~~~~~

- Manually input the '3' character through the serial port, it will prompt that the uart of the sending end needs to be configured as follows in advance：

- Serial port baud rate is set to ``115200bps`` , ``8 bits data length``  with ``one stop bit``  and ``no parity bit``

- The sender sends some bytes

- Waiting for the corresponding error status to be detected


.. code-block:: console

   3
   framing error test start.......
   ##############################################################
   step.1......
   For the sender, please set the following uart configuration:
   baudrate:115200
   data bits:8
   parity bits:none
   stop bits:1bits
   ##############################################################
   step.2......
   Please send length more than 16 bytes
   ##############################################################
   step.3......
   waiting the rxline status detection......
   framing error status: true
   ##############################################################


Known Issues
^^^^^^^^^^^^

- When the stop bit settings of the sender and receiver are inconsistent, the frame error detection function cannot be guaranteed.

line break test
~~~~~~~~~~~~~~~

- connect uart rx pin to break signal pin(Please refer to   :ref:`Pin Description <board_resource>` )

- Manually input the '4' character through the serial port

- If the uart rx pin is connected the break signal pin, enter 't' to confirm

- Waiting for the corresponding error status to be detected


.. code-block:: console

   4
   line break test start.......
   ##############################################################
   step.1......
   Please connect uart rx pin to break signal pin. enter 't' to confirm.......
   t
   ##############################################################
   waiting the rxline status detection......
   line break status: true
   ##############################################################

