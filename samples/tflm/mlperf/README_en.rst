.. _mlperf_tiny_v0_7_hpmicro:

MLPerf Tiny v0.7 HPMicro
================================================

This is the HPMICRO port code for MLPerf V0.7.

For benchmark code and rules please see the `GitHub repository <https://github.com/mlcommons/tiny>`_.

Code Options
------------

- Add code before `find_package` to the project's CMakeLists.txt to get a higher performance


.. code-block:: cmake

           set(RV_ABI "ilp32d")
           set(RV_ARCH "rv32gc")
           set(CONFIG_HPM_MATH_NN_RVP32 1)


