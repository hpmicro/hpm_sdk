# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(include)

aux_source_directory(src SRC_LIST_TINY_ENGINE)
aux_source_directory(src/kernels/fp_backward_op SRC_LIST_TINY_ENGINE)
aux_source_directory(src/kernels/fp_requantize_op SRC_LIST_TINY_ENGINE)
aux_source_directory(src/kernels/int_forward_op SRC_LIST_TINY_ENGINE)
sdk_src(${SRC_LIST_TINY_ENGINE})
