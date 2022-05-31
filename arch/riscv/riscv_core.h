/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef RISCV_CORE_H
#define RISCV_CORE_H

#include "hpm_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define write_fcsr(v) __asm volatile("fscsr %0" : : "r"(v))

#define clear_csr(csr_num, bit) __asm volatile("csrc %0, %1" : : "i"(csr_num), "r"(bit))

#define set_csr(csr_num, bit) __asm volatile("csrs %0, %1" : : "i"(csr_num), "r"(bit))

#define write_csr(csr_num, v) __asm volatile("csrw %0, %1" : : "i"(csr_num), "r"(v))

#define read_csr(csr_num) ({ uint32_t v; __asm volatile("csrr %0, %1" : "=r"(v) : "i"(csr_num)); v; })

#define read_fcsr() ({ uint32_t v; __asm volatile("frcsr %0" : "=r"(v)); v; })

#define fencei() __asm volatile("fence.i")

#ifdef __cplusplus
}
#endif


#endif /* RISCV_CORE_H */
