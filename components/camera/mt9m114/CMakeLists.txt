# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(hpm_mt9m114.c)
sdk_src_ifdef(CONFIG_HPM_CAMERA hpm_camera_mt9m114.c)
