.. _dma_use_gptmr_event_transfer:

DMA Use GPTMR Event Transfer
========================================================

Overview
--------

- The sample shows the use of gptmr event triggers to request DMA transfers.

- the dst address for DMA transmission is the TOGGLE register of the GPIO port of the onboard LED,when the gptmr event occurs,the LED will toggle level and flash.

Board Setting
-------------

Please refer to  :ref:`Pin Description <board_resource>`  for specific board
- use the serial debug assistant and input the gptmr event request configure number

- when the gptmr event triggers to request DMA transfers is input_signal_toggle event, need to connect external PWM signal to GPTMR capture pin.

- The serial port is set to ``115200bps`` , ``1 stop bit`` , ``no parity bit``

Notice
------

-  When using the input_signal_toggle event trigger, it is recommended that the PWM input used be within 30Hz so that the LED flashing can be observed. If the input is higher than 30Hz, please use an oscilloscope to view the corresponding LED GPIO pin changes.

GPTMR V2 Notes
------------------

GPTMR V2 adds a ``prescaler`` field compared to V1. Configure the target counter clock via macro ``APP_GPTMR_TARGET_FREQ``:

.. code-block:: c

   #ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
   config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
   reload_ticks = ((uint64_t)APP_BOARD_RELOAD_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
   cmp_ticks = ((uint64_t)APP_BOARD_CMP_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
   #endif

- Prescaler formula: **Counter Clock = Source Clock / prescaler**
- V2 reload/cmp registers are 24-bit (max 16,777,215), V1 is 32-bit
- ``APP_BOARD_RELOAD_MS`` and ``APP_BOARD_CMP_MS`` control the timer reload period and compare period, with default values of 500ms and 100ms respectively


input_signal_toggle Input Frequency Range
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When ``gptmr_dma_request_on_input_signal_toggle`` (menu item 2) is selected, GPTMR operates in ``gptmr_work_mode_capture_at_both_edge`` dual-edge capture mode.
The measurable input PWM frequency range depends on both counter bit-width and prescaler:

- GPTMR V2 counter is 24-bit, max count 16,777,215; V1 is 32-bit
- Input signal period = tick difference between adjacent same-direction edges × tick period

Input Frequency Range Estimation (assuming 100MHz source clock):

.. list-table::
   :header-rows: 1

   * - APP_GPTMR_TARGET_FREQ
     - prescaler
     - Tick Period
     - Max Measurable Freq
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

   - For visual observation of LED blinking, input PWM is recommended within 30Hz; use an oscilloscope for higher frequencies
   - ``APP_GPTMR_TARGET_FREQ`` affects input capture resolution and measurable frequency range; choose an appropriate value based on the actual input signal frequency

Running the example
-------------------

- use the serial debug assistant, press the enter key to print a prompt,select the corresponding event trigger according to the prompt, and the onboard LED will flash at different frequencies.

- If you need to change the blinking frequency of the LED in 0, 1, and 3 in the demo menu, you can change the default macros APP_BOARD_RELOAD_MS and APP_BOARD_CMP_MS in the code, which are 500ms and 100ms by default. 2 LED blinking effect is determined by the input PWM of the GPTMR capture pin.

- log as follows:


.. code-block:: console

   -----------------------------------------------------------------------------------
   *                                                                                 *
   *                   trigger dma transfer using gptmr event demo                   *
   *                                                                                 *
   *        0. use gptmr cmp0 event                                                  *
   *        1. use gptmr cmp1 event                                                  *
   *        2. use gptmr input_signal_toggle event                                   *
   *        3. use gptmr reload event                                                *
   *                                                                                 *
   *---------------------------------------------------------------------------------*

