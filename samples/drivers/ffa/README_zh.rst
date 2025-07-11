.. _ffa:

FFA
======

概述
------
本示例展示了HPM FFA的三个主要功能：
- 快速傅里叶变换(FFT)
- 逆快速傅里叶变换(IFFT)
- 有限冲激响应(FIR)滤波

实现原理
------------
本示例通过三个测试用例展示FFA的功能：

1. FFT测试
   - 支持多种点数的复数FFT运算(16/32/64/256/512点)
   - 使用Q31格式的复数输入数据
   - 通过与预设的黄金数据比对验证计算结果

2. IFFT测试
   - 支持多种点数的复数IFFT运算(8/16/32点)
   - 使用Q31格式的复数输入数据
   - 通过与预设的黄金数据比对验证计算结果

3. FIR滤波测试
   - 使用预设的滤波器系数
   - 对输入数据进行实数FIR滤波运算
   - 通过与预设的黄金数据比对验证滤波结果

硬件要求
------------
- 带有FFA外设的开发板

运行现象
------------
- 串口终端显示各项测试的运行结果
- 显示每项测试是否通过(PASSED/FAILED)

硬件设置
------------

无特殊设置

运行现象
------------

当该例程成功运行后，会在串口终端打印如下菜单:


.. code-block:: console

   *******************************************************************************
   *                                                                             *
   *    FFA Test demo                                                            *
   *    1. Complex Q31 FFT Test                                                  *
   *    2. 8-Point Complex Q31 IFFT Test                                         *
   *    3. Real Q31 FIR Test                                                     *
   *                                                                             *
   *******************************************************************************

