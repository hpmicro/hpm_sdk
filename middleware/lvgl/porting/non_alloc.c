/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "non_alloc.h"

typedef struct mem_blk_info {
    struct na_list list;
    struct mem_blk_info *pre;
    struct mem_blk_info *nxt;
    na_addr_t blk_start;
    na_size_t blk_len: 30;
#define NON_ALLOC_STATE_UNUSED 0x00
#define NON_ALLOC_STATE_FREE 0x01
#define NON_ALLOC_STATE_ALLOCATED 0x02
    na_size_t state: 2;
} mem_blk_info_t;

#define NA_MEMORY_CHECK_DEBUG 1
#if defined(NA_MEMORY_CHECK_DEBUG) && NA_MEMORY_CHECK_DEBUG
#include <stdio.h>
static void na_memory_check(non_alloc_context_t *ctx);
#endif

#define NA_NULL (void *)0
#define NON_ALLOC_MEM_INFO_ALIGN 4
#define NON_ALLOC_ALIGN(x, align) (((x) + (align) - 1) & ~((align) - 1))
#define NON_ALLOC_SIZE2BLK_LEN(size, align) (NON_ALLOC_ALIGN(size, align) + NON_ALLOC_ALIGN(sizeof(mem_blk_info_t *), align))
#define NON_ALLOC_BLK2MEM(addr, align) (void *)((addr) + NON_ALLOC_ALIGN(sizeof(mem_blk_info_t *), align))
#define NON_ALLOC_MEM2BLK(mem, align) ((na_addr_t)(mem) - NON_ALLOC_ALIGN(sizeof(mem_blk_info_t *), align))

static inline void na_list_node_init(na_list_t *node)
{
    node->nxt = node;
    node->pre = node;
}

static inline void na_list_del(na_list_t *node)
{
    node->pre->nxt = node->nxt;
    node->nxt->pre = node->pre;
}

static inline void na_list_add(na_list_t *head, na_list_t *node)
{
    node->nxt = head->nxt;
    node->pre = head;
    head->nxt->pre = node;
    head->nxt = node;
}

static inline na_list_t *na_list_node_get(na_list_t *head)
{
    na_list_t *node = NA_NULL;

    if (head->nxt != head) {
        node = head->nxt;
        na_list_del(node);
    }

    return node;
}

#define na_list_for_each(node, head)\
for (na_list_t *node = (head)->nxt; node != (head); node = node->nxt)

int na_init(non_alloc_context_t *ctx, na_size_t mem_align, void *non_addr, na_size_t non_len, void *info_addr, na_size_t info_len)
{
    mem_blk_info_t *mem_blk_info;
    na_addr_t info_addr_align;
    na_addr_t info_len_align;
    na_size_t mem_blk_info_size_align;

    if (!ctx || info_len < sizeof(mem_blk_info_t) * NON_ALLOC_MIN_MEM_INFO_NUM ||\
         non_len < NON_ALLOC_MIN_NON_MEM_LEN)
        return -1;

    ctx->mem_info_num = 0;
    ctx->align = mem_align;
    ctx->non_addr = NON_ALLOC_ALIGN((na_addr_t)non_addr, mem_align);
    ctx->non_len = non_len - ((na_addr_t)non_addr - ctx->non_addr);
    ctx->info_addr = NON_ALLOC_ALIGN((na_addr_t)info_addr, NON_ALLOC_MEM_INFO_ALIGN);
    ctx->info_len = info_len - ((na_addr_t)info_addr - ctx->info_addr);
    na_list_node_init(&ctx->unused);
    na_list_node_init(&ctx->free);
    na_list_node_init(&ctx->allocated);

    info_addr_align = ctx->info_addr;
    info_len_align = ctx->info_len;
    mem_blk_info_size_align = NON_ALLOC_ALIGN(sizeof(mem_blk_info_t), NON_ALLOC_MEM_INFO_ALIGN);
    while (info_len_align >= mem_blk_info_size_align) {
        na_list_add(&ctx->unused, (na_list_t *)info_addr_align);
        info_addr_align += mem_blk_info_size_align;
        info_len_align -= mem_blk_info_size_align;
        ctx->mem_info_num++;
    }

    mem_blk_info = (mem_blk_info_t *)na_list_node_get(&ctx->unused);
    mem_blk_info->blk_start = ctx->non_addr;
    mem_blk_info->blk_len = ctx->non_len;
    mem_blk_info->state = NON_ALLOC_STATE_FREE;
    mem_blk_info->nxt = NA_NULL;
    mem_blk_info->pre = NA_NULL;
    na_list_add(&ctx->free, (na_list_t *)mem_blk_info);

    return ctx->mem_info_num;
}

void *na_malloc(non_alloc_context_t *ctx, na_size_t size)
{
    mem_blk_info_t *mem_blk_info;
    mem_blk_info_t *free_blk = NA_NULL;
    mem_blk_info_t *allocated_blk = NA_NULL;
    void *mem = NA_NULL;

    if (!ctx || !size)
        goto ERROR;

    size = NON_ALLOC_SIZE2BLK_LEN(size, ctx->align);

    na_list_for_each(node, &ctx->free) {
        mem_blk_info = (mem_blk_info_t *)node;
        if (mem_blk_info->blk_len >= size) {
            free_blk = mem_blk_info;
            break;
        }
    }

    if (!free_blk)
        goto ERROR;

    if (free_blk->blk_len == size) {
        na_list_del((na_list_t *)free_blk);
        na_list_add(&ctx->allocated, (na_list_t *)free_blk);
        free_blk->state = NON_ALLOC_STATE_ALLOCATED;
        allocated_blk = free_blk;
    } else {
        allocated_blk = (mem_blk_info_t *)na_list_node_get(&ctx->unused);
        if (!allocated_blk)
            goto ERROR;

        allocated_blk->blk_start = free_blk->blk_start;
        allocated_blk->blk_len = size;
        free_blk->blk_start += allocated_blk->blk_len;
        free_blk->blk_len -= allocated_blk->blk_len;

        allocated_blk->nxt = free_blk;
        allocated_blk->pre = free_blk->pre;

        if (free_blk->pre)
            free_blk->pre->nxt = allocated_blk;
        free_blk->pre = allocated_blk;

        na_list_add(&ctx->allocated, (na_list_t *)allocated_blk);
        allocated_blk->state = NON_ALLOC_STATE_ALLOCATED;
    }

    /* save blk info address to noncache memory */
    *(mem_blk_info_t **)allocated_blk->blk_start = allocated_blk;
    mem = NON_ALLOC_BLK2MEM(allocated_blk->blk_start, ctx->align);
ERROR:
#if defined(NA_MEMORY_CHECK_DEBUG) && NA_MEMORY_CHECK_DEBUG
    na_memory_check(ctx);
#endif
    return mem;
}

void na_free(non_alloc_context_t *ctx, void *mem)
{
    if (!ctx || !mem)
        return;

    na_addr_t blk_start = NON_ALLOC_MEM2BLK(mem, ctx->align);
    mem_blk_info_t *free_blk = *(mem_blk_info_t **)blk_start;
    mem_blk_info_t *nxt_blk = free_blk->nxt;
    mem_blk_info_t *pre_blk = free_blk->pre;

    if (nxt_blk && (nxt_blk->state & NON_ALLOC_STATE_FREE)) {
        free_blk->blk_len += nxt_blk->blk_len;

        if (nxt_blk->nxt)
            nxt_blk->nxt->pre = free_blk;
        free_blk->nxt = nxt_blk->nxt;

        na_list_del((na_list_t *)nxt_blk);
        na_list_add(&ctx->unused, (na_list_t *)nxt_blk);
        nxt_blk->state = NON_ALLOC_STATE_UNUSED;
    }

    if (pre_blk && (pre_blk->state == NON_ALLOC_STATE_FREE)) {
        pre_blk->blk_len += free_blk->blk_len;

        if (free_blk->nxt)
            free_blk->nxt->pre = pre_blk;
        pre_blk->nxt = free_blk->nxt;

        na_list_del((na_list_t *)free_blk);
        na_list_add(&ctx->unused, (na_list_t *)free_blk);
        free_blk->state = NON_ALLOC_STATE_UNUSED;
    } else {
        na_list_del((na_list_t *)free_blk);
        na_list_add(&ctx->free, (na_list_t *)free_blk);
        free_blk->state = NON_ALLOC_STATE_FREE;
    }
#if defined(NA_MEMORY_CHECK_DEBUG) && NA_MEMORY_CHECK_DEBUG
    na_memory_check(ctx);
#endif
}

#if defined(NA_MEMORY_CHECK_DEBUG) && NA_MEMORY_CHECK_DEBUG
static void na_memory_check(non_alloc_context_t *ctx)
{
    struct check_info {
        na_size_t unused_cnt;
        na_size_t free_cnt;
        na_size_t allocated_cnt;
        na_size_t free_size;
        na_size_t allocated_size;
    };

    struct check_info info = {0};
    mem_blk_info_t *blk_info;

    na_list_for_each(node, &ctx->unused) {
        info.unused_cnt++;
    }

    na_list_for_each(node, &ctx->free) {
        info.free_cnt++;
        blk_info = (mem_blk_info_t *)node;
        info.free_size += blk_info->blk_len;
    }

    na_list_for_each(node, &ctx->allocated) {
        info.allocated_cnt++;
        blk_info = (mem_blk_info_t *)node;
        info.allocated_size += blk_info->blk_len;
    }

    if (info.free_cnt + info.allocated_cnt + info.unused_cnt != ctx->mem_info_num) {
        printf("[%s]: mem_info_num[%ld] != free_cnt[%ld] + allocated_cnt[%ld] + unsed_cnt[%ld]\n",\
            __FUNCTION__, ctx->mem_info_num, info.free_cnt, info.allocated_cnt, info.unused_cnt);
    }

    if (info.free_size + info.allocated_size != ctx->non_len) {
        printf("[%s]: non_len[%ld] != free_size[%ld] + allocated_size[%ld]\n",\
            __FUNCTION__, ctx->non_len, info.free_size, info.allocated_size);
    }
}

#endif
