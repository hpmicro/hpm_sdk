.. _tsw_port_configurations_en:

**TSW Port Configurations**
================================================================

- For the samples supporting only one TSW port:  A development board with multiple TSW ports supports the configurations in the `CMakeLists.txt` as follows:

  - set(APP_USE_TSW_PORT_COUNT 1):  Enable a RGMII port corresponding to ENet0 on the development board <sup> **1/2** </sup>

**Notes:**

1. Port type depends on hardware design
2. Ethernet PHY types are matched by CMake build process automatically
