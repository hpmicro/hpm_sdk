.. _t_shape_accel_decel_pulses:

GPTMR Generates T-Shaped Acceleration and Deceleration Pulses
=============================================================

Overview
--------

- The `t_shape_accel_decel` example project demonstrates the functionality of GPTMR generating a specified number of T-shaped acceleration and deceleration pulses within a specified time. It can also serve as a reference for generating a specific number of pulses.

- The pulse generation automatically stops after the DMA transfer is completed.

Hardware Setup
--------------

- Refer to the specific development board's :ref:`Pin Description <board_resource>` section for details.

- Connect the GPTMR compare output pin to an oscilloscope or logic analyzer.

Software Setup
--------------

- For details about the GPTMR driver interface, refer to :ref:`gptmr <gptmr>`.

- If you need to set the initial output compare polarity of GPTMR to high, to avoid unexpected output pulses during initialization, use the `gptmr_channel_set_cmp_initial_polarity` function to configure the initial polarity before pinmux initialization:

  - By default, the initial polarity is set to low. To configure it as high, set the macro `APP_CMP_INITIAL_POLARITY_HIGH` to 1.
  - The `gptmr_config` API in the example implements the relevant GPTMR configuration and initialization.

- Logic for generating a specified number of pulses:

  - Use DMA transfers to dynamically configure the CMP1 and RELOAD registers of GPTMR.
  - Each RELOAD event of GPTMR triggers a DMA transfer to update the values of CMP1 and RELOAD registers.

- **Notes**:

  - For SOCs with `DMA_SOC_MAX_COUNT == 1`, 32-bit HDMA is used, which can only transfer one 32-bit register value (RELOAD). Therefore, only the pulse frequency can be adjusted, but the duty cycle cannot.
  - For SOCs with `DMA_SOC_MAX_COUNT > 1`, 64-bit XDMA is used, which can transfer two consecutive 32-bit register values (CMP1 and RELOAD), allowing simultaneous adjustment of pulse frequency and duty cycle.
  - Pulses are generated in center-aligned PWM mode, requiring `0 < CMP0 < CMP1 < RELOAD`, where the duty cycle is `CMP1 - CMP0` and the period is `RELOAD`.
  - After generating the specified number of pulses, the timer counter must be stopped to avoid continuous pulse generation. The example achieves this through DMA linked transfers, which automatically configure the timer to stop counting on the last pulse.
  - The default value of CMP0 is 100 (defined by the macro `APP_OFFSET_CMP0_TICK`). It is recommended not to set this value too low to ensure sufficient time for DMA linked transfers.
  - You can adjust the macros `APP_MAX_CYCLE_US` and `APP_MIN_CYCLE_US` to configure the maximum and minimum pulse periods (in microseconds). The default maximum period is 100us, and the minimum period is 10us.
  - You can adjust the macro `APP_CYCLE_COUNT` to configure the number of pulses. By default, 100 pulses are generated.

Execution Phenomenon
--------------------

- Run the project, and the example executes the GPTMR functionality to generate T-shaped acceleration and deceleration pulses. Use an oscilloscope to observe the compare output pin. Upon power-up, the CMP1 compare values and RELOAD values for different pulses will be printed, followed by pulses with fixed pulse widths and varying frequencies every 500ms. The LED will keep flashing. The serial terminal outputs the following information:

- For SOCs using HDMA, the CMP1 compare value remains the same, while the RELOAD value changes:

.. code-block:: console

    generate T_shape_accel_decel test
    shape_table[0] compare_value:449, reload:9898
    shape_table[1] compare_value:449, reload:9616
    shape_table[2] compare_value:449, reload:9333
    shape_table[3] compare_value:449, reload:9052
    shape_table[4] compare_value:449, reload:8771
    shape_table[5] compare_value:449, reload:8491
    shape_table[6] compare_value:449, reload:8212
    shape_table[7] compare_value:449, reload:7935
    shape_table[8] compare_value:449, reload:7660
    shape_table[9] compare_value:449, reload:7388
    shape_table[10] compare_value:449, reload:7117
    shape_table[11] compare_value:449, reload:6850
    shape_table[12] compare_value:449, reload:6585
    shape_table[13] compare_value:449, reload:6324
    shape_table[14] compare_value:449, reload:6066
    shape_table[15] compare_value:449, reload:5813
    shape_table[16] compare_value:449, reload:5563
    shape_table[17] compare_value:449, reload:5317
    shape_table[18] compare_value:449, reload:5076
    shape_table[19] compare_value:449, reload:4840
    shape_table[20] compare_value:449, reload:4608
    shape_table[21] compare_value:449, reload:4382
    shape_table[22] compare_value:449, reload:4162
    shape_table[23] compare_value:449, reload:3947
    shape_table[24] compare_value:449, reload:3738
    shape_table[25] compare_value:449, reload:3535
    shape_table[26] compare_value:449, reload:3338
    shape_table[27] compare_value:449, reload:3148
    shape_table[28] compare_value:449, reload:2964
    shape_table[29] compare_value:449, reload:2787
    shape_table[30] compare_value:449, reload:2617
    shape_table[31] compare_value:449, reload:2455
    shape_table[32] compare_value:449, reload:2300
    shape_table[33] compare_value:449, reload:2152
    shape_table[34] compare_value:449, reload:2012
    shape_table[35] compare_value:449, reload:1879
    shape_table[36] compare_value:449, reload:1755
    shape_table[37] compare_value:449, reload:1639
    shape_table[38] compare_value:449, reload:1531
    shape_table[39] compare_value:449, reload:1431
    shape_table[40] compare_value:449, reload:1339
    shape_table[41] compare_value:449, reload:1256
    shape_table[42] compare_value:449, reload:1181
    shape_table[43] compare_value:449, reload:1115
    shape_table[44] compare_value:449, reload:1058
    shape_table[45] compare_value:449, reload:1009
    shape_table[46] compare_value:449, reload:969
    shape_table[47] compare_value:449, reload:938
    shape_table[48] compare_value:449, reload:916
    shape_table[49] compare_value:449, reload:903
    shape_table[50] compare_value:449, reload:899
    shape_table[51] compare_value:449, reload:903
    shape_table[52] compare_value:449, reload:916
    shape_table[53] compare_value:449, reload:938
    shape_table[54] compare_value:449, reload:969
    shape_table[55] compare_value:449, reload:1009
    shape_table[56] compare_value:449, reload:1058
    shape_table[57] compare_value:449, reload:1115
    shape_table[58] compare_value:449, reload:1181
    shape_table[59] compare_value:449, reload:1256
    shape_table[60] compare_value:449, reload:1339
    shape_table[61] compare_value:449, reload:1431
    shape_table[62] compare_value:449, reload:1531
    shape_table[63] compare_value:449, reload:1639
    shape_table[64] compare_value:449, reload:1755
    shape_table[65] compare_value:449, reload:1879
    shape_table[66] compare_value:449, reload:2012
    shape_table[67] compare_value:449, reload:2152
    shape_table[68] compare_value:449, reload:2300
    shape_table[69] compare_value:449, reload:2455
    shape_table[70] compare_value:449, reload:2617
    shape_table[71] compare_value:449, reload:2787
    shape_table[72] compare_value:449, reload:2964
    shape_table[73] compare_value:449, reload:3148
    shape_table[74] compare_value:449, reload:3338
    shape_table[75] compare_value:449, reload:3535
    shape_table[76] compare_value:449, reload:3738
    shape_table[77] compare_value:449, reload:3947
    shape_table[78] compare_value:449, reload:4162
    shape_table[79] compare_value:449, reload:4382
    shape_table[80] compare_value:449, reload:4608
    shape_table[81] compare_value:449, reload:4840
    shape_table[82] compare_value:449, reload:5076
    shape_table[83] compare_value:449, reload:5317
    shape_table[84] compare_value:449, reload:5563
    shape_table[85] compare_value:449, reload:5813
    shape_table[86] compare_value:449, reload:6066
    shape_table[87] compare_value:449, reload:6324
    shape_table[88] compare_value:449, reload:6585
    shape_table[89] compare_value:449, reload:6850
    shape_table[90] compare_value:449, reload:7117
    shape_table[91] compare_value:449, reload:7388
    shape_table[92] compare_value:449, reload:7660
    shape_table[93] compare_value:449, reload:7935
    shape_table[94] compare_value:449, reload:8212
    shape_table[95] compare_value:449, reload:8491
    shape_table[96] compare_value:449, reload:8771
    shape_table[97] compare_value:449, reload:9052
    shape_table[98] compare_value:449, reload:9333
    shape_table[99] compare_value:449, reload:9616

   ....

- For SOCs using XDMA, both CMP1 and RELOAD values change:

.. code-block:: console

    generate T_shape_accel_decel test
    shape_table[0] compare_value:5099, reload:9898
    shape_table[1] compare_value:4958, reload:9616
    shape_table[2] compare_value:4816, reload:9333
    shape_table[3] compare_value:4676, reload:9052
    shape_table[4] compare_value:4535, reload:8771
    shape_table[5] compare_value:4395, reload:8491
    shape_table[6] compare_value:4256, reload:8212
    shape_table[7] compare_value:4117, reload:7935
    shape_table[8] compare_value:3980, reload:7660
    shape_table[9] compare_value:3844, reload:7388
    shape_table[10] compare_value:3708, reload:7117
    shape_table[11] compare_value:3575, reload:6850
    shape_table[12] compare_value:3442, reload:6585
    shape_table[13] compare_value:3312, reload:6324
    shape_table[14] compare_value:3183, reload:6066
    shape_table[15] compare_value:3056, reload:5813
    shape_table[16] compare_value:2931, reload:5563
    shape_table[17] compare_value:2808, reload:5317
    shape_table[18] compare_value:2688, reload:5076
    shape_table[19] compare_value:2570, reload:4840
    shape_table[20] compare_value:2454, reload:4608
    shape_table[21] compare_value:2341, reload:4382
    shape_table[22] compare_value:2231, reload:4162
    shape_table[23] compare_value:2123, reload:3947
    shape_table[24] compare_value:2019, reload:3738
    shape_table[25] compare_value:1917, reload:3535
    shape_table[26] compare_value:1819, reload:3338
    shape_table[27] compare_value:1724, reload:3148
    shape_table[28] compare_value:1632, reload:2964
    shape_table[29] compare_value:1543, reload:2787
    shape_table[30] compare_value:1458, reload:2617
    shape_table[31] compare_value:1377, reload:2455
    shape_table[32] compare_value:1300, reload:2300
    shape_table[33] compare_value:1226, reload:2152
    shape_table[34] compare_value:1156, reload:2012
    shape_table[35] compare_value:1089, reload:1879
    shape_table[36] compare_value:1027, reload:1755
    shape_table[37] compare_value:969, reload:1639
    shape_table[38] compare_value:915, reload:1531
    shape_table[39] compare_value:865, reload:1431
    shape_table[40] compare_value:819, reload:1339
    shape_table[41] compare_value:778, reload:1256
    shape_table[42] compare_value:740, reload:1181
    shape_table[43] compare_value:707, reload:1115
    shape_table[44] compare_value:679, reload:1058
    shape_table[45] compare_value:654, reload:1009
    shape_table[46] compare_value:634, reload:969
    shape_table[47] compare_value:619, reload:938
    shape_table[48] compare_value:608, reload:916
    shape_table[49] compare_value:601, reload:903
    shape_table[50] compare_value:599, reload:899
    shape_table[51] compare_value:601, reload:903
    shape_table[52] compare_value:608, reload:916
    shape_table[53] compare_value:619, reload:938
    shape_table[54] compare_value:634, reload:969
    shape_table[55] compare_value:654, reload:1009
    shape_table[56] compare_value:679, reload:1058
    shape_table[57] compare_value:707, reload:1115
    shape_table[58] compare_value:740, reload:1181
    shape_table[59] compare_value:778, reload:1256
    shape_table[60] compare_value:819, reload:1339
    shape_table[61] compare_value:865, reload:1431
    shape_table[62] compare_value:915, reload:1531
    shape_table[63] compare_value:969, reload:1639
    shape_table[64] compare_value:1027, reload:1755
    shape_table[65] compare_value:1089, reload:1879
    shape_table[66] compare_value:1156, reload:2012
    shape_table[67] compare_value:1226, reload:2152
    shape_table[68] compare_value:1300, reload:2300
    shape_table[69] compare_value:1377, reload:2455
    shape_table[70] compare_value:1458, reload:2617
    shape_table[71] compare_value:1543, reload:2787
    shape_table[72] compare_value:1632, reload:2964
    shape_table[73] compare_value:1724, reload:3148
    shape_table[74] compare_value:1819, reload:3338
    shape_table[75] compare_value:1917, reload:3535
    shape_table[76] compare_value:2019, reload:3738
    shape_table[77] compare_value:2123, reload:3947
    shape_table[78] compare_value:2231, reload:4162
    shape_table[79] compare_value:2341, reload:4382
    shape_table[80] compare_value:2454, reload:4608
    shape_table[81] compare_value:2570, reload:4840
    shape_table[82] compare_value:2688, reload:5076
    shape_table[83] compare_value:2808, reload:5317
    shape_table[84] compare_value:2931, reload:5563
    shape_table[85] compare_value:3056, reload:5813
    shape_table[86] compare_value:3183, reload:6066
    shape_table[87] compare_value:3312, reload:6324
    shape_table[88] compare_value:3442, reload:6585
    shape_table[89] compare_value:3575, reload:6850
    shape_table[90] compare_value:3708, reload:7117
    shape_table[91] compare_value:3844, reload:7388
    shape_table[92] compare_value:3980, reload:7660
    shape_table[93] compare_value:4117, reload:7935
    shape_table[94] compare_value:4256, reload:8212
    shape_table[95] compare_value:4395, reload:8491
    shape_table[96] compare_value:4535, reload:8771
    shape_table[97] compare_value:4676, reload:9052
    shape_table[98] compare_value:4816, reload:9333
    shape_table[99] compare_value:4958, reload:9616

   ....