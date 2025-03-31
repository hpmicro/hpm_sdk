.. _uart_rx_timeout:

UART rx timeout
==============================

Overview
--------

In this example project, UART can use the principle of rx fifo timeout to receives variable length data (data size less than defined BUFF_SIZE in program).

Compared with uart_irq sample,this method can reduce the number of interrupt times triggered by reception.For example,to receive 1000 bytes,1000 interrupts are required for single-byte reception,but this method only needs 77 times.Moreover,the FIFO timeout can be used to judge whether the uart bus is idle,so as to receive data of variable length.
If the hardware supports receiving idle detection, uart can use uart_hardware_rx_idle sample.

if you want use software to support idle detection,you can use uart_software_rx_idle sample

Port Settings
-------------

- Serial port baud rate is set to ``115200bps`` , with ``one stop bit``  and ``no parity bit``

Board Setting
-------------

- Use the usb2ttl module for testing,connect the tx pin of the module to rx pin of the board to be tested, connect the rx pin of the module to tx pin of the board to be tested,and connect th gnd pin of the module to gnd pin of the board to be tested

- Please refer to  :ref:`Pin Description <board_resource>`  for specific board

Running the example
-------------------

When the sample is running correctly, input some data through the serial port, and the serial port will send the corresponding data. The serial port terminal will output information in the following form:


.. code-block:: console

   UART RX FIFO timeout example
   uart rx data size:10 interrupt count is :1
   uart rx data size:20 interrupt count is :2
   uart rx data size:30 interrupt count is :3
   uart rx data size:40 interrupt count is :4
   uart rx data size:50 interrupt count is :4
   uart rx data size:60 interrupt count is :5
   uart rx data size:70 interrupt count is :6
   uart rx data size:80 interrupt count is :7
   uart rx data size:90 interrupt count is :7
   uart rx data size:100 interrupt count is :8
   uart rx data size:200 interrupt count is :16
   uart rx data size:300 interrupt count is :24
   uart rx data size:400 interrupt count is :31
   uart rx data size:500 interrupt count is :39
   uart rx data size:600 interrupt count is :47
   uart rx data size:700 interrupt count is :54
   uart rx data size:800 interrupt count is :62
   uart rx data size:900 interrupt count is :70
   uart rx data size:1000 interrupt count is :77

-  Phenomenon of serial tools

 .. image:: doc/uart_rxfifo_timeout.png
    :alt:
