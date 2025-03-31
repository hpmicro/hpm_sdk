.. _cdc_acm_uart_com:

CDC ACM Uart COM
================================

Overview
--------

This example project shows USB CDC Serial

- PC sees a serial port via Device Manager

- The USB CDC ACM is used to the UART serial port of the MCU, and the pins description in the readme of the board folder, which name is USB_CDC_ACM_UART.

- The UART receives data using DMA cyclic reception method. Start the timer to periodically query data, and if there is any data, it will be sent out through a USB IN transaction.

- UART sends data in DMA normal mode. When the USB OUT transaction receives data, UART DMA sends data.

- The UART rx buffer of the application is read every 5ms, with a buffer size of 512, so the maximum baudrate is 921600. If users wish to increase the maximum baudrate, they can shorten the timer period or increase the UART rx buffer.

Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

- Using USB to TTL level serial port module, USB side connected to computer, UART side connected to USB_CDC_ACM_UART pins

Running the example
-------------------

- Download the program to the development board and run it. The computer can automatically recognize and install the serial port driver, and list a COM port device called COM_A.

- Connect USB to TTL level serial port module to computer, the computer recognizes a COM port device called COM_B.

- Open two "Serial Port Debugging Assistant" software, with the same communication settings(baudrate, parity, etc.), and one open COM_A, one open COM_B.

- Enter characters in the serial port debugging assistant of COM_A, click send, COM_B's serial port debugging assistant will receive the characters. Enter characters in the serial port debugging assistant of COM_B, click send, COM_A's serial port debugging assistant will receive the characters.
