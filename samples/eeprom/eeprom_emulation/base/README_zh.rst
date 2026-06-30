.. _eeprom_emulation_base_test:

EEPROM 模拟基础功能测试
========================

概述
----

本示例演示如何使用 EEPROM 模拟组件（eeprom_emulation）实现基本掉电保护数据存储。示例通过 NOR Flash 模拟 EEPROM 特性，支持数据的写入、读取、更新、删除和整理操作。

组件特点
--------

- **掉电保护**：双半区设计，任何掉电场景均可安全恢复
- **CRC32 校验**：读取时自动验证数据完整性
- **自动整理**：空间不足时自动触发整理，回收废弃数据
- **可移植性**：通过函数指针抽象 Flash 驱动，支持不同介质

测试内容
--------

本示例包含以下功能测试：

1. **写入测试**

   - 写入 4 个测试变量（var1 ~ var4），对应固定 block ID（HPMC、A、AB、ABC）
   - 通过 `e2p_write()` 写入字符串数据
   - 检查写入返回值

2. **读取测试**

   - 读出已写入的 4 个变量
   - 通过 `e2p_read()` 读取数据
   - 检查读取返回值并与写入数据比较

3. **更新测试**

   - 更新 var1 和 var2 的数据
   - 再次读取并验证更新结果
   - 验证旧数据被新数据覆盖

4. **删除测试**

   - 删除变量 var4
   - 通过 `e2p_delete()` 标记条目为无效
   - 验证删除操作返回成功

5. **整理测试**

   - 强制触发数据整理
   - 通过 `e2p_flush(E2P_FLUSH_FORCE)` 整理数据
   - 回收已删除/废弃数据空间并切换活动区
   - 调用 `e2p_show_info()` 显示整理后状态

6. **清空测试**

   - 擦除整个存储区域（两个半区）
   - 通过 `e2p_clear()` 清空所有数据
   - 清空后显示状态

7. **状态查看**

   - 打印当前存储区域状态信息（选项 7）
   - 显示起始地址、扇区数量、剩余空间、有效/总条目数等

Flash 区域布局
--------------

EEPROM 模拟使用 NOR Flash 的连续区域，分为两个半区（A 和 B）：

.. code-block:: text

    +--------------------------------------------------+
    |                  Half Area A                     |
    |  +--------------------------------------------+  |
    |  | Data (增长 ↑)  |  Info (增长 ↓)            |  |
    |  |  - 用户数据    |  - 数据块描述符          |  |
    |  |  - 尾部 Header |                            |  |
    |  +--------------------------------------------+  |
    +--------------------------------------------------+
    |                  Half Area B                     |
    |  （布局同上）                                   |
    +--------------------------------------------------+

- **Data 区域**：存储实际用户数据，从区域起始地址向高地址增长
- **Info 区域**：存储数据块描述符，从区域尾部向低地址增长
- Info 描述符包含：`block_id`、`data_addr`、`length`、`valid_state`、`crc32`
- 活动区域和备份区域通过状态机切换，实现掉电保护

状态机与掉电保护
----------------

组件使用 4 位状态机，通过 NOR Flash 的 1→0 位翻转实现（无需擦除）：

.. list-table::
   :widths: 20 30 55
   :header-rows: 1

   * - 状态值
     - 状态名称
     - 说明
   * - 0
     - valid
     - 正常工作状态
   * - 8
     - finish
     - 数据复制完成，准备擦除旧区
   * - 12
     - write
     - 正在向新区写入数据
   * - 14
     - start
     - 整理操作开始
   * - 15
     - invalid
     - 未初始化/擦除后的状态

**状态转换流程**：

.. code-block:: text

    invalid(15) → start(14) → write(12) → finish(8) → valid(0)

**掉电恢复策略**：

.. list-table::
   :widths: 35 15 15 55
   :header-rows: 1

   * - 掉电时机
     - Area A
     - Area B
     - 恢复动作
   * - 首次启动
     - 15
     - 15
     - 选中 Area A，标记为 valid
   * - 整理后掉电（旧区已擦除）
     - 0 (valid)
     - 8 (finish)
     - 擦除 Area A，切换到 B，标记 B 为 valid
   * - 整理写入中掉电
     - 0 (valid)
     - 12 (write)
     - 擦除 B，重新整理
   * - 整理开始前掉电
     - 0 (valid)
     - 14 (start)
     - 擦除 B，重新整理

配置参数
--------

本示例使用以下配置参数（在 `main.c` 中）：

.. list-table::
   :widths: 25 60
   :header-rows: 1

   * - 宏
     - 说明
   * - DEMO_ERASE_SIZE
     - Flash 擦除单元大小（默认 4096）
   * - DEMO_SECTOR_CNT
     - 扇区数量，必须为偶数（默认 32）
   * - DEMO_MANAGE_SIZE
     - 总管理大小（DEMO_ERASE_SIZE * DEMO_SECTOR_CNT）
   * - DEMO_MANAGE_OFFSET
     - 相对 Flash 基址偏移（BOARD_FLASH_SIZE - DEMO_MANAGE_SIZE * 2）

单个变量数据大小不应超过 `E2P_FLUSH_BUF_SIZE`（默认 512 字节），可在 `user_config.h` 中调整。

使用步骤
--------

1. 初始化 NOR Flash 端口

   .. code-block:: c

      nor_flash_init(&g_nor_cfg);

2. 配置 eeprom_emulation 上下文

   .. code-block:: c

      g_e2p_ctx.config.start_addr = g_nor_cfg.base_addr + DEMO_MANAGE_OFFSET;
      g_e2p_ctx.config.erase_size = DEMO_ERASE_SIZE;
      g_e2p_ctx.config.sector_cnt = DEMO_SECTOR_CNT;
      g_e2p_ctx.config.version = 0x4553;  /* 'E' 'S' */
      g_e2p_ctx.config.flash_read = demo_flash_read;
      g_e2p_ctx.config.flash_write = demo_flash_write;
      g_e2p_ctx.config.flash_erase = demo_flash_erase;

3. 初始化 eeprom_emulation

   .. code-block:: c

      e2p_config(&g_e2p_ctx);

4. 生成 block_id（如 var1 ~ var4）

   .. code-block:: c

      uint32_t var1 = e2p_generate_id("HPMC");

5. 写入数据

   .. code-block:: c

      e2p_write(var1, sizeof("test"), (uint8_t *)"test");

6. 读取数据

   .. code-block:: c

      uint8_t buf[32];
      e2p_read(var1, sizeof(buf), buf);

7. 删除数据（可选）

   .. code-block:: c

      e2p_delete(var1);

8. 整理数据（可选；空间不足时也会自动触发）

   .. code-block:: c

      e2p_flush(E2P_FLUSH_FORCE);

注意事项
--------

- 单次写入的数据长度不要超过 `E2P_FLUSH_BUF_SIZE`
- `sector_cnt` 必须是偶数（需要两个半区）
- 删除操作仅标记条目为无效，空间回收在下次 flush 时完成
- 配置中版本号变更时，组件会自动擦除并重新初始化
- 可修改 debug 等级查看详细日志

运行示例
--------

示例启动后，串口输出菜单：

.. code-block:: console

   ========================================
     EEPROM Emulation Demo Menu
   ========================================
     1 - Write test data
     2 - Read test data
     3 - Update test data
     4 - Delete a variable
     5 - Flush (defragment)
     6 - Clear all data
     7 - Show status info
     i - Show this menu
   ========================================

**操作示例**：

1. 按 `1` 写入 4 个测试变量
2. 按 `2` 读取并验证数据
3. 按 `3` 更新 var1 和 var2
4. 按 `4` 删除 var4
5. 按 `5` 强制整理（回收已删除的空间）
6. 按 `6` 清空所有数据
7. 按 `7` 查看当前状态信息

**控制台输出示例**：

.. code-block:: console

   1
   Writing 4 variables...
   Write completed successfully
   2
   Reading variables...
   var1 = abcdef
   var2 = 1234
   var3 = hello,world
   var4 = eeprom_demo
   3
   Updating var1 and var2...
   var1 = qwe
   var2 = 5678
   Update completed successfully
   4
   Deleting var4...
   Delete successful
   5
   Flushing (defragmenting)...
   Flush completed successfully
   [E2P INFO] ------------ e2p init ok -----------
   [E2P INFO] start address : 0x80fd0000
   [E2P INFO] sector count  : 16
   [E2P INFO] erase size    : 4096
   [E2P INFO] version       : 0x4553
   [E2P INFO] end address   : 0x80fe0000
   [E2P INFO] data_ptr=0x80fd0015  info_ptr=0x80fdffb4  remain=0xff9f
   [E2P INFO] entries valid / total : 3 / 3
   [E2P INFO] ------------------------------------
   6
   Clearing all data...
   Clear completed
   [E2P INFO] ------------ e2p init ok -----------
   [E2P INFO] start address : 0x80fd0000
   [E2P INFO] sector count  : 16
   [E2P INFO] erase size    : 4096
   [E2P INFO] version       : 0x4553
   [E2P INFO] end address   : 0x80fe0000
   [E2P INFO] data_ptr=0x80fd0015  info_ptr=0x80fdffb4  remain=0xff9f
   [E2P INFO] entries valid / total : 0 / 3
   [E2P INFO] ------------------------------------
