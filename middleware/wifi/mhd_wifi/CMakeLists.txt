# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(wmhd_hpm_port.c)
if(CONFIG_MHD_4345X)
    sdk_src("./nvram_ap6256.c")
    sdk_link_libraries("./libmhd4345x_${RV_ABI}.a")
endif ()