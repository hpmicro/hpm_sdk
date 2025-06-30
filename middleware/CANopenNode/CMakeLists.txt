# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(CANopen.c)
add_subdirectory(301)
add_subdirectory(303)
add_subdirectory(304)
add_subdirectory(305)
add_subdirectory(309)
add_subdirectory(extra)
add_subdirectory(port)
add_subdirectory(storage)
