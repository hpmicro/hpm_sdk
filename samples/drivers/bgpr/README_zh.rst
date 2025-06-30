.. _bgpr_retention:

BGPR Retention
======================

概述
------

本示例工程展示BGPR寄存器数据保持功能。

设置BPOR的Retention位域，掉电后，若VBAT引脚有电，则BGPR数据不会丢失。

硬件设置
------------

需确保VBAT引脚有电，例如大于3V。

HPM6800EVK开发板VBAT这块有一个已知问题，具体请参见 :ref:`板卡已知问题详情 <board_known_issues>`

运行现象
------------

- 当BPOR的Retention位域初始化被置位时，串口终端会输出如下信息，且LED不会点亮。

  .. code-block:: console

    bgpr retention example
    bgpr retention enable and data initialized, please keep vbat and power down!

  或者

  .. code-block:: console

    bgpr retention example
    bgpr data retention failure, index: 0!
    bgpr data retention failure, index: 1!
    bgpr data retention failure, index: 2!
    bgpr data retention failure, index: 3!
    bgpr data retention failure, index: 4!
    bgpr data retention failure, index: 5!
    bgpr data retention failure, index: 6!
    bgpr data retention failure, index: 7!
    Because bgpr data compare failure, so re-initialize bgpr data, please keep vbat and power down again!


- 断电后再次上电或按Reset复位，串口终端会输出数据保持功能验证结果，例如验证成功时，输出log如下：

  .. code-block:: console

    bgpr retention example
    bgpr data retention success!

- 此外，LED灯的状态也能表示数据保持功能验证结果，若验证成功，LED会点亮；否则熄灭。

