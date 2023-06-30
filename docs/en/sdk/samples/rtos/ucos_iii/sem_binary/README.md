# uC/OS-III Sem Binary

## Overview

The example shows how to use uC/OS-III's sem as mutex。

## Board Setting

No special settings are required

## Running the example
There are two tasks. An OS_SEM is created to protect resource shared by the two tasks. Using mutex instead of the OS_SEM has the same phenomenon.
Led0 task should print "led0 task write", float task should print "float task write"。
When the example runs successfully, following message is displayed in the terminal:
```console
led0 task use share_data, prio : 4
led0 task : led0 task write.
float task use share_data, prio : 4
float task : float task write.
led0 task use share_data, prio : 4
led0 task : led0 task write.
float task use share_data, prio : 4
float task : float task write.
led0 task use share_data, prio : 4
led0 task : led0 task write.
float task use share_data, prio : 4
float task : float task write.
led0 task use share_data, prio : 4
led0 task : led0 task write.
float task use share_data, prio : 4
```
