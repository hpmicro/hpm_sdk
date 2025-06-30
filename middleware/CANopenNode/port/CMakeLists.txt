# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(hpm_canopen.c)
sdk_src(CO_driver.c)
add_subdirectory(objdict)
add_subdirectory_ifdef(HPMSOC_HAS_HPMSDK_CAN hpm_canopen_can)
add_subdirectory_ifdef(HPMSOC_HAS_HPMSDK_MCAN hpm_canopen_mcan)