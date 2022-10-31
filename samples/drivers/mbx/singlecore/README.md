# MBX singlecore
## Overview

This example demonstrate how MBX works. The data sent by interface A can be received by interface B. Similarly, data sent by interface B can be received by interface A.

## Board Setting

No special settings

## Running the Example

When the project runs correctly, and the serial port terminal will output the following information:
```
mbx single core example
mbx singleword communication start
mbxa sending 0
mbxb got 0
mbxb sending 1
mbxa: got 1
mbx singleword communication done
mbx multiword communication start
mbxa got 4 free space
mbxa sending fifo
mbxa sent fifo 4
mbxb sent 0
mbxb sent 1
mbxb sent 2
mbxb sent 3
mbxb getting fifo messages
mbxb got fifo 4
mbxb got 0
mbxb got 1
mbxb got 2
mbxb got 3
mbx multiword communication done

```
