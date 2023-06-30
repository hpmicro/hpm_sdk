# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_AZURE_RTOS_THREADX OR CONFIG_AZURE_RTOS_FILEX 
    OR CONFIG_AZURE_RTOS_USBX OR CONFIG_AZURE_RTOS_GUIX)
    set(THREADX_ARCH "riscv32")
    set(THREADX_TOOLCHAIN "gnu")
endif()

add_subdirectory_ifdef(CONFIG_AZURE_RTOS_THREADX threadx)
add_subdirectory_ifdef(CONFIG_AZURE_RTOS_FILEX filex)
add_subdirectory_ifdef(CONFIG_AZURE_RTOS_USBX usbx)
add_subdirectory_ifdef(CONFIG_AZURE_RTOS_NETXDUO netxduo)
add_subdirectory_ifdef(CONFIG_AZURE_RTOS_GUIX guix)
