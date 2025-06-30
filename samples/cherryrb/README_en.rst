.. _cherryrb:

CherryRB
================

Overview
------------

- The cherryrb sample project demonstrates basic read/write operations using the CherryRB ring buffer.
- Implements the producer-consumer pattern in a multi-threaded environment.

Functional Description
------------------------

- Core functionality: Asynchronous data transfer between two FreeRTOS threads via ring buffer
- Producer thread: Writes "hello world" string to the ring buffer every second
- Consumer thread: Reads data from the ring buffer every second and prints to UART

Hardware Setup
------------------

No special hardware configuration required

Runtime Behavior
------------------

When the program runs correctly, the UART terminal will display FIFO operations:

.. code-block:: console

   chry_ringbuffer_init success
   [P] write success
   [C] read success, read 11 byte
   hello world
   [P] write success
   [C] read success, read 11 byte
   hello world
   [P] write success
   [C] read success, read 11 byte
   hello world
   [P] write success
   [C] read success, read 11 byte
   hello world
   [P] write success
   [C] read success, read 11 byte
   hello world
   [P] write success
   [C] read success, read 11 byte
   hello world
   [P] write success
   [C] read success, read 11 byte

