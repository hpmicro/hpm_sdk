.. _cherryrb:

CherryRB
================

概述
------

- cherryrb示例工程展示了cherryRB环形缓冲区简单读写操作。
- 主要演示了多线程环境下生产者-消费者模型的实现原理

实例功能说明
------------
- 核心功能：通过环形缓冲区实现两个FreeRTOS线程间的异步数据传输
- 生产者线程：每秒向环形缓冲区写入"hello world"字符串
- 消费者线程：每秒从环形缓冲区读取数据并打印到串口


硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端读写FIFO情况：

.. code-block:: console

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

