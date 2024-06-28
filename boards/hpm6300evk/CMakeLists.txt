# Copyright (c) 2022-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(pinmux.c)
sdk_src(board.c)

if(APP_USE_ENET_PORT_COUNT EQUAL 1)
    set(ENV{CONFIG_ENET_PHY_RTL8201} 1)
    sdk_compile_definitions(-DRMII=1)
endif()
