# Copyright (c) 2022-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)

if($ENV{CONFIG_ENET_PHY_RTL8211})
    add_subdirectory(rtl8211)
    message(STATUS "ENet PHY Enabled: RTL8211")
endif()

if($ENV{CONFIG_ENET_PHY_RTL8201})
    add_subdirectory(rtl8201)
    message(STATUS "ENet PHY Enabled: RTL8201")
endif()

if($ENV{CONFIG_ENET_PHY_DP83867})
    add_subdirectory(dp83867)
    message(STATUS "ENet PHY Enabled: DP83867")
endif()

if($ENV{CONFIG_ENET_PHY_DP83848})
    add_subdirectory(dp83848)
    message(STATUS "ENet PHY Enabled: DP83848")
endif()