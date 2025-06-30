HPM MCL V2 Middleware
=====================

Overview
--------

HPM MCL V2 is a middleware library for motor control, providing a complete motor control solution. The middleware supports various motor control algorithms and features, including FOC (Field-Oriented Control), SMC (Sliding Mode Control), hardware FOC acceleration, hardware hybrid loop acceleration, and more.

Key Features
------------

- Support for multiple motor control algorithms
  - Field-Oriented Control (FOC)
  - Sliding Mode Control (SMC)
  - Sensorless Control
  - Hardware FOC Control
    - Complete hardware-based current loop using VSC, CLC, and QEO modules
    - Hardware-accelerated Clarke/Park transforms and current control
    - Real-time hardware PWM generation
  - Hardware Hybrid Loop FOC Control
    - VSC (Vector Signal Controller) hardware acceleration for Clarke/Park transforms
    - CLC (Current Loop Controller) hardware acceleration for current control
    - QEO (Quadrature Encoder Output) hardware acceleration for inverse transforms
    - Software-hardware data exchange through callback functions
    - Dynamic enable/disable of individual hardware modules
  - Stepper Motor FOC Control
    - Support for trapezoidal curve path planning
    - Support for acceleration, deceleration, speed and time parameters
- Support for various encoder types
- Dead zone compensation
- DQ axis decoupling
- Angle prediction
- Complete physical parameter configuration interface
- Multiple debugging capabilities
- Offline parameter detection
- Multiple filter types
  - IIR Filter
  - PLL Type I Filter
  - PLL Type II Filter

Directory Structure
-------------------

- core/ - Core control algorithm implementation
  - control/ - Control algorithm implementation
    - hpm_mcl_control.h/c - Core control algorithm implementation
    - hpm_mcl_filter.h/c - Filter implementation
    - hpm_mcl_path_plan.h/c - Stepper motor path planning implementation
  - loop/ - Control loop implementation
  - sensor/ - Sensor-related implementation
  - drivers/ - Driver layer implementation
  - detect/ - Detection function implementation
- drivers/ - Driver layer implementation
  - hpm_mcl_hw_loop.h/c - Hardware hybrid loop implementation
- encoder/ - Encoder-related implementation
- hpm_mcl_common.h - Common definitions and macros
- hpm_mcl_physical.h - Physical parameter definitions
- hpm_mcl_math.h - Mathematical operation functions
- hpm_mcl_debug.h - Debugging features
- hpm_mcl_cfg.h - Configuration options

Control Features
----------------

- Current Control
  - D-axis current PID control
  - Q-axis current PID control
  - Hardware CLC acceleration for current loop control
- Speed Control
  - Speed PID control
- Position Control
  - Position PID control
- Motor Angle Alignment
  - Multiple alignment algorithms support
    - Basic single-stage alignment: Simple and fast alignment with configurable parameters
    - Three-stage alignment: Enhanced robust alignment for challenging positions (e.g., 90-degree mechanical angle)
      - Stage 1: High current coarse alignment with q-axis disturbance
      - Stage 2: Moderate current fine alignment
      - Stage 3: Low current stabilization
  - Hardware/Software FOC compatibility
    - Users select appropriate current values based on their mode
    - No internal mode distinction in middleware
- Hardware FOC Control
  - Complete hardware-based current loop implementation
  - VSC hardware acceleration for coordinate transformations
  - CLC hardware acceleration for current regulation
  - QEO hardware acceleration for PWM generation
  - Integrated hardware control pipeline
- Hardware Hybrid Loop Control
  - VSC hardware acceleration
    - Clarke transformation (abc to αβ)
    - Park transformation (αβ to dq)
    - Hardware-accelerated coordinate transformations
  - CLC hardware acceleration
    - Hardware-based current loop control
    - D-axis and Q-axis current regulation
    - Real-time PID calculation in hardware
  - QEO hardware acceleration
    - Inverse Park transformation (dq to αβ)
    - Inverse Clarke transformation (αβ to abc)
    - Hardware-generated PWM duty cycles
  - Software-hardware data exchange through callback functions
  - Dynamic control of individual hardware modules
- Stepper Motor FOC Control
  - Trapezoidal curve path planning
  - Support for acceleration, deceleration, speed and time parameters
- Filters
  - IIR Filter (DF1 structure)
  - PLL Type I Filter
  - PLL Type II Filter
- Offline Parameter Detection
  - Resistance detection
  - Inductance detection
  - Flux linkage detection

Configuration Options
---------------------

- MCL_EN_THETA_FORECAST - Enable angle prediction
- MCL_EN_DQ_AXIS_DECOUPLING_FUNCTION - Enable DQ axis decoupling
- MCL_EN_DEAD_AREA_COMPENSATION - Enable dead zone compensation
- MCL_EN_SENSORLESS_SMC - Enable sensorless SMC control
- HW_CURRENT_FOC_ENABLE - Enable hardware FOC mode (complete hardware current loop)
- MCL_USER_DEFINED_DEBUG_FIFO - Debug FIFO size

Usage Instructions
------------------

1. Configure Physical Parameters
   - Motor parameters (resistance, inductance, pole pairs, etc.)
   - Board parameters (sampling resistance, ADC reference voltage, etc.)
   - Time parameters (PWM period, control period, etc.)

2. Select Control Algorithm
   - Choose appropriate control algorithm based on application requirements
   - Configure corresponding parameters
   - Select appropriate filters

3. Motor Angle Alignment Configuration
   - Choose alignment algorithm based on application requirements:
     - Basic algorithm: For simple and fast alignment
     - Three-stage algorithm: For robust alignment, especially when motor may start at challenging positions
   - Configure algorithm-specific parameters:
     - For basic algorithm: Set d_current, q_current, and delay_ms
     - For three-stage algorithm: Configure each stage with appropriate current and timing
   - Select appropriate current values based on hardware/software FOC mode:
     - Hardware mode: Typically uses higher current values (e.g., 6-8A for d-axis)
     - Software mode: Typically uses lower current values (e.g., 2-4A for d-axis)
   - Example usage:
     - motor_angle_align() - Uses three-stage algorithm with mode-specific parameters
     - Custom implementation possible by selecting different algorithms and parameters

4. Hardware FOC Configuration
   - Enable HW_CURRENT_FOC_ENABLE macro for complete hardware current loop
   - Configure VSC, CLC, and QEO hardware modules
   - Set loop mode to mcl_mode_hardware_foc
   - Initialize hardware components (VSC, CLC, QEO)
   - Configure trigger matrix for hardware data flow

5. Hardware Hybrid Loop Configuration
   - Set loop mode to mcl_mode_hybrid_foc
   - Configure VSC, CLC, and QEO hardware modules
   - Set up conversion callbacks for software-hardware data exchange
   - Initialize hardware loop components using hpm_mcl_hw_loop_init()
   - Enable specific hardware acceleration modules using:
     - hpm_mcl_enable_vsc_hardware_loop() for VSC acceleration
     - hpm_mcl_enable_clc_hardware_loop() for CLC acceleration
     - hpm_mcl_enable_qeo_hardware_loop() for QEO acceleration
   - Each module can be enabled/disabled independently at runtime

6. Stepper Motor FOC Control
   - Configure trapezoidal curve path planning parameters
   - Set acceleration, deceleration and speed
   - Configure running time
   - Enable closed-loop control if needed
   - Configure position and speed control parameters for closed-loop mode

7. Debugging Features
   - Use provided debugging interfaces for system debugging
   - Record key data through FIFO
   - Use offline parameter detection

Notes
-----

- Ensure all physical parameters are correctly configured before use
- Debugging features may affect real-time performance, recommended for development phase only
- Different control algorithms may require different parameter configurations, please refer to specific algorithm documentation
- Hardware FOC mode requires compatible HPM MCU with VSC, CLC, and QEO modules and provides complete hardware current loop
- Hardware hybrid loop requires compatible HPM MCU with VSC, CLC, and QEO modules for partial hardware acceleration
- Hardware hybrid loop modules are enabled through function calls, not compile-time macros
- Conversion callback functions must be properly implemented for hardware-software data exchange in hybrid mode
- Pay attention to acceleration and deceleration settings in stepper motor path planning to avoid mechanical impact
- Filter parameters need to be adjusted according to actual application scenarios

API Compatibility
-----------------

For information about API compatibility between different versions, please refer to:

- `API Compatibility Guide (English) <API_COMPATIBILITY_GUIDE_en.rst>`_

This guide provides detailed information about:

- API changes between v1.9.0 and v1.10.0
- Backward compatibility solutions
- Migration recommendations
- Usage examples for both old and new APIs
