# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(lodepng.c)

sdk_compile_definitions(-DLODEPNG_NO_COMPILE_DISK)