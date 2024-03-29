# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(src)
sdk_inc(src/dep)

sdk_app_src(src/arith.c)
sdk_app_src(src/bmc.c)
sdk_app_src(src/protocol.c)
sdk_app_src(src/ptpd.c)

sdk_app_src(src/dep/msg.c)
sdk_app_src(src/dep/net.c)
sdk_app_src(src/dep/servo.c)
sdk_app_src(src/dep/startup.c)
sdk_app_src(src/dep/time.c)
sdk_app_src(src/dep/timer.c)