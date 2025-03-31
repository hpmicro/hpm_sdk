.. _mlperf_tiny_v0_7_hpmicro:

MLPerf Tiny v0.7 HPMicro
================================================

这是 MLPerf V0.7 版本的 HPMICRO 移植代码。

MLPerf 代码、使用方法和规则请参见`GitHub仓库 <https://github.com/mlcommons/tiny>`_.

代码选项
------------

- 请在该工程的CMakeLists.txt的`find_package`前添加如下代码，获得更高性能


.. code-block:: cmake

           set(RV_ABI "ilp32d")
           set(RV_ARCH "rv32gc")
           set(CONFIG_HPM_MATH_NN_RVP32 1)


