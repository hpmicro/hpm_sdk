.. _adc_v2:

ADC v2
======

Overview
--------

The ADC v2 component provides a unified HAL over HPMicro ADC12 and ADC16 peripherals.
It replaces per-IP ``adc12_*`` / ``adc16_*`` branching in application code. IP type is
detected at runtime from the handle base address via ``hpm_adc_v2_get_ip_type``.

Enable the component in a sample or application CMakeLists.txt:

.. code-block:: cmake

    set(CONFIG_HPM_ADC_V2 1)

Typical use cases include motor FOC preemption sampling, sequence DMA capture, and
period/oneshot conversion. See ``samples/motor_ctrl/bldc_foc`` for a full preempt-DMA
example.

Features
--------

1. Unified instance handle

   - ``HPM_ADC_V2_HANDLE(base)`` creates a stateless ``adc_v2_handle_t``
   - ``hpm_adc_v2_get_ip_type`` matches ``base`` against SoC ``HPM_ADCx_BASE`` macros;
     invalid addresses return ``adc_v2_ip_unknown``
   - On dual-IP SoCs (``HPM_ADC_V2_HAS_DUAL_IP``), ``HPM_ADC3_BASE`` maps to ADC16;
     other valid ADC bases map to ADC12

2. Conversion modes (``adc_v2_conv_mode_t``)

   - Oneshot: ``hpm_adc_v2_get_oneshot_result``
   - Period: ``hpm_adc_v2_set_period_config`` and ``hpm_adc_v2_get_prd_result``
   - Sequence: ``hpm_adc_v2_set_sequence_config``, sequence DMA, SW/HW trigger
   - Preemption: ``hpm_adc_v2_set_preempt_config``, preempt DMA, motor FOC path

3. Compile-time capability macros

   Gate optional features in application code:

   - ``HPM_ADC_V2_HAS_ADC12`` / ``HPM_ADC_V2_HAS_ADC16``: SoC IP presence
   - ``HPM_ADC_V2_HAS_DUAL_IP``: both ADC12 and ADC16 on the same SoC
   - ``HPM_ADC_V2_HAS_SIGNAL_MODE``: ADC12 single-ended / differential via ``signal_mode``
   - ``HPM_ADC_V2_HAS_DIFF_MODE``: ADC16 differential mode (configure via ``hpm_adc16_drv.h``)
   - ``HPM_ADC_V2_HAS_ADC_LOOP``: ADC16 channel oversampling loop
   - ``HPM_ADC_V2_HAS_DMA_SEQ16BIT``: ADC16 sequence DMA 16-bit-only output
   - ``HPM_ADC_V2_HAS_SEQ_STOP_POS_INT``: ADC16 STOP_POS interrupt bit
   - ``HPM_ADC_V2_HAS_MOTOR_MODE``: ADC16 motor-control data path

4. Interrupt and DMA helpers

   - Unified ``adc_v2_event_t`` flags for status, enable, and clear APIs
   - ``hpm_adc_v2_parse_pmt_dma_word`` / ``hpm_adc_v2_parse_seq_dma_word`` decode packed
     DMA words (do not use fixed bit masks on ADC16; layout depends on SoC IP version)

Data Structures
---------------

Main structures (defined in ``hpm_adc_v2.h``):

1. ``adc_v2_handle_t``

   - ``base``: peripheral base address

2. ``adc_v2_config_t``

   Instance configuration: resolution, conversion mode, clock divider, and IP-specific
   fields (``signal_mode`` for ADC12, ``port3_realtime`` / ``dma_seq16bit`` for ADC16).

3. ``adc_v2_channel_config_t``

   Per-channel sample time, watchdog thresholds, and optional oversampling (ADC16).

4. ``adc_v2_pmt_config_t`` (alias ``adc_v2_preempt_config_t``)

   Preemption trigger slots, channels, and interrupt enables.

5. ``adc_v2_seq_config_t`` / ``adc_v2_dma_config_t``

   Sequence queue and DMA buffer layout (including ``stop_pos`` / ``stop_en``).

6. ``adc_v2_dma_sample_t``

   Parsed DMA sample fields returned by the parse APIs.

API Description
---------------

Initialization
~~~~~~~~~~~~~~

- ``hpm_adc_v2_get_ip_type``: detect ADC12 vs ADC16 from handle base
- ``hpm_adc_v2_get_default_config`` / ``hpm_adc_v2_get_channel_default_config``: fill defaults
- ``hpm_adc_v2_init`` / ``hpm_adc_v2_deinit``: apply or tear down instance config
- ``hpm_adc_v2_init_channel``: configure one channel

Mode configuration
~~~~~~~~~~~~~~~~~~

- ``hpm_adc_v2_set_period_config``: period-mode timing
- ``hpm_adc_v2_set_sequence_config``: sequence queue and triggers
- ``hpm_adc_v2_set_preempt_config``: preemption (motor) trigger mapping
- ``hpm_adc_v2_set_seq_stop_pos``: sequence DMA stop index

DMA and triggers
~~~~~~~~~~~~~~~~

- ``hpm_adc_v2_init_pmt_dma`` / ``hpm_adc_v2_init_seq_dma``: start preempt or sequence DMA
- ``hpm_adc_v2_is_seq_dma_16bit_enabled``: query ADC16 16-bit sequence DMA mode
- ``hpm_adc_v2_trigger_seq_by_sw`` / ``hpm_adc_v2_trigger_pmt_by_sw``: software triggers
- ``hpm_adc_v2_enable_pmt_queue`` / ``hpm_adc_v2_disable_pmt_queue``: ADC16 queue control

Results and status
~~~~~~~~~~~~~~~~~~

- ``hpm_adc_v2_get_oneshot_result`` / ``hpm_adc_v2_get_prd_result``: read conversion data
- ``hpm_adc_v2_get_conv_valid_status``: channel valid flag
- ``hpm_adc_v2_get_status_flags`` / ``hpm_adc_v2_clear_status_flags``
- ``hpm_adc_v2_enable_interrupts`` / ``hpm_adc_v2_disable_interrupts``
- ``hpm_adc_v2_set_nonblocking_read`` / ``hpm_adc_v2_set_blocking_read``

Motor control (optional)
~~~~~~~~~~~~~~~~~~~~~~~~

When ``HPM_ADC_V2_HAS_MOTOR_MODE`` is 1, call ``hpm_adc_v2_enable_motor_mode`` after
preempt DMA setup in motor FOC samples.

Usage Example
-------------

Preemption (motor FOC) flow:

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

Related Samples
---------------

- ``samples/motor_ctrl/bldc_foc``: preempt DMA and FOC control loop
- Other motor samples under ``samples/motor_ctrl/`` that set ``CONFIG_HPM_ADC_V2``

Notes
-----

- Prefer ``hpm_adc_v2.h`` over the deprecated ``hpm_adc_interface`` component.
- ADC12 differential input uses ``signal_mode`` in ``adc_v2_config_t`` or
  ``adc_v2_channel_config_t``.
- When ``HPM_ADC_V2_HAS_DIFF_MODE`` is 1, use ADC16 differential APIs in
  ``hpm_adc16_drv.h`` (see ``samples/drivers/adc/adc16_differential``).
