# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(hpm_canopen.c)

if(HPMSOC_HAS_HPMSDK_CAN)
sdk_src(hpm_canopen_can.c)
endif()

if(HPMSOC_HAS_HPMSDK_MCAN)
sdk_src(hpm_canopen_mcan.c)
endif()
