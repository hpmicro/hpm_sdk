.. _modbus_rtu_master:

modbus rtu master
==================================

概述
------

- 该工程演示了modbus rtu主机功能

- 在这个示例工程中， 一个板子作为rtu master， 另一个板子作为rtu slave。 Master发送读取保持寄存器等待slave回应请求数据。

注意
------

- 使用uart进行modbus通信，与rtu从机通信，必须先复位从机再复位主机。

硬件设置
------------

- 请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。

- 将两个板子的modbus uart引脚相连。

- 两个板子必须相连双方GND进行共地，保证信号传输。

运行现象
------------

- 先运行slave，再运行master

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   modbus rtu master example
   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

   recv slave msg len:25
   Hold Registers[0~9]:0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 0x0008 0x0009

