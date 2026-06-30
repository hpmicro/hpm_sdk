.. _adc16_differential:

ADC16 Differential
==================

Overview
--------

This example demonstrates ADC16 differential input using master/slave ADC pairs
in sequence mode with hardware trigger and DMA. Results are printed on the
console UART.

Board Setting
-------------

- See the **ADC16 Differential Input** section in the target board README for ADC instances, pins, and connectors
- Connect differential inputs as described in the board documentation

Running the example
-------------------

- Build and flash for the target board
- View periodic differential conversion results on the console UART

Expected console output
-----------------------

- The UART first shows board init text (clock summary, banner, etc.; varies by board), then the sample output below
- After one startup line, a differential result is printed about every 500 ms (from ``board_delay_ms(500)`` in the main loop, not the ADC hardware rate)
- The line prefix depends on the board master/slave ADC pair and may be ``ADC2/ADC3`` or ``ADC0/ADC1``; see the target board README
- Each line is ``0xXXXX (N)``: ``0xXXXX`` is the DMA low 16-bit raw hex; ``(N)`` is decimal decoded from the master diff config. Under the default config ``(N)`` is signed and typically drifts slightly around zero at near-zero differential; see the *Diff result encoding* section below

  Examples below assume default config ``position_mode=differential``, ``full_resolution=0``:

  .. code-block:: console

   ADC16 differential sample
   ADC2/ADC3 diff result: 0x0003 (3)
   ADC2/ADC3 diff result: 0xfffc (-4)
   ADC2/ADC3 diff result: 0x0001 (1)

  Boards using an ADC0/ADC1 pair may show:

  .. code-block:: console

   ADC16 differential sample
   ADC0/ADC1 diff result: 0x0000 (0)
   ADC0/ADC1 diff result: 0xfff8 (-8)

  Examples below use theoretical codes at Vref about 3.3 V under the default config; measured values may vary slightly on hardware.

  With about 1.5 V on the master pin and 1.0 V on the slave pin (0.5 V differential), typical output from this sample looks like:

  .. code-block:: console

   ADC0/ADC1 diff result: 0x1388 (5000)

  With about 1.5 V on the master pin and 0.5 V on the slave pin (about 1.0 V differential), typical output from this sample looks like:

  .. code-block:: console

   ADC2/ADC3 diff result: 0x2710 (10000)

Differential master/slave and polarity
--------------------------------------

- Master/slave are defined in the target board ``board.h``: use ``BOARD_APP_ADC16_DIFF_PAIR23_*`` for the fixed ADC2/ADC3 pair and ``BOARD_APP_ADC16_DIFF_PAIR01_*`` for the fixed ADC0/ADC1 pair (``PAIR23``/``PAIR01`` are SoC diff-pair IDs, not ADC instance numbers). If the board designates one ADC as master, the sample reads its DMA buffer and prints from that side. To swap master within the pair, update ``diff_cfg_*_master.master`` and move DMA plus printing to the new master instance
- ADC0/ADC1 and ADC2/ADC3 pairs follow the same master/slave and polarity rules

Diff result encoding (position_mode / full_resolution)
------------------------------------------------------

Two master CONV_CFG0 fields select how the differential result is encoded on the 16-bit bus (not how the analog inputs are wired):

- full_resolution (FULL): when 1, clamps algebraic diff less than 0 to 0 and ignores position_mode
- position_mode (POS_MODE): only effective when full_resolution is 0

.. list-table:: Encoding summary
   :header-rows: 1

   * - full_resolution
     - position_mode
     - Bus semantics
     - Zero differential
   * - 0
     - differential (POS=0)
     - Signed −32768..+32767; positive when master pin is higher than slave
     - 0 (0x0000)
   * - 0
     - single_ended (POS=1)
     - Unsigned 0..65535 offset code
     - 32768 (0x8000)
   * - 1
     - (any, ignored)
     - Unsigned 0..65535; algebraic diff less than 0 clamped to 0
     - 0 (0x0000)

This sample default
~~~~~~~~~~~~~~~~~~~

- adc16_differential.c sets master to position_mode=differential, full_resolution=0 (adc16_get_default_diff_mode_config defaults are single_ended and 1)
- UART 0x%04x (N): 0x%04x is the averaged DMA raw; (N) is decoded per the table above and matches HW bus semantics (no zero remapping)

Voltage and polarity examples
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Examples below assume Vref about 3.3 V and default config (full_resolution=0, position_mode=differential); codes are approximate raw units, not mV
- About 0.5 V differential (e.g. 1.5 V master, 1.0 V slave): master higher than slave about +5000 (0x1388); reversed about −5000 (0xec78)
- About 1.0 V differential (e.g. 1.5 V master, 0.5 V slave): master higher than slave about +10000 (0x2710); reversed about −10000 (0xd8f0)
- Set master full_resolution to 1 and rebuild (POS_MODE ignored): at 0.5 V differential about +9930 (0x26ca) when master is higher; about 0 when reversed
- Table lists ADC0/ADC1 first, then ADC2/ADC3; swapping which instance is master flips polarity per master definition; same rules apply

.. list-table:: Polarity at default config (approx.)
   :header-rows: 1

   * - Master
     - Slave
     - Master pin
     - Slave pin
     - Result (approx.)
   * - ADC0
     - ADC1
     - 1.5 V
     - 1.0 V
     - +5000 (0x1388)
   * - ADC0
     - ADC1
     - 1.0 V
     - 1.5 V
     - -5000 (0xec78)
   * - ADC2
     - ADC3
     - 1.5 V
     - 1.0 V
     - +5000 (0x1388)
   * - ADC2
     - ADC3
     - 1.0 V
     - 1.5 V
     - -5000 (0xec78)

Note
----

- SoC differential pairing is fixed: **ADC0 pairs with ADC1 only, ADC2 pairs with ADC3 only**; cross pairing (e.g. ADC0 with ADC3) is not supported
- Both the master and slave ADC in the pair must **enable differential mode together**; do not enable it on a single ADC alone
- The instance with ``DIFF_MASTER=1`` outputs **this ADC minus its pair**; exactly **one** member of the pair is master
- ADC16 differential mode **does not support software trigger**; besides sequence mode, preemption mode is also supported, and both require hardware **same-source triggering** (this sample demonstrates hardware triggering in sequence mode)
- Keep neither differential leg too close to 0 V or 3.3 V with respect to ground; otherwise either ADC input may saturate and bias the differential result
