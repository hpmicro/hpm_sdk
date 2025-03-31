.. _uart_rxline_status:

串口接收状态检测
================

概述
------

该示例工程中，展示串口在接收时检测各种错误状态，支持数据过载，奇偶校验错误，帧错误，传输打断错误。

硬件设置
------------

- 除了break测试，其他需要使用usb转ttl模块进行测试，将模块的TX接在开发板待测的RX，模块的RX接在开发板待测的TX，模块的GND接在开发板的GND

- 请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。

注意
------

- 根据每个检测测试，设置的串口配置不一样，需要根据终端输出的串口配置信息进行配置。

运行步骤
------------

当工程正确运行后，串口终端会收到如下界面信息：

.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                   uart rxline status demo                   *
   *                                                             *
   *        1 - overrun error test for rx line status            *
   *        2 - parity error test for rx line status             *
   *        3 - framing error test for rx line status            *
   *        4 - line break test for rx line status               *
   *-------------------------------------------------------------*


数据过载测试
~~~~~~~~~~~~~~~~~~

- 通过串口手动输入'1'字符，会提示发送端的串口需要提前配置以下：

- 波特率设置为 ``115200bps`` ，``8个数据位`` ，``1个停止位`` ，``无奇偶校验位``

- 发送的数据需要大于RXFIFO长度，比如以下log提示的16字节

- 等待检测到对应错误状态


.. code-block:: console

   1
   overrun error test start.......
   ##############################################################
   step.1......
   For the sender, please set the following uart configuration:
   baudrate:115200
   data bits:8
   parity bits:none
   stop bits:1bits
   ##############################################################
   step.2......
   Please send length more than 16 bytes
   ##############################################################
   step.3......
   waiting the rxline status detection......
   overrun error status: true
   ##############################################################


奇偶校验错误测试
~~~~~~~~~~~~~~~~~~~~~~~~

- 通过串口手动输入'2'字符，会提示发送端的串口需要提前配置以下：

- 发送端串口设置为 ``115200bps`` ，``8个数据位`` ，``1个停止位`` ，``even偶数校验位``

- 发送端发送一些字节

- 等待检测到对应错误状态


.. code-block:: console

   2
   parity error test start.......
   ##############################################################
   step.1......
   For the sender, please set the following uart configuration:
   baudrate:115200
   data bits:8
   parity bits:even
   stop bits:1bits
   ##############################################################
   step.2......
   Please send some bytes
   ##############################################################
   step.3......
   waiting the rxline status detection......
   parity error status: true
   ##############################################################


帧错误测试
~~~~~~~~~~~~~~~

- 通过串口手动输入'3'字符，会提示发送端的串口需要提前配置以下：

- 发送端串口设置为 ``115200bps`` ，``8个数据位`` ，``1个停止位`` ，``无奇偶校验位``

- 发送端发送一些字节

- 等待检测到对应错误状态


.. code-block:: console

   3
   framing error test start.......
   ##############################################################
   step.1......
   For the sender, please set the following uart configuration:
   baudrate:115200
   data bits:8
   parity bits:none
   stop bits:1bits
   ##############################################################
   step.2......
   Please send length more than 16 bytes
   ##############################################################
   step.3......
   waiting the rxline status detection......
   framing error status: true
   ##############################################################


已知问题
^^^^^^^^^^^^

- 当收发双方停止位设置不一致时，该frame error检测功能无法保证。

传输打断测试
~~~~~~~~~~~~~~~~~~

- 接收RX引脚接入到break信号引脚(请参考  :ref:`引脚描述 <board_resource>`  部分)

- 通过串口手动输入'4'字符

- 如果已经连接break信号引脚，输入't'确认

- 等待检测到对应错误状态


.. code-block:: console

   4
   line break test start.......
   ##############################################################
   step.1......
   Please connect uart rx pin to break signal pin. enter 't' to confirm.......
   t
   ##############################################################
   waiting the rxline status detection......
   line break status: true
   ##############################################################

