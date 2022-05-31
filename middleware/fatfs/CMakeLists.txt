# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(src/common)
sdk_src(src/common/ff.c)
sdk_src(src/common/ffunicode.c)

add_subdirectory(src/portable)
