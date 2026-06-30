# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
#

sdk_compile_definitions(-DCONFIG_MQUICKJS=1)

# mquickjs include directories
sdk_inc(.)
sdk_inc(port)
sdk_inc(port/peripheral/gpio)

# mquickjs source files
sdk_src(mquickjs.c)
sdk_src(cutils.c)
sdk_src(dtoa.c)
sdk_src(libm.c)
sdk_src(port/peripheral/gpio/hpm_gpio.c)
sdk_src(port/mquickjs_port.c)
