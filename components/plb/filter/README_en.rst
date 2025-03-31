.. _plb_filter_component:

PLB Filter Component
=====================

Overview
--------

The TRIGMUX of HPMicro MCU has filter function, providing digital filters for some input signals, supporting rapid change mode, delay mode, stable low mode, and stable high mode. But this filter only applies to input signals from external pins and cannot filter internal signals.

To solve this problem, this component uses PLB to implement filters, which also support rapid change mode, delay mode, stable low mode, and stable high mode. By configuring TRIGMUX, it can filter both external pin input signals and internal signals that can be connected to TRIGMUX.

Introduction to Filter Design
-----------------------------

Rapid Change Mode
~~~~~~~~~~~~~~~~~

- After the input of the filter is flipped, the output will immediately flip and then ignore the input of the filter for a certain period of time. In this mode, the filter output will closely follow the input while avoiding any spikes in the output signal.

  .. image:: doc/rapid_change.png

- Outline Design

  - This design has two stages, with the first stage being the TYPE-B logic unit (outputting OUT0 as a latch signal) and the second stage being the TYPE-A logic unit.
  - TYPE_B Design

    - The edge signal of the original signal is used as the input signal IN0 for the PLB TYPE-B logic unit
    - The output signal of PLB is OUT0, which also serves as the input signal IN1 for the PLB TYPE-B logic unit
    - The output signal of PLB is OUT1, which also serves as the input signal IN2 for the PLB TYPE-B logic unit
    - Define the truth table as follows:

      .. list-table::
         :header-rows: 1

         * - IN2
           - IN1
           - IN0
           - Operation
         * - 0
           - 0
           - 0
           - keep cnt
         * - 0
           - 0
           - 1
           - cnt + 1
         * - 0
           - 1
           - 0
           - cnt + 1
         * - 0
           - 1
           - 1
           - cnt + 1
         * - 1
           - x
           - x
           - cnt from cmp0, cmp0 value is 0

    - The output logic of OUT0 is: when the cnt value is `greater than` cmp0, the output of OUT0 is 1; otherwise, it is 0.
    - The output logic of OUT1 is: when the cnt value is `greater than or equal to` cmp1, the output of OUT1 is 1; otherwise, it is 0. The value of cmp1 can be set, which is the delay time that the user needs to define, measured in plb clock tick;
    - The initial value of cnt is 0
  - TYPE_A Design

    - The original signal serves as the input signal IN0 for the PLB TYPE-A logic unit;
    - The edge signal of the original signal serves as the input signal IN1 for the PLB TYPE-A logic unit;
    - The output signal OUT0 of TYPE-B logic unit serves as the input signal IN2 of TYPE-A logic unit;
    - The output signal OUT0 of TYPE-A logic unit serves as the input signal IN3 of TYPE-A logic unit;
    - Define the truth table as follows:

      .. list-table::
         :header-rows: 1

         * - IN3
           - IN2
           - IN1
           - IN0
           - Output
         * - 0
           - 0
           - x
           - 0
           - 0
         * - 0
           - 0
           - 1
           - 1
           - 1
         * - 1
           - 1
           - x
           - x
           - 1
         * - 1
           - 0
           - x
           - 1
           - 1
         * - 1
           - 0
           - 0
           - 0
           - 0
         * - 1
           - 0
           - 1
           - 0
           - 0
         * - 0
           - 1
           - x
           - x
           - 0
         * - 0
           - 0
           - 0
           - 1
           - 1

    - OUT0 is the filtered output signal.

Delay Mode
~~~~~~~~~~

- After the input of the filter is flipped, it needs to be held for a certain period of time before the output of the filter can be flipped.

  .. image:: doc/delay_mode.png

- Outline Design
  - The original signal serves as the input signal IN0 for the PLB TYPE-B logic unit, and the edge signal of the original signal serves as the input signal IN2 for the PLB TYPE-B logic unit;
  - The output signal of PLB is OUT0, which also serves as the input signal IN1 for the PLB TYPE-B logic unit;
  - Define the truth table as follows:

    .. list-table::
       :header-rows: 1

       * - IN2
         - IN1
         - IN0
         - Operation
         - Note
       * - 0
         - 0
         - 0
         - keep cnt
         - stable
       * - 0
         - 0
         - 1
         - cnt + 1
         -
       * - 0
         - 1
         - 0
         - cnt - 1
         -
       * - 0
         - 1
         - 1
         - keep cnt
         - stable
       * - 1
         - 0
         - 0
         - cnt + 1
         - signal jitter
       * - 1
         - 0
         - 1
         - cnt from cmp1, cmp1 value is 0
         - rising edge
       * - 1
         - 1
         - 0
         - cnt from cmp2, cmp2 value is 2*cmp0
         - down edge
       * - 1
         - 1
         - 1
         - cnt - 1
         - signal jitter

  - The output logic of OUT0 is: when the cnt value is `greater than or equal to` cmp0, the output of OUT0 is 1; otherwise, it is 0. The value of cmp0 can be set, which is the delay time that the user needs to define, measured in plb clock tick;
  - The initial value of cnt is 0;
  - OUT0 is the filtered output signal.

Stable Low Mode
~~~~~~~~~~~~~~~

- After the input of the filter is set to logic 0, it needs to be held for a certain period of time before the output of the filter is set to logic 0. However, when the input of the filter is set to 1, the output of the filter will immediately be set to 1.

  .. image:: doc/stable_low.png

- Outline Design
  - The original signal serves as the input signal IN0 for the PLB TYPE-B logic unit;
  - The output signal of PLB is OUT0, which also serves as the input signal IN1 for the PLB TYPE-B logic unit;
  - Define the truth table as follows:

    .. list-table::
       :header-rows: 1

       * - IN1
         - IN0
         - Operation
       * - x
         - 1
         - cnt from cmp1, cmp1 value is 0
       * - 1
         - 0
         - cnt + 1
       * - 0
         - 0
         - keep cnt

  - The output logic of OUT0 is: when the cnt value is `less than` cmp0, the output of OUT0 is 1; otherwise, it is 0. The value of cmp0 can be set, which is the delay time that the user needs to define, measured in plb clock tick;
  - The initial value of cnt is 0;
  - OUT0 is the filtered output signal.

Stable High Mode
~~~~~~~~~~~~~~~~

- After setting the input of the filter to logic 1, it needs to be held for a certain period of time before the output of the filter will be set to logic 1, while setting the input of the filter to 0 will immediately set the output of the filter to 0.

  .. image:: doc/stable_high.png

- Outline Design
  - The original signal serves as the input signal IN0 for the PLB TYPE-B logic unit;
  - The output signal of PLB is OUT0, which also serves as the input signal IN1 for the PLB TYPE-B logic unit;
  - Define the truth table as follows:

    .. list-table::
       :header-rows: 1

       * - IN1
         - IN0
         - Operation
       * - x
         - 0
         - cnt from cmp1, cmp1 value is 0
       * - 0
         - 1
         - cnt + 1
       * - 1
         - 1
         - keep cnt

  - The output logic of OUT0 is: when the cnt value is `greater than or equal to` cmp0, the output of OUT0 is 1; otherwise, it is 0. The value of cmp0 can be set, which is the delay time that the user needs to define, measured in plb clock tick;
  - The initial value of cnt is 0;
  - OUT0 is the filtered output signal.

Usage
-----

- Add the following content to the CMakeLists.txt file in the application to enable the PLB filter component:

  .. code-block:: cmake

     set(CONFIG_HPM_PLB_FILTER 1)

- The application calls the following API for configuration:

  .. code-block:: c

     void plb_filter_init(filter_config_t *cfg)
