# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(lib)
sdk_inc(port/${HPM_SOC})
add_subdirectory(port/${HPM_SOC})
add_subdirectory(lib)
