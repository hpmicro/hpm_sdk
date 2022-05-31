# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(src_common)
sdk_src(src_common/freemaster_bdm.c)
sdk_src(src_common/freemaster_rec.c)
sdk_src(src_common/freemaster_scope.c)
sdk_src(src_common/freemaster_lin.c)
sdk_src(src_common/freemaster_sfio.c)
sdk_src(src_common/freemaster_pipes.c)
sdk_src(src_common/freemaster_appcmd.c)
sdk_src(src_common/freemaster_can.c)
sdk_src(src_common/freemaster_protocol.c)
sdk_src(src_common/freemaster_tsa.c)
sdk_src(src_common/freemaster_serial.c)

sdk_inc(src_platforms/hpm)
sdk_src(src_platforms/hpm/freemaster_hpm.c)
