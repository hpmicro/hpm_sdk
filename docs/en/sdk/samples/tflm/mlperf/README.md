# MLPerf Tiny v0.7 HPMicro

This is the HPMICRO port code for MLPerf V0.7.

For benchmark code and rules please see the [GitHub repository](https://github.com/mlcommons/tiny).

## Code Options

- Add `sdk_compile_options("-mabi=ilp32d") sdk_compile_options("-march=rv32gc") set(CONFIG_HPM_MATH_NN_RVP32 1)` to the project's CMakeLists.txt to get a higher performance
