.. _threadx_hello:

ThreadX Hello
==========================

Overview
--------

The threadx hello example shows threadx task scheduling function. This example create 8 threads. Thread0 print statue message to console periodically and wakeup thread5. Thread 1 send data to message queue and thread 2 wait on it. Thread 3 and 4 try to take one semaphore and hold it for a short time. Thread 6 and 7 try to take one mutex and hold it for a short time.

Board Setting
-------------

No special settings are required

Project Configuration
---------------------

set(CONFIG_ECLIPSE_THREADX_TIMER_RESOURCE_MTIMER 1) to use mchtmr as system's tick source

Running the example
-------------------

When the example runs successfully, the thread run information will be displayed in the terminal periodically:

.. code-block:: console

   thread_0_entry
   ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro

              thread 0 events sent:          0
              thread 1 messages sent:        0
              thread 2 messages received:    0
              thread 3 obtained semaphore:   0
              thread 4 obtained semaphore:   0
              thread 5 events received:      0
              thread 6 mutex obtained:       0
              thread 7 mutex obtained:       0

   thread_5_entry
   thread_3_and_4_entry
   thread_3_and_4_entry
   thread_6_and_7_entry
   thread_6_and_7_entry
   thread_1_entry
   thread_2_entry
   ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro

              thread 0 events sent:          1
              thread 1 messages sent:        135701
              thread 2 messages received:    135659
              thread 3 obtained semaphore:   2
              thread 4 obtained semaphore:   2
              thread 5 events received:      0
              thread 6 mutex obtained:       2
              thread 7 mutex obtained:       2

