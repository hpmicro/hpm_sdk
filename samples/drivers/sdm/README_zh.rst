.. _sdm:

SDM
======

概述
------

SDM示例工程演示了SDM外设的对Sigma-Delta调制器输出的数据流进行数据采样和幅值检测的功能。

硬件设置
------------

请参考具体开发板 :ref:`引脚描述 <board_resource>` 部分的描述。
根据开发板是否板载AD采样芯片，采用不同的接线方式。

- 如开发板上无板载AD采样芯片，请使用外接采样芯片，将采样芯片的输出CLK和DAT引脚到板上SDM的CLK和DAT。

- 如开发板上板载AD采样芯片，仅需将模拟信号接到采样芯片输入接口。

注意
------

本示例中提供了适用于AD7400、NSI1306等Sigma-Delta调制器(Full-scale ±320 mV)的输入电压与输出数值的转换公式， 如使用不同类型Sigma-Delta调制器请检查转换公式。

运行现象
------------

运行工程，串口会输出如下信息，输入不同数字执行不同的测试功能：

.. code-block:: console

   sdm example
   *********************************************************************
   *                                                                   *
   *                         SDM Example Menu                          *
   * Please enter one of following SDM function test(e.g. 1 or 2 ...): *
   * 1 - SDM sample data in polling mode                               *
   * 2 - SDM amplitude threshold check                                 *
   * 3 - SDM sample data in interrupt mode                             *
   * 4 - SDM sample data with sync signal                              *
   *                                                                   *
   *********************************************************************


1. SDM sample data in polling mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SDM轮询数据有效标志并从FIFO中读出采样数据。

.. code-block:: console

   Selected option: 1

   sdm sample data in polling mode.
   203.57mV
   203.17mV
   203.31mV
   203.14mV
   203.01mV
   203.28mV
   203.17mV
   203.05mV
   203.19mV
   203.21mV
   203.02mV
   203.19mV
   203.43mV
   203.17mV
   203.29mV
   203.24mV


2. SDM amplitude threshold check
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SDM使用幅值监测通道监测电压超限事件。设置幅值检测的高电压阈值为SDM_AMPLITUDE_HIGH_THRESHOLD，当输入模拟电压由小于阈值的情况增大到超出阈值时，触发错误中断。

.. code-block:: console

   Selected option: 2

   Set amplitude high threshold 240mV
   sdm channel comparator detected amplitude over the high threshold
   voltage: 253.75mV


3. SDM sample data in interrupt mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SDM使用FIFO的数据有效标志进行采样，当FIFO中的数据达到预设的阈值时，SDM会触发数据有效中断，中断中读取数据，当读取的数据数目达到TEST_DATA_COUNT时停止继续采样。

.. code-block:: console

   Selected option: 3

   sdm sample data in interrupt mode.
   203.39mV
   203.08mV
   202.96mV
   202.87mV
   202.85mV
   202.86mV
   202.86mV
   202.84mV
   202.81mV
   202.88mV
   203.01mV
   203.23mV
   203.08mV
   202.97mV
   202.92mV
   202.97mV


4. SDM sample data with sync signal
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

本例中使用GPTMR输出通过TRGMMUX配置连接到SDM的同步输入信号，当GPTMR的输出信号上升沿时触发SDM的同步事件。
SDM会根据同步信号进行采样，当同步事件发生时，SDM开始采样数据进入FIFO；当FIFO中的数据达到预设的阈值时，SDM会触发数据有效中断去读取数据，并且硬件会清除同步事件标志，停止继续采样直到下一个同步事件发生。

.. code-block:: console

   Selected option: 4

   sdm sample data with sync signal. 1 time
   203.88mV
   203.84mV
   203.90mV
   203.87mV
   203.91mV
   203.92mV
   203.85mV
   203.85mV
   sdm sample data with sync signal. 2 time
   203.90mV
   203.96mV
   203.97mV
   203.93mV
   203.94mV
   203.86mV
   203.89mV
   203.90mV
   sdm sample data with sync signal. 3 time
   203.94mV
   204.03mV
   203.97mV
   203.91mV
   203.89mV
   203.87mV
   203.93mV
   203.97mV
   sdm sample data with sync signal. 4 time
   203.93mV
   204.13mV
   204.14mV
   204.16mV
   204.17mV
   204.13mV
   204.14mV
   204.12mV

