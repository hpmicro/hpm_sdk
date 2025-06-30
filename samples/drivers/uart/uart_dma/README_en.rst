.. _use_dma_to_receive_and_send_uart_data:

Use DMA to receive and send UART data
==========================================================================

Overview
--------

In this example project, UART receives/sends data via DMA.

Port Settings
-------------

- Serial port baud rate is set to ``115200bps`` , with ``one stop bit``  and ``no parity bit``

Configuration
-------------

- Enable FIFO mode and set FIFO thresholds. Enable DMA via `dma_enable` and `fifo_enable` in `uart_config_t` structure.

  - DMA request triggers when:
    - RX FIFO data exceeds threshold
    - TX FIFO free space exceeds threshold

  - Two FIFO threshold width modes (defined in `uart_fifo_trg_lvl_t` from `hpm_uart_drv.h`):

    - **2-bit width thresholds** (0-3):

      - TX FIFO Thresholds:

        .. list-table::
                :header-rows: 1

                * - Enum
                  - 16-byte FIFO
                  - 32-byte FIFO
                * - uart_tx_fifo_trg_not_full
                  - Not full
                  - Not full
                * - uart_tx_fifo_trg_lt_three_quarters
                  - <12 bytes
                  - <24 bytes
                * - uart_tx_fifo_trg_lt_half
                  - <8 bytes
                  - <16 bytes
                * - uart_tx_fifo_trg_lt_one_quarter
                  - <4 bytes
                  - <8 bytes

      - RX FIFO Thresholds:

        .. list-table::
                :header-rows: 1

                * - Enum
                  - 16-byte FIFO
                  - 32-byte FIFO
                * - uart_rx_fifo_trg_not_empty
                  - Not empty
                  - Not empty
                * - uart_rx_fifo_trg_gt_one_quarter
                  - >3 bytes
                  - >7 bytes
                * - uart_rx_fifo_trg_gt_half
                  - >7 bytes
                  - >15 bytes
                * - uart_rx_fifo_trg_gt_three_quarters
                  - >13 bytes
                  - >27 bytes

      - Examples:
        - `tx_fifo_level = uart_tx_fifo_trg_lt_half`: Triggers DMA when TX FIFO <8 bytes (16-byte FIFO)
        - `rx_fifo_level = uart_rx_fifo_trg_gt_one_quarter`: Triggers DMA when RX FIFO >3 bytes (16-byte FIFO)

    - **4-bit width thresholds** (0-31) for 1-32 byte granularity:
      - Requires `HPM_IP_FEATURE_UART_FINE_FIFO_THRLD` in `hpm_soc_ip_feature.h`

      - Examples:
        - `tx_fifo_level = 15`: Triggers DMA when TX FIFO <= 15 bytes (32-byte FIFO)
        - `rx_fifo_level = 27`: Triggers DMA when RX FIFO >= 27 bytes (32-byte FIFO)

- **Note**:

  - RX FIFO timeout will trigger DMA request even when FIFO data below threshold
  - To disable timeout-triggered DMA when below threshold:
    - Use `uart_disable_rx_timeout_trig_dma` API
    - Requires SOC support via `HPM_IP_FEATURE_UART_DISABLE_DMA_TIMEOUT` in hpm_soc_ip_feature.h
    - Default configuration: RX FIFO timeout DMA trigger disabled

Note
----

- The input string length should not exceed TEST_BUFF_SIZE defined in the program every time

Running the example
-------------------

When the project runs correctly, manually input the char through the serial port, such as '1234567887654321', and the serial port terminal will receive the following information:

.. code-block:: console

   UART DMA example
   UART will send back received characters, echo every 16 bytes
   1234567887654321

