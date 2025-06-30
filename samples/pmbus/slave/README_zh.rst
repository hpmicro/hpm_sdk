.. _pmbus_slave:

Pmbus slave
======================

概述
------

PMbus slave示例工程展示了I2C在slave模式作为pmbus进行接收和发送数据的功能.需要注册需要解析的PMbus命令事务处理，PMbus从站协议栈会通过完成回调函数打印接收或发送的数据以及命令相关消息。

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

   PMbus slave test
   pmbus(cmd:0x05) slave command transaction install success
   pmbus(cmd:0x06) slave command transaction install success
   pmbus(cmd:0x00) slave command transaction install success
   pmbus(cmd:0x03) slave command transaction install success
   pmbus(cmd:0x22) slave command transaction install success
   cb:cmd: 0x05, read:true, pec_check:true, len:128, data_addr:0x0120a054
   cb:cmd: 0x00, read:true, pec_check:true, len:1, data_addr:0x0120a054
   cb:cmd: 0x03, read:true, pec_check:true, len:0, data_addr:0x0120a054
   cb:cmd: 0x22, read:true, pec_check:true, len:2, data_addr:0x0120a054
   cb:cmd: 0x00, read:false, pec_check:true, len:1, data_addr:0x01209fd4
   cb:cmd: 0x06, read:false, pec_check:true, len:128, data_addr:0x01209fd4

