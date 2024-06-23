# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

set(ENV{CONFIG_PANEL_RGB_TM070RDH13} 1)
sdk_compile_definitions_ifdef(BUILD_FOR_SECONDARY_CORE BOARD_RUNNING_CORE=HPM_CORE1)

sdk_inc(.)
sdk_src(pinmux.c)
sdk_src(board.c)

if(APP_USE_ENET_PORT_COUNT EQUAL 2)
    set(ENV{CONFIG_ENET_PHY_RTL8211} 1)
    set(ENV{CONFIG_ENET_PHY_RTL8201} 1)
elseif(APP_USE_ENET_PORT_COUNT EQUAL 1)
    if(APP_USE_ENET_ITF_RGMII)
        set(ENV{CONFIG_ENET_PHY_RTL8211} 1)
    elseif(APP_USE_ENET_ITF_RMII)
        set(ENV{CONFIG_ENET_PHY_RTL8201} 1)
    elseif(APP_USE_ENET_PHY_RTL8211)
        set(ENV{CONFIG_ENET_PHY_RTL8211} 1)
    elseif(APP_USE_ENET_PHY_RTL8201)
        set(ENV{CONFIG_ENET_PHY_RTL8201} 1)
    else()
        set(ENV{CONFIG_ENET_PHY_RTL8211} 1)
    endif()
endif()