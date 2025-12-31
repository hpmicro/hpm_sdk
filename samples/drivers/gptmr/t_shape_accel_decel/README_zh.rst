.. _t_shape_accel_decel_pulses:

GPTMR生成T型加减速脉冲
============================

概述
------

- `t_shape_accel_decel` 示例工程演示了GPTMR在指定时间内生成指定数量的T型加减速脉冲的功能，同时可作为输出指定脉冲数量的参考。

- 脉冲数量通过DMA传输完成后自动停止。

硬件设置
------------

- 请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。

- 连接GPTMR比较输出引脚至示波器或者逻辑分析仪。

软件设置
------------

- GPTMR驱动接口详情请参见 :ref:`gptmr <gptmr>`。

- 如果需要设置GPTMR的输出比较初始电平为高电平，为避免初始化过程中出现不可预期的输出脉冲，可在pinmux初始化GPTMR之前使用 `gptmr_channel_set_cmp_initial_polarity` 函数设置初始极性为高电平：

  - 示例默认初始电平为低电平。若需设置为高电平，可将 `APP_CMP_INITIAL_POLARITY_HIGH` 宏定义置为1。
  - 示例中的 `gptmr_config` API 实现了GPTMR的相关配置初始化。

- 输出指定脉冲数量的实现逻辑：

  - 使用DMA传输动态配置GPTMR的CMP1和RELOAD寄存器。
  - 每次GPTMR的RELOAD事件触发DMA传输，更新CMP1和RELOAD寄存器的值。

- **注意事项**：

  - 对于 `DMA_SOC_MAX_COUNT == 1` 的SOC，使用32bit的HDMA，仅能传输一个32bit寄存器值（RELOAD），因此只能改变脉冲频率，无法调整占空比。
  - 对于 `DMA_SOC_MAX_COUNT > 1` 的SOC，使用64bit的XDMA，可传输两个连续的32bit寄存器值（CMP1和RELOAD），从而同时调整脉冲频率和占空比。
  - 脉冲采用中心对齐PWM模式，要求 `0 < CMP0 < CMP1 < RELOAD`，其中占空比为 `CMP1 - CMP0`，周期为 `RELOAD`。
  - 输出指定脉冲数量后需停止定时器计数器以避免持续产生脉冲。示例通过DMA链式传输实现：在最后一个脉冲时，自动指向停止计数器的DMA链式配置。
  - CMP0值默认为100（对应 `APP_OFFSET_CMP0_TICK` 宏定义），建议不要设置过低以确保DMA链式传输有足够时间。
  - 可通过调整 `APP_MAX_CYCLE_US` 和 `APP_MIN_CYCLE_US` 宏定义设置脉冲的最大和最小周期（单位：微秒），默认最大周期为100us，最小周期为10us。
  - 可通过调整 `APP_CYCLE_COUNT` 宏定义设置脉冲数量，默认生成100个脉冲。

运行现象
------------

- 运行工程，示例执行GPTMR的生成T型加减速脉冲的功能，使用示波器观测输出比较的引脚。上电后会打印不同脉冲的CMP1比较值和重装载值，随后每隔500ms输出固定脉宽不同频率的指定数量脉冲，LED会持续闪烁。串口终端输出如下信息：

- 对于使用HDMA的SOC，CMP1比较值相同，RELOAD值不同：

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

- 对于使用XDMA的SOC，CMP1和RELOAD值均不同：

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

