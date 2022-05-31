# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(${HPM_SOC})
sdk_src(${HPM_SOC}/lv_port_disp.c)
sdk_src_ifdef(CONFIG_LV_INDEV ${HPM_SOC}/lv_port_indev.c)
