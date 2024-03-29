# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_ECLIPSE_THREADX OR CONFIG_ECLIPSE_THREADX_FILEX 
    OR CONFIG_ECLIPSE_THREADX_USBX OR CONFIG_ECLIPSE_THREADX_GUIX)
    set(THREADX_ARCH "riscv32")
    set(THREADX_TOOLCHAIN "gnu")
endif()

add_subdirectory_ifdef(CONFIG_ECLIPSE_THREADX threadx)
add_subdirectory_ifdef(CONFIG_ECLIPSE_THREADX_FILEX filex)
add_subdirectory_ifdef(CONFIG_ECLIPSE_THREADX_USBX usbx)
add_subdirectory_ifdef(CONFIG_ECLIPSE_THREADX_NETXDUO netxduo)
add_subdirectory_ifdef(CONFIG_ECLIPSE_THREADX_GUIX guix)
