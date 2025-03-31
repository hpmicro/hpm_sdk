.. _dma_use_gptmr_event_transfer:

定时器触发请求DMA传输
========================

概述
------

- 本示例工程展示了使用定时器事件触发请求DMA传输。

- DMA传输的设备地址是板载的LED的GPIO口的TOGGLE寄存器。当发生对应定时器事件，对应的LED会翻转电平进行闪烁。

硬件设置
------------

请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
- 使用串口调试助手进行输入定时器请求配置。

- 请求配置为输入捕获翻转时，需要连接外部PWM至GPTMR输入捕获引脚。

- 串口波特率设置为 ``115200bps`` ，``1个停止位`` ，``无奇偶校验位``

注意
------

- 当使用input_signal_toggle事件触发时，所使用的PWM输入建议在30Hz以内，以便可观察LED闪烁情况，若高于30Hz输入请使用示波器查看对应的LED GPIO引脚变化

运行现象
------------

- 使用串口调试助手，敲击enter按键会打印提示，根据提示选择对应的事件触发，板载的LED会进行不同频率的闪烁。

- demo菜单中0、1、3如果若需要改变LED的闪烁频率，可以改变代码中的缺省宏APP_BOARD_RELOAD_MS和APP_BOARD_CMP_MS，缺省下分别为500ms和100ms。2的LED闪烁效果由GPTMR的输入捕获引脚的输入PWM决定。

- 信息如下：


.. code-block:: console

   -----------------------------------------------------------------------------------
   *                                                                                 *
   *                   trigger dma transfer using gptmr event demo                   *
   *                                                                                 *
   *        0. use gptmr cmp0 event                                                  *
   *        1. use gptmr cmp1 event                                                  *
   *        2. use gptmr input_signal_toggle event                                   *
   *        3. use gptmr reload event                                                *
   *                                                                                 *
   *---------------------------------------------------------------------------------*

