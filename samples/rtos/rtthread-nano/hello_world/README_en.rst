.. _rt_thread_nano_hello_world:

RT-thread-nano Hello World
====================================================

Overview
--------

The example shows RT-thread kernel task scheduling function.This case creates 8 threads and a timer which executes the print status task every cycle. Thread 1 sends messages to the event queue, and thread 0 receives messages from the event queue. Threads 2 and 3 use mailbox for communication. Threads 4 and 5 obtain the same semaphore for synchronization, and the difference in corresponding counts is within 1. Thread 6 continuously applies for memory from memory pool, while thread 7 releases the requested memory.

This case uses MSH as the console, where you can type "help" in the terminal to obtain registered commands for testing and control.

Board Setting
-------------

No special settings are required

Running the example
-------------------

When the example runs successfully, following message is displayed in the terminal:

.. code-block:: console

          rtthread basic function demo
   ----------------------------------------
       thread0 event receive:        499
       thread1 event send:           499
       thread2 mbox receive:         8
       thread3 mbox send:            8
       thread4 semphore task 1:      88
       thread5 semphore task 2:      88
       thread6 malloc:               2016
       thread7 free:                 2016
   ----------------------------------------

          rtthread basic function demo
   ----------------------------------------
       thread0 event receive:        1499
       thread1 event send:           1499
       thread2 mbox receive:         58
       thread3 mbox send:            58
       thread4 semphore task 1:      472
       thread5 semphore task 2:      472
       thread6 malloc:               26016
       thread7 free:                 26016
   ----------------------------------------

