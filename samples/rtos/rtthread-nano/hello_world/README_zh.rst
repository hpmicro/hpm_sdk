.. _rt_thread_nano_hello_world:

RT-thread-nano Hello World
====================================================

概述
------

该工程演示了RT-thread内核的一些基本功能。本例程创建了8个线程和一个定时器，定时器每个周期执行打印状态任务。线程1发送消息到事件队列，线程0从事件队列接收消息。线程2、3使用邮箱进行通信。线程4、5获取同一个信号量进行同步，相对应的count的差值在1以内。线程6不断地申请内存，线程7释放申请到的内存。

该用例使用msh作为控制台，可在串口助手中输入“help”获取已注册的命令进行测试控制。

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

          rtthread basic function demo
   ----------------------------------------
       thread0 event receive:        499
       thread1 event send:           499
       thread2 mbox receive:         8
       thread3 mbox send:            8
       thread4 semphore task 1:      88
       thread5 semphore task 2:      88
       thread6 malloc:               2016
       thread7 free:                 2016
   ----------------------------------------

          rtthread basic function demo
   ----------------------------------------
       thread0 event receive:        1499
       thread1 event send:           1499
       thread2 mbox receive:         58
       thread3 mbox send:            58
       thread4 semphore task 1:      472
       thread5 semphore task 2:      472
       thread6 malloc:               26016
       thread7 free:                 26016
   ----------------------------------------

