.. _ethernet_port_configurations_zh:

**以太网端口配置**
======================

- 对于仅支持单网口的sample：开发板上有双网口，在`CMakeLists.txt`中，支持如下配置：

  - set(APP_USE_ENET_PORT_COUNT 1): 使能开发板上ENet0对应的RGMII或RMII端口 <sup> **5/6** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1)且set(APP_USE_ENET_ITF_RGMII 1): 使能开发板上RGMII端口 <sup> **6** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1)且set(APP_USE_ENET_ITF_RMII 1): 使能开发板上RMII端口 <sup> **6** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1)且set(APP_USE_ENET_PHY_DP83867 1): 使能开发板上RGMII端口 <sup> **1** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1)且set(APP_USE_ENET_PHY_RTL8211 1): 使能开发板上RGMII端口 <sup> **2** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1)且set(APP_USE_ENET_PHY_DP83848 1): 使能开发板上RMII端口 <sup> **3** </sup>

  - set(APP_USE_ENET_PORT_COUNT 1)且set(APP_USE_ENET_PHY_RTL8201 1): 使能开发板上RMII端口 <sup> **4** </sup>

- 对于仅支持单网口的sample：开发板上只有一个网口，在`CMakeLists.txt`中，仅支持如下配置：

  - set(APP_USE_ENET_PORT_COUNT 1): 使能开发板上RGMII或RMII端口 <sup> **5/6** </sup>

- 对于支持双网口的sample：开发板上必需有双网口，在`CMakeLists.txt`中，仅支持如下配置：

  -  set(APP_USE_ENET_PORT_COUNT 2): 使能开发板上双以太网端口 <sup> **5/6** </sup>

**注** :

1. 仅在有DP83867的开发板上支持
2. 仅在有RTL8211的开发板上支持
3. 仅在有DP83848的开发板上支持
4. 仅在有RTL8201的开发板上支持
5. RGMII或RMII端口取决于硬件设计
6. 以太网PHY类型由CMake构建过程根据硬件设计自动匹配
