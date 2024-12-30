# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_inc(../ports/hpmicro)
sdk_inc(../ports/hpmicro/sample_application)

sdk_src(
    cpf.c
    encap.c
    endianconv.c
)
