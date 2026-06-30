.. _can:

CAN
======

概述
------

CAN 例程按照与 MCAN 例程一致的方式拆分为公共框架和分功能源码文件，提供如下功能：

- SoC支持的所有CAN外设的回环测试，主要集中在CAN和CAN-FD测试

- 开发板所支持的CAN外设的回环测试，基于中断模式

- 双板之间的回声（Echo）测试

- 用于检测CAN总线连接性的多包发送

- CAN的错误相关的检测

- CAN过滤器

- CAN时间戳功能演示

源码组织
------------

- ``src/can_app_common.c`` 和 ``src/can_app_common.h`` 保存公共入口、菜单、CAN 实例表、消息显示辅助函数以及 ISR 回调分发逻辑。

- ``src/can_loopback_demo.c`` 保存所有 CAN 实例的阻塞式回环测试，以及开发板 CAN 实例的中断模式回环测试。

- ``src/can_echo_demo.c`` 保存双板 Echo 发起端和响应端流程。

- ``src/can_txrx_demo.c`` 保存经典 CAN 和 CAN FD 的批量发送演示，用于总线连通性检查。

- ``src/can_error_demo.c`` 保存错误检测演示，以及关闭自动重传后的错误场景演示。

- ``src/can_filter_demo.c`` 保存验收过滤器测试。

- ``src/can_timestamp_demo.c`` 保存基于 PTPC 的经典 CAN 和 CAN FD 时间戳演示。

硬件设置
------------

无特殊设置

运行现象
------------

当该例程成功运行后，会在串口终端打印如下菜单:


.. code-block:: console

   *********************************************************************************
   *                                                                               *
   *                         CAN Example Menu                                      *
   *                                                                               *
   * 0 - Run loopback test for all supported CAN controllers (CAN and CANFD)       *
   * 1 - Run loopback test for board supported CAN controller (interrupt mode)     *
   * 2 - Echo test between two boards:initiator                                    *
   * 3 - Echo test between two boards:responder                                    *
   * 4 - Send mulitple messages for transmission check                             *
   * 5 - CAN error test (Need to remove current node from CAN BUS for this test)   *
   * 6 - CAN filter test                                                           *
   * 7 - Send multiple CANFD messages for transmission check                       *
   * 8 - CAN error test with re-transmission disabled                              *
   * 9 - CAN timestamp feature demo                                                *
   *                                                                               *
   *********************************************************************************


注意事项
~~~~~~~~~~~~

- 对于菜单选项 0 或 1, 用户可以直接在串口终端输入0 或 1 来启

- 对于菜单选项 2 和 3， 用户必须连接两块开发板，并在两个开发板上同时运行该例程。然后先启动响应者（responder），再启动发起者（initiator）

- 对于菜单选项 4, 用户需确认开发板挂载到有效的CAN总线上。 如果开发板连接在CAN分析仪上，用户可以通过CAN分析仪观测到开发板在运行该选项后发送了多包数据

- 对于菜单选项 7, 用户需确认开发板挂载到有效的CAN总线上且总线支持5Mbits/s的传输速度。 如果开发板连接在CAN分析仪上，用户可以通过CAN分析仪观测到开发板在运行该选项后发送了多包数据

- 针对驱动回归和运行时覆盖，请使用 ``unit_test/drivers/can`` 目标。该单元测试覆盖了辅助 API、回环、CAN FD、时间戳以及边界场景。
