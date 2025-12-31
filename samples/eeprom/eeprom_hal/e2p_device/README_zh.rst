.. _eeprom_hal_device_test:

Eeprom hal device 功能测试
==========================================

概述
------

EEPROM HAL DEVICE 示例演示如何使用 EEPROM HAL 驱动程序接口来访问 EEPROM 设备。该示例默认使用板级 I2C 驱动的实体 EEPROM 设备来实现。

板级设置
------------

- 硬件无需特殊设置

测试内容
------------

- 基本功能：单字节读写测试
- 小块数据读写测试：页内非对齐地址读写测试
- 大块数据读写测试：跨页非对齐地址读写测试(防回卷测试)

运行示例
------------

当工程运行后，串口会打印以下信息：

.. code-block:: console

    EEPROM HAL sample test start.

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

扩展
------------

- EEPROM HAL 组件支持 spi 驱动的实体 eeprom，如果用户板级有 spi eeprom 设备或者外接 spi 驱动的 eeprom，可以以当前示例为参考，修改 cmake 配置，启用 spi 设备支持，具体修改方法如下：

    set(CONFIG_EEPROM_DEVICE_SPI 1)

- 以上，启用 spi eeprom 的驱动宏后，就会自行将组件加入工程，而如何使用 spi eeprom 驱动，用户可以参考 eeprom_hal 组件下的 README。



