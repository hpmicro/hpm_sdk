.. _qeo:

QEO
===

概述
----

QEO (Quadrature Encoder Output) 是一个正交编码器输出外设。QEO可以根据输入的位置信息生成多种类型的输出信号：

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
    - 支持VD/VQ控制
    - 支持波形缩放、限幅和死区补偿
    - 通过DAC输出模拟信号

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

波形输出相关API
~~~~~~~~~~~~~~~~

.. code-block:: c

    /* 获取波形模式默认配置 */
    void qeo_wave_get_default_mode_config(QEO_Type *ptr, qeo_wave_mode_t *config);

    /* 配置波形模式 */
    void qeo_wave_config_mode(QEO_Type *ptr, qeo_wave_mode_t *config);

    /* 设置波形分辨率线数 */
    void qeo_wave_set_resolution_lines(QEO_Type *ptr, uint32_t lines);

    /* 设置波形相位偏移 */
    void qeo_wave_set_phase_shift(QEO_Type *ptr, uint8_t wave_index, uint32_t phase_shift);

    /* 获取波形输出值 */
    uint16_t qeo_get_wave_output_val(QEO_Type *ptr, uint8_t wave_index);

ABZ输出相关API
~~~~~~~~~~~~~~~

.. code-block:: c

    /* 获取ABZ模式默认配置 */
    void qeo_abz_get_default_mode_config(QEO_Type *ptr, qeo_abz_mode_t *config);

    /* 配置ABZ模式 */
    void qeo_abz_config_mode(QEO_Type *ptr, qeo_abz_mode_t *config);

    /* 设置ABZ分辨率线数 */
    void qeo_abz_set_resolution_lines(QEO_Type *ptr, uint32_t lines);

    /* 设置ABZ最大输出频率 */
    hpm_stat_t qeo_abz_set_max_frequency(QEO_Type *ptr, uint32_t src_freq_hz, uint32_t max_freq_hz);

    /* 配置ABZ位置同步 */
    void qeo_abz_position_sync(QEO_Type *ptr, uint32_t position);

PWM输出相关API
~~~~~~~~~~~~~~~

.. code-block:: c

    /* 获取PWM模式默认配置 */
    void qeo_pwm_get_default_mode_config(QEO_Type *ptr, qeo_pwm_mode_t *config);

    /* 配置PWM模式 */
    void qeo_pwm_config_mode(QEO_Type *ptr, qeo_pwm_mode_t *config);

    /* 设置PWM分辨率线数 */
    void qeo_pwm_set_resolution_lines(QEO_Type *ptr, uint32_t lines);

    /* 配置PWM换相表 */
    void qeo_pwm_config_phase_table(QEO_Type *ptr, uint8_t phase_num, uint8_t *phase_table);

位置控制相关API
~~~~~~~~~~~~~~~

.. code-block:: c

    /* 使能软件位置注入 */
    void qeo_enable_software_position_inject(QEO_Type *ptr);

    /* 禁用软件位置注入 */
    void qeo_disable_software_position_inject(QEO_Type *ptr);

    /* 软件注入位置值 */
    void qeo_software_position_inject(QEO_Type *ptr, uint32_t position);


示例
----

.. toctree::
   :maxdepth: 3

   abz_out/README_zh
   dac_out/README_zh
   pwm_out/README_zh

