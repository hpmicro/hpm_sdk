# uC/OS-III Task Embed Sem

## Overview

The example shows how to use uC/OS-III's task embed sem。

## Board Setting

No special settings are required

## Running the example
There are two tasks using TASK_SEM to realize the synchronization.
The float task will pend until the led task post the float task's TASK_SEM.
When the example runs successfully, following message is displayed in the terminal:

```console
float task wait Task Sem .
led0 task task sem.
float task get Task Sem .
float task wait Task Sem .
led0 task task sem.
float task get Task Sem .
float task wait Task Sem .
led0 task task sem.
float task get Task Sem .
float task wait Task Sem .
led0 task task sem.
float task get Task Sem .
float task wait Task Sem .
```
