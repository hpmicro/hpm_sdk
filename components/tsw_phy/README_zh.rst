.. _tsw_phy_component:

TSW PHY 组件
============

概述
----

TSW PHY 组件为 TSW 应用和板级支持文件提供通用 PHY 端口 helper，用于将
PHY 型号选择、PHY 地址选择、初始化和链路状态轮询从应用层代码中收敛出来。

应用层应使用 ``board_init_tsw_port_phy()`` 和
``board_get_tsw_port_phy_status()`` 等板级 API。板级文件通过
``hpm_tsw_phy_port_init()`` 和 ``hpm_tsw_phy_port_get_status()`` 将每个
TSW 端口映射到当前板卡实际使用的 PHY 型号和地址。

支持的 PHY 驱动
---------------

该组件可分发到以下 TSW PHY 驱动：

- RTL8211
- JL1111

只有被板级 CMake 配置选中的 PHY 驱动会参与编译。组件本身由
``CONFIG_TSW_PHY`` 使能。

端口配置
--------

通用端口配置由 ``hpm_tsw_phy_port_config_t`` 定义：

.. code-block:: c

    typedef struct {
        TSW_Type *base;
        hpm_tsw_phy_type_t type;
        uint8_t addr;
        uint8_t port;
    } hpm_tsw_phy_port_config_t;

- ``base`` 为 TSW 控制器基地址。
- ``type`` 用于选择具体 PHY 驱动。
- ``addr`` 用于选择 MDIO 总线上的 PHY 地址。
- ``port`` 用于选择访问 PHY 的 TSW 端口。

当板卡使用所选 PHY 驱动定义的默认 PHY 地址时，``addr`` 可设置为
``HPM_TSW_PHY_USE_DEFAULT_ADDR``。PHY 地址是 5 位 MDIO 地址，因此 ``0xFF``
在该组件中作为哨兵值使用，不是有效的物理 PHY 地址。

板级接入
--------

板级支持文件应创建端口配置并传入通用 helper：

.. code-block:: c

    hpm_stat_t board_init_tsw_port_phy(TSW_Type *ptr)
    {
        hpm_tsw_phy_port_config_t phy_config;

        phy_config.base = ptr;
        phy_config.type = hpm_tsw_phy_rtl8211;
        phy_config.addr = HPM_TSW_PHY_USE_DEFAULT_ADDR;
        phy_config.port = BOARD_TSW_PORT;

        return hpm_tsw_phy_port_init(&phy_config);
    }

多端口板卡应为每个 TSW PHY 端口创建独立配置，支持以下典型场景：

- 不同端口使用不同 PHY 型号
- 不同端口使用相同 PHY 型号但不同地址
- 不同端口使用相同 PHY 型号但不同 TSW 访问端口

应用层用法
----------

应用和中间件应避免直接调用具体 TSW PHY driver API，而是调用板级 API：

.. code-block:: c

    if (board_init_tsw_port_phy(BOARD_TSW) == status_success) {
        board_get_tsw_port_phy_status(BOARD_TSW_PORT, &status);
    }

这样可以将 PHY 细节保留在板级文件中，使 TSN sample 能够适配不同板卡的
PHY 配置。
