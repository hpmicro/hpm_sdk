.. _cherry_shell_barebone_usb:

Cherry Shell Barebone USB
==================================================

概述
------

Cherry Shell Barebone示例工程展示了shell的裸机模式基本使用。在这个工程中，需要连接 USB，shell默认需要登陆，密码为12345678。输入密码后回车，进入shell，输入help可以查看命令和变量。
请勿使用串口助手，推荐下载使用 `MobaXterm <https://mobaxterm.mobatek.net/download.html>`_

硬件设置
------------

无特殊设置

运行现象
------------

- 当工程正确运行后，串口终端会输出如下信息：


.. code-block:: console

   [I/USB] Open ep:0x83 type:3 mps:8
   [I/USB] Open ep:0x01 type:2 mps:512
   [I/USB] Open ep:0x81 type:2 mps:512
   Initialize shell successfully

- 打开USB虚拟串口

- 如果此时通过键盘手动输入正确密码"12345678"并回车，终端会输出如下信息：


.. code-block:: console

   login as: cherry
   cherry@hpm5301evklite's password:
   welcome to cherry shell
   cherry@hpm5301evklite:/$


- 如果此时通过键盘手动输入命令"help"并回车，终端会输出如下信息：


.. code-block:: console

   cherry@hpm5301evklite:/$ help
   total function 6
     test       -> /bin
     toggle_led -> /bin
     write_led  -> /bin
     exit       -> /sbin
     help       -> /sbin
     shsize     -> /sbin

   total variable 2
     $PATH r-  11
     $ZERO r-   1
   cherry@hpm5301evklite:/$


- 如果此时通过键盘手动输入命令"toggle_led"并回车，LED将会翻转

- 如果此时通过键盘手动输入命令"write_led 1"并回车，LED将会点亮

- 如果此时通过键盘手动输入命令"write_led 0"并回车，LED将会熄灭
