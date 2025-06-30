.. _qeo_abz_out:

QEO_ABZ_OUT
======================

概述
---------

QEO_ABZ_OUT示例工程展示了QEO (Quadrature Encoder Output) 外设根据位置信息生成ABZ正交编码器信号的功能。QEO可以接收软件注入或硬件（如MMC）提供的位置信息，结合预设的分辨率线数，生成标准的ABZ正交编码器信号，支持ABZ信号输出的位置同步功能。

工作流程
-----------

本示例提供了两种工作模式演示：

1. 软件位置注入模式
   配置步骤：

   - 初始化QEO模块，配置ABZ信号输出模式
   - 配置ABZ信号生成参数：

     * 设置编码器分辨率线数（1024线/圈）
     * 设置最大输出频率（250kHz）
     * 配置A/B信号4倍频输出
     * 配置Z信号每圈输出一个脉冲

   - 软件位置注入流程：

     * 计算位置增量值
     * 使能软件位置注入
     * 循环注入位置值模拟正向旋转2圈
     * 等待ABZ信号到达注入位置
     * 复位位置到0并关闭注入

2. 硬件位置输入模式

   本例中使用MMC (Motor Motion Control) 外设提供位置信息给QEO，QEO根据接收到的位置信息实时生成ABZ信号输出。

   配置步骤：

   - 初始化QEO模块，配置与软件模式相同的ABZ参数
   - 配置硬件位置输入：

     * 通过TRGM连接MMC位置输出到QEO
     * 配置MMC开环预测模式

   - 使能电机系统时间戳功能

硬件设置
-----------

使用示波器/逻辑分析仪观察QEO ABZ引脚输出的正交编码器信号，(请参考 :ref:`引脚描述 <board_resource>` 部分)。

注意事项
-----------

1. QEO输出ABZ信号的均匀性问题：

   - 当QEO的位置输入频率大于2倍输出ABZ信号频率时，也就是每次位置输入位置对应abz信号的差不超过abz的1/4线，此时是均匀性
   - 当QEO的位置输入频率小于2倍输出ABZ信号频率时，也就是每次位置输入位置对应abz信号的差超过abz的1/4线，需要通过动态调整ABZ信号的最小宽度来保证均匀性

2. QEO输出ABZ信号的位置同步功能

   - 当QEO的当前位置和要同步位置的落在相同的1/4线，会造成同步错误， 需要调整当前位置或者要同步的位置，错开1/4线，参考`qeo_abz_position_sync`函数

运行现象
-----------

当工程正确运行后，可以观察到以下现象：

1. 串口终端输出信息：

.. code-block:: console

   QEO ABZ example
   QEO generate ABZ signal with software inject position
   QEO generate ABZ signal with hardware(MMC) provide position

2. ABZ信号输出：

使用示波器/逻辑分析仪可以观察到ABZ信号波形：
- A/B信号：90度相位差的正交方波
- Z信号：每转一圈的索引脉冲

.. image:: doc/qeo_abz.png
   :alt: ABZ信号波形示意图
