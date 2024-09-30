# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_inc(base)
sdk_inc(kernel/driver/tsn_ipms/ep)
sdk_inc(kernel/net)
sdk_inc(tools)

sdk_src(kernel/base/kernel.c)
sdk_src(kernel/base/klibc.c)
sdk_src(kernel/base/rtc.c)
sdk_src(kernel/driver/tsn_ipms/ep/tsn_ctrl_drv.c)
sdk_src(kernel/net/ether.c)
sdk_src(kernel/net/netb.c)
sdk_src(kernel/net/netdev.c)

sdk_src(tools/ptp4tsync.c)
sdk_src(tools/traceptp.c)
