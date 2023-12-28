# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(Include)
add_subdirectory_ifdef(CONFIG_FREERTOS FreeRTOS)
