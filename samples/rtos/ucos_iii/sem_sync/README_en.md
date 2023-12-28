# uC/OS-III Sem Sync

## Overview

The example shows how to use uC/OS-III's sem to sync tasks。

## Board Setting

No special settings are required

## Running the example

There are two tasks using OS_SEM to realize the synchronization.
The float task will pend until the led task post the OS_SEM.
When the example runs successfully, following message is displayed in the terminal:

```console
led0 task sfloat task weait Mutex ait Mutex SeSem tm to Sync.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
led0 task send mutex sem.
float task get sem
float task wait Mutex Sem to Sync.
```
