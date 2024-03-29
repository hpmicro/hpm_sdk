# CherryRB

## 概述

cherryrb示例工程展示了cherryRB环形缓冲区简单读写操作。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端读写FIFO情况：
```console

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

```
