.. _tsw_phy_component:

TSW PHY Component
=================

Overview
--------

The TSW PHY component provides a common PHY port helper for TSW applications
and board support files. It keeps PHY model selection, PHY address selection,
initialization, and link status polling out of application code.

Applications should use board-level APIs such as ``board_init_tsw_port_phy()``
and ``board_get_tsw_port_phy_status()``. Board support files use
``hpm_tsw_phy_port_init()`` and ``hpm_tsw_phy_port_get_status()`` to map each
TSW port to the PHY model and address on that board.

Supported PHY Drivers
---------------------

The component can dispatch to the following TSW PHY drivers:

- RTL8211
- JL1111

Only the PHY drivers selected by board CMake configuration are built. The
component itself is enabled by ``CONFIG_TSW_PHY``.

Port Configuration
------------------

The generic port configuration is defined by ``hpm_tsw_phy_port_config_t``:

.. code-block:: c

    typedef struct {
        TSW_Type *base;
        hpm_tsw_phy_type_t type;
        uint8_t addr;
        uint8_t port;
    } hpm_tsw_phy_port_config_t;

- ``base`` is the TSW controller base address.
- ``type`` selects the concrete PHY driver.
- ``addr`` selects the PHY address on the MDIO bus.
- ``port`` selects the TSW port used for PHY access.

Use ``HPM_TSW_PHY_USE_DEFAULT_ADDR`` when the board should use the default PHY
address defined by the selected PHY driver. PHY addresses are 5-bit MDIO
addresses, so ``0xFF`` is reserved as this component sentinel and is not a
valid physical PHY address.

Board Integration
-----------------

Board support files should create the port configuration and pass it to the
common helper:

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

Multi-port boards should create one configuration per TSW PHY port. The
following typical configurations are supported:

- Different ports use different PHY models
- Different ports use the same PHY model with different addresses
- Different ports use the same PHY model with different TSW access ports

Application Usage
-----------------

Applications and middleware should avoid calling concrete TSW PHY driver APIs
directly. They should call board-level APIs instead:

.. code-block:: c

    if (board_init_tsw_port_phy(BOARD_TSW) == status_success) {
        board_get_tsw_port_phy_status(BOARD_TSW_PORT, &status);
    }

This keeps PHY details in board files and allows TSN samples to work across
different board PHY configurations.
