.. _dma_mgr_component:

DMA Manager 组件
================

概述
----

- DMA Manager 组件在底层 DMA 驱动之上提供了一套统一的资源管理层，负责 DMA 通道的分配、配置与生命周期管理。支持多 DMA 实例（HDMA、XDMA 等），并提供中断回调注册与通道控制能力。以下是该组件的主要特征：

  - 集中式 DMA 通道资源管理

    - 自动分配和释放 DMA 通道，避免手动管理冲突。

    - 支持从指定 DMA 实例请求通道。

  - 统一的通道配置接口

    - 通过 ``dma_mgr_chn_conf_t`` 结构体封装 DMA 通道配置（源/目的地址、传输宽度、突发大小、工作模式等），提供一次性配置 API ``dma_mgr_setup_channel``。

  - 灵活的中断回调机制

    - 支持分别注册传输完成（TC）、半传输完成（Half TC）、错误（Error）和终止（Abort）的回调函数。

  - 支持 DMAV2 高级特性

    - 无限循环传输、自定义突发大小、握手选项、字节序交换等。

相关枚举和结构体介绍
--------------------

- 枚举 / 宏介绍

  - DMA 传输宽度：定义每次传输的数据宽度。

    .. code-block:: c

       #define DMA_MGR_TRANSFER_WIDTH_BYTE        DMA_TRANSFER_WIDTH_BYTE        /* 8位 */
       #define DMA_MGR_TRANSFER_WIDTH_HALF_WORD   DMA_TRANSFER_WIDTH_HALF_WORD   /* 16位 */
       #define DMA_MGR_TRANSFER_WIDTH_WORD        DMA_TRANSFER_WIDTH_WORD        /* 32位 */
       #define DMA_MGR_TRANSFER_WIDTH_DOUBLE_WORD DMA_TRANSFER_WIDTH_DOUBLE_WORD /* 64位 */

  - DMA 每次突发传输的大小：

    .. code-block:: c

       #define DMA_MGR_NUM_TRANSFER_PER_BURST_1T    DMA_NUM_TRANSFER_PER_BURST_1T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_2T    DMA_NUM_TRANSFER_PER_BURST_2T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_4T    DMA_NUM_TRANSFER_PER_BURST_4T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_8T    DMA_NUM_TRANSFER_PER_BURST_8T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_16T   DMA_NUM_TRANSFER_PER_BURST_16T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_32T   DMA_NUM_TRANSFER_PER_BURST_32T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_64T   DMA_NUM_TRANSFER_PER_BURST_64T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_128T  DMA_NUM_TRANSFER_PER_BURST_128T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_256T  DMA_NUM_TRANSFER_PER_BURST_256T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_512T  DMA_NUM_TRANSFER_PER_BURST_512T
       #define DMA_MGR_NUM_TRANSFER_PER_BURST_1024T DMA_NUM_TRANSFER_PER_BURST_1024T

  - DMA 握手模式：普通模式（软件触发）或握手模式（硬件触发）。

    .. code-block:: c

       #define DMA_MGR_HANDSHAKE_MODE_NORMAL    DMA_HANDSHAKE_MODE_NORMAL
       #define DMA_MGR_HANDSHAKE_MODE_HANDSHAKE DMA_HANDSHAKE_MODE_HANDSHAKE

  - DMA 地址控制模式：递增、递减或固定。

    .. code-block:: c

       #define DMA_MGR_ADDRESS_CONTROL_INCREMENT DMA_ADDRESS_CONTROL_INCREMENT
       #define DMA_MGR_ADDRESS_CONTROL_DECREMENT DMA_ADDRESS_CONTROL_DECREMENT
       #define DMA_MGR_ADDRESS_CONTROL_FIXED     DMA_ADDRESS_CONTROL_FIXED

  - DMA 中断掩码：

    .. code-block:: c

       #define DMA_MGR_INTERRUPT_MASK_NONE  DMA_INTERRUPT_MASK_NONE
       #define DMA_MGR_INTERRUPT_MASK_ERROR DMA_INTERRUPT_MASK_ERROR
       #define DMA_MGR_INTERRUPT_MASK_ABORT DMA_INTERRUPT_MASK_ABORT
       #define DMA_MGR_INTERRUPT_MASK_TC    DMA_INTERRUPT_MASK_TERMINAL_COUNT
       #define DMA_MGR_INTERRUPT_MASK_ALL   DMA_INTERRUPT_MASK_ALL

  - DMA 通道状态：

    .. code-block:: c

       #define DMA_MGR_CHANNEL_STATUS_ONGOING DMA_CHANNEL_STATUS_ONGOING
       #define DMA_MGR_CHANNEL_STATUS_ERROR   DMA_CHANNEL_STATUS_ERROR
       #define DMA_MGR_CHANNEL_STATUS_ABORT   DMA_CHANNEL_STATUS_ABORT
       #define DMA_MGR_CHANNEL_STATUS_TC      DMA_CHANNEL_STATUS_TC

  - DMA 通道优先级：

    .. code-block:: c

       #define DMA_MGR_CHANNEL_PRIORITY_LOW  DMA_CHANNEL_PRIORITY_LOW
       #define DMA_MGR_CHANNEL_PRIORITY_HIGH DMA_CHANNEL_PRIORITY_HIGH

- 结构体介绍

  - DMA 资源结构体：封装一个 DMA 通道资源，包含 DMA 实例、通道号及中断号。

    .. code-block:: c

       typedef struct _dma_resource {
           DMA_Type *base;   /**< 所属的 DMA 实例 */
           uint32_t channel; /**< 通道索引 */
           int32_t irq_num;  /**< DMA 中断号 */
       } dma_resource_t;

  - DMA 通道配置结构体：用于配置 DMA 通道的各个参数。

    .. code-block:: c

       typedef struct hpm_dma_mgr_chn_conf {
           bool en_dmamux;                   /**< DMAMUX 使能 */
           uint8_t dmamux_src;               /**< DMAMUX 源 */
           uint8_t priority;                 /**< 通道优先级 */
           uint8_t src_burst_size;           /**< 源端突发大小 */
           uint8_t src_mode;                 /**< 源端工作模式: 0-普通, 1-握手 */
           uint8_t dst_mode;                 /**< 目的端工作模式: 0-普通, 1-握手 */
           uint8_t src_width;                /**< 源端数据位宽 */
           uint8_t dst_width;                /**< 目的端数据位宽 */
           uint8_t src_addr_ctrl;            /**< 源端地址控制: 0-递增, 1-递减, 2-固定 */
           uint8_t dst_addr_ctrl;            /**< 目的端地址控制: 0-递增, 1-递减, 2-固定 */
           uint16_t interrupt_mask;          /**< 中断掩码 */
           uint32_t src_addr;                /**< 源地址 */
           uint32_t dst_addr;                /**< 目的地址 */
           uint32_t linked_ptr;              /**< 下一个链表描述符 */
           uint32_t size_in_byte;            /**< 传输总字节数 */
           bool en_infiniteloop;             /**< 无限循环传输。注意：仅 DMAV2 支持 */
           uint8_t handshake_opt;            /**< 握手选项。注意：仅 DMAV2 支持 */
           uint8_t burst_opt;                /**< 突发大小选项。注意：仅 DMAV2 支持 */
       } dma_mgr_chn_conf_t;

  - DMA 回调函数类型：

    .. code-block:: c

       typedef void (*dma_mgr_chn_cb_t)(DMA_Type *base, uint32_t channel, void *cb_data_ptr);

API 调用流程介绍
----------------

初始化 DMA Manager
~~~~~~~~~~~~~~~~~~

- 调用 ``dma_mgr_init`` API 初始化 DMA Manager 上下文。在进行任何 DMA 操作之前必须执行此步骤。

- ``dma_mgr_init`` API 原型：

    .. code-block:: c

       void dma_mgr_init(void);

请求 DMA 资源
~~~~~~~~~~~~~

- 调用 ``dma_mgr_request_resource`` API 请求一个可用的 DMA 通道。DMA Manager 会在所有 DMA 实例中自动查找未被占用的通道。

- ``dma_mgr_request_resource`` API 原型：

    .. code-block:: c

       hpm_stat_t dma_mgr_request_resource(dma_resource_t *resource);

  - 返回值：

    - ``status_success``：DMA 通道分配成功。

    - ``status_invalid_argument``：``resource`` 参数为 NULL。

    - ``status_dma_mgr_no_resource``：所有 DMA 通道已被占用。

- 如果需要从**指定**的 DMA 实例（如 HPM_HDMA 或 HPM_XDMA）请求通道，使用：

    .. code-block:: c

       hpm_stat_t dma_mgr_request_specified_resource(dma_resource_t *resource, DMA_Type *base);

- **举例**： 请求一个 DMA 通道。

    .. code-block:: c

       dma_resource_t resource;

       if (dma_mgr_request_resource(&resource) != status_success) {
           printf("Failed to request DMA resource\n");
           return;
       }
       printf("DMA channel allocated: instance=%p, channel=%d\n", resource.base, resource.channel);

获取缺省通道配置
~~~~~~~~~~~~~~~~

- 调用 ``dma_mgr_get_default_chn_config`` API 为 ``dma_mgr_chn_conf_t`` 结构体填充默认值，之后可根据需要修改各个字段。

- ``dma_mgr_get_default_chn_config`` API 原型：

    .. code-block:: c

       void dma_mgr_get_default_chn_config(dma_mgr_chn_conf_t *config);

  - 默认值：

    - DMAMUX 关闭，DMAMUX 源 = 0

    - 低优先级

    - 源端突发大小 = 1 次传输

    - 源端/目的端工作模式 = 普通模式

    - 源端/目的端位宽 = 字节

    - 源端/目的端地址控制 = 递增

    - 源端/目的端地址 = 0

    - 传输大小 = 0，链表指针 = 0

    - 所有中断屏蔽

    - 无限循环关闭，突发选项 = 标准大小，握手选项 = 单次突发

配置 DMA 通道
~~~~~~~~~~~~~

- 调用 ``dma_mgr_setup_channel`` API 将配置一次性写入 DMA 通道的硬件寄存器，并可选配置 DMAMUX。

- ``dma_mgr_setup_channel`` API 原型：

    .. code-block:: c

       hpm_stat_t dma_mgr_setup_channel(const dma_resource_t *resource, dma_mgr_chn_conf_t *config);

  - 返回值：

    - ``status_success``：通道配置成功。

    - ``status_invalid_argument``：无效参数。

注册 DMA 传输完成回调
~~~~~~~~~~~~~~~~~~~~~

- 调用 ``dma_mgr_install_chn_tc_callback`` API 为 DMA 通道注册传输完成回调函数，该回调在 DMA 中断服务程序中调用。

- ``dma_mgr_install_chn_tc_callback`` API 原型：

    .. code-block:: c

       hpm_stat_t dma_mgr_install_chn_tc_callback(const dma_resource_t *resource, dma_mgr_chn_cb_t callback, void *user_data);

  - 返回值：

    - ``status_success``：回调注册成功。

    - ``status_invalid_argument``：无效参数。

- 类似地，还可以注册半传输完成、错误和终止回调：

    .. code-block:: c

       hpm_stat_t dma_mgr_install_chn_half_tc_callback(const dma_resource_t *resource, dma_mgr_chn_cb_t callback, void *user_data);

       hpm_stat_t dma_mgr_install_chn_error_callback(const dma_resource_t *resource, dma_mgr_chn_cb_t callback, void *user_data);

       hpm_stat_t dma_mgr_install_chn_abort_callback(const dma_resource_t *resource, dma_mgr_chn_cb_t callback, void *user_data);

使能 DMA 通道并启动传输
~~~~~~~~~~~~~~~~~~~~~~~

- 调用 ``dma_mgr_enable_chn_irq`` API 为通道使能所需的中断源。

- ``dma_mgr_enable_chn_irq`` API 原型：

    .. code-block:: c

       hpm_stat_t dma_mgr_enable_chn_irq(const dma_resource_t *resource, uint32_t irq_mask);

  - ``irq_mask`` 可为 ``DMA_MGR_INTERRUPT_MASK_TC``、``DMA_MGR_INTERRUPT_MASK_HALF_TC``、``DMA_MGR_INTERRUPT_MASK_ERROR``、``DMA_MGR_INTERRUPT_MASK_ABORT`` 的组合。

- 调用 ``dma_mgr_enable_dma_irq_with_priority`` API 使能全局 DMA 中断并设置优先级。

- ``dma_mgr_enable_dma_irq_with_priority`` API 原型：

    .. code-block:: c

       hpm_stat_t dma_mgr_enable_dma_irq_with_priority(const dma_resource_t *resource, uint32_t priority);

- 调用 ``dma_mgr_enable_channel`` API 启动 DMA 传输。

- ``dma_mgr_enable_channel`` API 原型：

    .. code-block:: c

       hpm_stat_t dma_mgr_enable_channel(const dma_resource_t *resource);

  - 返回值：

    - ``status_success``：通道使能成功，传输已启动。

    - ``status_invalid_argument``：无效参数。

完整 DMA 传输示例
~~~~~~~~~~~~~~~~~

以下是一个使用 DMA Manager 组件的典型 DMA 传输流程：

.. code-block:: c

    #include "hpm_dma_mgr.h"
    #include "hpm_uart_drv.h"

    ATTR_PLACE_AT_NONCACHEABLE static char tx_buff[256];
    static volatile bool tx_dma_complete;

    /* DMA 传输完成回调 */
    static void tx_dma_complete_cb(DMA_Type *base, uint32_t channel, void *user_data)
    {
        (void)base;
        (void)channel;
        (void)user_data;
        tx_dma_complete = true;
    }

    void uart_dma_send_example(void)
    {
        dma_mgr_chn_conf_t chn_config;
        dma_resource_t tx_dma_resource;

        /* 1. 初始化 DMA Manager */
        dma_mgr_init();

        /* 2. 请求一个 DMA 通道 */
        if (dma_mgr_request_resource(&tx_dma_resource) != status_success) {
            printf("Failed to request DMA resource\n");
            return;
        }

        /* 3. 获取缺省通道配置 */
        dma_mgr_get_default_chn_config(&chn_config);

        /* 4. 配置 DMA 通道用于 UART 发送 */
        chn_config.src_width = DMA_MGR_TRANSFER_WIDTH_BYTE;
        chn_config.dst_width = DMA_MGR_TRANSFER_WIDTH_BYTE;
        chn_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)tx_buff);
        chn_config.dst_addr = (uint32_t)&HPM_UART0->DATA;
        chn_config.src_mode = DMA_MGR_HANDSHAKE_MODE_NORMAL;
        chn_config.dst_mode = DMA_MGR_HANDSHAKE_MODE_HANDSHAKE;
        chn_config.src_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_INCREMENT;
        chn_config.dst_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_FIXED;
        chn_config.size_in_byte = sizeof(tx_buff);
        chn_config.en_dmamux = true;
        chn_config.dmamux_src = HPM_DMA_SRC_UART0_TX;
        chn_config.interrupt_mask = DMA_MGR_INTERRUPT_MASK_NONE;

        dma_mgr_setup_channel(&tx_dma_resource, &chn_config);

        /* 5. 注册传输完成回调 */
        dma_mgr_install_chn_tc_callback(&tx_dma_resource, tx_dma_complete_cb, NULL);

        /* 6. 使能通道中断和全局 DMA 中断 */
        dma_mgr_enable_chn_irq(&tx_dma_resource, DMA_MGR_INTERRUPT_MASK_TC);
        dma_mgr_enable_dma_irq_with_priority(&tx_dma_resource, 1);

        /* 7. 启动 DMA 传输 */
        dma_mgr_enable_channel(&tx_dma_resource);

        /* 8. 等待传输完成 */
        while (!tx_dma_complete) {
        }
        printf("UART DMA transfer complete\n");
    }

通道控制 API
~~~~~~~~~~~~

- **禁能通道**: 停止 DMA 通道传输。

    .. code-block:: c

       hpm_stat_t dma_mgr_disable_channel(const dma_resource_t *resource);

- **终止传输**: 强制终止 DMA 通道传输。

    .. code-block:: c

       hpm_stat_t dma_mgr_abort_chn_transfer(const dma_resource_t *resource);

- **查询传输状态**: 查询 DMA 通道的当前状态。

    .. code-block:: c

       hpm_stat_t dma_mgr_check_chn_transfer_status(const dma_resource_t *resource, uint32_t *status);

- **获取剩余传输字节**: 查询 DMA 通道尚未传输的字节数。

    .. code-block:: c

       hpm_stat_t dma_mgr_get_chn_remaining_transize(const dma_resource_t *resource, uint32_t *size);

- **设置传输大小**: 修改当前通道的传输大小。

    .. code-block:: c

       hpm_stat_t dma_mgr_set_chn_transize(const dma_resource_t *resource, uint32_t size);

- **释放 DMA 资源**: 将 DMA 通道归还给管理器。

    .. code-block:: c

       hpm_stat_t dma_mgr_release_resource(const dma_resource_t *resource);

逐字段配置 API
~~~~~~~~~~~~~~

如果不想使用一次性配置 ``dma_mgr_setup_channel``，也可以对各字段逐一配置：

.. code-block:: c

    dma_mgr_set_chn_src_addr(&resource, src_addr);
    dma_mgr_set_chn_dst_addr(&resource, dst_addr);
    dma_mgr_set_chn_src_width(&resource, width);
    dma_mgr_set_chn_dst_width(&resource, width);
    dma_mgr_set_chn_src_work_mode(&resource, mode);
    dma_mgr_set_chn_dst_work_mode(&resource, mode);
    dma_mgr_set_chn_src_addr_ctrl(&resource, addr_ctrl);
    dma_mgr_set_chn_dst_addr_ctrl(&resource, addr_ctrl);
    dma_mgr_set_chn_src_burst_size(&resource, burstsize);
    dma_mgr_set_chn_transize(&resource, size);
    dma_mgr_set_chn_priority(&resource, priority);
    dma_mgr_set_chn_infinite_loop_mode(&resource, enable);
    dma_mgr_set_chn_handshake_option(&resource, handshake_opt);

.. note::

    - DMA Manager 是 SDK 中的基础组件，被以下上层组件使用：

        - ``components/spi``：SPI 组件通过 DMA Manager 管理 SPI DMA 传输。

        - ``components/i2c``：I2C 组件通过 DMA Manager 管理 I2C DMA 传输。

        - ``components/serial_nor``：Serial NOR 组件通过 DMA Manager 管理 Flash DMA 传输。

        - ``components/i2s_over_spi``：I2S over SPI 组件通过 DMA Manager 管理音频 DMA 传输。

