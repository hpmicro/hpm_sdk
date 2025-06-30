.. _uart_lin:

UART LIN
========

概述
----
UART LIN 组件提供了基于 UART 外设实现 LIN 协议的功能。该组件支持：

- 主从模式通信
- 增强型校验和模式
- 支持轮询和中断两种传输方式

功能特性
--------
1. 主机模式功能

   - 发送/接收 LIN 帧
   - 发送唤醒信号
   - 支持同步间隔控制
   - 支持增强型校验和
   - 支持最大 8 字节数据传输

2. 从机模式功能

   - 响应主机请求
   - 自动校验帧头格式
   - 支持增强型校验和
   - 支持最大 8 字节数据传输

3. 传输模式

   - 支持轮询方式传输
   - 支持中断方式传输
   - 支持 FIFO 触发等级配置

数据结构
--------

主要结构体
~~~~~~~~~~

1. uart_lin_master_config_t

   主机配置结构体，包含以下字段：

   - ``ptr``: UART 外设基地址
   - ``id``: LIN 帧 ID（6 位）
   - ``data``: LIN 数据配置（uart_lin_data_t 类型）
   - ``pin_ctrl``: 引脚控制配置（uart_lin_master_pin_ctrl_t 类型）

2. uart_lin_master_pin_ctrl_t

   主机引脚控制结构体，包含以下字段：

   - ``ptr``: GPIO 外设基地址
   - ``baudrate``: LIN 波特率
   - ``config_uart_pin``: UART 引脚配置函数
   - ``config_uart_pin_as_gpio``: UART 引脚配置为 GPIO 函数
   - ``delay_us``: 微秒延时函数
   - ``tx_port``: 发送引脚端口号
   - ``tx_pin``: 发送引脚编号

3. uart_lin_slave_config_t

   从机配置结构体，包含以下字段：

   - ``ptr``: UART 外设基地址
   - ``pid``: LIN 帧受保护 ID（8 位）
   - ``data``: LIN 数据配置（uart_lin_data_t 类型）

4. uart_lin_data_t

   数据传输配置结构体，包含以下字段：

   - ``buff``: 数据缓冲区指针
   - ``length``: 数据长度（最大 8 字节）
   - ``enhance_checksum``: 是否使用增强型校验和

API 说明
---------

主机模式 API
~~~~~~~~~~~~
1. 轮询传输接口（阻塞式）

   - ``hpm_uart_lin_master_send_frame``: 发送完整的 LIN 帧
     * 功能：发送同步间隔、同步字段、PID、数据和校验和
     * 阻塞等待发送完成
     * 适用场景：简单的数据发送，不需要中断处理

   - ``hpm_uart_lin_master_receive_frame``: 接收完整的 LIN 帧
     * 功能：发送帧头并接收数据和校验和
     * 阻塞等待接收完成
     * 适用场景：简单的数据接收，不需要中断处理

2. 非阻塞式传输接口

   - ``hpm_uart_lin_master_send_head``: 发送帧头
     * 功能：发送同步间隔、同步字段和 PID
     * 将数据写入FIFO后即返回

   - ``hpm_uart_lin_master_send_data``: 发送数据和校验和
     * 功能：将数据和校验和写入 UART FIFO
     * 将数据写入FIFO后即返回
     * 适用场景：需要在中断中处理发送完成事件

   - ``hpm_uart_lin_master_receive_data``: 接收并校验数据
     * 功能：从 UART FIFO 读取数据并校验
     * 适用场景：当RX FIFO接收完数据后，触发中断，在中断中调用该接口从FIFO中读取数据并校验

3. 其他功能接口

   - ``hpm_uart_lin_send_wakeup``: 发送唤醒信号（阻塞式）
   - ``hpm_uart_lin_calculate_protected_id``: 计算受保护的 ID

从机模式 API
~~~~~~~~~~~~
1. 数据传输接口（非阻塞式）

   - ``hpm_uart_lin_slave_send_data``: 发送数据和校验和
     * 功能：将数据和校验和写入 UART FIFO
     * 需要配合 UART TX FIFO 中断使用
     * 适用场景：在检测到主机请求后发送数据

   - ``hpm_uart_lin_slave_receive_data``: 接收并校验数据
     * 功能：从 UART FIFO 读取数据并校验
     * 需要配合 UART RX 超时中断使用
     * 适用场景：在接收到主机数据后进行处理

使用示例
--------

主机模式示例
~~~~~~~~~~~~
1. 轮询方式示例

.. code-block:: c

    /* 配置 UART LIN 主机 */
    uart_lin_master_config_t uart_lin_config = {
        .ptr = TEST_UART,
        .pin_ctrl = {
            .ptr = HPM_GPIO0,
            .baudrate = TEST_UART_LIN_BAUDRATE,
            .config_uart_pin = init_uart_pins,
            .config_uart_pin_as_gpio = init_uart_pin_as_gpio,
            .delay_us = board_delay_us,
            .tx_port = BOARD_UART_LIN_TX_PORT,
            .tx_pin = BOARD_UART_LIN_TX_PIN,
        },
    };

    /* 发送数据（阻塞式） */
    uart_lin_config.id = TEST_MASTER_SEND_ID;
    uart_lin_config.data.buff = send_data;
    uart_lin_config.data.length = TEST_MASTER_SEND_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_SEND_ENHANCE_CHECKSUM;
    hpm_uart_lin_master_send_frame(&uart_lin_config);

2. 中断方式示例

.. code-block:: c

    /* 在主循环中发起传输 */
    uart_lin_config.id = TEST_MASTER_SEND_ID;
    uart_lin_config.data.buff = send_data;
    uart_lin_config.data.length = TEST_MASTER_SEND_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_SEND_ENHANCE_CHECKSUM;
    hpm_uart_lin_master_send_head(&uart_lin_config);
    hpm_uart_lin_master_send_data(&uart_lin_config);

    /* 在中断处理函数中 */
    void uart_isr(void)
    {
        volatile uint8_t irq_id = uart_get_irq_id(TEST_UART);
        if (irq_id == uart_intr_id_tx_slot_avail) {
            uart_lin_send_complete = true;
            uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
        }

        if (irq_id == uart_intr_id_rx_timeout) {
            if (uart_lin_success == hpm_uart_lin_master_receive_data(&uart_lin_config)) {
                uart_lin_receive_complete = true;
            }
            uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        }
    }

从机模式示例
~~~~~~~~~~~~
以下是使用中断方式接收和发送数据的示例：

.. code-block:: c

    /* 配置 UART LIN 从机 */
    uart_lin_slave_config_t uart_lin_config = {
        .ptr = TEST_UART,
    };

    /* 配置 UART FIFO 控制 */
    uart_fifo_ctrl_t fifo_ctrl = {
        .tx_fifo_level = uart_tx_fifo_trg_not_full,
        .rx_fifo_level = uart_rx_fifo_trg_not_empty,
        .dma_enable = false,
        .fifo_enable = true,
    };

    /* 在中断中处理接收到的帧头 */
    void uart_lin_check_head_and_respond(UART_Type *ptr, uint8_t *head_buff)
    {
        uint8_t pid = head_buff[1];
        uint8_t id = pid & 0x3f;

        /* 检查同步字段和 PID */
        if (0x55 != head_buff[0] ||
            pid != hpm_uart_lin_calculate_protected_id(id)) {
            return;
        }

        switch (id) {
        case TEST_SLAVE_RECEIVE_ID:
            /* 配置接收参数 */
            uart_lin_config.pid = pid;
            uart_lin_config.data.buff = receive_data;
            uart_lin_config.data.length = TEST_SLAVE_RECEIVE_DATA_LENGTH;
            uart_lin_config.data.enhance_checksum = TEST_SLAVE_RECEIVE_ENHANCE_CHECKSUM;
            break;
        case TEST_SLAVE_SEND_ID:
            /* 配置发送参数并发送数据 */
            uart_lin_config.pid = pid;
            uart_lin_config.data.buff = send_data;
            uart_lin_config.data.length = TEST_SLAVE_SEND_DATA_LENGTH;
            uart_lin_config.data.enhance_checksum = TEST_SLAVE_SEND_ENHANCE_CHECKSUM;
            hpm_uart_lin_slave_send_data(&uart_lin_config);
            uart_enable_irq(ptr, uart_intr_tx_slot_avail);
            break;
        }
    }

    /* 在中断处理函数中 */
    void uart_isr(void)
    {
        volatile uint8_t irq_id = uart_get_irq_id(TEST_UART);

        /* 检测到 LIN break 信号 */
        if (irq_id == uart_intr_id_rx_line_stat) {
            if (uart_check_status(TEST_UART, uart_stat_line_break)) {
                uart_clear_rx_fifo(TEST_UART);
                head_index = 0;
                uart_config_fifo_ctrl(TEST_UART, &fifo_ctrl);
                uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
            }
        }

        /* 接收并处理帧头 */
        if (irq_id == uart_intr_id_rx_data_avail) {
            if (uart_check_status(TEST_UART, uart_stat_data_ready)) {
                head_buff[head_index++] = uart_read_byte(TEST_UART);
            }

            if (head_index == 2) {
                uart_lin_check_head_and_respond(TEST_UART, head_buff);
            }
        }

        /* 发送完成处理 */
        if (irq_id == uart_intr_id_tx_slot_avail) {
            uart_lin_send_complete = true;
            uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
        }

        /* 接收完成处理 */
        if (irq_id == uart_intr_id_rx_timeout) {
            if (uart_lin_success == hpm_uart_lin_slave_receive_data(&uart_lin_config)) {
                uart_lin_receive_complete = true;
            }
            uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        }
    }

