.. _gpio:

GPIO
========

Overview
--------

The GPIO example shows how to change GPIO output pin level and use GPIO input pin trigger interrupt.

Board Setting
-------------

No special settings are required

Known Issue
-----------

When GPIO is used as the key input pin detection, the input level jitter will cause GPIO to detect the change of input level multiple times due to the mechanical jitter of the key.

Running the example
-------------------

- chips that support both-edge trigger mode, the project are configured in both-edge trigger mode,so every time a button is pressed and released, the LED state will be toggle, and the LED toggle logs will be printed twice.

- When the example runs successfully, the LED will flash 5 times, then press GPIO button (please check  :ref:`Button <board_resource>`  section of specific board) will toggle the LED status, and the following message is displayed in the terminal:


.. code-block:: console

   toggling led 5 times in total
   toggling led 1/5 times
   toggling led 2/5 times
   toggling led 3/5 times
   toggling led 4/5 times
   toggling led 5/5 times
   input interrupt
   user led will be switched on off based on user switch
   toggle led pin output
   toggle led pin output

API Usage
------------

- The I/O subsystem comprises General-purpose IO Control Module (IOC), GPIO Controller, Fast GPIO Controller, and GPIO Manager

- For detailed API specifications, refer to `hpm_gpio_drv.h` and relevant user manuals

- For Fast GPIO Controller and GPIO Manager usage examples, see :ref:`gpiom <gpiom>`

IO Initialization
^^^^^^^^^^^^^^^^^^^

- Primarily initializes the General-purpose IO Control Module (IOC), including configuration of:

  * IO electrical characteristics via `PAD_CTL`
  * Peripheral functionality via `FUNC_CTL`

- Power Management Domain IO Controller (PIOC):

  * Controls PY ports with equivalent functionality to general IOC

- Battery Backup Domain IO Controller (BIOC):

  * Controls PZ ports with equivalent functionality to general IOC

Configuring IO Properties via `PAD_CTL`
""""""""""""""""""""""""""""""""""""""""

- `PAD_CTL` configures electrical characteristics including:

  * Open-drain selection
  * Drive strength
  * Pull-up/pull-down configuration
  * Refer to `hpm_ioc_regs.h` for register definitions

- Example configuration:

  .. code-block:: c

    /* Configure PA09 with pull-up enabled and Schmitt trigger enabled */
    HPM_IOC->PAD[IOC_PAD_PA09].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);

Peripheral Function Configuration via `FUNC_CTL`
""""""""""""""""""""""""""""""""""""""""""""""""""

- `FUNC_CTL` controls three key functions (definitions in `hpm_iomux.h`):

  * Loopback functionality (LOOP_BACK)
  * Analog mode (ANALOG)
  * Peripheral function mapping (ALT_SELECT)

Key applications:

- **Loopback**: Essential for clock-dependent peripherals (e.g., SPI SCLK, I2C SCL)
- **Analog mode**: Used for analog signal processing (ADC, MIPI interfaces)
- **ALT_SELECT**: Determines peripheral function mapping (GPIO, I2C, SPI, etc.)

**Important Notes**:

- For GPIOY pins:

  * Configure as GPIO in IOC's FUNC_CTL
  * Example: Configure PY06 as UART1_RXD

  .. code-block:: c

        HPM_PIOC->PAD[IOC_PAD_PY06].FUNC_CTL = PIOC_PY06_FUNC_CTL_SOC_PY_06;
        HPM_IOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY06_FUNC_CTL_UART1_RXD;

- For GPIOZ pins:

  * Configure as GPIO in IOC's FUNC_CTL
  * Example: Configure PZ02 as GPIO

  .. code-block:: c

        HPM_IOC->PAD[IOC_PAD_PZ02].FUNC_CTL = IOC_PZ02_FUNC_CTL_GPIO_Z_02;
        HPM_BIOC->PAD[IOC_PAD_PZ02].FUNC_CTL = BIOC_PZ02_FUNC_CTL_SOC_PZ_02;

- SPI Configuration Example:

  .. code-block:: c

        /* Configure SPI1 pins with SCLK loopback */
        HPM_IOC->PAD[IOC_PAD_PA26].FUNC_CTL = IOC_PA26_FUNC_CTL_SPI1_CS_0;
        HPM_IOC->PAD[IOC_PAD_PA27].FUNC_CTL = IOC_PA27_FUNC_CTL_SPI1_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
        HPM_IOC->PAD[IOC_PAD_PA28].FUNC_CTL = IOC_PA28_FUNC_CTL_SPI1_MISO;
        HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PA29_FUNC_CTL_SPI1_MOSI;

GPIO Controller
^^^^^^^^^^^^^^^^^^^

- The GPIO controller is primarily used to control the input/output modes of IOs, read input states, set output states, and configure interrupt trigger modes.

- The default controller for IOs is GPIO. To use FGPIO, refer to the :ref:`gpiom <gpiom>` example via GPIO Manager.

- The `hpm_ioc_regs.h` header file defines PAD register groups for each IO (e.g., `PA26` is defined as `IOC_PAD_PA26`).

- The `hpm_gpio_drv.h` header file provides `GPIO_GET_PORT_INDEX` and `GPIO_GET_PIN_INDEX` macros to determine the port number and pin index for each IO.

- These macros allow quick identification of port/pin configuration. For example, ``IOC_PAD_PA26`` has port number GPIO_GET_PORT_INDEX(IOC_PAD_PA26) and pin index GPIO_GET_PIN_INDEX(IOC_PAD_PA26).



Configuring IO Modes
""""""""""""""""""""""""

- Two methods are provided for setting output mode: with or without initial level.

  - API with initial level:

    .. code-block:: c

        void gpio_set_pin_output_with_initial(GPIO_Type *ptr, uint32_t port, uint8_t pin, uint8_t initial);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DO_GPIOA)
            * - pin
              - uint8_t
              - GPIO pin index (0-31)
            * - initial
              - uint8_t
              - Initial level (0: low, 1: high)

    - Example: Configure PA26 as output with initial high level:

      .. code-block:: c

        gpio_set_pin_output_with_initial(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26), 1);

  - API without initial level:

    .. code-block:: c

        void gpio_set_pin_output(GPIO_Type *ptr, uint32_t port, uint8_t pin);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DO_GPIOA)
            * - pin
              - uint8_t
              - GPIO pin index (0-31)

    - Example: Configure PA26 as output:

      .. code-block:: c

            gpio_set_pin_output(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26));

- Input Mode Configuration API:

  .. code-block:: c

        void gpio_set_pin_input(GPIO_Type *ptr, uint32_t port, uint8_t pin);

  - Parameter Description:

    .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DI_GPIOA)
            * - pin
              - uint8_t
              - GPIO pin index (0-31)

  - Example: Configure PA26 as input:

    .. code-block:: c

            gpio_set_pin_input(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26));


IO Level Control
""""""""""""""""""""""""

- Output APIs support three operations: set single pin level, toggle pin level, and set port level.

  - Prerequisite: IO must be in output mode

  - Set Single Pin Level:

    .. code-block:: c

        void gpio_write_pin(GPIO_Type *ptr, uint32_t port, uint8_t pin, uint8_t high);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DO_GPIOA)
            * - pin
              - uint8_t
              - GPIO pin index (0-31)
            * - high
              - uint8_t
              - Output level (0: low, 1: high)

    - Example: Set PA26 to high:

      .. code-block:: c

        gpio_write_pin(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26), 1);

  - Toggle Pin Level:

    .. code-block:: c

        void gpio_toggle_pin(GPIO_Type *ptr, uint32_t port, uint8_t pin);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DO_GPIOA)
            * - pin
              - uint8_t
              - GPIO pin index (0-31)

    - Example: Toggle PA26:

      .. code-block:: c

        gpio_toggle_pin(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26));


  - Set Port Level:

    .. code-block:: c

        void gpio_write_port(GPIO_Type *ptr, uint32_t port, uint32_t value);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DO_GPIOA)
            * - value
              - uint32_t
              - Port level (0x00000000-0xFFFFFFFF)

    - Example: Set all PA pins to high:

      .. code-block:: c

        gpio_write_port(HPM_GPIO, GPIO_DO_GPIOA, 0xFFFFFFFF);

  - Clear Specified Pins to Low:

    .. code-block:: c

        void gpio_set_port_low_with_mask(GPIO_Type *ptr, uint32_t port, uint32_t mask);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DO_GPIOA)
            * - mask
              - uint32_t
              - Bitmask for pins to clear (1=clear to low)

    - Example: Clear PA01 and PA02 to low:

      .. code-block:: c

        uint32_t mask = (1 << GPIO_GET_PIN_INDEX(IOC_PAD_PA01)) | (1 << GPIO_GET_PIN_INDEX(IOC_PAD_PA02));
        gpio_set_port_low_with_mask(HPM_GPIO, GPIO_DO_GPIOA, mask);


Input State Reading
""""""""""""""""""""""""

- Note: Works even when IO is configured for peripheral function (e.g., SPI MISO)

  - Read Single Pin:

    .. code-block:: c

        uint8_t gpio_read_pin(GPIO_Type *ptr, uint32_t port, uint8_t pin);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DI_GPIOA)
            * - pin
              - uint8_t
              - GPIO pin index (0-31)

    - Example: Read PA26:

      .. code-block:: c

        uint8_t level = gpio_read_pin(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26));

  - Read Entire Port:

    .. code-block:: c

        uint32_t gpio_read_port(GPIO_Type *ptr, uint32_t port);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - GPIO_Type *
              - Base address of GPIO controller
            * - port
              - uint32_t
              - GPIO port number (e.g., GPIO_DI_GPIOA)

    - Example: Read PA port:

      .. code-block:: c

        uint32_t level = gpio_read_port(HPM_GPIO, GPIO_DI_GPIOA);

GPIO Interrupts
"""""""""""""""

- FGPIO does not support interrupts

- Interrupt Related Enumerations:

  - Interrupt Trigger Modes:

    .. code-block:: c

        typedef enum gpio_interrupt_trigger {

            /* Level-triggered high (interrupt persists during high state) */
            gpio_interrupt_trigger_level_high = 0,

            /* Level-triggered low (interrupt persists during low state) */
            gpio_interrupt_trigger_level_low,

            /* Rising-edge triggered */
            gpio_interrupt_trigger_edge_rising,

            /* Falling-edge triggered */
            gpio_interrupt_trigger_edge_falling,

            /* Dual-edge triggered */
        #if defined(GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT) && (GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT == 1)
            gpio_interrupt_trigger_edge_both,
        #endif
        } gpio_interrupt_trigger_t;

    - **Note**:

      - Dual-edge triggering is not supported by all SOCs. Check `hpm_soc_feature.h` for `GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT` macro definition
      - Before using interrupts:
        1. Configure IO in input mode
        2. Set interrupt trigger mode

- Interrupt Configuration API:

  .. code-block:: c

        void gpio_config_pin_interrupt(GPIO_Type *ptr, uint32_t gpio_index, uint8_t pin_index, gpio_interrupt_trigger_t trigger);

  - Parameter Description:

    .. list-table::
          :header-rows: 1

          * - Parameter
            - Type
            - Description
          * - ptr
            - GPIO_Type *
            - Base address of GPIO controller
          * - gpio_index
            - uint32_t
            - GPIO port number (e.g. GPIO_DI_GPIOA)
          * - pin_index
            - uint8_t
            - GPIO pin index (0-31)
          * - trigger
            - gpio_interrupt_trigger_t
            - Trigger mode from `gpio_interrupt_trigger_t` enum

  - Example: Configure PA26 as input with rising-edge trigger

    .. code-block:: c

        gpio_config_pin_interrupt(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26), gpio_interrupt_trigger_edge_rising);

- Interrupt Enable API:

  .. code-block:: c

        void gpio_enable_pin_interrupt(GPIO_Type *ptr, uint32_t port, uint8_t pin);

  - Parameter Description:

    .. list-table::
          :header-rows: 1

          * - Parameter
            - Type
            - Description
          * - ptr
            - GPIO_Type *
            - Base address of GPIO controller
          * - port
            - uint32_t
            - GPIO port number (e.g. GPIO_DI_GPIOA)
          * - pin
            - uint8_t
            - GPIO pin index (0-31)

- Interrupt Disable API:

  .. code-block:: c

        void gpio_disable_pin_interrupt(GPIO_Type *ptr, uint32_t port, uint8_t pin);

  - Parameter Description:

    .. list-table::
          :header-rows: 1

          * - Parameter
            - Type
            - Description
          * - ptr
            - GPIO_Type *
            - Base address of GPIO controller
          * - port
            - uint32_t
            - GPIO port number (e.g. GPIO_DI_GPIOA)
          * - pin
            - uint8_t
            - GPIO pin index (0-31)

- Interrupt Clear API (Call in ISR):

  .. code-block:: c

        bool gpio_check_clear_interrupt_flag(GPIO_Type *ptr, uint32_t port, uint8_t pin);

  - Parameter Description:

    .. list-table::
          :header-rows: 1

          * - Parameter
            - Type
            - Description
          * - ptr
            - GPIO_Type *
            - Base address of GPIO controller
          * - port
            - uint32_t
            - GPIO port number (e.g. GPIO_DI_GPIOA)
          * - pin
            - uint8_t
            - GPIO pin index (0-31)

  - Return Value:

    - true: Interrupt flag cleared successfully
    - false: No interrupt flag to clear