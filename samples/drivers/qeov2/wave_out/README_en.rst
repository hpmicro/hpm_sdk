.. _qeov2_wave_out:

QEOV2_WAVE_OUT
============================

Overview
--------

The QEO_WAVE_OUT sample project demonstrates the functionality of QEOv2 (Quadrature Encoder Output Version 2) peripheral in generating three-phase waveforms based on position information. QEOv2 can receive position information through software injection, and generate three phase-adjustable cosine waves based on preset resolution lines, which can be used to modulate PWM duty cycles.

Working Process
----------------

This example demonstrates waveform generation in software position injection mode:

Configuration steps:

- Initialize QEO module and configure wave output mode:

  * Set wave type as cosine
  * Set resolution lines (4 lines)
  * Configure phase shifts for three-phase output (120°/240°)

- Software position injection process:

  * Calculate position increment value
  * Enable software position injection
  * Cyclically inject position values for wave generation
  * Sample and record wave output values
  * Disable position injection

Features
-----------

1. Wave Generation Features:

   - Supports three independent cosine wave outputs
   - Configurable phase shift for each output
   - Selectable wave type (cosine)
   - Supports wave amplitude range limitation
   - Supports VD/VQ input (can be from CLC)
   - Supports PWM duty cycle modulation

Running the Example
-------------------

When the project runs correctly, you will observe the following:

1. Serial terminal output:

.. code-block:: console

   QEO DAC wave example
   QEO generate wave with sofeware inject postion
   qeo wave0 output:
   4294967231
   4294320511
   4292380543
   4289148543
   4284626559
   4278817215
   4271723967
   4263351231
   4253703935
   4242787967
   4230609855
   4217176959
   4202497343
   4186579839
   4169434111
   4151070399
   ...
   qeo wave1 output:
   1073742315
   1028424448
   983780736
   939838187
   896622827
   854161110
   812478230
   771599680
   731549760
   692352875
   654032427
   616611283
   580112595
   544558249
   509969001
   476366270
   443770281
   412200745
   381676478
   352215358
   323836286
   296555113
   ...
   qeo wave2 output:
   1073742315
   1119706795
   1166290390
   1213465131
   1261202603
   1309473664
   1358249600
   1407501035
   1457197867
   1507310891
   1557809281
   1608663254
   1659841537
   1711313494
   1763048278
   1815014315
   1867180993
   1919516545
   1971989142
   2024567723
   2077220118
   2129915137
   2182619773
   2235304168
   2287935613
   2340482429
   2392912936
   2445195581
   ...

2. Wave Output:

Using tools like Excel to process the serial output data, you can obtain the three-phase cosine waveform:
- Three outputs are cosine waves with 120° phase differences
- Wave period is determined by resolution lines
- Wave amplitude range is 0-0xFFFFFFFF (when not limited)

.. image:: doc/qeo_dac_1.png
   :alt: Three-phase Cosine Waveform
