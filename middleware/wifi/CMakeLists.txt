# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if (CONFIG_MHD_4345X)
    set(CONFIG_MHD_WIFI 1)
endif ()
add_subdirectory_ifdef(CONFIG_MHD_WIFI mhd_wifi)