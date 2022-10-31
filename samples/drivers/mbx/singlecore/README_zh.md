# MBX singlecore
## 概述

该示例程序演示了MBX如何工作。通过MBXA发送的数据，可以通过MBXB接收； 通过MBXB发送的数据， 可以通过MBXA接收。

## 开发板设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
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