.. _adc_v2:

ADC v2
======

概述
----

ADC v2 组件在 HPMicro ADC12 与 ADC16 外设之上提供统一 HAL，应用层无需再按 IP
分别调用 ``adc12_*`` / ``adc16_*``。通过 ``HPM_ADC_V2_HANDLE(base)`` 创建句柄，
``hpm_adc_v2_get_ip_type`` 根据基地址在运行时识别 ADC12 或 ADC16。

在 sample 或应用的 CMakeLists.txt 中启用组件：

.. code-block:: cmake

    set(CONFIG_HPM_ADC_V2 1)

典型场景包括电机 FOC 抢占采样、序列 DMA 采集以及周期/单次转换。完整抢占 DMA
示例见 ``samples/motor_ctrl/bldc_foc``。

功能特性
--------

1. 统一实例句柄

   - ``HPM_ADC_V2_HANDLE(base)`` 创建无状态的 ``adc_v2_handle_t``
   - ``hpm_adc_v2_get_ip_type`` 将 ``base`` 与 SoC ``HPM_ADCx_BASE`` 宏比对；
     非法地址返回 ``adc_v2_ip_unknown``
   - 双 IP SoC（``HPM_ADC_V2_HAS_DUAL_IP``）上 ``HPM_ADC3_BASE`` 对应 ADC16，
     其余合法 ADC 基地址对应 ADC12

2. 转换模式（``adc_v2_conv_mode_t``）

   - 单次：``hpm_adc_v2_get_oneshot_result``
   - 周期：``hpm_adc_v2_set_period_config`` 与 ``hpm_adc_v2_get_prd_result``
   - 序列：``hpm_adc_v2_set_sequence_config``、序列 DMA、软/硬件触发
   - 抢占：``hpm_adc_v2_set_preempt_config``、抢占 DMA、电机 FOC 路径

3. 编译期能力宏

   在应用代码中按需开关可选特性：

   - ``HPM_ADC_V2_HAS_ADC12`` / ``HPM_ADC_V2_HAS_ADC16``：SoC 是否包含对应 IP
   - ``HPM_ADC_V2_HAS_DUAL_IP``：同一 SoC 同时存在 ADC12 与 ADC16
   - ``HPM_ADC_V2_HAS_SIGNAL_MODE``：ADC12 单端/差分（``signal_mode``）
   - ``HPM_ADC_V2_HAS_DIFF_MODE``：ADC16 差分模式（通过 ``hpm_adc16_drv.h`` 配置）
   - ``HPM_ADC_V2_HAS_ADC_LOOP``：ADC16 通道过采样 loop
   - ``HPM_ADC_V2_HAS_DMA_SEQ16BIT``：ADC16 序列 DMA 仅 16 位输出
   - ``HPM_ADC_V2_HAS_SEQ_STOP_POS_INT``：ADC16 STOP_POS 中断位
   - ``HPM_ADC_V2_HAS_MOTOR_MODE``：ADC16 电机控制数据路径

4. 中断与 DMA 辅助

   - 统一的 ``adc_v2_event_t`` 事件掩码，用于状态查询、使能与清除
   - ``hpm_adc_v2_parse_pmt_dma_word`` / ``hpm_adc_v2_parse_seq_dma_word`` 解析
     打包 DMA 字（ADC16 布局随 SoC IP 版本变化，勿在应用层写死位域）

数据结构
--------

主要结构体（定义于 ``hpm_adc_v2.h``）：

1. ``adc_v2_handle_t``

   - ``base``：外设基地址

2. ``adc_v2_config_t``

   实例配置：分辨率、转换模式、时钟分频及 IP 相关字段（ADC12 的 ``signal_mode``，
   ADC16 的 ``port3_realtime`` / ``dma_seq16bit`` 等）。

3. ``adc_v2_channel_config_t``

   单通道采样时间、看门狗阈值及可选过采样（ADC16）。

4. ``adc_v2_pmt_config_t``（别名 ``adc_v2_preempt_config_t``）

   抢占触发槽位、通道映射及中断使能。

5. ``adc_v2_seq_config_t`` / ``adc_v2_dma_config_t``

   序列队列与 DMA 缓冲区配置（含 ``stop_pos`` / ``stop_en``）。

6. ``adc_v2_dma_sample_t``

   DMA 解析 API 返回的语义化采样字段。

API 说明
--------

初始化
~~~~~~

- ``hpm_adc_v2_get_ip_type``：根据句柄基地址识别 ADC12 / ADC16
- ``hpm_adc_v2_get_default_config`` / ``hpm_adc_v2_get_channel_default_config``：填充默认值
- ``hpm_adc_v2_init`` / ``hpm_adc_v2_deinit``：应用或撤销实例配置
- ``hpm_adc_v2_init_channel``：配置单个通道

模式配置
~~~~~~~~

- ``hpm_adc_v2_set_period_config``：周期模式时序
- ``hpm_adc_v2_set_sequence_config``：序列队列与触发
- ``hpm_adc_v2_set_preempt_config``：抢占（电机）触发映射
- ``hpm_adc_v2_set_seq_stop_pos``：序列 DMA 停止位置

DMA 与触发
~~~~~~~~~~

- ``hpm_adc_v2_init_pmt_dma`` / ``hpm_adc_v2_init_seq_dma``：启动抢占或序列 DMA
- ``hpm_adc_v2_is_seq_dma_16bit_enabled``：查询 ADC16 16 位序列 DMA 模式
- ``hpm_adc_v2_trigger_seq_by_sw`` / ``hpm_adc_v2_trigger_pmt_by_sw``：软件触发
- ``hpm_adc_v2_enable_pmt_queue`` / ``hpm_adc_v2_disable_pmt_queue``：ADC16 队列控制

结果与状态
~~~~~~~~~~

- ``hpm_adc_v2_get_oneshot_result`` / ``hpm_adc_v2_get_prd_result``：读取转换结果
- ``hpm_adc_v2_get_conv_valid_status``：通道有效标志
- ``hpm_adc_v2_get_status_flags`` / ``hpm_adc_v2_clear_status_flags``
- ``hpm_adc_v2_enable_interrupts`` / ``hpm_adc_v2_disable_interrupts``
- ``hpm_adc_v2_set_nonblocking_read`` / ``hpm_adc_v2_set_blocking_read``

电机控制（可选）
~~~~~~~~~~~~~~~~

当 ``HPM_ADC_V2_HAS_MOTOR_MODE`` 为 1 时，在电机 FOC sample 中于抢占 DMA
配置完成后调用 ``hpm_adc_v2_enable_motor_mode``。

使用示例
--------

抢占（电机 FOC）典型流程：

.. code-block:: c

    adc_v2_handle_t adc = HPM_ADC_V2_HANDLE(BOARD_BLDC_ADC_U_BASE);
    adc_v2_config_t cfg;
    adc_v2_channel_config_t ch_cfg;
    adc_v2_pmt_config_t pmt_cfg;

    hpm_adc_v2_get_default_config(adc, &cfg);
    cfg.conv_mode = adc_v2_conv_mode_preemption;
    hpm_adc_v2_init(adc, &cfg);

    hpm_adc_v2_get_channel_default_config(adc, &ch_cfg);
    ch_cfg.ch = BOARD_BLDC_ADC_CH_U;
    ch_cfg.signal_mode = adc_v2_signal_mode_single_ended;
    hpm_adc_v2_init_channel(adc, &ch_cfg);

    hpm_adc_v2_set_preempt_config(adc, &pmt_cfg);
    hpm_adc_v2_enable_pmt_queue(adc, BOARD_BLDC_ADC_TRG);
    hpm_adc_v2_init_pmt_dma(adc, dma_buffer_sys_addr);
    #if HPM_ADC_V2_HAS_MOTOR_MODE
    hpm_adc_v2_enable_motor_mode(adc);
    #endif
    hpm_adc_v2_enable_interrupts(adc, HPM_ADC_V2_EVENT_TRIG_COMPLETE);

相关 Sample
-----------

- ``samples/motor_ctrl/bldc_foc``：抢占 DMA 与 FOC 控制环
- ``samples/motor_ctrl/`` 下其他设置 ``CONFIG_HPM_ADC_V2`` 的电机 sample

说明
----

- 新代码请使用 ``hpm_adc_v2.h``，勿再依赖已弃用的 ``hpm_adc_interface`` 组件。
- ADC12 差分输入通过 ``adc_v2_config_t`` 或 ``adc_v2_channel_config_t`` 中的
  ``signal_mode`` 配置。
- 当 ``HPM_ADC_V2_HAS_DIFF_MODE`` 为 1 时，ADC16 差分请使用 ``hpm_adc16_drv.h``
  中的 API（参考 ``samples/drivers/adc/adc16_differential``）。
