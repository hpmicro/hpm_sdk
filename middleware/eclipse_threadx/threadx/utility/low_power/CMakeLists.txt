# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(tx_low_power.c)
sdk_compile_definitions(-DTX_LOW_POWER=1)
sdk_compile_definitions(-DTX_ENABLE_WFI=1)