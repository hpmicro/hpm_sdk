# MLPerf Tiny v0.7 HPMicro

这是 MLPerf V0.7 版本的 HPMICRO 移植代码。

MLPerf 代码、使用方法和规则请参见[GitHub仓库](https://github.com/mlcommons/tiny).

## 代码选项

- 如果需要更快的代码运行速度请在该工程的CMakeLists.txt中添加`sdk_compile_options("-mabi=ilp32d") sdk_compile_options("-march=rv32gc") set(CONFIG_HPM_MATH_NN_RVP32 1)`语句，获得较高的性能提升