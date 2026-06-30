.. _eeprom_emulation_component:

EEPROM Emulation Component
===========================

概述
----

EEPROM 模拟组件（eeprom_emulation）在 NOR Flash 上实现类似 EEPROM 的掉电保护数据存储功能。组件通过双区域设计和状态机机制，确保在任何掉电场景下数据的安全性和一致性。

组件架构
--------

.. code-block:: text

    +--------------------------------------------------+
    |            应用层（Application）                  |
    |  - 通过 e2p_write/read/delete 操作数据           |
    +--------------------------------------------------+
                         |
                         | API 调用
                         |
    +--------------------------------------------------+
    |      EEPROM 模拟核心层（Core）                   |
    |  - eeprom_emulation.h / eeprom_emulation.c       |
    |  - 状态管理、索引表、CRC 校验                    |
    +--------------------------------------------------+
                         |
                         | 函数指针
                         |
    +------------------------+-------------------------+
    |                        |                         |
    +-----------------------+  |   +----------------------+
    |   NOR Flash 端口      |  |   |  其他 Flash 端口     |
    | (port/hpm_nor_flash)  |  |   |  (可扩展)            |
    +-----------------------+  |   +----------------------+
    |                        |                         |
    +--------------------------------------------------+
    |          Flash 驱动层（Driver）                   |
    |  - ROM API / XPI Driver                          |
    +--------------------------------------------------+
                         |
    +--------------------------------------------------+
    |              硬件层（Hardware）                   |
    |              XPI 外部 Flash                       |
    +--------------------------------------------------+

核心特性
--------

- **掉电保护**

  - 双半区（Half-Area）设计，任何掉电场景均可安全恢复
  - 状态机通过 1→0 位翻转实现（无需擦除 flash）
  - 自动检测并恢复未完成的整理操作

- **数据完整性**

  - 每个条目包含 CRC32 校验
  - 读取时自动验证，损坏数据返回错误

- **空间管理**

  - 自动整理：空间不足时自动触发
  - 删除标记：`e2p_delete()` 标记无效，下次 flush 回收
  - 废弃数据回收：同一 block_id 的旧数据在整理时自动清理

- **可移植性**

  - 通过函数指针抽象 flash 驱动
  - 支持不同 flash 介质（NOR Flash、SPI Flash 等）
  - 端口层独立，易于扩展

- **配置灵活**

  - 可配置擦除块大小（erase_size）
  - 可配置扇区数量（sector_cnt）
  - 可配置版本号（version）变更自动擦除

Flash 区域布局
--------------

EEPROM 模拟使用 NOR Flash 的连续区域，分为两个半区（A 和 B）：

.. code-block:: text

    +--------------------------------------------------+
    |                   Half Area A                    |
    |  +------------------------------------------+    |
    |  | Data 区域（地址增长方向：↑）             |    |
    |  |  - 用户数据                              |    |
    |  |  - 数据块：block_id + data               |    |
    |  +------------------------------------------+    |
    |  | Info 区域（地址增长方向：↓）             |    |
    |  |  - Info 描述符数组                       |    |
    |  |  - 每个 Info：16 bytes                   |    |
    |  |  - 尾部：Header（12 bytes）              |    |
    |  +------------------------------------------+    |
    +--------------------------------------------------+
    |                   Half Area B                    |
    |  （布局同上）                                   |
    +--------------------------------------------------+

- **Data 区域**：存储实际用户数据，从区域起始地址向高地址增长
- **Info 区域**：存储数据块描述符，从区域尾部向低地址增长
- **Area Header**：每个半区尾部 12 bytes，包含 version、magic、state
- **Info 描述符结构**

.. code-block:: c

    typedef struct {
        uint32_t block_id;      /* 块 ID（用户自定义）*/
        uint32_t data_addr;     /* 数据在 Data 区域的地址 */
        uint16_t length;        /* 数据长度 */
        uint16_t valid_state;   /* 有效性标记 */
        uint32_t crc;          /* CRC32 校验值 */
    } e2p_block_t;

状态机与掉电保护
----------------

组件使用 4 位状态机，通过 NOR Flash 的 1→0 位翻转实现（无需擦除）：

.. list-table::
   :widths: 20 30 60
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

**状态转换流程**

.. code-block:: text

    invalid(15) → start(14) → write(12) → finish(8) → valid(0)

**掉电恢复策略**

.. list-table::
   :widths: 35 15 15 60
   :header-rows: 1

   * - 掉电时机
     - Area A 状态
     - Area B 状态
     - 恢复动作
   * - 首次启动
     - 15
     - 15
     - 选中 Area A，标记为 valid
   * - 整理中掉电（数据已复制）
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

主要 API
--------

初始化与配置
~~~~~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_config(e2p_t *e2p);

- **功能**：初始化 EEPROM 模拟，校验 flash 头部，恢复掉电状态，重建索引表
- **参数**：
  - `e2p`: 上下文结构体指针，需预先配置 flash 操作函数指针
- **返回**： `E2P_STATUS_OK` 成功，错误码失败

写入数据
~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_write(uint32_t block_id, uint16_t length, const uint8_t *data);

- **功能**：写入数据，自动更新相同 block_id 的旧数据
- **参数**：
  - `block_id`: 用户自定义标识符（通过 `e2p_generate_id()` 生成）
  - `length`: 数据长度（字节）
  - `data`: 数据缓冲区指针
- **返回**：`E2P_STATUS_OK` 成功，错误码失败

读取数据
~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_read(uint32_t block_id, uint16_t length, uint8_t *data);

- **功能**：读取数据，自动验证 CRC32
- **参数**：
  - `block_id`: 用户自定义标识符
  - `length`: 缓冲区大小（可小于存储长度）
  - `data`: 输出缓冲区指针
- **返回**：`E2P_STATUS_OK` 成功，`E2P_ERROR_NOT_FOUND` 未找到，`E2P_ERROR_CRC` 校验失败

删除数据
~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_delete(uint32_t block_id);

- **功能**：删除指定 block_id 的数据（标记为无效）
- **参数**：
  - `block_id`: 用户自定义标识符
- **返回**：`E2P_STATUS_OK` 成功，`E2P_ERROR_NOT_FOUND` 未找到

整理数据
~~~~~~~~

.. code-block:: c

    hpm_stat_t e2p_flush(uint8_t flag);

- **功能**：整理数据，回收空间
- **参数**
  - `flag`: `E2P_FLUSH_TRY` 仅在空间不足时整理或 `E2P_FLUSH_FORCE` 强制整理
- **返回**：`E2P_STATUS_OK` 成功，错误码失败

生成 ID
~~~~~~~

.. code-block:: c

    uint32_t e2p_generate_id(const char *name);

- **功能**：从字符串生成 32 位 block_id
- **参数**：
  - `name`: 字符串指针（前 4 个字符被打包）
- **返回**：32 位 block_id

清空数据
~~~~~~~~

.. code-block:: c

    void e2p_clear(void);

- **功能**：擦除整个存储区域（两个半区）

显示状态信息
~~~~~~~~~~~~

.. code-block:: c

    void e2p_show_info(void);

- **功能**：打印当前 EEPROM 模拟状态（当前活动区、剩余空间等）
- **参数**：无
- **返回**：无

配置参数
--------

核心配置结构体：

.. code-block:: c

    typedef struct {
        uint32_t start_addr;
        uint32_t sector_cnt;
        uint32_t erase_size;
        uint32_t version;

        hpm_stat_t (*flash_read)(uint8_t *buf, uint32_t addr, uint32_t size);
        hpm_stat_t (*flash_write)(const uint8_t *buf, uint32_t addr, uint32_t size);
        hpm_stat_t (*flash_erase)(uint32_t start_addr, uint32_t size);
    } e2p_config_t;

.. list-table::
   :widths: 25 60
   :header-rows: 1

   * - 字段名称
     - 说明
   * - start_addr
     - Flash 起始地址
   * - sector_cnt
     - 扇区数量（必须是偶数）
   * - erase_size
     - 擦除块大小（字节）
   * - version
     - 版本号（变更时自动擦除）
   * - flash_read/write/erase
     - Flash 操作函数指针

用户配置（user_config.h）：

.. list-table::
   :widths: 30 60
   :header-rows: 1

   * - 配置
     - 说明
   * - E2P_DEBUG_LEVEL
     - 调试级别（0-4）
   * - E2P_MAX_VAR_CNT
     - 最大变量数量（默认 100）
   * - E2P_FLUSH_BUF_SIZE
     - 整理缓冲区大小（默认 512）

快速开始
--------

1. 包含头文件及 NOR Flash 端口：

   .. code-block:: c

       #include "eeprom_emulation.h"
       #include "hpm_nor_flash.h"

2. 配置 NOR Flash 与 EEPROM 模拟上下文（见下方使用示例）。
3. 调用 `nor_flash_init()` 后调用 `e2p_config()`。
4. 使用 `e2p_generate_id("NAME")` 生成 block_id，再调用 `e2p_write()` / `e2p_read()`。

使用示例
--------

基本用法示例：

.. code-block:: c

    #include "eeprom_emulation.h"
    #include "hpm_nor_flash.h"

    static nor_flash_config_t g_nor_cfg;
    static e2p_t g_e2p_ctx;

    /* Flash 操作封装 */
    static hpm_stat_t my_read(uint8_t *buf, uint32_t addr, uint32_t size) {
        return nor_flash_read(&g_nor_cfg, buf, addr, size);
    }
    static hpm_stat_t my_write(const uint8_t *buf, uint32_t addr, uint32_t size) {
        return nor_flash_write(&g_nor_cfg, buf, addr, size);
    }
    static hpm_stat_t my_erase(uint32_t start_addr, uint32_t size) {
        return nor_flash_erase(&g_nor_cfg, start_addr, size);
    }

    int main(void) {
        /* 配置 NOR Flash 端口 */
        g_nor_cfg.xpi_base = BOARD_APP_XPI_NOR_XPI_BASE;
        g_nor_cfg.base_addr = BOARD_FLASH_BASE_ADDRESS;
        g_nor_cfg.opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
        g_nor_cfg.opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
        g_nor_cfg.opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

        /* 配置 EEPROM 模拟 */
        g_e2p_ctx.config.start_addr = 0x80080000;
        g_e2p_ctx.config.erase_size = 4096;
        g_e2p_ctx.config.sector_cnt = 128;
        g_e2p_ctx.config.version = 0x4553;
        g_e2p_ctx.config.flash_read = my_read;
        g_e2p_ctx.config.flash_write = my_write;
        g_e2p_ctx.config.flash_erase = my_erase;

        /* 初始化 */
        nor_flash_init(&g_nor_cfg);
        if (e2p_config(&g_e2p_ctx) != E2P_STATUS_OK) {
            printf("Init failed\n");
            return -1;
        }

        /* 写入数据 */
        uint32_t var_id = e2p_generate_id("TEST");
        const char *data = "hello,world";
        e2p_write(var_id, strlen(data), (uint8_t *)data);

        /* 读取数据 */
        uint8_t buf[32];
        if (e2p_read(var_id, sizeof(buf), buf) == E2P_STATUS_OK) {
            printf("Read: %s\n", buf);
        }

        return 0;
    }

扩展 Flash 端口
---------------

若需要支持其他 Flash 介质（如 SPI Flash、内部 Data Flash），在 `port/` 目录下新增驱动文件：

.. code-block:: c

    /* port/my_flash.h */
    typedef struct {
        /* 端口特定配置 */
    } my_flash_config_t;

    hpm_stat_t my_flash_init(my_flash_config_t *cfg);
    hpm_stat_t my_flash_read(my_flash_config_t *cfg,
                             uint8_t *buf, uint32_t addr, uint32_t size);
    hpm_stat_t my_flash_write(my_flash_config_t *cfg,
                              const uint8_t *buf, uint32_t addr, uint32_t size);
    hpm_stat_t my_flash_erase(my_flash_config_t *cfg,
                              uint32_t start_addr, uint32_t size);

使用方法与 NOR Flash 端口相同，只需将函数指针注册到 `e2p_config_t` 即可。

错误码
------

.. list-table::
   :widths: 25 10 60
   :header-rows: 1

   * - 错误码
     - 值
     - 说明
   * - E2P_STATUS_OK
     - 0
     - 成功
   * - E2P_ERROR
     - 1
     - 通用错误
   * - E2P_ERROR_NO_MEM
     - 2
     - 空间不足
   * - E2P_ERROR_INIT_ERR
     - 3
     - 初始化错误
   * - E2P_ERROR_BAD_ID
     - 4
     - 无效的 block_id
   * - E2P_ERROR_BAD_ADDR
     - 5
     - 无效的地址
   * - E2P_ERROR_OVERFLOW
     - 6
     - 索引表溢出
   * - E2P_ERROR_CRC
     - 7
     - CRC 校验失败
   * - E2P_ERROR_NOT_FOUND
     - 8
     - 未找到

最佳实践
--------

- **块大小**：单变量长度建议小于等于 `E2P_FLUSH_BUF_SIZE`,大数据拆分为多个变量。
- **整理**：避免在实时性要求高的路径中调用 `e2p_flush(E2P_FLUSH_FORCE)`；擦除耗时较长。
- **临界区**：在 `E2P_CRITICAL_ENTER` / `E2P_CRITICAL_EXIT` 内尽量缩短代码，避免耗时操作。
- **版本号**：修改配置中的 `version` 会触发全量擦除并重新初始化；适用于布局或格式变更时。

故障排除
--------

- **初始化失败**：检查 `start_addr`、 `erase_size`、 `sector_cnt` 及 flash 读写擦除函数指针是否已设置。
- **E2P_ERROR_NO_MEM**：执行 `e2p_flush(E2P_FLUSH_FORCE)` 回收空间，或减少变量数量/大小。
- **E2P_ERROR_CRC**：存储数据可能损坏；重新写入该变量或检查 flash 硬件。
- **E2P_ERROR_NOT_FOUND**：确认该 block 已写入且 `block_id` 一致（如通过 `e2p_generate_id()` 生成）。

注意事项
--------

1. **sector_cnt 必须是偶数**

   组件需要两个半区实现掉电保护。

2. **单次写入大小限制**

   单个变量长度不应超过 `E2P_FLUSH_BUF_SIZE`
   大数据应拆分为多个变量存储。

3. **版本号变更**

   修改 `version` 字段后，组件会自动擦除并重新初始化。

4. **临界区保护**

   `E2P_CRITICAL_ENTER` / `E2P_CRITICAL_EXIT` 宏应尽可能短。
   避免在临界区内调用耗时操作。

5. **Flash 擦除时间**

   擦除操作通常耗时较长。
   避免在实时性要求高的代码路径中调用 flush。
