# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(finsh)

file(GLOB_RECURSE RTT_FINSH_SRC finsh/*.c)
sdk_src(${RTT_FINSH_SRC})