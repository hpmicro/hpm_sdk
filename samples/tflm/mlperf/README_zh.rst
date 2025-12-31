.. _mlperf_tiny_v0_7_hpmicro:

MLPerf Tiny v0.7 HPMicro
================================================

这是 MLPerf V0.7 版本的 HPMICRO 移植代码。

MLPerf 代码、使用方法和规则请参见`GitHub仓库 <https://github.com/mlcommons/tiny>`_.

工具链要求
------------

本示例需要支持DSP的工具链才能使用硬件DSP指令进行神经网络加速：

- **IDE**: Segger Embedded Studio (或其他支持DSP工具链的IDE)
- **GCC工具链**: Andes RISC-V工具链（带DSP扩展支持）或 ZCC工具链（带DSP扩展支持）

.. note::
   不支持DSP扩展的标准GCC工具链无法正确编译和运行本示例。DSP指令对于高效的神经网络推理至关重要。

代码选项
------------

- 请在该工程的CMakeLists.txt的`find_package`前添加如下代码，获得更高性能


.. code-block:: cmake

           set(RV_ABI "ilp32d")
           set(RV_ARCH "rv32gc")
           set(CONFIG_HPM_MATH_NN_RVP32 1)


