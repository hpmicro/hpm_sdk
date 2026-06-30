.. _gptmr_measure_pwm:

GPTMR measure PWM
==================================

Overview
--------

The example shows measure PWM wave.

Board Setting
-------------

Please refer to  :ref:`Pin Description <board_resource>`  for specific board.
Connect external PWM signal to GPTMR capture pin.

Running the example
-------------------

Running the example,if there is a PWM wave input, the serial terminal outputs the following information,which are the frequency and duty cycle of the PWM wave.

.. code-block:: console

   gptmr pwm measure test
   gptmr pwm measure test
   [table_0]measured frequency: 1000.049987
   [table_0]measured duty cycle: 50.00%
   [table_1]measured frequency: 1000.049987
   [table_1]measured duty cycle: 50.00%
   [table_2]measured frequency: 1000.049987
   [table_2]measured duty cycle: 50.00%
   [table_3]measured frequency: 1000.049987
   [table_3]measured duty cycle: 50.00%
   [table_4]measured frequency: 1000.049987
   [table_4]measured duty cycle: 50.00%
   [table_5]measured frequency: 1000.049987
   [table_5]measured duty cycle: 50.00%
   [table_6]measured frequency: 1000.049987
   [table_6]measured duty cycle: 50.00%
   [table_7]measured frequency: 1000.049987
   [table_7]measured duty cycle: 50.00%
   [table_8]measured frequency: 1000.049987
   [table_8]measured duty cycle: 50.00%
   [table_9]measured frequency: 1000.059997
   [table_9]measured duty cycle: 50.00%

GPTMR V2 Notes
------------------

GPTMR V2 adds a ``prescaler`` field compared to V1. Configure the target counter clock via macro ``APP_GPTMR_TARGET_FREQ``:

.. code-block:: c

   #ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
   config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
   #endif

- Prescaler formula: **Counter Clock = Source Clock / prescaler**
- V2 reload/cmp registers are 24-bit (max 16,777,215), V1 is 32-bit

Measurement Range Estimation (assuming 100MHz source clock):

.. list-table::
   :header-rows: 1

   * - APP_GPTMR_TARGET_FREQ
     - prescaler
     - Tick Period
     - Max Reliable Freq
     - Min Measurable Freq
   * - 100KHz (default)
     - 1000
     - 10 µs
     - ~50KHz
     - ~0.006 Hz
   * - 1MHz
     - 100
     - 1 µs
     - ~500KHz
     - ~0.06 Hz
   * - 10MHz
     - 10
     - 100 ns
     - ~5MHz
     - ~0.6 Hz
   * - 100MHz (prescaler=1)
     - 1
     - 10 ns
     - ~50MHz
     - ~6 Hz

.. note::

   - Increasing ``APP_GPTMR_TARGET_FREQ`` improves resolution and max measurable frequency, but reduces low-frequency capability
   - In V2, prescaler must be ≥ 1 (default 1), otherwise ``gptmr_channel_config()`` returns ``status_invalid_argument``

