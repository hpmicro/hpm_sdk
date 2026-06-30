.. _dma_mgr_component:

DMA Manager Component
=====================

Overview
--------

- The DMA Manager component provides a unified resource management layer on top of the low-level DMA driver, responsible for the allocation, configuration, and lifecycle management of DMA channels. It supports multiple DMA instances (HDMA, XDMA, etc.) and offers interrupt callback registration and channel control capabilities.
  The following are the key features of this component:

  - Centralized DMA channel resource management

    - Automatically allocates and releases DMA channels, avoiding manual management and conflicts.

    - Supports requesting a channel from a specified DMA instance.

  - Unified channel configuration interface

    - Encapsulates DMA channel configuration (source/destination address, transfer width, burst size, work mode, etc.) through the ``dma_mgr_chn_conf_t`` structure, providing a one-time setup API ``dma_mgr_setup_channel``.

  - Flexible interrupt callback mechanism

    - Supports separate registration of transfer complete (TC), half transfer complete (Half TC), error, and abort callbacks.

  - Supports advanced DMAV2 features

    - Infinite loop transfer, custom burst size, handshake options, byte order swap, etc.

Related Enums and Structs
--------------------------

- Enum / Macro Introduction

  - DMA transfer width: Defines the data width per transfer.

    .. code-block:: c

       #define DMA_MGR_TRANSFER_WIDTH_BYTE        DMA_TRANSFER_WIDTH_BYTE        /* 8-bit */
       #define DMA_MGR_TRANSFER_WIDTH_HALF_WORD   DMA_TRANSFER_WIDTH_HALF_WORD   /* 16-bit */
       #define DMA_MGR_TRANSFER_WIDTH_WORD        DMA_TRANSFER_WIDTH_WORD        /* 32-bit */
       #define DMA_MGR_TRANSFER_WIDTH_DOUBLE_WORD DMA_TRANSFER_WIDTH_DOUBLE_WORD /* 64-bit */

  - DMA burst size per transfer:

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

  - DMA handshake mode: Normal (software trigger) or Handshake (hardware trigger).

    .. code-block:: c

       #define DMA_MGR_HANDSHAKE_MODE_NORMAL    DMA_HANDSHAKE_MODE_NORMAL
       #define DMA_MGR_HANDSHAKE_MODE_HANDSHAKE DMA_HANDSHAKE_MODE_HANDSHAKE

  - DMA address control mode: Increment, Decrement, or Fixed.

    .. code-block:: c

       #define DMA_MGR_ADDRESS_CONTROL_INCREMENT DMA_ADDRESS_CONTROL_INCREMENT
       #define DMA_MGR_ADDRESS_CONTROL_DECREMENT DMA_ADDRESS_CONTROL_DECREMENT
       #define DMA_MGR_ADDRESS_CONTROL_FIXED     DMA_ADDRESS_CONTROL_FIXED

  - DMA interrupt mask:

    .. code-block:: c

       #define DMA_MGR_INTERRUPT_MASK_NONE  DMA_INTERRUPT_MASK_NONE
       #define DMA_MGR_INTERRUPT_MASK_ERROR DMA_INTERRUPT_MASK_ERROR
       #define DMA_MGR_INTERRUPT_MASK_ABORT DMA_INTERRUPT_MASK_ABORT
       #define DMA_MGR_INTERRUPT_MASK_TC    DMA_INTERRUPT_MASK_TERMINAL_COUNT
       #define DMA_MGR_INTERRUPT_MASK_ALL   DMA_INTERRUPT_MASK_ALL

  - DMA channel status:

    .. code-block:: c

       #define DMA_MGR_CHANNEL_STATUS_ONGOING DMA_CHANNEL_STATUS_ONGOING
       #define DMA_MGR_CHANNEL_STATUS_ERROR   DMA_CHANNEL_STATUS_ERROR
       #define DMA_MGR_CHANNEL_STATUS_ABORT   DMA_CHANNEL_STATUS_ABORT
       #define DMA_MGR_CHANNEL_STATUS_TC      DMA_CHANNEL_STATUS_TC

  - DMA channel priority:

    .. code-block:: c

       #define DMA_MGR_CHANNEL_PRIORITY_LOW  DMA_CHANNEL_PRIORITY_LOW
       #define DMA_MGR_CHANNEL_PRIORITY_HIGH DMA_CHANNEL_PRIORITY_HIGH

- Struct Introduction

  - DMA Resource Structure: Encapsulates a DMA channel resource, including DMA instance, channel number, and IRQ number.

    .. code-block:: c

       typedef struct _dma_resource {
           DMA_Type *base;   /**< The DMA instance that the allocated channel belongs to */
           uint32_t channel; /**< Channel index */
           int32_t irq_num;  /**< DMA IRQ number */
       } dma_resource_t;

  - DMA Channel Configuration Structure: Used to configure DMA channel parameters, including source/destination address, transfer width, burst size, work mode, address control mode, interrupt mask, etc.

    .. code-block:: c

       typedef struct hpm_dma_mgr_chn_conf {
           bool en_dmamux;                   /**< DMAMUX enable */
           uint8_t dmamux_src;               /**< DMAMUX source */
           uint8_t priority;                 /**< Channel priority */
           uint8_t src_burst_size;           /**< Source burst size */
           uint8_t src_mode;                 /**< Source work mode: 0-Normal, 1-Handshake */
           uint8_t dst_mode;                 /**< Destination work mode: 0-Normal, 1-Handshake */
           uint8_t src_width;                /**< Source width */
           uint8_t dst_width;                /**< Destination width */
           uint8_t src_addr_ctrl;            /**< Source address control: 0-inc, 1-dec, 2-fix */
           uint8_t dst_addr_ctrl;            /**< Destination address control: 0-inc, 1-dec, 2-fix */
           uint16_t interrupt_mask;          /**< Interrupt mask */
           uint32_t src_addr;                /**< Source address */
           uint32_t dst_addr;                /**< Destination address */
           uint32_t linked_ptr;              /**< Next linked descriptor */
           uint32_t size_in_byte;            /**< Total size to be transferred in byte */
           bool en_infiniteloop;             /**< Infinite loop transfer. Attention: only DMAV2 support */
           uint8_t handshake_opt;            /**< Handshake option. Attention: only DMAV2 support */
           uint8_t burst_opt;                /**< Burst size option. Attention: only DMAV2 support */
       } dma_mgr_chn_conf_t;

  - DMA Callback Type:

    .. code-block:: c

       typedef void (*dma_mgr_chn_cb_t)(DMA_Type *base, uint32_t channel, void *cb_data_ptr);

API Call Flow Introduction
---------------------------

Initialize DMA Manager
~~~~~~~~~~~~~~~~~~~~~~

- Call the ``dma_mgr_init`` API to initialize the DMA manager context. This step must be performed before any DMA operations.

- Prototype of the ``dma_mgr_init`` API:

    .. code-block:: c

       void dma_mgr_init(void);

Request DMA Resource
~~~~~~~~~~~~~~~~~~~~~

- Call the ``dma_mgr_request_resource`` API to request an available DMA channel. The DMA manager automatically finds an unallocated channel among all DMA instances.

- Prototype of the ``dma_mgr_request_resource`` API:

    .. code-block:: c

       hpm_stat_t dma_mgr_request_resource(dma_resource_t *resource);

  - Return Values:

    - ``status_success``: DMA channel successfully allocated.

    - ``status_invalid_argument``: The ``resource`` parameter is NULL.

    - ``status_dma_mgr_no_resource``: All DMA channels are already occupied.

- If you need to request a DMA channel from a **specified** DMA instance (e.g., HPM_HDMA or HPM_XDMA), use:

    .. code-block:: c

       hpm_stat_t dma_mgr_request_specified_resource(dma_resource_t *resource, DMA_Type *base);

- **Example**: Request a DMA channel.

    .. code-block:: c

       dma_resource_t resource;

       if (dma_mgr_request_resource(&resource) != status_success) {
           printf("Failed to request DMA resource\n");
           return;
       }
       printf("DMA channel allocated: instance=%p, channel=%d\n", resource.base, resource.channel);

Get Default Channel Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Call the ``dma_mgr_get_default_chn_config`` API to fill the ``dma_mgr_chn_conf_t`` structure with default values. You can then modify individual fields as needed.

- Prototype of the ``dma_mgr_get_default_chn_config`` API:

    .. code-block:: c

       void dma_mgr_get_default_chn_config(dma_mgr_chn_conf_t *config);

  - Default values:

    - DMAMUX disabled, DMAMUX source = 0

    - Low priority

    - Source burst size = 1 transfer

    - Source/Destination work mode = Normal

    - Source/Destination width = Byte

    - Source/Destination address control = Increment

    - Source/Destination address = 0

    - Transfer size = 0, Linked pointer = 0

    - All interrupts masked

    - Infinite loop disabled, burst option = standard size, handshake option = one burst

Setup DMA Channel
~~~~~~~~~~~~~~~~~

- Call the ``dma_mgr_setup_channel`` API to apply the configuration to the DMA channel. This API writes all the configuration fields to the hardware registers and optionally configures DMAMUX.

- Prototype of the ``dma_mgr_setup_channel`` API:

    .. code-block:: c

       hpm_stat_t dma_mgr_setup_channel(const dma_resource_t *resource, dma_mgr_chn_conf_t *config);

  - Return Values:

    - ``status_success``: Channel successfully configured.

    - ``status_invalid_argument``: Invalid parameter.

Install DMA Transfer Complete Callback
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Call the ``dma_mgr_install_chn_tc_callback`` API to register a callback for DMA transfer completion. This callback is invoked in the DMA ISR when the transfer is complete.

- Prototype of the ``dma_mgr_install_chn_tc_callback`` API:

    .. code-block:: c

       hpm_stat_t dma_mgr_install_chn_tc_callback(const dma_resource_t *resource, dma_mgr_chn_cb_t callback, void *user_data);

  - Return Values:

    - ``status_success``: Callback successfully installed.

    - ``status_invalid_argument``: Invalid parameter.

- Similarly, you can also register callbacks for half transfer complete, error, and abort events:

    .. code-block:: c

       hpm_stat_t dma_mgr_install_chn_half_tc_callback(const dma_resource_t *resource, dma_mgr_chn_cb_t callback, void *user_data);

       hpm_stat_t dma_mgr_install_chn_error_callback(const dma_resource_t *resource, dma_mgr_chn_cb_t callback, void *user_data);

       hpm_stat_t dma_mgr_install_chn_abort_callback(const dma_resource_t *resource, dma_mgr_chn_cb_t callback, void *user_data);

Enable DMA Channel and Start Transfer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Call the ``dma_mgr_enable_chn_irq`` API to enable the required interrupt sources for the channel.

- Prototype of the ``dma_mgr_enable_chn_irq`` API:

    .. code-block:: c

       hpm_stat_t dma_mgr_enable_chn_irq(const dma_resource_t *resource, uint32_t irq_mask);

  - ``irq_mask`` can be a combination of ``DMA_MGR_INTERRUPT_MASK_TC``, ``DMA_MGR_INTERRUPT_MASK_HALF_TC``, ``DMA_MGR_INTERRUPT_MASK_ERROR``, ``DMA_MGR_INTERRUPT_MASK_ABORT``.

- Call the ``dma_mgr_enable_dma_irq_with_priority`` API to enable the global DMA interrupt with a specified priority.

- Prototype of the ``dma_mgr_enable_dma_irq_with_priority`` API:

    .. code-block:: c

       hpm_stat_t dma_mgr_enable_dma_irq_with_priority(const dma_resource_t *resource, uint32_t priority);

- Call the ``dma_mgr_enable_channel`` API to start the DMA transfer.

- Prototype of the ``dma_mgr_enable_channel`` API:

    .. code-block:: c

       hpm_stat_t dma_mgr_enable_channel(const dma_resource_t *resource);

  - Return Values:

    - ``status_success``: Channel successfully enabled and transfer started.

    - ``status_invalid_argument``: Invalid parameter.

Complete DMA Transfer Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following is a typical DMA transfer flow using the DMA Manager component:

.. code-block:: c

    #include "hpm_dma_mgr.h"
    #include "hpm_uart_drv.h"

    ATTR_PLACE_AT_NONCACHEABLE static char tx_buff[256];
    static volatile bool tx_dma_complete;

    /* DMA transfer complete callback */
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

        /* 1. Initialize DMA Manager */
        dma_mgr_init();

        /* 2. Request a DMA channel */
        if (dma_mgr_request_resource(&tx_dma_resource) != status_success) {
            printf("Failed to request DMA resource\n");
            return;
        }

        /* 3. Get default channel config */
        dma_mgr_get_default_chn_config(&chn_config);

        /* 4. Configure DMA channel for UART TX */
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

        /* 5. Install transfer complete callback */
        dma_mgr_install_chn_tc_callback(&tx_dma_resource, tx_dma_complete_cb, NULL);

        /* 6. Enable channel interrupt and global DMA interrupt */
        dma_mgr_enable_chn_irq(&tx_dma_resource, DMA_MGR_INTERRUPT_MASK_TC);
        dma_mgr_enable_dma_irq_with_priority(&tx_dma_resource, 1);

        /* 7. Start DMA transfer */
        dma_mgr_enable_channel(&tx_dma_resource);

        /* 8. Wait for transfer completion */
        while (!tx_dma_complete) {
        }
        printf("UART DMA transfer complete\n");
    }

Channel Control APIs
~~~~~~~~~~~~~~~~~~~~

- **Disable Channel**: Stop DMA channel transfer.

    .. code-block:: c

       hpm_stat_t dma_mgr_disable_channel(const dma_resource_t *resource);

- **Abort Transfer**: Forcefully abort a DMA channel transfer.

    .. code-block:: c

       hpm_stat_t dma_mgr_abort_chn_transfer(const dma_resource_t *resource);

- **Check Transfer Status**: Query the current status of a DMA channel.

    .. code-block:: c

       hpm_stat_t dma_mgr_check_chn_transfer_status(const dma_resource_t *resource, uint32_t *status);

- **Get Remaining Transfer Size**: Query the remaining bytes to be transferred.

    .. code-block:: c

       hpm_stat_t dma_mgr_get_chn_remaining_transize(const dma_resource_t *resource, uint32_t *size);

- **Set Transfer Size**: Modify the transfer size of a channel.

    .. code-block:: c

       hpm_stat_t dma_mgr_set_chn_transize(const dma_resource_t *resource, uint32_t size);

- **Release DMA Resource**: Release the DMA channel back to the manager.

    .. code-block:: c

       hpm_stat_t dma_mgr_release_resource(const dma_resource_t *resource);

Per-Field Configuration APIs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you do not wish to use the one-time ``dma_mgr_setup_channel``, you can also configure each field individually:

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

    - The DMA Manager is a foundational component in the SDK, used by the following higher-level components:

        - ``components/spi``: The SPI component manages SPI DMA transfers via DMA Manager.

        - ``components/i2c``: The I2C component manages I2C DMA transfers via DMA Manager.

        - ``components/serial_nor``: The Serial NOR component manages Flash DMA transfers via DMA Manager.

        - ``components/i2s_over_spi``: The I2S over SPI component manages audio DMA transfers via DMA Manager.

