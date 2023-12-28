# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory_ifdef(CONFIG_PTPD_V1 v1)
add_subdirectory_ifdef(CONFIG_PTPD_V2 v2)

