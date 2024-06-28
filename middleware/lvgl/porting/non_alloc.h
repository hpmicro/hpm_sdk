/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef NON_ALLOC_H
#define NON_ALLOC_H

#define NON_ALLOC_MIN_NON_MEM_LEN 128
#define NON_ALLOC_MIN_MEM_INFO_NUM 32

#include <stdint.h>
#if UINTPTR_MAX == 0xffffffff
#define NA_INFO_CELL_SIZE 24
#elif UINTPTR_MAX == 0xffffffffffffffff
#define NA_INFO_CELL_SIZE 48
#else
#error "Unknown architecture"
#endif

typedef unsigned long na_addr_t;
typedef unsigned long na_size_t;

typedef struct na_list {
    struct na_list *pre;
    struct na_list *nxt;
} na_list_t;

typedef struct non_alloc_context {
    na_addr_t non_addr;
    na_size_t non_len;
    na_addr_t info_addr;
    na_addr_t info_len;
    struct na_list unused;
    struct na_list free;
    struct na_list allocated;
    na_size_t align;
    na_size_t mem_info_num;
} non_alloc_context_t;

int na_init(non_alloc_context_t *ctx, na_size_t mem_align, void *non_addr, na_size_t non_len, void *info_addr, na_size_t info_len);
void *na_malloc(non_alloc_context_t *ctx, na_size_t size);
void na_free(non_alloc_context_t *ctx, void *mem);

#endif

