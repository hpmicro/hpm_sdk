# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_DMA_MGR)
sdk_compile_definitions(-DUSE_I2C_DMA_MGR=1)
endif()

sdk_inc(.)
sdk_src(hpm_i2c.c)

