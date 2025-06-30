.. _gptmr:

GPTMR
=====

Overview
--------

- Key Features

  - Describes HPMicro's GPTMR peripheral driver interfaces and usage. For details refer to `hpm_gptmr_drv.h` API documentation and user manual

  - 4 independent channels per timer, each with:

    - 32-bit up-counting timer
    - Configurable 32-bit reload register

  - Channel counter synchronization support. See :ref:`gptmr_sync_counter <gptmr_sync_counter>`

  - Simultaneous input capture/output compare per channel. Examples:

    - :ref:`gptmr_input_captrue <gptmr_input_captrue>`
    - :ref:`gptmr_output_compare <gptmr_output_compare>`

  - Dual output comparators per channel supporting:

    - Edge-aligned PWM
    - Center-aligned PWM
    - Ref: :ref:`gptmr_generate_pwm <gptmr_generate_pwm>`

  - Hardware-based PWM measurement:

    - Period measurement
    - Duty cycle calculation
    - Ref: :ref:`gptmr_measure_pwm <gptmr_measure_pwm>`

  - External signal counting: :ref:`gptmr_external_counter_mode <gptmr_external_counter_mode>`

  - DMA request generation: :ref:`dma_use_gptmr_event_transfer <dma_use_gptmr_event_transfer>`

  - Operational modes:

    - One-shot mode
    - Burst mode
    - Continuous mode
    - Ref: :ref:`gptmr_one_shot_counter_mode <gptmr_one_shot_counter_mode>`

  - Interrupt generation: :ref:`timer_basic <timer_basic>`

GPTMR Channel Configuration
--------------------------------

- Prerequisites

  - Ensure GPTMR clock source is enabled and GPTMR peripheral pins initialized
  - Use `clock_add_to_group` to add GPTMR clock source to clock group

- Channel Features

  - Independent configuration per channel
  - Supports multiple functions: input capture, output compare, PWM generation

- Enumerations

  - Synchronization Edge Selection

    .. code-block:: c

        typedef enum gptmr_synci_edge {
            /* No trigger */
            gptmr_synci_edge_none = 0,
            /* Falling edge trigger */
            gptmr_synci_edge_falling = GPTMR_CHANNEL_CR_SYNCIFEN_MASK,
            /* Rising edge trigger */
            gptmr_synci_edge_rising = GPTMR_CHANNEL_CR_SYNCIREN_MASK,
            /* Both edge trigger */
            gptmr_synci_edge_both = gptmr_synci_edge_falling | gptmr_synci_edge_rising,
        } gptmr_synci_edge_t;

    - Configures hardware sync input edge detection
    - SYNCI signal comes from TRGM module in motor control system
    - Use `gptmr_channel_select_synci_valid_edge` for configuration
    - For HPM6800: Sync source from GPTMR7 CMP0~3 outputs

  - Input Capture Mode

    .. code-block:: c

        typedef enum gptmr_work_mode {
            /* No capture */
            gptmr_work_mode_no_capture = 0,
            /* Rising edge capture */
            gptmr_work_mode_capture_at_rising_edge = 1,
            /* Falling edge capture */
            gptmr_work_mode_capture_at_falling_edge = 2,
            /* Dual-edge capture */
            gptmr_work_mode_capture_at_both_edge = 3,
            /* PWM measurement mode */
            gptmr_work_mode_measure_width = 4,
        } gptmr_work_mode_t;

    - Configure with `gptmr_channel_set_capmode()`
    - Query with `gptmr_channel_get_capmode()`
    - PWM mode triggers interrupt/DMA on rising edge

  - DMA Request Events

    .. code-block:: c

        typedef enum gptmr_dma_request_event {
            /* Trigger DMA on CMP0 match */
            gptmr_dma_request_on_cmp0 = 0,
            /* Trigger DMA on CMP1 match */
            gptmr_dma_request_on_cmp1 = 1,
            /* Trigger DMA on input toggle */
            gptmr_dma_request_on_input_signal_toggle = 2,
            /* Trigger DMA on reload */
            gptmr_dma_request_on_reload = 3,
            /* Disable DMA requests */
            gptmr_dma_request_disabled = 0xFF,
        } gptmr_dma_request_event_t;

  - Counter Types

    .. code-block:: c

        typedef enum gptmr_counter_type {
            /* Rising edge counter */
            gptmr_counter_type_rising_edge = 0,
            /* Falling edge counter */
            gptmr_counter_type_falling_edge,
            /* PWM period measurement */
            gptmr_counter_type_measured_period,
            /* PWM duty cycle */
            gptmr_counter_type_measured_duty_cycle,
            /* Normal counter */
            gptmr_counter_type_normal,
        } gptmr_counter_type_t;

    - Retrieve values with `gptmr_channel_get_counter()`

  - Counter Modes

    .. code-block:: c

        typedef enum gptmr_counter_mode {
            /* Internal clock mode */
            gptmr_counter_mode_internal = 0,
            /* External clock mode */
            gptmr_counter_mode_external,
        } gptmr_counter_mode_t;

    - Requires HPM_IP_FEATURE_GPTMR_CNT_MODE

  - Signal Monitoring Types

    .. code-block:: c

        typedef enum gptmr_channel_monitor_type {
            /* Signal period monitoring */
            monitor_signal_period = 0,
            /* High-level duration monitoring */
            monitor_signal_high_level_time,
        } gptmr_channel_monitor_type_t;

    - Requires HPM_IP_FEATURE_GPTMR_MONITOR

- Data Structures

  - Channel Monitor Configuration

    .. code-block:: c

        typedef struct gptmr_channel_monitor_config {
            /* Monitoring type */
            gptmr_channel_monitor_type_t monitor_type;
            /* Threshold values */
            uint32_t max_value;
            uint32_t min_value;
        } gptmr_channel_monitor_config_t;

  - Channel Configuration

    .. code-block:: c

        typedef struct gptmr_channel_config {
            /* Input capture working mode */
            gptmr_work_mode_t mode;
            /* DMA event selection */
            gptmr_dma_request_event_t dma_request_event;
            /* Sync edge configuration */
            gptmr_synci_edge_t synci_edge;
            /* Comparator values */
            uint32_t cmp[GPTMR_CH_CMP_COUNT];
            /* Reload value */
            uint32_t reload;
            /* Comparator output initial polarity */
            bool cmp_initial_polarity_high;
            /* Comparator output enable */
            bool enable_cmp_output;
            /* Chain reload with previous channel */
            bool enable_sync_follow_previous_channel;
            /* Software sync enable */
            bool enable_software_sync;
            /* Debug mode control */
            bool debug_mode;

        #if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR == 1)
            /* Monitor function enable */
            bool enable_monitor;
            /* Monitor configuration */
            gptmr_channel_monitor_config_t monitor_config;
        #endif

        #if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE == 1)
            /* Clock source selection */
            gptmr_counter_mode_t counter_mode;
        #endif

        #if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE == 1)
            /* Operation mode enable */
            bool enable_opmode;
        #endif
        } gptmr_channel_config_t;

    - One-shot mode operation:

      1. Timer stops at reload value
      2. Software must clear CEN bit then re-set CEN for next cycle
      3. Use `gptmr_start_counter()` for restart (automates clear/set)
    - Reload interrupt handling in one-shot mode:

      - 1. Disable counter (CEN=0)
      - 2. Reset counter
      - 3. Clear reload interrupt flag
      - 4. Recommended to use comparator interrupt instead
      - Ref: :ref:`gptmr_one_shot_counter_mode <gptmr_one_shot_counter_mode>`

- Default Configuration API:

  - Recommended to call `gptmr_channel_get_default_config` before configuration

  .. code-block:: c

        void gptmr_channel_get_default_config(GPTMR_Type *ptr, gptmr_channel_config_t *config);

  - Default values:

    - Input capture mode: `gptmr_work_mode_no_capture`
    - DMA request: `gptmr_dma_request_disabled`
    - Sync edge: `gptmr_synci_edge_none`
    - Comparator values: `0xFFFFFFFEUL`
    - Reload value: `0xFFFFFFFEUL`
    - Output polarity: `high`
    - Comparator output: `true`
    - Chain reload: `false`
    - Software sync: `true`
    - Debug mode: `true`
    - Monitor enable: `false`
    - Counter mode: `gptmr_counter_mode_internal`
    - Operation mode: `false`

  - **Note**:

    - For constant output: Set CMP0=0xFFFFFFFF for low, CMP1=0xFFFFFFFF for high
    - Comparator outputs override when both CMP0/CMP1 set to 0xFFFFFFFF

- Channel Configuration API:

  .. code-block:: c

        hpm_stat_t gptmr_channel_config(GPTMR_Type *ptr, uint8_t ch_index,gptmr_channel_config_t *config, bool enable);

  - Parameters:

    .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - GPTMR_Type *
          - GPTMR controller pointer
        * - ch_index
          - uint8_t
          - Channel index (0-3)
        * - config
          - gptmr_channel_config_t *
          - Channel config structure
        * - enable
          - bool
          - Enable counter immediately

  - Return values:

    .. list-table::
        :header-rows: 1

        * - Value
          - Description
        * - status_invalid_argument
          - Invalid parameters
        * - status_success
          - Configuration success

  - Example 1: 1ms timer

    .. code-block:: c

        gptmr_channel_config_t config;
        uint32_t gptmr_freq;
        gptmr_channel_get_default_config(HPM_GPTMR0, &config);
        gptmr_freq = clock_get_frequency(clock_gptmr0); /* Get clock */
        config.mode = gptmr_work_mode_no_capture;
        config.reload = gptmr_freq / 1000; /* 1ms */
        config.enable_cmp_output = false;
        config.enable_software_sync = false;
        gptmr_channel_config(HPM_GPTMR0, 0, &config, true);

  - Example 2: 100KHz PWM

    .. code-block:: c

        gptmr_channel_config_t config;
        uint32_t gptmr_freq;
        gptmr_channel_get_default_config(HPM_GPTMR0, &config);
        gptmr_freq = clock_get_frequency(clock_gptmr0);
        config.mode = gptmr_work_mode_no_capture;
        config.reload = gptmr_freq / 100000; /* 100KHz */
        config.cmp[0] = config.reload / 2;   /* 50% duty */
        config.enable_cmp_output = true;
        config.enable_software_sync = false;
        gptmr_channel_config(HPM_GPTMR0, 1, &config, true);

Burst Mode
---------------

- Burst mode: The timer stops counting after reaching the reload value for the specified number of times (target_burst_count).
- Only applicable to SOCs where the macro `HPM_IP_FEATURE_GPTMR_BURST_MODE` is defined in `hpm_soc_ip_feature.h`.

- Introduction to related enumeration values:

  - Two counter start modes in Burst mode

    .. code-block:: c

        typedef enum gptmr_burst_counter_mode {
            gptmr_burst_counter_restart = 0,   /* Restart counting */
            gptmr_burst_counter_continue,      /* Continue counting */
        } gptmr_burst_counter_mode_t;

    - `gptmr_burst_counter_restart`: Restart mode. In this mode, when starting the counter, the current count is reset (usually starting from 0), the `BURST_COUNT` of GPTMR is cleared, and a new burst cycle begins.
    - `gptmr_burst_counter_continue`: Continue mode. In this mode, when starting the counter, if the current burst count has not reached the maximum configured number of times (set by `BURST_CFG`), counting continues from the current value; if the maximum count has been reached, it resets and continues.

- **Note**:

  - Only one of OP mode and Burst mode can be enabled at most.

    - Burst mode can be enabled using the `gptmr_channel_enable_burst_mode` API. If single-cycle mode is enabled, this API will return an invalid argument error.
    - Burst mode can be disabled using the `gptmr_channel_disable_burst_mode` API.
    - To check if Burst mode is enabled, use the `gptmr_channel_is_burst_mode` API.

  - When `BURST_COUNT` of GPTMR equals `BURST_CFG`, `BURST_COUNT` will no longer increment, and the counter will stop counting. To start the next count, clear 'CEN' and set 'CEN' again, then reset the counter 'CNTRST'. Resetting the `BURST_COUNT` of GPTMR can be indirectly achieved by resetting the counter 'CNTRST'.

    - To start counting in Burst mode, use the `gptmr_channel_burst_mode_start_counter` API. The counting mode is configured according to `gptmr_burst_counter_mode_t`.
    - To stop counting in Burst mode, use the `gptmr_channel_burst_mode_stop_counter` API.

    - `BURST_COUNT` of GPTMR can be obtained using the `gptmr_channel_get_current_burst_count` API.
    - `BURST_CFG` of GPTMR can be set using the `gptmr_channel_set_target_burst_count` API.
    - `BURST_CFG` of GPTMR can be obtained using the `gptmr_channel_get_target_burst_count` API.

- Example: Configuration of GPTMR0 channel 1 in Burst mode with a reload value of 1 second and counting 10 times in restart mode

  .. code-block:: c

        #include <stdio.h>
        #include "board.h"
        #include "hpm_sysctl_drv.h"
        #include "hpm_gptmr_drv.h"
        #include "hpm_debug_console.h"

        #define APP_BOARD_GPTMR               BOARD_GPTMR
        #define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
        #define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ

        #define APP_TICK_MS                   (1000)

        static void timer_config(void);

        uint32_t target_burst_count, current_burst_count;

        SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr)
        void tick_ms_isr(void)
        {
            target_burst_count = gptmr_channel_get_target_burst_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            current_burst_count = gptmr_channel_get_current_burst_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH))) {
                gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
                printf("timer tick %d ms\n", APP_TICK_MS);
                board_led_toggle();
                /* If BURST_COUNT equals BURST_CFG, it indicates that the count has reached BURST_CFG times */
                if (target_burst_count == current_burst_count) {
                    gptmr_channel_burst_mode_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_burst_counter_restart);
                    printf("gptmr channel burst\n");
                }
            }
        }

        int main(void)
        {
            board_init();
            board_init_led_pins();
            init_gptmr_pins(APP_BOARD_GPTMR);
            printf("gptmr timer basic test\n");
            timer_config();
            while (1) {
            }
        }

        static void timer_config(void)
        {
            uint32_t gptmr_freq;
            gptmr_channel_config_t config;

            gptmr_freq = board_init_gptmr_clock(APP_BOARD_GPTMR);
            /* The default configuration disables single-cycle mode (OP mode) */
            gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
            /* Reload value set to 1 second */
            config.reload = gptmr_freq / 1000 * APP_TICK_MS;
            gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);

            /* Set `BURST_CFG` of GPTMR to 10 (count 10 times) */
            gptmr_channel_set_target_burst_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, 10);
            /* Enable Burst mode */
            gptmr_channel_enable_burst_mode(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            /* Enable the counter */
            gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            /* Enable reload interrupt */
            gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
            intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
        }


GPTMR samplse
----------------

.. toctree::
  :maxdepth: 3

  cntmode/README_en
  counter_sync/README_en
  input_capture/README_en
  monitor/README_en
  opmode/README_en
  output_compare/README_en
  pwm_generate/README_en
  pwm_measure/README_en
  sent_signal_decode/README_en
  t_shape_accel_decel/README_en
  timer_basic/README_en
  qeimode/README_en

