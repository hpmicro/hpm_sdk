.. _tsn_credit_based_shaper:

tsn_credit_based_shaper
==============================================

Overview
--------

This example shows TSN CBS(IEEE802.1 Qav) traffic shaper output

- Observe the difference in frame intervals between the same stream output from queues with and without CBS shaping enabled.

  The output streams of queues without CBS enabled are not limited in bandwidth with a smaller frame interval. The output streams of queues with CBS enabled are limited in bandwidth with a bigger frame interval.

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect an Ethernet port on PC to a RGMII port on the development board with an Ethernet cable

Project Configurations
----------------------

- None

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


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



