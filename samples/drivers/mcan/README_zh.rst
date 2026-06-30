.. _mcan:

MCAN
========

概述
------

本例程演示了 HPMicro SoC 上 MCAN（Bosch M_CAN）外设的一系列功能。所有演示
共享同一基础设施（``mcan_app_common.c/.h``），并通过交互式串口菜单呈现。
每个菜单选项均自包含：初始化 CAN 外设、运行测试、打印结果，然后返回菜单。

源文件结构
----------

例程按功能领域拆分为独立文件，每个演示均可单独阅读。

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 源文件
     - 说明
   * - ``mcan_app_common.c/.h``
     - 共享基础设施：``main()``、交互菜单、统一 ISR 分发器
       （``board_can_isr``）、CAN 实例表（``s_can_info[]``）、
       AHB-RAM 消息缓冲区辅助函数（``mcan_setup_msg_buf``），以及公共
       工具函数（``show_received_can_message``、``get_timestamp_hex_string``、
       ``handle_can_error``）。
   * - ``mcan_loopback_demo.c``
     - 内部回环测试：以轮询模式对所有 SoC CAN 实例（CAN 2.0 和 CAN-FD）
       逐一执行回环；同时提供基于中断的板载 CAN 回环测试。
   * - ``mcan_echo_demo.c``
     - 双板回声测试。一块开发板作为发起方（发送一帧），另一块作为响应方
       （用不同 ID 将载荷回送）。
   * - ``mcan_txrx_demo.c``
     - 批量发送：2048 帧经典 CAN 2.0，随后 2048 帧 CAN-FD（手动配置
       位定时，并可选启用发送延迟补偿 TDC）。
   * - ``mcan_filter_demo.c``
     - 六个过滤器测试用例，覆盖所有 MCAN 过滤器类型：经典过滤器（STD 和
       EXT ID）、范围过滤器、双 ID（指定 ID）过滤器。每个用例发送 2048
       帧并统计接受帧数。
   * - ``mcan_error_demo.c``
     - 错误处理：在 CAN 总线上无其他节点（无 ACK）的情况下，分别针对
       CAN 2.0B 和 CAN-FD 触发错误，并通过 ``handle_can_error()`` 解码并
       打印 PSR / ECR 寄存器状态。
   * - ``mcan_rxbuf_demo.c``
     - 专用接收缓冲区（RXBUF）演示。经典过滤器将 ID 0x123 的帧路由至
       RXBUF 索引 1，其余 ID 全部拒收。发送端使用专用 TX 缓冲区的非阻塞
       模式。
   * - ``mcan_tx_event_demo.c``
     - TX 事件 FIFO 演示。每帧携带 8 位消息标记；发送后硬件将 TX 事件
       元素回写，应用可据此将每帧发送结果与原始提交关联。
   * - ``mcan_timestamp_demo.c``
     - 三种时间戳捕获模式：32 位 TSU（时间戳单元）、64 位 TSU（扩展精度）
       和 MCAN 内部 16 位计数器。每种模式均打印 TX 事件时间戳与 RX 消息
       时间戳以便对比。
   * - ``mcan_timeout_demo.c``
     - 超时计数器功能：依次演示四种触发模式——连续（自由运行）、TX 事件
       FIFO 触发、RX FIFO0 触发、RX FIFO1 触发。每个用例完全自包含。
   * - ``mcan_txcancel_demo.c``
     - TX 取消：演示对 TX FIFO 条目和专用 TX 缓冲区条目的选择性取消。
       槽 0 正常完成，槽 1-3 被取消，并验证取消完成标志。
   * - ``mcan_txqueue_demo.c``
     - TX 队列模式（中断驱动，全异步）。CPU 尽可能快地向硬件队列填入 2048
       帧（ID 0x000-0x7FF）；队列满时忙等直至硬件释放空槽，无需与 TX 完成
       中断同步。ISR 并发接收回环回声。硬件按 CAN 优先级（ID 越小优先级越
       高）调度发送，与按提交顺序的 TX FIFO 模式不同。

硬件设置
--------

自包含测试（选项 0、1、6-d）无需特殊硬件设置，以下选项除外：

- **选项 2 / 3（回声测试）：** 将两块开发板连接至同一 CAN 总线。
- **选项 4 / 5（批量发送）：** 将开发板连接至 CAN 总线或 CAN 分析仪，
  以便观测发出的帧。
- **选项 6（错误测试）：** 将开发板 *从* CAN 总线断开（断开 CAN 收发器），
  以触发无 ACK 条件。

运行现象
--------

例程成功运行后，串口终端将打印如下菜单：

.. code-block:: console

   *******************************************************************************
   *                                                                             *
   *                         CAN Example Menu                                    *
   *                                                                             *
   * 0 - Run loopback test for all supported CAN controllers (CAN and CANFD)     *
   * 1 - Run loopback test for board supported CAN controller (interrupt mode)   *
   * 2 - Echo test between two board:initiator                                   *
   * 3 - Echo test between two board:responder                                   *
   * 4 - Send multiple classic CAN messages for transmission check               *
   * 5 - Send multiple CANFD messages for transmission check                     *
   * 6 - CAN error test (Need to remove current node from CAN BUS for this test) *
   * 7 - CAN filter test                                                         *
   * 8 - CAN RXBUF test                                                          *
   * 9 - CAN TX EVENT FIFO test                                                  *
   * a - CAN Timestamp Test                                                      *
   * b - CAN timeout counter Test                                                *
   * c - CAN TXBUF cancellation Test                                             *
   * d - CAN TX Queue test                                                       *
   *                                                                             *
   *******************************************************************************

菜单选项说明
~~~~~~~~~~~~

.. list-table::
   :widths: 8 18 74
   :header-rows: 1

   * - 按键
     - 名称
     - 功能说明 / 预期现象
   * - ``0``
     - 回环（全部 CAN）
     - 遍历 SoC 上所有 MCAN 实例，对每个实例以轮询模式分别执行一次
       CAN 2.0 回环和一次 CAN-FD 回环，打印每个实例的 PASSED / FAILED。
       无需外部连接。
   * - ``1``
     - 回环（中断模式）
     - 在 ``BOARD_APP_CAN_BASE`` 上以中断模式收发 2048 帧回环，
       逐帧打印接收到的 ID。无需外部连接。
   * - ``2``
     - 回声发起方
     - 以 500 kbps 正常模式初始化 CAN，发送一帧（STD ID 0x123，DLC=8），
       等待响应方返回的回声帧，打印回声载荷。
   * - ``3``
     - 回声响应方
     - 以 500 kbps 正常模式初始化 CAN，等待发起方的帧，打印帧内容，
       然后以 STD ID 0x321 将载荷回送。
   * - ``4``
     - 批量发送经典 CAN
     - 以 1 Mbps 正常模式发送 2048 帧 CAN 2.0（ID 0x000-0x7FF）。
       开发板须连接至 CAN 总线或分析仪。
   * - ``5``
     - 批量发送 CAN-FD
     - 以手动配置的位定时（仲裁段 1 Mbps / 数据段 2 Mbps）发送 2048
       帧 CAN-FD（DLC=15，64 字节）。开发板须连接至 CAN 总线或分析仪。
   * - ``6``
     - 错误测试
     - 在无其他节点的总线上尝试发送，触发无 ACK 错误。分别针对
       CAN 2.0B（500 kbps，DLC=8）和 CAN-FD（500 kbps / 2 Mbps，
       DLC=15）运行，打印 PSR 和 ECR 寄存器状态。
       **开发板须从 CAN 总线断开。**
   * - ``7``
     - 过滤器测试
     - 依次运行六个子用例，每次以不同过滤器类型重新初始化外设并发送
       2048 帧，验证期望的接受帧数：

       - 用例 0：经典过滤器，STD ID，掩码 bit[2:0] → 期望接受 256 帧
       - 用例 1：经典过滤器，精确 STD ID（16 个 ID）→ 期望接受 16 帧
       - 用例 2：范围过滤器，STD ID 0x000-0x00F → 期望接受 16 帧
       - 用例 3：双 ID 过滤器，STD ID 0x22 & 0x33 → 期望接受 2 帧
       - 用例 4：双 ID 过滤器，相同 ID，重复运行 → 期望接受 2 帧
       - 用例 5：经典过滤器，EXT ID，掩码 bit[2:0] → 期望接受 256 帧
   * - ``8``
     - RXBUF 测试
     - 配置经典过滤器将 STD ID 0x123 路由至专用 RX 缓冲区索引 1。
       发送 2048 帧，仅 ID 0x123 的帧应被存入缓冲区。打印接收到的 ID
       及 PASSED / FAILED。
   * - ``9``
     - TX 事件 FIFO
     - 通过 TX FIFO 以 ``event_fifo_control=1`` 和 8 位消息标记发送
       256 帧。每帧发送后读回 TX 事件元素并打印标记，以验证端到端关联。
   * - ``a``
     - 时间戳
     - 依次运行三种时间戳模式（32 位 TSU、64 位 TSU、内部 16 位计数器）。
       每种模式发送一帧回环并同时打印 TX 事件时间戳和 RX 消息时间戳
       以便对比。
   * - ``b``
     - 超时计数器
     - 依次运行四种超时触发模式：连续、TX 事件 FIFO 触发、RX FIFO0
       触发、RX FIFO1 触发。每种模式打印 PASSED / FAILED。
   * - ``c``
     - TX 取消
     - 向 TX FIFO 槽 0-3 提交四帧后立即取消槽 1-3；随后对专用 TX
       缓冲区 0-3 重复相同操作。每个场景均打印 PASSED（若三次取消均
       完成）。
   * - ``d``
     - TX 队列
     - 启用 TX 队列模式（硬件按优先级调度）。尽可能快地向队列填入
       2048 帧（ID 0x000-0x7FF）；ISR 异步接收回环回声。验证全部
       2048 个 ID 均被回送后打印 PASSED / FAILED。
