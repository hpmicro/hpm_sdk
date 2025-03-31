.. _adc16:

ADC16
==========

Overview
--------

- This example shows ADC16 conversions and results in four working modes.

Notes:

* When ADC is configured to non-bus-read (oneshot) mode, it can't be accessed otherwise bus will be stuck.

* When ADC is configured to bus-read (oneshot) mode,  CPU has to complete initialization before accessing to the ADC register list.

Board Setting
-------------

- Input voltage at the specified pin. （Please refer to   :ref:`Pin Description <board_resource>` ）

- If necessary, connect a jumper for VREF pin according to the HW design. （Please refer to   :ref:`Pin Description <board_resource>` ）

Note:  The input voltage range at ADC input pins: 0~VREFH

Running the example
-------------------

- Running log is shown in the serial terminal as follows


  .. code-block:: console

   This is an ADC16 demo:
   1. Oneshot    mode
   2. Period     mode
   3. Sequence   mode
   4. Preemption mode
   Please enter one of ADC conversion modes above (e.g. 1 or 2 ...):


- Select one of ADC working modes to start ADC conversion,  and then watch conversion results

  - Oneshot mode


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 1
     Oneshot Mode - ADC3 [channel 2] - Result: 0xffe8
     Oneshot Mode - ADC3 [channel 2] - Result: 0xfff4
     Oneshot Mode - ADC3 [channel 2] - Result: 0xffff



  - Period mode


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 2
     Period Mode - ADC3 [channel 2] - Result: 0xfff3
     Period Mode - ADC3 [channel 2] - Result: 0xfff5
     Period Mode - ADC3 [channel 2] - Result: 0xfff7



  - Sequence mode


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 3
     Sequence Mode - ADC3 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 02 - Result: 0xffff
     Sequence Mode - ADC3 - Cycle Bit: 00 - Sequence Number:00 - ADC Channel: 02 - Result: 0xffd9
     Sequence Mode - ADC3 - Cycle Bit: 01 - Sequence Number:00 - ADC Channel: 02 - Result: 0xffff



  - Preemption mode


    .. code-block:: console

     Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): 4
     Preemption Mode - ADC3 - Trigger Channel: 00 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 02 - Result: 0xffff
     Preemption Mode - ADC3 - Trigger Channel: 00 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 02 - Result: 0xffff
     Preemption Mode - ADC3 - Trigger Channel: 00 - Cycle Bit: 01 - Sequence Number: 00 - ADC Channel: 02 - Result: 0xffff



Note
----

- How to use WDOG feature

  - Channel initialization

    - Set ch_cfg. wdog_int_en to true

    - Set ch_cfg.thshdl/ch_cfg.thshdh

      The ch_cfg.thshdl/ch_cfg.thshdh can be configured from 0 to 65535. If any ADC conversion result is out of the thresholds (thshdl, thsdhh), a WDOG interrupt will occur.
  - Call adc16_init_channel () API.

  - ISR

    - Set one or more WDOG event flags depending on  ADC channels

    - Disable one or more corresponding WDOG interrupts

  - Main loop

    - Handle with WDOG events

    - Enable one or more corresponding WDOG interrupts

- Trigger source

  - In this sample, for sequence mode and preemption mode, PWM is selected as the trigger source. Other peripherals can also be selected as the trigger source.

  - The frequency of the trigger signal(20KHz by default) can be configured in sample-level CMakeLists.txt(e.g. sdk_compile_definitions(-DAPP_ADC16_TRIG_SRC_FREQUENCY=20000))

- Exception exit

  Press the "space" key to abort the test, and then a test mode can be reselected.
