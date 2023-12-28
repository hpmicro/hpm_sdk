# uC/OS-III Time Slice

## Overview

The example shows uC/OS-III time slice scheduling feature.

## Board Setting

No special settings are required

## Running the example

When the example runs successfully, task float will print runtimes and five numbers, led0_task will print runtimes and "LED : 12345".
The dual task are created at same priority, and both of the task does not call sched functions to trigger task switch. So without time slice scheduling method, it will run only one of the tasks. Because of time slice scheduling feature, tasks at same priority will be scheduled.
"run x times" message will be print to console without task schedule because CPU_CRITICAL_ENTER() is called.
Other message maybe interrupted randomly.
following message is displayed in the terminal:

```console
led0_task run 0 times
LED : 1234float run 0 times
5 float : 6789
0
LED : 12float : 345
67890
LED floa: 12345
t : 67890
LED : 12345 float : 678
90
LED : 1234float :5
 67890
led0_task run 1 times
LED :float run 1 times
 12float : 6789345
0
LED float : : 12345
67890
floLED : 12345 at : 67890

LED : 123float : 6745
890
LED : float 12345
: 67890
led0_task run 2 times
float run 2 times
LED float : 6789: 12345
0
float :LED : 12345 67890

flLED : 123oat : 67890 45

LED :float : 67 12345
890
float LED : 12345 : 67890
```
