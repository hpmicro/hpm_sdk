.. _enet_phy_component:

ENET PHY 组件
=============

概述
----

ENET PHY 组件为 ENET 应用和板级支持文件提供通用 PHY 端口抽象，用于将
PHY 型号选择、PHY 地址选择、初始化和链路状态轮询从应用层代码中收敛出来。

应用层应使用 ``board_init_enet_phy()`` 和 ``board_get_enet_phy_status()``
等板级 API。板级文件通过 ``hpm_enet_phy_port_init()`` 和
``hpm_enet_phy_port_get_status()`` 将每个 ENET 端口映射到当前板卡实际使用
的 PHY 型号和地址。

支持的 PHY 驱动
---------------

该组件可分发到以下 ENET PHY 驱动：

- DP83867
- RTL8211
- DP83848
- RTL8201
- LAN8720
- JL1111

只有被板级 CMake 配置选中的 PHY 驱动会参与编译。组件本身由
``CONFIG_ENET_PHY`` 使能。

端口配置
--------

通用端口配置由 ``hpm_enet_phy_port_config_t`` 定义：

.. code-block:: c

    typedef struct {
        ENET_Type *base;
        hpm_enet_phy_type_t type;
        uint8_t addr;
        uint8_t interface;
        uint8_t rmii_refclk_dir;
        uint32_t init_flags;
    } hpm_enet_phy_port_config_t;

- ``base`` 为 ENET 控制器基地址。
- ``type`` 用于选择具体 PHY 驱动。
- ``addr`` 用于选择 MDIO 总线上的 PHY 地址。
- ``interface`` 用于选择 RGMII、RMII、MII 等 ENET 介质接口。
- ``rmii_refclk_dir`` 用于在需要时选择 RMII 参考时钟方向。
- ``init_flags`` 用于请求可选 PHY 初始化行为。

``HPM_ENET_PHY_PORT_INIT_FLAG_DISABLE_BROADCAST_RESPONSE`` 用于请求关闭广播响应。
helper 会先校验所选 PHY 是否支持该选项，然后再应用。

当板卡使用所选 PHY 驱动定义的默认 PHY 地址时，``addr`` 可设置为
``HPM_ENET_PHY_USE_DEFAULT_ADDR``。PHY 地址是 5 位 MDIO 地址，因此 ``0xFF``
在该组件中作为哨兵值使用，不是有效的物理 PHY 地址。

板级接入
--------

单端口板卡可以创建一份端口配置并传入通用 helper：

.. code-block:: c

    hpm_stat_t board_init_enet_phy(ENET_Type *ptr)
    {
        hpm_enet_phy_port_config_t phy_config = {0};

        phy_config.base = ptr;
        phy_config.type = hpm_enet_phy_rtl8211;
        phy_config.addr = HPM_ENET_PHY_USE_DEFAULT_ADDR;
        phy_config.interface = enet_inf_rgmii;
        phy_config.init_flags = 0;

        return hpm_enet_phy_port_init(&phy_config);
    }




多端口板卡应为每个 ENET 端口创建独立配置，支持以下典型场景：

- 不同端口使用不同 PHY 型号
- 不同端口使用相同 PHY 型号但不同地址
- 不同端口使用相同 PHY 型号但不同介质接口，需 PHY 支持

应用层用法
----------

应用和中间件应避免直接调用具体 PHY driver API，而是调用板级 API：

.. code-block:: c

    if (board_init_enet_phy(ENET) == status_success) {
        board_get_enet_phy_status(0, &status);
    }

这样可以将 PHY 细节保留在板级文件中，使通用网络代码能够适配不同板卡和多端口
配置。
