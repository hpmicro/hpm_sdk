.. _gptmr_qei_mode:

定时器QEI模式功能
====================

概述
------

该示例工程演示了定时器QEI模式功能功能，支持QEI UD模式、AB模式和PD模式。

硬件设置
------------

- 使用GPTMR的2个capture通道，ch0和ch1为一组，ch2和ch3为另外一组。

- 将编码信号连接到GPTMR的capture引脚，2个通道，请参考 :ref:`引脚描述 <board_resource>` 。

运行现象
------------

- 运行工程，示例执行定时器QEI模式功能功能，串口终端输出如下信息，用户输入0-2选择QEI工作模式。


.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                       GPTMR QEI DEMO                        *
   *                                                             *
   * 0 - ud mode                                                 *
   * 1 - ab mode                                                 *
   * 2 - pd mode                                                 *
   *-------------------------------------------------------------*


- 选择模式后，每1s输出一次phcnt数据


.. code-block:: console

   * 1 - ab mode                                                 *
   qei_phcnt: 0
   qei_phcnt: 0
   qei_phcnt: 8
   qei_phcnt: 16
   qei_phcnt: 40
   qei_phcnt: 56
   qei_phcnt: 80

