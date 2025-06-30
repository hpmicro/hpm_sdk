.. _uart_lin:

UART LIN
========

Overview
--------
The UART LIN component provides LIN protocol functionality based on the UART peripheral. This component supports:

- Master-slave communication
- Enhanced checksum mode
- Both polling and interrupt transfer modes

Features
--------
1. Master Mode Features

   - Send/Receive LIN frames
   - Send wakeup signal
   - Support sync interval control
   - Support enhanced checksum
   - Support up to 8 bytes data transfer

2. Slave Mode Features

   - Respond to master requests
   - Automatic frame header validation
   - Support enhanced checksum
   - Support up to 8 bytes data transfer

3. Transfer Modes

   - Support polling transfer mode
   - Support interrupt transfer mode
   - Support FIFO trigger level configuration

Data Structures
----------------

Main Structures
~~~~~~~~~~~~~~~~

1. uart_lin_master_config_t

   Master configuration structure, containing the following fields:

   - ``ptr``: UART peripheral base address
   - ``id``: LIN frame ID (6 bits)
   - ``data``: LIN data configuration (uart_lin_data_t type)
   - ``pin_ctrl``: Pin control configuration (uart_lin_master_pin_ctrl_t type)

2. uart_lin_master_pin_ctrl_t

   Master pin control structure, containing the following fields:

   - ``ptr``: GPIO peripheral base address
   - ``baudrate``: LIN baudrate
   - ``config_uart_pin``: UART pin configuration function
   - ``config_uart_pin_as_gpio``: Function to configure UART pin as GPIO
   - ``delay_us``: Microsecond delay function
   - ``tx_port``: Transmit pin port number
   - ``tx_pin``: Transmit pin number

3. uart_lin_slave_config_t

   Slave configuration structure, containing the following fields:

   - ``ptr``: UART peripheral base address
   - ``pid``: LIN frame protected ID (8 bits)
   - ``data``: LIN data configuration (uart_lin_data_t type)

4. uart_lin_data_t

   Data transfer configuration structure, containing the following fields:

   - ``buff``: Data buffer pointer
   - ``length``: Data length (maximum 8 bytes)
   - ``enhance_checksum``: Whether to use enhanced checksum

API Description
----------------

Master Mode APIs
~~~~~~~~~~~~~~~~~
1. Polling Transfer APIs (Blocking)

   - ``hpm_uart_lin_master_send_frame``: Send complete LIN frame
     * Function: Send sync break, sync field, PID, data, and checksum
     * Blocks until transmission is complete
     * Use case: Simple data transmission without interrupt handling

   - ``hpm_uart_lin_master_receive_frame``: Receive complete LIN frame
     * Function: Send frame header and receive data and checksum
     * Blocks until reception is complete
     * Use case: Simple data reception without interrupt handling

2. Non-blocking Transfer APIs

   - ``hpm_uart_lin_master_send_head``: Send frame header
     * Function: Send sync break, sync field, and PID
     * Returns after writing data to FIFO

   - ``hpm_uart_lin_master_send_data``: Send data and checksum
     * Function: Write data and checksum to UART FIFO
     * Returns after writing to FIFO
     * Use case: When transmission complete handling is needed in interrupt

   - ``hpm_uart_lin_master_receive_data``: Receive and verify data
     * Function: Read data from UART FIFO and verify
     * Use case: When RX FIFO receives data, trigger interrupt to read and verify data from FIFO

3. Other Function APIs

   - ``hpm_uart_lin_send_wakeup``: Send wakeup signal (blocking)
   - ``hpm_uart_lin_calculate_protected_id``: Calculate protected ID

Slave Mode APIs
~~~~~~~~~~~~~~~~
1. Data Transfer APIs (Non-blocking)

   - ``hpm_uart_lin_slave_send_data``: Send data and checksum
     * Function: Write data and checksum to UART FIFO
     * Must be used with UART TX FIFO interrupt
     * Use case: Send data after detecting master request

   - ``hpm_uart_lin_slave_receive_data``: Receive and verify data
     * Function: Read data from UART FIFO and verify
     * Must be used with UART RX timeout interrupt
     * Use case: Process data after receiving from master

Usage Examples
---------------

Master Mode Examples
~~~~~~~~~~~~~~~~~~~~~
1. Polling Mode Example

.. code-block:: c

    /* Configure UART LIN master */
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

    /* Send data (blocking) */
    uart_lin_config.id = TEST_MASTER_SEND_ID;
    uart_lin_config.data.buff = send_data;
    uart_lin_config.data.length = TEST_MASTER_SEND_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_SEND_ENHANCE_CHECKSUM;
    hpm_uart_lin_master_send_frame(&uart_lin_config);

2. Interrupt Mode Example

.. code-block:: c

    /* Initiate transfer in main loop */
    uart_lin_config.id = TEST_MASTER_SEND_ID;
    uart_lin_config.data.buff = send_data;
    uart_lin_config.data.length = TEST_MASTER_SEND_DATA_LENGTH;
    uart_lin_config.data.enhance_checksum = TEST_MASTER_SEND_ENHANCE_CHECKSUM;
    hpm_uart_lin_master_send_head(&uart_lin_config);
    hpm_uart_lin_master_send_data(&uart_lin_config);

    /* In interrupt handler */
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

Slave Mode Example
~~~~~~~~~~~~~~~~~~
Here's an example using interrupt mode for receiving and sending data:

.. code-block:: c

    /* Configure UART LIN slave */
    uart_lin_slave_config_t uart_lin_config = {
        .ptr = TEST_UART,
    };

    /* Configure UART FIFO control */
    uart_fifo_ctrl_t fifo_ctrl = {
        .tx_fifo_level = uart_tx_fifo_trg_not_full,
        .rx_fifo_level = uart_rx_fifo_trg_not_empty,
        .dma_enable = false,
        .fifo_enable = true,
    };

    /* Process received frame header in interrupt */
    void uart_lin_check_head_and_respond(UART_Type *ptr, uint8_t *head_buff)
    {
        uint8_t pid = head_buff[1];
        uint8_t id = pid & 0x3f;

        /* Check sync field and PID */
        if (0x55 != head_buff[0] ||
            pid != hpm_uart_lin_calculate_protected_id(id)) {
            return;
        }

        switch (id) {
        case TEST_SLAVE_RECEIVE_ID:
            /* Configure receive parameters */
            uart_lin_config.pid = pid;
            uart_lin_config.data.buff = receive_data;
            uart_lin_config.data.length = TEST_SLAVE_RECEIVE_DATA_LENGTH;
            uart_lin_config.data.enhance_checksum = TEST_SLAVE_RECEIVE_ENHANCE_CHECKSUM;
            break;
        case TEST_SLAVE_SEND_ID:
            /* Configure send parameters and send data */
            uart_lin_config.pid = pid;
            uart_lin_config.data.buff = send_data;
            uart_lin_config.data.length = TEST_SLAVE_SEND_DATA_LENGTH;
            uart_lin_config.data.enhance_checksum = TEST_SLAVE_SEND_ENHANCE_CHECKSUM;
            hpm_uart_lin_slave_send_data(&uart_lin_config);
            uart_enable_irq(ptr, uart_intr_tx_slot_avail);
            break;
        }
    }

    /* In interrupt handler */
    void uart_isr(void)
    {
        volatile uint8_t irq_id = uart_get_irq_id(TEST_UART);

        /* Detect LIN break signal */
        if (irq_id == uart_intr_id_rx_line_stat) {
            if (uart_check_status(TEST_UART, uart_stat_line_break)) {
                uart_clear_rx_fifo(TEST_UART);
                head_index = 0;
                uart_config_fifo_ctrl(TEST_UART, &fifo_ctrl);
                uart_enable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
            }
        }

        /* Receive and process frame header */
        if (irq_id == uart_intr_id_rx_data_avail) {
            if (uart_check_status(TEST_UART, uart_stat_data_ready)) {
                head_buff[head_index++] = uart_read_byte(TEST_UART);
            }

            if (head_index == 2) {
                uart_lin_check_head_and_respond(TEST_UART, head_buff);
            }
        }

        /* Handle transmission complete */
        if (irq_id == uart_intr_id_tx_slot_avail) {
            uart_lin_send_complete = true;
            uart_disable_irq(TEST_UART, uart_intr_tx_slot_avail);
        }

        /* Handle reception complete */
        if (irq_id == uart_intr_id_rx_timeout) {
            if (uart_lin_success == hpm_uart_lin_slave_receive_data(&uart_lin_config)) {
                uart_lin_receive_complete = true;
            }
            uart_disable_irq(TEST_UART, uart_intr_rx_data_avail_or_timeout);
        }
    }
