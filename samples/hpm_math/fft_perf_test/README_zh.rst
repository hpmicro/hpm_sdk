.. _fft_performance_test:

FFT 性能测试示例
================

概述
----

本示例演示了 HPM SDK 中不同 FFT 实现方式的性能对比。示例测试了多种 FFT 算法的执行时间和精度，包括 DSP radix-4、DSP radix-2、Q31 DSP radix-2 以及 Q31 FFA FFT（如果支持）。

功能说明
--------

- 生成包含三个正弦波的合成信号
- 测试不同 FFT 实现方式的性能：
  - DSP radix-4 复数 FFT
  - DSP radix-2 复数 FFT
  - Q31 DSP radix-2 复数 FFT
  - Q31 FFA FFT（如果支持）
- 验证 FFT 和 IFFT 的精度
- 测试不同点数（64-1024点）的性能

硬件要求
--------

- 测试FFA性能需要支持 FFA 功能的SOC（如 HPM63xx、HPM6Exx、HPM68xx、HPM6Pxx）

软件要求
--------

- HPM SDK
- CMake 3.20.0 或更高版本
- 交叉编译工具链

构建说明
--------

1. 进入示例目录
2. 创建构建目录并进入
3. 运行 CMake 配置
4. 编译项目

运行说明
--------

1. 将编译生成的二进制文件烧录到开发板
2. 通过串口查看输出结果
3. 观察不同 FFT 实现方式的性能数据

输出说明
--------

程序会输出以下信息：
- CPU 频率
- 不同 FFT 实现方式的执行时间
- 频率分量的幅度
- 精度验证结果

注意事项
--------

- 确保开发板供电正常
- 确保串口连接正确
- 观察 LED 状态以确认程序运行状态
- 不同开发板支持的 FFT 实现方式可能不同

程序设计
------------

软件生成生成几个幅值和相位不同的正弦波并叠加生成一个波形，然后使用fft变换求出幅频特性数据，最后做ifft回复原始数据，
并比对ifft的数据和fft变换的数据是否相一致。几个正弦波如下所示:

- 幅值11，频率8

- 幅值19，频率17

- 幅值13，频率24

硬件设置
------------

无特殊设置

性能优化
------------

- segger已经默认开启O3优化

- hpm_math默认开启了对CPU Cooley-Tukey算法的计算优化，会增加空间占用

- 复现本文档测试结果需要使用-DHPM_BUILD_TYPE=ram进行编译，这样可以确保DSP计算例程放置在紧耦合内存中以获得最佳性能。构建命令示例：

  .. code-block:: shell

     cmake -GNinja -DBOARD=hpm6750evkmini -DHPM_BUILD_TYPE=ram ..

运行现象
------------

当工程正确运行后，串口终端会输出计算的名称，采样点数，以及幅频特性的数据，以及每一个单元的运行时间，并在最后输出PASS或者ERROR NUM.

串口输出如下所示：

.. code-block:: console

   dsp fft radix-4 Total samples: 64.
   total times:5813 tick.
   ------------------------------------
   DC component amplitude:10.000006.
   Frequency:8.000000, Amplitude:11.000008.
   Frequency:17.000000, Amplitude:18.999988.
   Frequency:24.000000, Amplitude:23.000000.
   ------------------------------------
   dsp ifft radix-4 Total samples: 64.
   total times:5961 tick.
   ------------------------------------
   dsp fft radix-4 Total samples: 256.
   total times:29671 tick.
   ------------------------------------
   DC component amplitude:9.999996.
   Frequency:8.000000, Amplitude:11.000002.
   Frequency:17.000000, Amplitude:18.999998.
   Frequency:24.000000, Amplitude:23.000003.
   ------------------------------------
   dsp ifft radix-4 Total samples: 256.
   total times:30220 tick.
   ------------------------------------
   dsp fft radix-4 Total samples: 1024.
   total times:144992 tick.
   ------------------------------------
   DC component amplitude:9.999997.
   Frequency:8.000000, Amplitude:11.000000.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:23.000000.
   ------------------------------------
   dsp ifft radix-4 Total samples: 1024.
   total times:147161 tick.
   ------------------------------------
   **************************************
   dsp fft radix-2 Total samples: 64.
   total times:9280 tick.
   ------------------------------------
   DC component amplitude:10.000006.
   Frequency:8.000000, Amplitude:11.000008.
   Frequency:17.000000, Amplitude:18.999988.
   Frequency:24.000000, Amplitude:23.000000.
   ------------------------------------
   dsp ifft radix-2 Total samples: 64.
   total times:9544 tick.
   ------------------------------------
   dsp fft radix-2 Total samples: 128.
   total times:21119 tick.
   ------------------------------------
   DC component amplitude:9.999998.
   Frequency:8.000000, Amplitude:11.000001.
   Frequency:17.000000, Amplitude:18.999996.
   Frequency:24.000000, Amplitude:23.000003.
   ------------------------------------
   dsp ifft radix-2 Total samples: 128.
   total times:21575 tick.
   ------------------------------------
   dsp fft radix-2 Total samples: 256.
   total times:47643 tick.
   ------------------------------------
   DC component amplitude:9.999996.
   Frequency:8.000000, Amplitude:11.000002.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:23.000003.
   ------------------------------------
   dsp ifft radix-2 Total samples: 256.
   total times:48582 tick.
   ------------------------------------
   dsp fft radix-2 Total samples: 512.
   total times:106047 tick.
   ------------------------------------
   DC component amplitude:9.999997.
   Frequency:8.000000, Amplitude:11.000000.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:22.999998.
   ------------------------------------
   dsp ifft radix-2 Total samples: 512.
   total times:108005 tick.
   ------------------------------------
   dsp fft radix-2 Total samples: 1024.
   total times:234172 tick.
   ------------------------------------
   DC component amplitude:9.999997.
   Frequency:8.000000, Amplitude:11.000000.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:23.000000.
   ------------------------------------
   dsp ifft radix-2 Total samples: 1024.
   total times:237975 tick.
   ------------------------------------
   **************************************
   dsp fft q31 radix-2 Total samples: 64.
   total times:3782 tick.
   ------------------------------------
   DC component amplitude:10.000006.
   Frequency:8.000000, Amplitude:11.000009.
   Frequency:17.000000, Amplitude:18.999988.
   Frequency:24.000000, Amplitude:23.000000.
   ------------------------------------
   dsp ifft q31 radix-2 Total samples: 64.
   total times:4220 tick.
   ------------------------------------
   dsp fft q31 radix-2 Total samples: 128.
   total times:8252 tick.
   ------------------------------------
   DC component amplitude:9.999998.
   Frequency:8.000000, Amplitude:11.000002.
   Frequency:17.000000, Amplitude:18.999996.
   Frequency:24.000000, Amplitude:23.000003.
   ------------------------------------
   dsp ifft q31 radix-2 Total samples: 128.
   total times:9274 tick.
   ------------------------------------
   dsp fft q31 radix-2 Total samples: 256.
   total times:17995 tick.
   ------------------------------------
   DC component amplitude:9.999995.
   Frequency:8.000000, Amplitude:11.000002.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:23.000005.
   ------------------------------------
   dsp ifft q31 radix-2 Total samples: 256.
   total times:20619 tick.
   ------------------------------------
   dsp fft q31 radix-2 Total samples: 512.
   total times:39039 tick.
   ------------------------------------
   DC component amplitude:9.999995.
   Frequency:8.000000, Amplitude:11.000001.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:23.000000.
   ------------------------------------
   dsp ifft q31 radix-2 Total samples: 512.
   total times:45407 tick.
   ------------------------------------
   dsp fft q31 radix-2 Total samples: 1024.
   total times:85195 tick.
   ------------------------------------
   DC component amplitude:9.999995.
   Frequency:8.000000, Amplitude:11.000002.
   Frequency:17.000000, Amplitude:19.000003.
   Frequency:24.000000, Amplitude:23.000001.
   ------------------------------------
   dsp ifft q31 radix-2 Total samples: 1024.
   total times:99603 tick.
   ------------------------------------
   **************************************
   Software fft  cooley tukey Total samples: 64.
   total times:14493 tick.
   ------------------------------------
   DC component amplitude:10.000006.
   Frequency:8.000000, Amplitude:11.000014.
   Frequency:17.000000, Amplitude:18.999998.
   Frequency:24.000000, Amplitude:23.000019.
   ------------------------------------
   Software fft  cooley tukey Total samples: 128.
   total times:29009 tick.
   ------------------------------------
   DC component amplitude:9.999998.
   Frequency:8.000000, Amplitude:11.000004.
   Frequency:17.000000, Amplitude:19.000003.
   Frequency:24.000000, Amplitude:23.000015.
   ------------------------------------
   Software fft  cooley tukey Total samples: 256.
   total times:61116 tick.
   ------------------------------------
   DC component amplitude:9.999996.
   Frequency:8.000000, Amplitude:11.000000.
   Frequency:17.000000, Amplitude:18.999998.
   Frequency:24.000000, Amplitude:23.000000.
   ------------------------------------
   Software fft  cooley tukey Total samples: 512.
   total times:131520 tick.
   ------------------------------------
   DC component amplitude:9.999997.
   Frequency:8.000000, Amplitude:10.999999.
   Frequency:17.000000, Amplitude:19.000001.
   Frequency:24.000000, Amplitude:23.000000.
   ------------------------------------
   Software fft  cooley tukey Total samples: 1024.
   total times:285106 tick.
   ------------------------------------
   DC component amplitude:9.999997.
   Frequency:8.000000, Amplitude:10.999997.
   Frequency:17.000000, Amplitude:19.000000.
   Frequency:24.000000, Amplitude:22.999996.
   ------------------------------------
   **************************************
   ------------------------------------
   convert data from float to q31, nums:128.
   total times:3163 tick.
   convert data from q31 to float, nums:128.
   total times:1177 tick.
   ------------------------------------
   ------------------------------------
   convert data from float to q31, nums:256.
   total times:6225 tick.
   convert data from q31 to float, nums:256.
   total times:2325 tick.
   ------------------------------------
   ------------------------------------
   convert data from float to q31, nums:512.
   total times:12269 tick.
   convert data from q31 to float, nums:512.
   total times:4629 tick.
   ------------------------------------
   ------------------------------------
   convert data from float to q31, nums:1024.
   total times:24395 tick.
   convert data from q31 to float, nums:1024.
   total times:9237 tick.
   ------------------------------------
   ------------------------------------
   convert data from float to q31, nums:2048.
   total times:47949 tick.
   convert data from q31 to float, nums:2048.
   total times:18453 tick.
   ------------------------------------
   PASS.


性能如下表所示：

.. list-table::
   :header-rows: 1

   * - 采样点
     - 64
     - 128
     - 256
     - 512
     - 1024
   * - DSP radix-4（fpu）
     - 5900
     - N
     - 29869
     - N
     - 145366
   * - DSP radix-2（fpu）
     - 9345
     - 21220
     - 47762
     - 106211
     - 234534
   * - DSP radix-2（Q31）
     - 4378
     - 9581
     - 21177
     - 46126
     - 101331
   * - FFAQ31
     - 2729
     - 4923
     - 7769
     - 18999
     - N
   * - CPU（Cooley-Tukey）
     - 14495
     - 29171
     - 61176
     - 131084
     - 283441
   * - DSP radix-4（fpu）
     - 6016
     - N
     - 30384
     - N
     - 147651
   * - DSP radix-2（fpu）
     - 9558
     - 21632
     - 48733
     - 108296
     - 238775
   * - DSP radix-2（Q31）
     - 4641
     - 10301
     - 22980
     - 50645
     - 111359
   * - FFAQ31
     - 2687
     - 4898
     - 9152
     - 18956
     - N
   * - Q31 to Float
     - 1208
     - 2392
     - 4757
     - 9493
     - 18965
   * - Float to Q31
     - 5517
     - 10958
     - 21970
     - 44022
     - 88055

