.. _dsp:

DSP
======

概述
------

dsp_demo示例工程演示了DSP CFFT（复数快速傅里叶变换）和CIFFT（复数快速傅里叶逆变换）的计算。该示例执行32点FFT变换，并通过详细的误差分析验证结果。

示例包含以下内容：
- 使用radix-2算法的复数FFT（CFFT）计算
- 使用radix-2算法的复数IFFT（CIFFT）计算
- 输入信号生成和缩放
- 详细的误差分析，包括MAE、RMSD、NRMSD和SNR指标
- 与参考数据的对比验证

硬件设置
------------

无特殊设置要求。

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   --------------------
   after CFFT_RD2, maxdiff= 0x0080 [0.00390816]
   --------------------
   MAE is 0.00079375, RMSD is 0.00176055, NRMSD is 0.00005502, MAXDIFF is 0.00390816, SNR is 67.12823486
   CFFT_RD2 out scaleup by 64
   after CIFFT_RD2, maxdiff= 0x0009 [0.00029564]
   --------------------
   MAE is 0.00012192, RMSD is 0.00014900, NRMSD is 0.00007450, MAXDIFF is 0.00029564, SNR is 73.52618408
   CIFFT_RD2 out scaleup by 2
   CIFFT_RD2 PASS

输出信息包含：
- 计算结果与参考数据的最大差异
- 平均绝对误差（MAE）
- 均方根偏差（RMSD）
- 归一化均方根偏差（NRMSD）
- 最大差异（MAXDIFF）
- 信噪比（SNR）
- 计算中使用的缩放因子

测试成功的判定标准：
- CFFT：NRMSD小于阈值（普通编译时为0.00007，zcc编译时为0.0002）
- CIFFT：NRMSD小于阈值（普通编译时为0.0003，zcc编译时为0.005）

