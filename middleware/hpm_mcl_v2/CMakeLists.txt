# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_ld_options("-lm")
sdk_inc(.)
add_subdirectory(encoder)
add_subdirectory(drivers)
add_subdirectory(core)
