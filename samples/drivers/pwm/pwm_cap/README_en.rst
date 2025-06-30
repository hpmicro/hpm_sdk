.. _pwm_capture:

Pwm Capture
======================

Overiew
-------

The **PWM capture**  project shows capturing a centrosymmetric PWM waveform by PWM CMP and get the duty cycle of PWM by calculation.

- PWM Capture

- Center-aligned

Configurations
--------------

- Install the serial terminal, view  :ref:`board information <board_resource>` , and configure the serial terminal parameters

-  :ref:`PWM_P0 and PWM_P1 pins <board_resource>`  Check the information according to the board model

- Connect PWM_P0 and PWM_P1 pins

Details
-------

This example demonstrates how to use the PWM module's comparator (CMP) feature to capture a center-aligned PWM waveform, and calculate the duty cycle by measuring the counter values at rising and falling edges. The main process is as follows:

1. Initialize board peripherals and PWM-related pins.
2. Configure PWM output in center-aligned mode, with a period of 5ms and a duty cycle that changes within a certain range.
3. Configure PWM input capture to capture the waveform from the PWM output pin.
4. In the main loop, dynamically adjust the PWM output duty cycle, capture the actual duty cycle, and compare it with the set value.
5. If the captured duty cycle matches the set value within the allowed error, print PASS; otherwise, print FAIL.

Main Function Description
-------------------------

- `generate_central_aligned_waveform`: Configures and starts center-aligned PWM output.
- `config_pwm_input_capture`: Configures PWM input capture and enables interrupt.
- `isr_pwm_cap`: PWM capture interrupt service routine, gets counter values at rising and falling edges.
- `test_pwm_cap`: Main test process, dynamically adjusts duty cycle and verifies capture results.


Running the Demo
----------------

- PWM output duty cycle changes as a centrosymmetric waveform

- Capture waveform, calculate and output duty cycle

- Successfully capture the PWM signal, and the duty cycle is the same, the program returns PASS

- No waveform captured or the calculated duty cycle and output duty cycle do not match return FAIL

- Serial port printing information:


.. code-block:: console

   pwm capture example


   >> Test PWM output on P4, PWM capture P5

   Pwm output duty: 0.049995, Pwm cap duty: 0.054994.
   Pwm output duty: 0.049995, Pwm cap duty: 0.049995.
   Pwm output duty: 0.039996, Pwm cap duty: 0.044996.
   Pwm output duty: 0.039996, Pwm cap duty: 0.039996.
   Pwm output duty: 0.029997, Pwm cap duty: 0.034996.
   Pwm output duty: 0.029997, Pwm cap duty: 0.029997.
   Pwm output duty: 0.019998, Pwm cap duty: 0.024998.
   Pwm output duty: 0.019998, Pwm cap duty: 0.019998.
   Pwm output duty: 0.009999, Pwm cap duty: 0.014998.
   Pwm output duty: 0.009999, Pwm cap duty: 0.009999.
   PASS.
   test done


Notes
-----

- The duty cycle step is controlled by the `TEST_LOOP` macro; smaller steps mean more detailed testing.
- If the difference between the captured and set duty cycle is greater than 0.1, it is considered FAIL.
- Make sure the PWM output and capture pins are physically connected, otherwise the signal cannot be captured.
- The example uses serial port printing; please ensure the serial terminal parameters are configured correctly.

