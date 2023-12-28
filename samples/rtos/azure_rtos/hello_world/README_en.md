# ThreadX Hello

## Overview

The threadx hello example shows threadx task scheduling function. This example create 8 threads. Thread0 print statue message to console periodically and wakeup thread5. Thread 1 send data to message queue and thread 2 wait on it. Thread 3 and 4 try to take one semaphore and hold it for a short time. Thread 6 and 7 try to take one mutex and hold it for a short time.

## Board Setting

No special settings are required

## Running the example

When the example runs successfully, the thread run information will be displayed in the terminal periodically:
```console
ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro

           thread 0 events sent:          2
           thread 1 messages sent:        210731
           thread 2 messages received:    210635
           thread 3 obtained semaphore:   3
           thread 4 obtained semaphore:   3
           thread 5 events received:      1
           thread 6 mutex obtained:       3
           thread 7 mutex obtained:       3


ThreadX Hpmicro Demonstration **** (c) 2023 Hpmicro

           thread 0 events sent:          3
           thread 1 messages sent:        454838
           thread 2 messages received:    454818
           thread 3 obtained semaphore:   6
           thread 4 obtained semaphore:   5
           thread 5 events received:      2
           thread 6 mutex obtained:       6
           thread 7 mutex obtained:       5
```
