.. _ethernet_port_configurations_en:

**Ethernet Port Configurations**
================================================================

- For the samples supporting only one Ethernet port:  A development board with two Ethernet ports supports the configurations in the `CMakeLists.txt` as follows:

  - set(APP_USE_ENET_PORT_COUNT 1):  Enable a RGMII or a RMII port corresponding to ENet0 on the development board <sup> **5/6** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1) and set(APP_USE_ENET_ITF_RGMII 1):  Enable a RGMII port on the development board <sup> **6** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1) and set(APP_USE_ENET_ITF_RMII 1): Enable a RMII port on the development <sup> **6** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1) and set(APP_USE_ENET_PHY_DP83867 1): Enable a RGMII port on the development board <sup> **1** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1) and set(APP_USE_ENET_PHY_RTL8211 1): Enable a RGMII port on the development board <sup> **2** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1) and set(APP_USE_ENET_PHY_DP83848 1): Enable a RMII port on the development board <sup> **3** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1) and set(APP_USE_ENET_PHY_RTL8201 1): Enable a RMII port on the development board  <sup> **4** </sup>

- For the samples supporting only one Ethernet port:  A development board with only one Ethernet port supports the configurations in the `CMakeLists.txt` as follows:

  - set(APP_USE_ENET_PORT_COUNT 1): Enable a RGMII or a RMII port on the development board <sup> **5/6** </sup>

- For the samples supporting two Ethernet ports:  A development board with two necessary Ethernet ports  supports the configurations in the `CMakeLists.txt` as follows:

  - set(APP_USE_ENET_PORT_COUNT 2):  Enable two Ethernet ports on the developments <sup> **5/6** </sup>

**Notes:**

1. Only supported on the development board with DP83867
2. Only supported on the development board with RTL8211
3. Only supported on the development board with DP83848
4. Only supported on the development board with RTL8201
5. RGMII or RMII port depending on Hardware design
6. Ethernet PHY types are matched by CMake build process automatically
