HPM 数学中间件
============

概述
----

HPM 数学中间件是专为 HPMicro 微控制器设计的综合性数学库。它提供了针对数字信号处理（DSP）和神经网络操作的高度优化的数学函数，充分利用了硬件加速和软件实现。

功能特性
--------

DSP 功能
~~~~~~~~

DSP 库提供了一系列优化的数学运算：

1. 基础数学运算
   - 加法、减法、乘法、除法
   - 平方根、幂函数
   - 三角函数
   - 向量运算

2. 复数运算
   - 复数算术运算
   - 复数共轭
   - 复数幅值和相位
   - 复数向量运算

3. 矩阵运算
   - 矩阵加减法
   - 矩阵乘法
   - 矩阵转置
   - 矩阵求逆
   - 矩阵行列式

4. 滤波函数
   - FIR 滤波器
   - IIR 滤波器
   - 卷积运算
   - 相关运算
   - 数字滤波器设计

5. 变换函数
   - 快速傅里叶变换（FFT）
   - 离散傅里叶变换（DFT）
   - 逆 FFT
   - 实数 FFT
   - 复数 FFT

6. 统计函数
   - 均值、方差、标准差
   - 最大值和最小值
   - 均方根
   - 统计矩

7. 距离计算
   - 欧几里得距离
   - 曼哈顿距离
   - 余弦相似度
   - 相关距离

8. 排序算法
   - 快速排序
   - 归并排序
   - 冒泡排序
   - 选择排序

9. 支持向量机（SVM）
   - 线性 SVM
   - 多项式 SVM
   - RBF SVM
   - SVM 预测

10. 控制器函数
    - PID 控制器
    - 状态空间控制器
    - 数字控制器设计

神经网络功能
~~~~~~~~~~~

神经网络库为机器学习应用提供了优化的操作：

1. 基础神经网络运算
   - 层运算
   - 权重初始化
   - 偏置运算
   - 激活函数

2. 卷积运算
   - 2D 卷积
   - 深度卷积
   - 点卷积
   - 步长卷积
   - 扩张卷积

3. 池化运算
   - 最大池化
   - 平均池化
   - 全局池化
   - L2 池化

4. 激活函数
   - ReLU
   - Leaky ReLU
   - Sigmoid
   - Tanh
   - Softmax

5. 全连接层
   - 矩阵乘法
   - 偏置加法
   - 权重更新
   - 反向传播

6. Softmax 运算
   - Softmax 激活
   - Log softmax
   - 交叉熵损失

7. 连接运算
   - 通道连接
   - 深度连接
   - 空间连接

8. TinyEngine 支持
    - 针对嵌入式系统优化
    - 内存高效操作
    - 量化支持

架构
----

中间件分为三个主要组件：

1. DSP 库 (nds_dsp)
   - 针对 Andes DSP 硬件优化
   - 支持 GCC 和 IAR 工具链
   - 提供 SIMD 操作
   - 硬件加速函数
   - 低延迟操作

2. 神经网络库 (nds_nn)
   - 优化的神经网络操作
   - 支持各种神经网络层
   - 包含 TinyEngine 支持
   - 量化支持
   - 内存高效实现

3. 软件 DSP 库 (sw_dsp)
   - 纯软件实现
   - 平台无关操作
   - 后备实现
   - 参考实现

配置
----

中间件可以通过以下选项进行配置：

1. 库选择
   - HPM_EN_MATH_FFA_LIB：启用 FFA 库
   - HPM_EN_MATH_DSP_LIB：启用 DSP 库
   - HPM_EN_MATH_NN_LIB：启用神经网络库

2. DSP 配置
   - HPM_MATH_DSP_STATISTICS
   - HPM_MATH_DSP_BASIC
   - HPM_MATH_DSP_COMPLEX
   - HPM_MATH_DSP_CONTROLLER
   - HPM_MATH_DSP_DISTANCE
   - HPM_MATH_DSP_FILTERING
   - HPM_MATH_DSP_MATRIX
   - HPM_MATH_DSP_SVM
   - HPM_MATH_DSP_TRANSFORM
   - HPM_MATH_DSP_UTILS
   - HPM_MATH_DSP_SORT

3. 神经网络配置
   - HPM_MATH_NN_ACTIVATION
   - HPM_MATH_NN_TINYENGINE
   - HPM_MATH_NN_BASIC
   - HPM_MATH_NN_CONCATENATION
   - HPM_MATH_NN_CONVOLUTION
   - HPM_MATH_NN_CONNECTED
   - HPM_MATH_NN_POOLING
   - HPM_MATH_NN_SOFTMAX
   - HPM_MATH_NN_UTIL

使用方法
--------

使用 HPM 数学中间件：

1. 包含主头文件：
   ```c
   #include "hpm_math.h"
   ```

2. 通过配置宏配置所需功能

3. 初始化库（如果需要）

4. 调用所需的数学函数

示例：
```c
#include "hpm_math.h"

void example_fft(void)
{
    float32_t input[1024];
    float32_t output[1024];

    // 初始化输入数据
    for (int i = 0; i < 1024; i++) {
        input[i] = sinf(2 * HPM_MATH_PI * i / 1024.0f);
    }

    // 执行 FFT
    hpm_dsp_cfft_f32(input, output, 1024);
}
```

构建配置
--------

中间件需要特定的构建配置：

1. 工具链要求
   - 必须使用 nds 或 zcc 工具链
   - 支持 GCC 或 IAR 编译器
   - 启用 Andes DSP 扩展

2. 链接器配置
   - 链接 DSP 库 (-ldsp)
   - 链接数学库 (-lm)
   - 需要时链接神经网络库 (-lnn)

3. 编译器选项
   - 启用 DSP 扩展 (-mext-dsp)
   - 配置优化级别
   - 设置适当的内存模型

性能考虑
--------

1. 硬件加速
   - 在可用时利用 DSP 硬件
   - 优化 SIMD 操作
   - 最小化内存传输

2. 内存使用
   - 使用适当的数据类型
   - 考虑内存对齐
   - 优化缓冲区大小

3. 计算效率
   - 尽可能使用批量操作
   - 使用优化算法
   - 考虑定点运算

许可证
------

BSD-3-Clause