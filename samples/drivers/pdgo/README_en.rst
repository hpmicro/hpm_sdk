.. _pdgo:

PDGO
========

Overview
--------

The pdgo demo provides the following features:

- PDGO turn-off

- PDGO turn-off and one-shot wake-up

- PDGO turn-off and automatic wake-up

- PDGO GPR retention

- WKUP pin wakeup from shutdown mode

- WUIO pins wakeup from shutdown mode (4 configurable WUIO pins)

- PCAP (Pulse Capture) wakeup from shutdown mode

- PCNT (Pulse Counter) wakeup from shutdown mode

.. note::

   The WKUP pin, WUIO pins, PCAP, and PCNT wakeup features are only supported on some SoC series.
   On other SoCs, selecting these options will print a "not supported" message.

Board Setting
-------------

No special settings are required

To test external signal wakeup (supported on some SoC series):

- WKUP pin wakeup: Connect a signal source to WKUP pin

- WUIO pins wakeup: Connect signal sources to WUIO pins

- PCAP wakeup: Connect a pulse signal to the configured WUIO pin

- PCNT wakeup: Connect encoder signals (A/B/Z/H) to WUIO pins

Running the example
-------------------

When the example runs successfully, the board shows the following menu:

.. code-block:: console

   ***********************************************************************
   *                                                                     *
   *     DGO Turn-off and Wake-up test                                   *
   *                                                                     *
   *     a - DGO Turn off                                                *
   *     b - DGO Turn off, System one-shot wakeup                        *
   *     c - DGO Turn off, System auto wakeup                            *
   *         Note: After wake-up, use option 'a' to test auto wake-up    *
   *     d - DGO GPR retention, System one-shot wakeup                   *
   *     e - DGO Turn off, WKUP pin wakeup                               *
   *     f - DGO Turn off, WUIO pins wakeup                              *
   *     g - DGO Turn off, PCAP wakeup                                   *
   *     h - DGO Turn off, PCNT wakeup                                   *
   *                                                                     *
   ***********************************************************************
