.. _qeov2:

QEOV2
=====

Overview
--------

QEOv2 (Quadrature Encoder Output Version 2) is a new generation quadrature encoder output peripheral. QEOv2 can generate various types of output signals based on input position information:

- ABZ quadrature encoder signals
- Waveform signals (cosine wave, triangle wave, saddle wave, etc.)
- PWM commutation control signals

Features
--------

- Position Input Support:
    - Get position from system position manager
    - Software position injection
    - Support position compensation and line number conversion

- Waveform Output Features:
    - Support 4 waveforms: cosine wave, bread wave, saddle wave, sawtooth wave
    - 3 independent output channels with configurable phase shift
    - Support VD/VQ control (support getting VD/VQ from CLC)
    - Support waveform scaling, limiting and dead zone compensation
    - Support output PWM duty cycle adjustment

- ABZ Output Features:
    - Support 4 output modes:

        * A/B quadrature output with self-position
        * High-speed pulse (Pulse&Revise) output
        * Up&Down output
        * A/B quadrature output without self-position

    - Built-in position manager with position synchronization
    - Configurable minimum pulse width
    - Support watchdog function

- PWM Output Features:
    - Support 8 groups of PWM force output control signals
    - Configurable up to 12-phase commutation tables
    - Support safety mode configuration
    - Support three-phase BLDC and two-phase stepper motor control

API Description
----------------

Position Control APIs
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    /* Configure SEI interface sampling */
    void qeov2_sei_config(QEOV2_Type *ptr, qeov2_sei_config_t *config);

    /* Write position value by software */
    void qeov2_set_position(QEOV2_Type *ptr, uint32_t position);

    /* Get current position value */
    uint32_t qeov2_get_position(QEOV2_Type *ptr);

ABZ Output APIs
~~~~~~~~~~~~~~~~

.. code-block:: c

    /* Configure ABZ output parameters */
    void qeov2_abz_config(QEOV2_Type *ptr, qeov2_abz_config_t *config);

    /* Set encoder resolution */
    void qeov2_set_resolution(QEOV2_Type *ptr, uint32_t lines);

    /* Configure position synchronization */
    void qeov2_position_sync(QEOV2_Type *ptr, uint32_t position);

PWM and Waveform Output APIs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    /* Configure PWM output mode */
    void qeov2_pwm_config(QEOV2_Type *ptr, qeov2_pwm_config_t *config);

    /* Configure waveform output */
    void qeov2_wave_config(QEOV2_Type *ptr, qeov2_wave_config_t *config);

    /* Set phase shift */
    void qeov2_set_phase_shift(QEOV2_Type *ptr, uint32_t phase);

Examples
--------

.. toctree::
   :maxdepth: 3

   sei2abz/README_en
   pwm_out/README_en
   wave_out/README_en
   abz_out/README_en
