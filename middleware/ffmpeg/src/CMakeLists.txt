# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(libavcodec)
add_subdirectory(libavformat)
add_subdirectory(libavutil)
add_subdirectory(heap)
sdk_inc(.)
sdk_inc(compat)
sdk_inc(libavcodec)
sdk_inc(libavformat)
sdk_inc(libavutil)