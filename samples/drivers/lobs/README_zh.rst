.. _lobs:

LOBS
========

概述
------

LOBS示例工程展示LOBS实现的Trace功能。

本示例中，Trace Buffer设置可以记录1024笔数据，每笔数据16字节。设计为：触发点之前抓64笔数据，触发点之后抓（1024-64）=960 笔数据。

触发信号引脚详见开发板文档 :ref:`LOBS 触发信号 <board_resource>` 相关内容，trig pin0高电平触发，trig pin1低电平触发，trig pin0和trig pin1同时满足触发条件时触发。

用户可在CMakeLists.txt文件中，修改宏定义 LOBS_USE_TWO_GROUP 选择group工作模式。

硬件设置
------------

无特殊设置

运行现象
------------

- 将触发信号trig pin0接低电平GND，trig pin1接高电平3.3V，使其不满足触发条件。

- 将程序下载至开发板运行。

- 将触发信号trig pin0接高电平3.3V，trig pin1接低电平GND，使其满足触发条件。

- 完成触发后，串口将打印 `trace finish`。

备注： Trace数据存在AHB RAM中，用户可通过调试器查看memory的方式查看。


.. code-block:: console

    lobs example

    lobs trace finish! isr info.
    lobs trace finish!
    trace memory base addr: 0xf0200000, trace data final addr: 0xf020154c

    trig addr1: 0xf020193c, trig addr2: 0x0, trig addr3: 0x0, trig addr4: 0x0

