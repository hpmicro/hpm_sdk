.. _adc16_differential:

ADC16 差分采样
==============

概述
----

本示例演示 ADC16 差分输入：master/slave ADC 对以 sequence 模式运行，配合硬件
触发与 DMA，采样结果通过控制台 UART 打印。

板级设置
--------

- ADC 实例、引脚与连接器说明见目标 board README 中 **ADC16 差分输入** 小节
- 按板级文档连接差分输入

运行示例
--------

- 按目标 board 编译并烧录
- 在控制台 UART 查看周期性打印的差分采样结果

运行现象
--------

- 串口终端先显示 board 初始化信息（clock summary、banner 等，内容因板卡而异），随后进入示例输出
- 示例启动后打印一行标题，之后约每 500 ms 打印一次差分结果（间隔由主循环 ``board_delay_ms(500)`` 决定，非 ADC 硬件采样周期）
- 打印前缀随板级 master/slave ADC 配对而定，可能为 ``ADC2/ADC3`` 或 ``ADC0/ADC1``，详见目标 board README
- 每行 ``0xXXXX (N)``：``0xXXXX`` 为 DMA 低 16 位 raw 十六进制；``(N)`` 为按 master 侧差分配置解码的十进制。默认配置下 ``(N)`` 为有符号，近零差分时常在 0 附近小幅波动；编码规则见下文 *差分结果码值* 一节

  下列示例均假设默认配置 ``position_mode=differential``、``full_resolution=0``：

  .. code-block:: console

   ADC16 differential sample
   ADC2/ADC3 diff result: 0x0003 (3)
   ADC2/ADC3 diff result: 0xfffc (-4)
   ADC2/ADC3 diff result: 0x0001 (1)

  使用 ADC0/ADC1 配对的 board 可能显示：

  .. code-block:: console

   ADC16 differential sample
   ADC0/ADC1 diff result: 0x0000 (0)
   ADC0/ADC1 diff result: 0xfff8 (-8)

  下列标称电压示例均在 Vref 约 3.3 V、默认配置下；码值为理论示意（非 mV），板级实测可在一定范围内波动。

  master 引脚 1.5 V、slave 引脚 1.0 V（差分约 0.5 V）时，本 sample 典型输出类似：

  .. code-block:: console

   ADC0/ADC1 diff result: 0x1388 (5000)

  master 引脚 1.5 V、slave 引脚 0.5 V（差分约 1.0 V）时，本 sample 典型输出类似：

  .. code-block:: console

   ADC2/ADC3 diff result: 0x2710 (10000)

差分 master/slave 与极性
------------------------

- master/slave 由目标 board 的 ``board.h`` 定义：ADC2/ADC3 固定配对使用 ``BOARD_APP_ADC16_DIFF_PAIR23_*`` 宏，ADC0/ADC1 固定配对使用 ``BOARD_APP_ADC16_DIFF_PAIR01_*`` 宏（``PAIR23``/``PAIR01`` 表示 SoC 差分配对编号，非 ADC 实例号）；若 board 已将配对内一路设为 master，sample 从该路 DMA 读取并打印；若需对调 master，须修改 ``diff_cfg_*_master.master`` 并将 DMA 与打印改到新 master 侧
- ADC0/ADC1 与 ADC2/ADC3 配对适用相同的 master/slave 与极性规则

差分结果码值（position_mode / full_resolution）
-----------------------------------------------

master 侧 CONV_CFG0 用两个字段决定差分结果在 16 位总线上的编码（与模拟差分接线方式无关）：

- full_resolution（FULL）：为 1 时把代数差分小于 0 钳为 0，并忽略 position_mode
- position_mode（POS_MODE）：仅当 full_resolution 为 0 时有效

.. list-table:: 编码方式一览
   :header-rows: 1

   * - full_resolution
     - position_mode
     - 总线语义
     - 零差分
   * - 0
     - differential（POS=0）
     - 有符号 −32768～+32767；master 引脚高于 slave 为正，反接为负
     - 0（0x0000）
   * - 0
     - single_ended（POS=1）
     - 无符号 0～65535 偏移码
     - 32768（0x8000）
   * - 1
     - （任意，无效）
     - 无符号 0～65535；代数差分小于 0 时钳为 0
     - 0（0x0000）

本 sample 默认配置
~~~~~~~~~~~~~~~~~~

- adc16_differential.c 将 master 设为 position_mode=differential、full_resolution=0（驱动 adc16_get_default_diff_mode_config 默认分别为 single_ended、1）
- 串口 0x%04x (N) 中 0x%04x 为 DMA raw 均值；(N) 按上表解码，与硬件总线一致，不做零点平移

电压与极性示例
~~~~~~~~~~~~~~

- 下列示例均在 Vref 约 3.3 V、默认配置（full_resolution=0、position_mode=differential）下；码值为 raw 单位（约），非 mV
- 差分约 0.5 V（如 master 1.5 V、slave 1.0 V）：master 高于 slave 约 +5000（0x1388）；反接约 −5000（0xec78）
- 差分约 1.0 V（如 master 1.5 V、slave 0.5 V）：master 高于 slave 约 +10000（0x2710）；反接约 −10000（0xd8f0）
- 若仅将 master 的 full_resolution 改为 1 并重新编译（POS_MODE 无效）：0.5 V 差分时正接约 +9930（0x26ca）；反接约为 0
- 表格先列 ADC0/ADC1，再列 ADC2/ADC3；对调 master/slave 实例时极性随 master 定义翻转，规则相同

.. list-table:: 默认配置下的极性（约）
   :header-rows: 1

   * - Master
     - Slave
     - master 引脚电压
     - slave 引脚电压
     - 结果（约）
   * - ADC0
     - ADC1
     - 1.5 V
     - 1.0 V
     - +5000（0x1388）
   * - ADC0
     - ADC1
     - 1.0 V
     - 1.5 V
     - -5000（0xec78）
   * - ADC2
     - ADC3
     - 1.5 V
     - 1.0 V
     - +5000（0x1388）
   * - ADC2
     - ADC3
     - 1.0 V
     - 1.5 V
     - -5000（0xec78）

注意
----

- SoC 差分配对固定：**ADC0 仅与 ADC1 配对，ADC2 仅与 ADC3 配对**，不可交叉组合（如 ADC0 与 ADC3）
- 配对内的 master 与 slave 两路 ADC 须 **同时使能** 差分模式，不可单独一路使能
- ``DIFF_MASTER=1`` 的那一路输出 **本路 − 配对路**，同一对内 **有且仅有一路** 为 master
- ADC16 差分模式 **不支持软件触发**；除序列模式外也可通过抢占模式采样，均须使用硬件 **同源触发** （本 sample 演示序列模式下的硬件触发）
- 差分输入的正端与负端对地电压均不宜过于接近 0 V 或 3.3 V，否则任一端 ADC 均可能饱和，导致差分结果出现偏差
