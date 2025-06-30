.. _qeov2:

QEOv2
=====

概述
----

QEOv2 (Quadrature Encoder Output Version 2) 是新一代正交编码器输出外设。QEOv2可以根据输入的位置信息生成多种类型的输出信号：

- ABZ正交编码器信号
- 波形信号（余弦波、三角波、马鞍波等）
- PWM换相控制信号

特性
----

- 位置输入支持：
    - 从系统位置管理器获取位置
    - 软件位置注入
    - 支持位置补偿和线数转换

- 波形输出功能：
    - 支持4种波形：余弦波、馒头波、马鞍波、锯齿波
    - 3个独立输出通道，可配置相位偏移
    - 支持VD/VQ控制(支持从CLC获取VD/VQ)
    - 支持波形缩放、限幅和死区补偿
    - 支持输出调节PWM占空比

- ABZ输出功能：
    - 支持4种输出模式：

      * 带自有位置的A/B两相正交输出
      * 高速脉冲（Pulse&Revise）输出
      * 上下（Up&Down）输出
      * 不带自有位置的A/B两相正交输出

    - 内置位置管理器，支持位置同步
    - 可配置最小脉冲宽度
    - 支持看门狗功能

- PWM输出功能：
    - 支持8组PWM强制输出控制信号
    - 可配置最多12相换相表
    - 支持安全模式配置
    - 支持三相直流无刷和两相步进电机控制

API说明
-------

位置控制相关API
~~~~~~~~~~~~~~~

.. code-block:: c

    /* 配置SEI接口采样 */
    void qeov2_sei_config(QEOV2_Type *ptr, qeov2_sei_config_t *config);

    /* 软件写入位置值 */
    void qeov2_set_position(QEOV2_Type *ptr, uint32_t position);

    /* 获取当前位置值 */
    uint32_t qeov2_get_position(QEOV2_Type *ptr);

ABZ输出相关API
~~~~~~~~~~~~~~~

.. code-block:: c

    /* 配置ABZ输出参数 */
    void qeov2_abz_config(QEOV2_Type *ptr, qeov2_abz_config_t *config);

    /* 设置编码器分辨率 */
    void qeov2_set_resolution(QEOV2_Type *ptr, uint32_t lines);

    /* 配置位置同步 */
    void qeov2_position_sync(QEOV2_Type *ptr, uint32_t position);

PWM和波形输出相关API
~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    /* 配置PWM输出模式 */
    void qeov2_pwm_config(QEOV2_Type *ptr, qeov2_pwm_config_t *config);

    /* 配置波形输出 */
    void qeov2_wave_config(QEOV2_Type *ptr, qeov2_wave_config_t *config);

    /* 设置相位偏移 */
    void qeov2_set_phase_shift(QEOV2_Type *ptr, uint32_t phase);

示例
----

.. toctree::
   :maxdepth: 3

   sei2abz/README_zh
   pwm_out/README_zh
   wave_out/README_zh
   abz_out/README_zh
