.. _esp_hosted_wifi_demo:

esp_hosted_wifi_demo
==========================

概述
------

- Espressif 的esp-hosted MCU 方案允许主控 MCU 通过 SPI/SDIO 等接口与 ESP32/ESP32-C3 等 Wi-Fi 芯片通信，实现无线联网能力。目前适配仅支持SPI接口方式。
- 本示例使用 FreeRTOS 作为实时操作系统，LwIP 作为网络协议栈，通过 CherryShell 提供命令行交互界面。
- 该移植工程基于 FreeRTOS 和 LwIP 协议栈，支持常用的 Wi-Fi 操作命令和网络测试命令。
- 本实例验证使用Espressif的ESP32-WROVER-E核心板（搭载ESP32芯片）作为Wi-Fi模组，通过SPI接口与HPMicro主控板连接。

本工程主要特性：
- 支持 STA/AP/APSTA 模式切换
- 命令支持 Wi-Fi 扫描、连接、断开等操作
- 支持 DHCP、ping、iperf等网络功能
- 通过 CherryShell 提供命令行交互
- 适配 HPMicro SPI 驱动，支持 DMA 加速

硬件设置
------------

- 需要用到ESP-HOSTED的RESET/HANDSHAKE/DATA_READY引脚以及SPI引脚，引脚(请参考 :ref:`引脚描述 <board_resource>` 部分)

CMake 配置参数说明
------------------

本工程通过 CMake 构建，主要参数说明如下：

- ``CONFIG_FREERTOS``：启用 FreeRTOS 实时操作系统，必须使能。
- ``CONFIG_ESP_HOST_MCU``：启用 esp-hosted-mcu中间件
- ``CONFIG_ESP_HOSTED_SPI``：使用 SPI 作为主控与 ESP 芯片通信接口
- ``CONFIG_HPM_SPI``：启用 HPMicro 平台 SPI 组件
- ``CONFIG_DMA_MGR``：启用 DMA 管理器，提升 SPI 传输效率
- ``CONFIG_LWIP``：启用 LwIP 协议栈
- ``CONFIG_LWIP_DHCP``：启用 DHCP 客户端
- ``CONFIG_CHERRYSH``：启用 CherryShell 命令行
- ``CONFIG_CHERRYSH_INTERFACE``：命令行接口类型（如 uart）
- ``configTOTAL_HEAP_SIZE``：FreeRTOS 堆大小（如 61440 字节）
- ``CONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_PIN``：SPI 通信握手
- ``CONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_PIN``：数据就绪引脚
- ``CONFIG_ESP_HOSTED_SPI_CLOCK_FREQ_MHZ``：SPI 时钟频率，本示例设置为 10MHz，ESP32的SPI从机采样频率最大到10MHZ,请根据实际芯片能力调整.
- ``CONFIG_ESP_HOSTED_DFLT_TASK_STACK``: esp-hosted负责SPI通信的具体事务和负责处理接收到的数据的任务栈大小
- ``CONFIG_ESP_HOSTED_RPC_TASK_STACK_SIZE``：esp-hosted负责监听、接收来自远程过程调用（RPC）对端的数据包或消息，并进行处理、负责将本地需要通过 RPC 发送的数据包或消息，打包并发送到对端的线程栈大小
- ``DCONFIG_ESP_HOSTED_SPI_MODE``: SPI 模式选择，本示例使用模式 2（CPOL=1, CPHA=0）,需要与 ESP32 固件端保持一致。
- ``DCONFIG_SLAVE_IDF_TARGET_ESP32``：指定从机芯片类型为 ESP32。如需其他型号，比如 ESP32-C3，请修改为相应宏定义为DCONFIG_SLAVE_IDF_TARGET_ESP32C3。

详细参数可参考 ``CMakeLists.txt`` 文件。

运行示例
------------

* 编译下载程序

* 串口终端显示如下信息：

.. code-block:: console

        ----------------------------------------------------------------------
        $$\   $$\ $$$$$$$\  $$\      $$\ $$\
        $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
        $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
        $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
        $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
        $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
        $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
        \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
        ----------------------------------------------------------------------
        cherry@hpm5301evklite:/$ [I/H_API] I ESP-Hosted starting. Hosted_Tasks: prio:23, stack: 512 RPC_task_stack: 1024

        [I/H_API] I ** add_esp_wifi_remote_channels **

        [I/transport] I Add ESP-Hosted channel IF[1]: S[0] Tx[80057f10] Rx[8005f7ce]

        [I/transport] I Add ESP-Hosted channel IF[2]: S[0] Tx[8003c50c] Rx[8005f7ce]

        [I/spi_wrapper] I Transport: SPI, Mode:2 Freq:10MHz TxQ:2000 RxQ:2000
        GPIOs: CLK:0 MOSI:0 MISO:0 CS:0 HS:44 DR:45 SlaveReset:9

        [I/transport] I Attempt connection with slave: retry[0]

        [I/spi] I Resetting slave

        [I/spi] I Resetting slave on SPI bus with pin 9

        [I/spi] I Staring SPI task

        [I/transport] I Waiting for esp_hosted slave to be ready

        [I/transport] I Received INIT event from ESP32 peripheral

        [I/transport] I EVENT: 12

        [I/transport] I Identified slave [esp32]

        [I/transport] I EVENT: 11

        [I/transport] I capabilities: 0xf8

        [I/transport] I Features supported are:

        [I/transport] I            - HCI over SPI

        [I/transport] I            - BT/BLE dual mode

        [I/transport] I EVENT: 13

        [I/transport] I ESP board type is : 0


        [I/transport] I Base transport is set-up, TRANSPORT_TX_ACTIVE

        [I/H_API] I Transport active

        [I/transport] I Slave chip Id[12]

        [I/transport] I raw_tp_dir[-], flow_ctrl: low[0] high[0]

        [I/transport] I transport_delayed_init

        [I/spi] I Received INIT event

        [I/esp_wlan_ops] I AP started, SSID: hpm_ap_test

        [I/RPC_WRAP] I ESP Event: wifi station started

        [I/RPC_WRAP] I ESP Event: wifi station started

        [I/RPC_WRAP] I ESP Event: softap started

        [I/RPC_WRAP] I ESP Event: softap started

        [W/rpc_rsp] W Hosted RPC_Resp [0x21a], uid [7], resp code [12298]


命令行使用方法
----------------

上电后，串口终端会输出启动信息。输入 ``help`` 可查看所有支持的命令。常用命令如下：

- ``wifi scan``：扫描周围的 Wi-Fi AP
- ``wifi join <SSID> <PASSWORD>``：连接指定 Wi-Fi
- ``wifi stop``：断开当前 Wi-Fi 连接
- ``mem``：查看内存使用情况
- ``thread``：查看 FreeRTOS 任务信息
- ``iperf``：网络性能测试
- ``ping <ip/domain>``：网络连通性测试

典型使用流程
----------------

.. code-block:: console

   wifi scan
   wifi join MySSID MyPassword
   ping 192.168.1.1
   iperf -c 192.168.1.100

移植要点与注意事项
---------------------

- SPI 通信参数需与 ESP32 固件端保持一致（模式、时钟、引脚等）。
- Wi-Fi 模组需烧录支持 esp-hosted MCU 的slave固件。
- 若遇到连接异常、扫描失败等问题，请检查硬件连线和 SPI 配置。

参考资料
----------

- [Espressif esp-hosted 文档](https://components.espressif.com/components/espressif/esp_hosted/)

