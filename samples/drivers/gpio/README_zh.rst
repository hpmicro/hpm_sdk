.. _gpio:

GPIO
========

概述
------

GPIO示例工程演示了控制GPIO引脚输出电平变化和输入引脚触发中断的功能。

硬件设置
------------

无特殊设置

已知问题
------------

当GPIO用作按键输入引脚检测时，受按键机械抖动影响，输入电平抖动，会造成GPIO多次检测到输入电平变化的情况。

运行现象
------------

- 支持双沿触发模式的芯片，工程配置的是双沿触发模式，每次按键按下和释放都会翻转一次LED状态，并打印两次LED切换输出。

- 当工程正确运行后，可以观察到LED闪烁5次，之后按下GPIO按键(请确认具体开发板  :ref:`按键 <board_resource>` 部分描述)可以翻转LED的状态，串口终端会输出如下信息：


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


API使用说明
------------

- 输入输出模块包括通用IO控制模块IOC，GPIO控制器和快速GPIO控制器，以及GPIO管理器。

- 更多内容请参考 `hpm_gpio_drv.h` 的API说明以及相关用户手册。

- 快速GPIO控制器，以及GPIO管理器的使用可参考 :ref:`gpiom <gpiom>` 例子

IO初始化
^^^^^^^^^^^^^^^^^^^

- 主要是初始化通用IO控制模块IOC，包括配置IO的属性 `PAD_CTL` 以及IO的外设功能 `FUNC_CTL` 。

- 电源管理域IO控制器PIOC，控制PY端口，功能与通用IOC一致。

- 电池备份域IO控制器BIOC，控制PZ端口，功能与通用IOC一致。

配置IO的属性 `PAD_CTL`
""""""""""""""""""""""""

- `PAD_CTL` 主要用来配置IO的电气特性，比如开漏选择、驱动强度、上下拉等。 对应的定义可以查看 `hpm_ioc_regs.h` 头文件 。

- 举例：

  .. code-block:: c

    /* 配置 PA09的IO属性为上拉使能，施密特触发器使能  */
    HPM_IOC->PAD[IOC_PAD_PA09].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);

IO的外设功能 `FUNC_CTL`
""""""""""""""""""""""""""""

- `FUNC_CTL` 包括输出回送功能(LOOP_BACK)，模拟功能(ANALOG)和外设功能映(ALT_SELECT)。 对应的定义可以查看 `hpm_iomux.h` 头文件 。

  - 输出回送功能(LOOP_BACK)主要作用于一些需要时钟反馈的外设，比如SPI的SCLK，I2C的SCL等等。

  - 模拟功能(ANALOG)主要作用于一些需要模拟信号的外设，比如ADC，MIPI信号等等。

  - 外设功能映(ALT_SELECT)主要作用于一些需要映射到特定功能的IO，比如GPIO，I2C，SPI等等。

- **注意**：

  - 对于GPIOY的IO，需要在IOC的FUNC_CTL配置IO的功能为GPIO。

    - 例如：配置PY06的功能为uart1的rxd。

      .. code-block:: c

            HPM_PIOC->PAD[IOC_PAD_PY06].FUNC_CTL = PIOC_PY06_FUNC_CTL_SOC_PY_06;
            HPM_IOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY06_FUNC_CTL_UART1_RXD;

  - 对于GPIOZ的IO，需要在IOC的FUNC_CTL配置IO的功能为GPIO。

    - 例如：配置PZ02的功能为GPIO。

      .. code-block:: c

            HPM_IOC->PAD[IOC_PAD_PZ02].FUNC_CTL = IOC_PZ02_FUNC_CTL_GPIO_Z_02;
            HPM_BIOC->PAD[IOC_PAD_PZ02].FUNC_CTL = BIOC_PZ02_FUNC_CTL_SOC_PZ_02;


- 举例：

  .. code-block:: c

        /* 配置SPI引脚的功能为SPI1_SCLK，SPI1_MISO，SPI1_MOSI，并且配置SPI1_SCLK的输出回送功能 */
        HPM_IOC->PAD[IOC_PAD_PA26].FUNC_CTL = IOC_PA26_FUNC_CTL_SPI1_CS_0;
        HPM_IOC->PAD[IOC_PAD_PA27].FUNC_CTL = IOC_PA27_FUNC_CTL_SPI1_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
        HPM_IOC->PAD[IOC_PAD_PA28].FUNC_CTL = IOC_PA28_FUNC_CTL_SPI1_MISO;
        HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PA29_FUNC_CTL_SPI1_MOSI;

GPIO控制器
^^^^^^^^^^^^^^^^^^^

- GPIO控制器主要是用来控制IO的输入或者输出模式，读取IO的输入状态，设置IO的输出状态，以及设置IO的中断触发模式等等。

- IO默认选择的是GPIO控制器，如果需要选择FGPIO，可以使用GPIO管理器指定，可参考 :ref:`gpiom <gpiom>` 例子

- 在 `hpm_ioc_regs.h` 的头文件中，定义了每个IO的PAD寄存器组，比如 `PA26` 则定义 `IOC_PAD_PA26`

- 在 `hpm_gpio_drv.h` 的头文件中，使用 `GPIO_GET_PORT_INDEX` 和 `GPIO_GET_PIN_INDEX` 定义了每个IO所属的port端口号和引脚号

- 从上述可快速知道IO的port端口号和引脚号，比如 ``IOC_PAD_PA26`` 的port端口号为 GPIO_GET_PORT_INDEX(IOC_PAD_PA26)，引脚号为 GPIO_GET_PIN_INDEX(IOC_PAD_PA26)。



设置IO的输入输出模式
""""""""""""""""""""""""

- 设置IO为输出模式，提供两种方式：一种带初始化电平的设置方式，一种不带初始化电平的设置方式。

  - 带初始化电平的设置方式 API：

    .. code-block:: c

        void gpio_set_pin_output_with_initial(GPIO_Type *ptr, uint32_t port, uint8_t pin, uint8_t initial);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DO_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31
            * - initial
              - uint8_t
              - 初始化电平，取值范围为0~1，0表示低电平，1表示高电平。

    - 举例：这里使用PA26作为输出引脚，初始化电平为高电平。

      .. code-block:: c

        gpio_set_pin_output_with_initial(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26), 1);

  - 不带初始化电平的设置方式 API：

    .. code-block:: c

        void gpio_set_pin_output(GPIO_Type *ptr, uint32_t port, uint8_t pin);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DO_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31

    - 举例：这里使用PA26作为输出引脚.

      .. code-block:: c

            gpio_set_pin_output(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26));

- 设置IO为输入模式API：

  .. code-block:: c

        void gpio_set_pin_input(GPIO_Type *ptr, uint32_t port, uint8_t pin);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DI_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31

  - 举例：这里使用PA26作为输入引脚。

    .. code-block:: c

            gpio_set_pin_input(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26));


IO电平的设置与获取
""""""""""""""""""""""""

- 设置IO输出API：可以设置单个IO的输出电平、设置单个IO的电平翻转；设置Port端口下的所有IO的输出电平。

  - 需要将IO提前设置为输出模式

  - 设置单个IO的输出电平API：

    .. code-block:: c

        void gpio_write_pin(GPIO_Type *ptr, uint32_t port, uint8_t pin, uint8_t high);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DO_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31
            * - high
              - uint8_t
              - 输出电平，取值范围为0~1，0表示低电平，1表示高电平。

    - 举例：将PA26设置输出电平为高电平。

      .. code-block:: c

        gpio_write_pin(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26), 1);

  - 设置单个IO的电平翻转API：

    .. code-block:: c

        void gpio_toggle_pin(GPIO_Type *ptr, uint32_t port, uint8_t pin);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DO_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31

    - 举例：将PA26翻转输出电平。

      .. code-block:: c

        gpio_toggle_pin(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26));


  - 设置Port端口下的所有IO的输出电平API：

    .. code-block:: c

        void gpio_write_port(GPIO_Type *ptr, uint32_t port, uint32_t value);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DO_GPIOA
            * - value
              - uint32_t
              - 输出电平，取值范围为0~0xFFFFFFFF，0表示低电平，1表示高电平。

    - 举例：这里使用PA端口设置输出电平为高电平。

      .. code-block:: c

        gpio_write_port(HPM_GPIO, GPIO_DO_GPIOA, 0xFFFFFFFF);

  - 设置Port端口下指定的IO电平清除为低电平API：

    .. code-block:: c

        void gpio_set_port_low_with_mask(GPIO_Type *ptr, uint32_t port, uint32_t mask);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DO_GPIOA
            * - mask
              - uint32_t
              - 对应的位为1则表示对应的IO输出电平为低电平，取值范围为0~0xFFFFFFFF。

    - 举例：这里使用PA端口的PA01，PA02为低电平，其余的电平保持不变。

      .. code-block:: c

        uint32_t mask = (1 << GPIO_GET_PIN_INDEX(IOC_PAD_PA01)) | (1 << GPIO_GET_PIN_INDEX(IOC_PAD_PA02));
        gpio_set_port_low_with_mask(HPM_GPIO, GPIO_DO_GPIOA, mask);


- 读取IO输入状态API：

  - 即使IO功能为外设，比如SPI的MISO，也可以读取IO的输入状态。

  - 读取IO的电平状态API：

    .. code-block:: c

        uint8_t gpio_read_pin(GPIO_Type *ptr, uint32_t port, uint8_t pin);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DI_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31

    - 举例：读取PA26的输入电平状态。

      .. code-block:: c

        uint8_t level = gpio_read_pin(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26));

  - 读取Port端口下的所有IO的输入状态API：

    .. code-block:: c

        uint32_t gpio_read_port(GPIO_Type *ptr, uint32_t port);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DI_GPIOA

    - 举例：读取PA端口的所有IO的输入状态。

      .. code-block:: c

        uint32_t level = gpio_read_port(HPM_GPIO, GPIO_DI_GPIOA);


IO中断
""""""""""

- FGPIO不支持中断

- 相关枚举介绍：

  - 中断触发模式枚举：

    .. code-block:: c

        typedef enum gpio_interrupt_trigger {

            /* 高电平触发中断 只要在高电平阶段会一直触发中断 */
            gpio_interrupt_trigger_level_high = 0,

            /* 低电平触发中断 只要在低电平阶段会一直触发中断 */
            gpio_interrupt_trigger_level_low,

            /* 上升沿触发中断 上升沿阶段会一直触发中断 */
            gpio_interrupt_trigger_edge_rising,

            /* 下降沿触发中断 下降沿阶段会一直触发中断 */
            gpio_interrupt_trigger_edge_falling,

            /* 双沿触发中断 上升沿和下降沿阶段都会触发中断 */
        #if defined(GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT) && (GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT == 1)
            gpio_interrupt_trigger_edge_both,
        #endif
        } gpio_interrupt_trigger_t;

    - **注意**:

      - 对于双沿触发模式，并不是所有SOC都支持，此项支持需要看SOC的 `hpm_soc_feature.h` 是否定义 `GPIO_SOC_HAS_EDGE_BOTH_INTERRUPT` 宏
      - 在使用中断时，需要将IO提前设置为输入模式，并且需要配置IO的中断触发模式。

- 中断触发模式配置API：

  .. code-block:: c

        void gpio_config_pin_interrupt(GPIO_Type *ptr, uint32_t gpio_index, uint8_t pin_index, gpio_interrupt_trigger_t trigger);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - gpio_index
              - uint32_t
              - GPIO端口号，比如GPIO_DI_GPIOA
            * - pin_index
              - uint8_t
              - GPIO引脚号，取值范围为0~31
            * - trigger
              - gpio_interrupt_trigger_t
              - 中断触发模式，具体看 `gpio_interrupt_trigger_t` 枚举。

  - 举例：这里使用PA26作为输入引脚，配置为上升沿触发模式。

    .. code-block:: c

        gpio_config_pin_interrupt(HPM_GPIO, GPIO_GET_PORT_INDEX(IOC_PAD_PA26), GPIO_GET_PIN_INDEX(IOC_PAD_PA26), gpio_interrupt_trigger_edge_rising);

- 中断使能API：

  .. code-block:: c

        void gpio_enable_pin_interrupt(GPIO_Type *ptr, uint32_t port, uint8_t pin);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DI_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31

- 中断禁能API：

  .. code-block:: c

        void gpio_disable_pin_interrupt(GPIO_Type *ptr, uint32_t port, uint8_t pin);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DI_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31

- 中断清除API：在中断服务函数中调用。

  .. code-block:: c

        bool gpio_check_clear_interrupt_flag(GPIO_Type *ptr, uint32_t port, uint8_t pin);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - GPIO_Type *
              - GPIO控制器的基地址
            * - port
              - uint32_t
              - GPIO端口号，比如GPIO_DI_GPIOA
            * - pin
              - uint8_t
              - GPIO引脚号，取值范围为0~31

  - 返回值：

    - 如果中断标志被清除，则返回true，否则返回false。