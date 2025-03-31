# Copyright 2021-2023 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(inc)
add_subdirectory(VGLite)
add_subdirectory(VGLiteKernel)
sdk_compile_definitions(-DCONFIG_VGLITE=1)