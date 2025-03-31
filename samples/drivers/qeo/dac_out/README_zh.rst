.. _qeo_dac_out:

QEO_DAC_OUT
======================

概述
------

QEO_DAC_OUT示例工程展示QEO根据得到的位置信息，生成弦波，可通过DAC输出弦波波形的功能。

硬件设置
------------

观察指定DAC引脚的输出电压，(请参考 :ref:`引脚描述 <board_resource>` 部分)。

注意
------

QEO支持输出3路弦波信号，能从DAC引脚上观察弦波数目， 受到DAC数目限制。

运行现象
------------

- 当工程正确运行后， 通过示波器观察DAC引脚上的电压信号，串口终端会输出如下信息：


.. code-block:: console

   QEO DAC wave example
   QEO generate wave with sofeware inject postion
   qeo wave0 output:
   65535
   65526
   65496
   65447
   65378
   65290
   65182
   65054
   64907
   64740
   64554
   64349
   64126
   63883
   63621
   63342
   63043
   62726
   ...
   qeo wave1 output:
   16384
   15696
   15015
   14344
   13684
   13036
   12400
   11777
   11165
   10567
   9982
   9410
   8854
   8311
   7783
   7270
   ...
   qeo wave2 output:
   16382
   17081
   17792
   18512
   19240
   19976
   20721
   21472
   22231
   22995
   23766
   24542
   25323
   26108
   26897
   27690
   28486
   ...
   QEO generate wave base on hardware(MMC) provide position

串口输出软件注入位置模式下，3相弦波的数值，使用工具(excel等)，将数据拟合得到波形。


.. image:: doc/qeo_dac_1.png
   :alt:

使用示波器观察DAC引脚电压波形，下图为观察2路弦波的波形


.. image:: doc/qeo_dac_2.png
   :alt:
