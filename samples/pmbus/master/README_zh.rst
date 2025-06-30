.. _pmbus_master:

Pmbus master
========================

概述
------

PMbus master示例工程展示了I2C在master模式作为pmbus进行接收和发送数据的功能.循环使用不同的PMbus命令，包括字节、字、块指令，并打印出读取或写入的数据长度或者错误码。

提示
------

- 组件详情文档请参见 :ref:`pmbus <pmbus>`

硬件设置
------------

将两个板子的 :ref:`I2C引脚 <board_resource>` 相连。

运行现象
------------

- 先运行slave，再运行master

- 当工程正确运行后，串口终端会输出如下信息：


.. code-block:: console

   PMbus master test
   pmbus master write test begin...
   pmbus master write test end...

   pmbus master read test begin...
   pmbus(cmd:0x00) master read len:1
   pmbus(cmd:0x06) master read len:128
   pmbus(cmd:0x22) master read len:2
   pmbus master read test end...

