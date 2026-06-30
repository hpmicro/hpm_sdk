.. _eeprom_emulation_performance_test:

EEPROM 模拟性能测试
====================

概述
----

本示例用于测试 EEPROM 模拟组件的性能表现，通过大量读写与整理操作评估组件的效率，并输出 config（初始化）、写入、整理、读取、删除等操作的耗时。

测试参数
--------

默认配置（在 `main.c` 中修改）：

.. list-table::
   :widths: 28 60
   :header-rows: 1

   * - 宏
     - 说明
   * - DEMO_TEST_VAR_CNT
     - 测试变量数量（默认 50）
   * - DEMO_WRITE_CYCLE
     - 每个变量的写入/更新次数（默认 5）
   * - DEMO_ERASE_SIZE
     - Flash 擦除单元大小（字节，默认 4096）
   * - DEMO_SECTOR_CNT
     - 扇区数量，必须为偶数（默认 32）

- 写入性能测试中的总写入次数 = `DEMO_TEST_VAR_CNT × DEMO_WRITE_CYCLE`

性能指标
--------

本示例测量以下内容：

1. **配置（初始化）时间**

   - 清空存储后，测量 `e2p_config()` 执行时间
   - 包括头部校验、状态恢复、索引表重建

2. **写入性能**

   - 清空并初始化后，写入全部测试变量（每个变量更新 DEMO_WRITE_CYCLE 次）
   - 输出总耗时及平均每次写入耗时

3. **整理（flush）性能**

   - 清空、初始化、写入测试数据后，测量 `e2p_flush(E2P_FLUSH_FORCE)` 耗时
   - 包括状态切换、数据复制到备份区、旧区擦除

4. **读取性能**

   - 清空、初始化、写入测试数据后，测量读取全部变量的总时间
   - 输出总耗时及平均每次读取耗时（含 CRC32 校验）

5. **删除性能**

   - 清空、初始化、写入测试数据后，测量删除全部变量的总时间
   - 删除仅标记条目无效，空间在下次 flush 时回收

使用方法
--------

示例启动后，通过串口输入选择测试项：

.. code-block:: console

   ========================================
     EEPROM Emulation Perf Test
   ========================================
     1 - Test config/init time
     2 - Test write performance
     3 - Test flush performance
     4 - Test read performance
     5 - Test delete performance
     6 - Show status info
     i - Show this menu
   ========================================

**测试操作**：

- 按 `1` — 测试初始化（config）性能
- 按 `2` — 测试写入性能
- 按 `3` — 测试整理（flush）性能
- 按 `4` — 测试读取性能
- 按 `5` — 测试删除性能
- 按 `6` — 查看当前状态信息

输出示例
--------

**配置（初始化）性能**：

.. code-block:: console

   === Test: Configuration (Init) ===
   [E2P INFO] ------------ e2p init ok -----------
   [E2P INFO] start address : 0x80fd0000
   [E2P INFO] sector count  : 16
   [E2P INFO] erase size    : 4096
   [E2P INFO] version       : 0x4553
   [E2P INFO] end address   : 0x80fe0000
   [E2P INFO] data_ptr=0x80fd0000  info_ptr=0x80fdffa4  remain=0xffa4
   [E2P INFO] entries valid / total : 0 / 0
   [E2P INFO] ------------------------------------
     Config time: 32451 us

**写入性能**：

.. code-block:: console

   === Test: Write Performance ===
   Writing 50 variables, 5 updates each...
   Write completed: 50 variables x 5 updates = 250 total writes
     Total writes: 250
     Total time: 73824 us
     Avg per write: 295 us

**整理性能**：

.. code-block:: console

   === Test: Flush Performance ===
   Writing 50 variables, 5 updates each...
   Write completed: 50 variables x 5 updates = 250 total writes
     Flush time: 360264 us

**读取性能**：

.. code-block:: console

   === Test: Read Performance ===
     Success reads: 50 / 50
     Total time: 164 us
     Avg per read: 3 us

**删除性能**：

.. code-block:: console

   === Test: Delete Performance ===
     Success deletes: 50 / 50
     Total time: 24 us
     Avg per delete: 0 us

注意事项
------------

.. list-table::
   :widths: 28 60
   :header-rows: 1

   * - 优化项
     - 建议
   * - erase_size
     - 擦除块越大，整理耗时越长；根据实际数据量选择 4KB～64KB。
   * - sector_cnt
     - 必须为偶数；增加扇区数可减少 flush 频率，但会增加初始化/扫描时间。
   * - 单条目大小
     - 单次写入不超过 E2P_FLUSH_BUF_SIZE（在 user_config.h 中配置）。
   * - 临界区
     - E2P_CRITICAL_ENTER/EXIT 内尽量缩短代码，避免耗时操作。
