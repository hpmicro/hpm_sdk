.. _enet_phy_component:

ENET PHY Component
==================

Overview
--------

The ENET PHY component provides a common PHY port abstraction for ENET
applications and board support files. It keeps PHY model selection, PHY
address selection, initialization, and link status polling out of application
code.

Applications should use board-level APIs such as ``board_init_enet_phy()`` and
``board_get_enet_phy_status()``. Board support files use
``hpm_enet_phy_port_init()`` and ``hpm_enet_phy_port_get_status()`` to map each
ENET port to the PHY model and address on that board.

Supported PHY Drivers
---------------------

The component can dispatch to the following ENET PHY drivers:

- DP83867
- RTL8211
- DP83848
- RTL8201
- LAN8720
- JL1111

Only the PHY drivers selected by board CMake configuration are built. The
component itself is enabled by ``CONFIG_ENET_PHY``.

Port Configuration
------------------

The generic port configuration is defined by ``hpm_enet_phy_port_config_t``:

.. code-block:: c

    typedef struct {
        ENET_Type *base;
        hpm_enet_phy_type_t type;
        uint8_t addr;
        uint8_t interface;
        uint8_t rmii_refclk_dir;
        uint32_t init_flags;
    } hpm_enet_phy_port_config_t;

- ``base`` is the ENET controller base address.
- ``type`` selects the concrete PHY driver.
- ``addr`` selects the PHY address on the MDIO bus.
- ``interface`` selects the ENET media interface such as RGMII, RMII, or MII.
- ``rmii_refclk_dir`` selects the RMII reference clock direction when needed.
- ``init_flags`` requests optional PHY initialization behaviors.

``HPM_ENET_PHY_PORT_INIT_FLAG_DISABLE_BROADCAST_RESPONSE`` requests disabling
broadcast response. The helper validates that the selected PHY supports this
option before applying it.

Use ``HPM_ENET_PHY_USE_DEFAULT_ADDR`` when the board should use the default PHY
address defined by the selected PHY driver. PHY addresses are 5-bit MDIO
addresses, so ``0xFF`` is reserved as this component sentinel and is not a
valid physical PHY address.

Board Integration
-----------------

Single-port boards can create one port configuration and pass it to the common
helper:

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

Multi-port boards should create one configuration per ENET port. The following
typical configurations are supported:

- Different ports use different PHY models
- Different ports use the same PHY model with different addresses
- Different ports use the same PHY model with different media interfaces when
  supported by the PHY

Application Usage
-----------------

Applications and middleware should avoid calling concrete PHY driver APIs
directly. They should call board-level APIs instead:

.. code-block:: c

    if (board_init_enet_phy(ENET) == status_success) {
        board_get_enet_phy_status(0, &status);
    }

This keeps PHY details in board files and allows common networking code to work
across different boards and multi-port configurations.
