.. _use_dma_to_receive_and_send_uart_data:

DMA接收和发送数据UART
============================

概述
------

该示例工程中，展示UART采用DMA的方式接收和发送数据的操作。

端口设置
------------

- 串口波特率设置为 ``115200bps`` ，``1个停止位`` ，``无奇偶校验位``

配置说明
------------

- 使能FIFO模式，设置FIFO阈值。使能DMA。通过 `uart_config_t` 结构体中的 `dma_enable` 和 `fifo_enable` 参数配置。

  - 当FIFO接收到数据大于FIFO阈值时，会触发一次DMA请求。
  - 当FIFO发送数据小于FIFO阈值时，会触发一次DMA请求。

  - 对于FIFO阈值，有两个bit宽度方式的FIFO阈值。通过 `uart_config_t` 结构体中的 `tx_fifo_level` 和 `rx_fifo_level` 参数配置。可查看  `hpm_uart_drv.h` 中的 `uart_fifo_trg_lvl_t` 的定义。

    - 2bit位宽的FIFO阈值，范围为0-3，表示的FIFO阈值是：

      - 发送FIFO阈值

        .. list-table::
                :header-rows: 1

                * - 枚举定义
                  - 16字节的FIFO深度
                  - 32字节的FIFO深度
                * - uart_tx_fifo_trg_not_full
                  - 不为满
                  - 不为满
                * - uart_tx_fifo_trg_lt_three_quarters
                  - 小于12字节
                  - 小于24字节
                * - uart_tx_fifo_trg_lt_half
                  - 小于8字节
                  - 小于16字节
                * - uart_tx_fifo_trg_lt_one_quarter
                  - 小于4字节
                  - 小于8字节

      - 接收FIFO阈值

        .. list-table::
                :header-rows: 1

                * - 枚举定义
                  - 16字节的FIFO深度
                  - 32字节的FIFO深度
                * - uart_rx_fifo_trg_not_empty
                  - 不为空
                  - 不为空
                * - uart_rx_fifo_trg_gt_one_quarter
                  - 大于3字节
                  - 大于7字节
                * - uart_rx_fifo_trg_gt_half
                  - 大于7字节
                  - 大于15字节
                * - uart_rx_fifo_trg_gt_three_quarters
                  - 大于13字节
                  - 大于27字节

      - 举例:

        - tx_fifo_level = uart_tx_fifo_trg_lt_half， 在16字节FIFO深度的SOC下，发送FIFO深度小于8字节时，会触发一次DMA请求。
        - rx_fifo_level = uart_rx_fifo_trg_gt_one_quarter， 在16字节FIFO深度的SOC下，接收FIFO深度大于3字节时，会触发一次DMA请求。

    - 4bit位宽的FIFO阈值，范围为0-31，表示的FIFO阈值是：1到32字节的FIFO阈值。该部分定义只能支持的SOC是有在 `hpm_soc_ip_feature.h` 定义 `HPM_IP_FEATURE_UART_FINE_FIFO_THRLD` 宏定义。

      - 举例:

        - tx_fifo_level = 15， 在32字节FIFO深度的SOC下，发送FIFO深度小于等于15字节时，会触发一次DMA请求。
        - rx_fifo_level = 27， 在32字节FIFO深度的SOC下，接收FIFO深度大于等于27字节时，会触发一次DMA请求。

- **注意**：

  - 当RX FIFO发生timeout时，即使FIFO的数据不足FIFO阈值，也会触发一次DMA请求。
  - 如果需要RX FIFO发生timeout时，FIFO的数据不足FIFO阈值，不触发DMA请求，可通过 `uart_disable_rx_timeout_trig_dma` API 关闭RX FIFO timeout时触发DMA请求。该功能只能支持的SOC是有在 `hpm_soc_ip_feature.h` 定义 `HPM_IP_FEATURE_UART_DISABLE_DMA_TIMEOUT` 宏定义。uart驱动默认是关闭RX FIFO timeout时触发DMA请求的。



注意
------

- 通过串口单次输入的字符串长度不应该超过程序中定义的TEST_BUFF_SIZE

运行现象
------------

当工程正确运行后，通过串口手动输入字符串，如 '1234567887654321'，则串口终端会收到如下信息：：

.. code-block:: console

   UART DMA example
   UART will send back received characters, echo every 16 bytes
   1234567887654321

