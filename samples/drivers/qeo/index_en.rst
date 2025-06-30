.. _qeo:

QEO
===

Overview
--------

QEO (Quadrature Encoder Output) is a peripheral for generating quadrature encoder signals. QEO peripheral can generate various types of output signals based on input position information:

- ABZ quadrature encoder signals
- Waveform signals (cosine wave, triangle wave, saddle wave, etc.)
- PWM commutation control signals

Features
--------

- Position Input Support:
    - Position input from system position manager
    - Software position injection
    - Support for position compensation and line number conversion

- Waveform Output Features:
    - Support for 4 waveform types: cosine wave, triangle wave, saddle wave, sawtooth wave
    - 3 independent output channels with configurable phase shift
    - Support for VD/VQ control
    - Support for waveform scaling, limiting, and deadzone compensation
    - Analog signal output through DAC

- ABZ Output Features:
    - Support for 4 output modes:
        * A/B quadrature output with internal position
        * High-speed pulse (Pulse&Revise) output
        * Up&Down output
        * A/B quadrature output without internal position
    - Built-in position manager with position synchronization
    - Configurable minimum pulse width
    - Watchdog function support

- PWM Output Features:
    - Support for 8 groups of PWM force output control signals
    - Configurable up to 12-phase commutation table
    - Safety mode configuration support
    - Support for three-phase BLDC and two-phase stepper motor control

API Description
----------------

Waveform Output APIs
~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    /* Get default configuration for wave mode */
    void qeo_wave_get_default_mode_config(QEO_Type *ptr, qeo_wave_mode_t *config);

    /* Configure wave mode */
    void qeo_wave_config_mode(QEO_Type *ptr, qeo_wave_mode_t *config);

    /* Set wave resolution lines */
    void qeo_wave_set_resolution_lines(QEO_Type *ptr, uint32_t lines);

    /* Set wave phase shift */
    void qeo_wave_set_phase_shift(QEO_Type *ptr, uint8_t wave_index, uint32_t phase_shift);

    /* Get wave output value */
    uint16_t qeo_get_wave_output_val(QEO_Type *ptr, uint8_t wave_index);

ABZ Output APIs
~~~~~~~~~~~~~~~

.. code-block:: c

    /* Get default configuration for ABZ mode */
    void qeo_abz_get_default_mode_config(QEO_Type *ptr, qeo_abz_mode_t *config);

    /* Configure ABZ mode */
    void qeo_abz_config_mode(QEO_Type *ptr, qeo_abz_mode_t *config);

    /* Set ABZ resolution lines */
    void qeo_abz_set_resolution_lines(QEO_Type *ptr, uint32_t lines);

    /* Set ABZ maximum output frequency */
    hpm_stat_t qeo_abz_set_max_frequency(QEO_Type *ptr, uint32_t src_freq_hz, uint32_t max_freq_hz);

    /* Configure ABZ position synchronization */
    void qeo_abz_position_sync(QEO_Type *ptr, uint32_t position);

PWM Output APIs
~~~~~~~~~~~~~~~

.. code-block:: c

    /* Get default configuration for PWM mode */
    void qeo_pwm_get_default_mode_config(QEO_Type *ptr, qeo_pwm_mode_t *config);

    /* Configure PWM mode */
    void qeo_pwm_config_mode(QEO_Type *ptr, qeo_pwm_mode_t *config);

    /* Set PWM resolution lines */
    void qeo_pwm_set_resolution_lines(QEO_Type *ptr, uint32_t lines);

    /* Configure PWM phase table */
    void qeo_pwm_config_phase_table(QEO_Type *ptr, uint8_t phase_num, uint8_t *phase_table);

Position Control APIs
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    /* Enable software position injection */
    void qeo_enable_software_position_inject(QEO_Type *ptr);

    /* Disable software position injection */
    void qeo_disable_software_position_inject(QEO_Type *ptr);

    /* Inject position value through software */
    void qeo_software_position_inject(QEO_Type *ptr, uint32_t position);


Examples
--------

.. toctree::
    :maxdepth: 3

    abz_out/README_en
    dac_out/README_en
    pwm_out/README_en

