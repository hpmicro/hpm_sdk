.. _mhd_wifi_demo:

mhd_wifi_demo
==========================

概述
------

本示例展示基于AP6256 Wi-Fi 模组的 Wi-Fi示例

本示例支持如下命令:
- wifi 命令:

  - wifi scan: 扫描 AP 列表

  - wifi scanresults: 打印出扫描好的AP列表

  - wifi sta_start <SSID> <PASSWORD> : 连接指定的 AP

  - wifi sta_stop : 从连接好的AP断开

- 网络相关的命令:

  - iperf: 测试 iperf 的性能

  - ping

  - start_httpserver: 启动 http server

- 其他命令：

  - reset: 复位开发板

硬件设置
------------

- `AP6255_EVB` 开发板设置

  - 根据需要选择 `AP6255_EVB` 开发板的电压（若开发板SD卡支持1.8V电压，则选择1.8V，否则选择3.3V）

  - 将 `WL_REG_ON` 连接到 `board.h` 中由 `BOARD_APP_SDIO_WIFI_WL_REG_ON_PIN` 定义的引脚

  - 将 `WL_HWAKE` 连接到 `board.h` 中由 `BOARD_APP_SDIO_WIFI_OOB_PIN` 定义的引脚

  - 为 `AP6255_EVB` 供电 （可将该板上的USB口连接到USB充电器或电脑上的USB口）

- 开发板设置

  - 将 `AP6255_EVB` 连接到开发板的SD卡槽（需要 SD转TF转接板）

  - 将 开发板上的 `DEBUG` USB口连接到电脑上

工程配置
------------

**限制** :

- ``nds-gcc`` 工具链版本必须为v5.40或更高版本

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
     [WiFi] DHCP CLIENT hostname MHD_WIFI
     [WiFi] WL_REG_ON Down
     [WiFi] WL_REG_ON Up$
     [WiFi] SDIO Base F1134000
     [WiFi] SET F2 blksz 512
     [WiFi] chip_id 4345, chip_rev 9
     [WiFi] Wlan RAM size: c8000
     [WiFi] Time for WiFi FW/NV download: 36 ms
     [WiFi] HT CLK Activated [26]
     [WiFi] SR Enabled
     [WiFi] Wlan Bus Up
     [WiFi] MHD priority[50] stack[0x0] stack_size[4096]
     [WiFi] Turn on Polling mode [1]
     [WiFi] Started MHD Thread
     [WiFi] Turn off TX Glomming
     [WiFi] Turn on APSTA
     [WiFi] Turn on AMPDU TX
     [WiFi] Set country code US
     [WiFi] Set wl UP
     [WiFi] Set GMode
     [WiFi] Chip: 4345/9
     [WiFi] MAC: 08:E9:F6:0E:70:2A
     [WiFi] wl0: Nov 28 2024 08:43:46 version 7.45.96.219 (g27a577ef) FWID 01-e6290e47
     [WiFi] CLM: API: 12.2 Data: 1.0.1 Compiler: 1.29.4 ClmImport: 1.39.2 Creation: 2022-08-16 11:23:10
     [WiFi] MHD: Version 1.0.11 B003 Dec  5 2024 14:11:04

* 串口终端输入`help` 来查询支持的所有命令

* 典型应用场景


.. code-block:: console

      wifi scan
      wifi scanresults
      wifi sta_start <SSID> <PASSWORD>
      iperf -c <ip addr>
      ping <ip address / domain name>

