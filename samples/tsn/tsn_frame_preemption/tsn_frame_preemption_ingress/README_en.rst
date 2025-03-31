.. _tsn_frame_preemption_ingress:

tsn_frame_preemption_ingress
========================================================

Overview
--------

This example shows TSN frame preemption(IEEE802.1 Qbu) output

- Observe the value of  fragment tx counter from MMS

  - Increment indicates that the preemptible traffic is successfully sent.

- On the receiving end, run the sample tsn_frame_preemption_egress on another development board and observe the order of the receiving frame and the value of  assembly ok counter.

Board Settings
--------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect RGMII ports on the two development board with an Ethernet cable

Project Configurations
----------------------

- None

Run Example
-----------

- Compiling and Downloading

- Running log is shown in the serial terminal as follows:


.. code-block:: console

     This is a TSW demo: Frame Preemption Ingress
     TSW phy init passed !
     Link Status: Down
     Link Status: Up
     Link Speed:  1000Mbps
     Link Duplex: Full duplex
     tc: 1 - utag: 2 - tts(s): 842.005733760 - frame interval(ns): 0
     tc: 0 - utag: 1 - tts(s): 842.005735630 - frame interval(ns): 1870
     FPE MMS Fragment Tx Counter: 1
     ==================================================================
     tc: 1 - utag: 2 - tts(s): 842.526724320 - frame interval(ns): 0
     tc: 0 - utag: 1 - tts(s): 842.526726000 - frame interval(ns): 1680
     FPE MMS Fragment Tx Counter: 2
     ==================================================================
     tc: 1 - utag: 2 - tts(s): 843.047703500 - frame interval(ns): 0
     tc: 0 - utag: 1 - tts(s): 843.047705180 - frame interval(ns): 1680
     FPE MMS Fragment Tx Counter: 3
     ==================================================================


