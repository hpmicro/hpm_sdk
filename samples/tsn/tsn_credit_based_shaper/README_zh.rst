.. _tsn_credit_based_shaper:

tsn_credit_based_shaper
==============================================

概述
------

本示例展示了TSN CBS(IEEE802.1 Qav)流量整形输出

- 观察相同流分别从启用和不启用CBS整形的队列输出后的帧间隔差异

  未启用CBS的队列输出流不受带宽限制从而具有较小的帧间隔，启用CBS的队列输出流所占带宽受限从而具有较大的帧间隔

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用以太网线缆连接PC以太网端口和开发板RGMIII端口

工程配置
------------

- 无

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：


.. code-block:: console

     This is a TSW demo: Credit Based Shaper
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     tc: 0 - cbs enabled
     tc: 0 - utag: 1 - tts(s): 4.005782360 - frame interval(ns): 1790
     tc: 0 - utag: 1 - tts(s): 4.005783900 - frame interval(ns): 1540
     tc: 0 - utag: 1 - tts(s): 4.005785430 - frame interval(ns): 1530
     tc: 0 - utag: 1 - tts(s): 4.005786970 - frame interval(ns): 1540
     tc: 0 - utag: 1 - tts(s): 4.005788500 - frame interval(ns): 1530
     tc: 0 - utag: 1 - tts(s): 4.005790040 - frame interval(ns): 1540
     tc: 0 - utag: 1 - tts(s): 4.005791580 - frame interval(ns): 1540
     tc: 0 - utag: 1 - tts(s): 4.005793110 - frame interval(ns): 1530
     tc: 0 - utag: 1 - tts(s): 4.005794650 - frame interval(ns): 1540

     tc: 1 - cbs disabled
     tc: 1 - utag: 2 - tts(s): 4.015788850 - frame interval(ns): 760
     tc: 1 - utag: 2 - tts(s): 4.015789610 - frame interval(ns): 760
     tc: 1 - utag: 2 - tts(s): 4.015790370 - frame interval(ns): 760
     tc: 1 - utag: 2 - tts(s): 4.015791130 - frame interval(ns): 760
     tc: 1 - utag: 2 - tts(s): 4.015791890 - frame interval(ns): 760
     tc: 1 - utag: 2 - tts(s): 4.015792650 - frame interval(ns): 760
     tc: 1 - utag: 2 - tts(s): 4.015793410 - frame interval(ns): 760
     tc: 1 - utag: 2 - tts(s): 4.015794170 - frame interval(ns): 760
     tc: 1 - utag: 2 - tts(s): 4.015794930 - frame interval(ns): 760



