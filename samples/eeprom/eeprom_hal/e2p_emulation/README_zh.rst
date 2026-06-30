.. _eeprom_hal_emulation:

Eeprom hal emulation 功能测试
=============================

概述
----

本示例演示如何使用 EEPROM HAL 驱动接口访问 EEPROM 存储。默认使用 **e2p_emulation** 组件（基于 Flash 的 EEPROM 模拟），因此无需外接物理 EEPROM 硬件。同一套 HAL API 也可通过注册其他驱动用于物理 EEPROM 设备。

板级设置
--------

- 使用模拟后端时无需额外硬件（使用板载 NOR Flash）。

配置说明
--------

模拟相关参数在 `main.c` 中定义：

.. list-table::
   :widths: 32 60
   :header-rows: 1

   * - 宏
     - 说明
   * - BOARD_APP_EEPROM_EMU_ERASE_SIZE
     - Flash 擦除单元大小（默认 4096 字节）
   * - BOARD_APP_EEPROM_EMU_SECTOR_COUNT
     - 扇区数量，必须为偶数（默认 32）
   * - BOARD_APP_EEPROM_EMU_START_ADDR
     - 模拟区域起始地址（由 Flash 大小及上述参数推导）

模拟区域位于板载 NOR Flash 末尾（双半区设计，用于掉电保护）。

测试内容
--------

示例运行两类测试：

1. **基本 I/O（小块写入）**

   - 在固定地址（如 0x42）单字节写入与读取
   - 非对齐小块写入（地址 5 处 7 字节）并读回校验

2. **大块、跨页、非对齐 I/O**

   - 从非对齐地址 0xA 写入 150 字节
   - 验证跨页与防回卷行为
   - 读回并比较数据

测试均通过 HAL API：`hpm_eeprom_write_block()` 与 `hpm_eeprom_read_block()` 完成。

使用步骤
--------

1. 将模拟参数注册到设备结构体：

   .. code-block:: c

      e2p_emu_xpi_host_config_t emu_host = { .base = BOARD_APP_XPI_NOR_XPI_BASE };
      e2p_emu_xpi_attribute_config_t emu_config = {
          .flash_base_addr = BOARD_FLASH_BASE_ADDRESS,
          .version = 0x4553,
          .opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR,
          .opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0,
          .opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1,
          .start_addr = BOARD_APP_EEPROM_EMU_START_ADDR,
          .erase_size = BOARD_APP_EEPROM_EMU_ERASE_SIZE,
          .sector_cnt = BOARD_APP_EEPROM_EMU_SECTOR_COUNT,
      };
      hpm_eeprom_device_t dev = {
          .host = &emu_host,
          .flag = EEPROM_FLAG_EMULATION_XPI,
          .config = &emu_config,
      };
      e2p_emulation_xpi_register_param(&dev);

2. 向 HAL 注册设备并打开：

   .. code-block:: c

      hpm_eeprom_register(&dev, "e2p_test");
      hpm_eeprom_device_t *device = hpm_eeprom_find("e2p_test");
      hpm_eeprom_init(device);

3. 使用 HAL 读写 API：

   .. code-block:: c

      hpm_eeprom_write_block(device, addr, len, data);
      hpm_eeprom_read_block(device, addr, len, buf);

运行示例
--------

工程运行后，串口会输出初始化与测试结果。使用 e2p_emulation 组件时，初始化输出类似：

.. code-block:: console

   EEPROM HAL sample test start.

   ------------ flash->eeprom init ok -----------
   start address: 0x80fc0000
   sector count: 16
   flash erase granularity: 4096
   version: 0x4553
   end address: 0x80fd0000
   data write addr = 0x80fc013c, info write addr = 0x80fcff84, remain flash size = 0xfe48
   valid count percent info count( 3 / 6 )
   ----------------------------------------------

随后为测试输出：

.. code-block:: console

   --- Running Test: Basic I/O (Small Writes) ---
   Testing single byte write at a known-good address (e.g., 0x42)...
   [PASS] Single Byte I/O: Data verified successfully.
   Testing a small, non-aligned write...
   [PASS] Small Non-aligned Write: Data verified successfully.
   --- [PASS] Basic I/O (Small Writes) ---

   --- Running Test: Large Block, Multi-Page, Non-Aligned I/O ---
   Writing a large block (150 bytes) from a non-aligned address (0xA)...
   [PASS] Large Block Verification: Data verified successfully.
   --- [PASS] Large Block, Multi-Page, Non-Aligned I/O ---

   Result: ALL TESTS PASSED!
   EEPROM HAL sample test done.
