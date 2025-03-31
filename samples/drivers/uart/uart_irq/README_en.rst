.. _uart_interrupt:

UART Interrupt
==============================================================================

Overview
--------

The **uart** (Universal Asynchronous Receiver/Transmitter) driver sample project demonstrates the configuration and usage methods of UART, implementing serial data reception and transmission using interrupts.

- In the sample code, UART port resources are initialized and configured, including setting up the serial clock and the depth of the transmit and receive FIFOs, configuring interrupts to facilitate sending and receiving data through the serial port.

Port Settings
-------------

- Serial port baud rate is set to ``115200bps`` , with ``one stop bit``  and ``no parity bit``

Notice
------

-  In the project's CMakeLists.txt file, ``sdk_compile_definitions(-DCONFIG_UART_FIFO_MODE=1)`` enables FIFO by default. To disable FIFO, change the definition in your project to ``sdk_compile_definitions(-DCONFIG_UART_FIFO_MODE=0)`` or remove ``sdk_compile_definitions(-DCONFIG_UART_FIFO_MODE=1)``.

-  In FIFO mode, data in the receive FIFO will trigger a valid receive data interrupt only when the data reaches the configured receive FIFO threshold.

-  In the example, the receive FIFO threshold is set to half of the FIFO depth. Each SoC may have a different FIFO depth, and thus, different FIFO thresholds. The FIFO depth can be obtained using the uart_get_fifo_size API or the UART_SOC_FIFO_SIZE macro.

-  When FIFO mode is enabled, a receive interrupt is triggered each time the received data reaches the configured FIFO threshold, followed by data transmission. Data echoing occurs in multiples of the FIFO threshold.

-  When FIFO mode is disabled, a receive interrupt is triggered for every byte received, followed by data transmission. The number of received bytes equals the number of echoed bytes.

-  When FIFO mode is enabled, if the input string is not an integer multiple of the FIFO threshold, the echoed string will be returned in multiples of the FIFO threshold. The remaining bytes will wait for subsequent receptions to complete the threshold before being echoed.

Running the example
-------------------

- When the project runs correctly, manually input a string through the serial port, such as ``1234567890987654``. The serial terminal will then display the following information:

.. code-block:: console

    uart driver example
    use fifo mode, the uart RX/TX FIFO depth:16 bytes
    set fifo threshold to 8 bytes; uart will send back received characters on every 8 bytes
    1234567890987654

- From the serial terminal output, it can be seen that the example uses FIFO mode, with a receive FIFO depth of 16 bytes and a configured receive FIFO threshold of 8 bytes
