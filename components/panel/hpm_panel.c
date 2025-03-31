
/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_panel.h"
#include "hpm_clock_drv.h"

extern hpm_panel_t panel_tm070rdh13;
extern hpm_panel_t panel_cc10128007;
extern hpm_panel_t panel_mc10128007_31b;
extern hpm_panel_t panel_tm103xdgp01;

static hpm_panel_t *panel_list[] = {
#if defined(CONFIG_PANEL_RGB_TM070RDH13) && CONFIG_PANEL_RGB_TM070RDH13
    &panel_tm070rdh13,
#endif

#if defined(CONFIG_PANEL_LVDS_CC10128007) && CONFIG_PANEL_LVDS_CC10128007
    &panel_cc10128007,
#endif

#if defined(CONFIG_PANEL_MIPI_MC10128007_31B) && CONFIG_PANEL_MIPI_MC10128007_31B
    &panel_mc10128007_31b,
#endif

#if defined(CONFIG_PANEL_LVDS_TM103XDGP01) && CONFIG_PANEL_LVDS_TM103XDGP01
    &panel_tm103xdgp01,
#endif
};

hpm_panel_t *hpm_panel_find_device_default(void)
{
    if (sizeof(panel_list) > 0)
        return panel_list[0];
    return NULL;
}

hpm_panel_t *hpm_panel_find_device(const char *name)
{
    int n = sizeof(panel_list) / sizeof(panel_list[0]);

    for (int i = 0; i < n; i++)
        if (!strcmp(panel_list[i]->name, name))
            return panel_list[i];

    return NULL;
}

const char *hpm_panel_get_name(hpm_panel_t *panel)
{
    return panel->name;
}

const hpm_panel_timing_t *hpm_panel_get_timing(hpm_panel_t *panel)
{
    return &panel->timing;
}

hpm_panel_if_type_t hpm_panel_get_if_type(hpm_panel_t *panel)
{
    return panel->if_type;
}

void hpm_panel_register_interface(hpm_panel_t *panel, hpm_panel_hw_interface_t *hw_if)
{
    if (hw_if)
        memcpy(&panel->hw_if, hw_if, sizeof(*hw_if));
}

void hpm_panel_reset(hpm_panel_t *panel)
{
    if (panel->funcs.reset)
        panel->funcs.reset(panel);
}

void hpm_panel_init(hpm_panel_t *panel)
{
    if (panel->funcs.init)
        panel->funcs.init(panel);
}

void hpm_panel_power_on(hpm_panel_t *panel)
{
    if (panel->funcs.power_on)
        panel->funcs.power_on(panel);
}

void hpm_panel_power_off(hpm_panel_t *panel)
{
    if (panel->funcs.power_off)
        panel->funcs.power_off(panel);
}

void hpm_panel_set_backlight(hpm_panel_t *panel, uint16_t percent)
{
    if (percent > 100)
        percent = 100;

    if (panel->hw_if.set_backlight &&
        panel->state.backlight_percent != percent) {
        panel->hw_if.set_backlight(percent);
        panel->state.backlight_percent = percent;
    }
}

uint8_t hpm_panel_get_backlight(hpm_panel_t *panel)
{
    return panel->state.backlight_percent;
}

void hpm_panel_delay_ms(uint32_t ms)
{
    clock_cpu_delay_ms(ms);
}

void hpm_panel_delay_us(uint32_t us)
{
    clock_cpu_delay_us(us);
}

typedef struct panel_list {
    struct panel_list *pre;
    struct panel_list *nxt;
} panel_list_t;

typedef struct panel_memory_node {
    panel_list_t list;
    void *pool_addr;
    char *addr;
    uint32_t size;
    uint32_t is_alloced: 1;
    uint32_t is_used: 1;
} panel_memory_node_t;

#define PANEL_ROUNDUP(x, y) ((((x) + ((y) - 1)) / (y)) * y)
#define PANEL_MEM_ALIGN 64
#define PANEL_MEM_NODE_NUM 16

static panel_memory_node_t panel_mem_nodes[PANEL_MEM_NODE_NUM];
static panel_list_t panel_mem_pool[HPM_PANEL_MEM_TYPE_MAX];

static inline void panel_list_node_init(panel_list_t *node)
{
    node->nxt = node;
    node->pre = node;
}

static inline void panel_list_del(panel_list_t *node)
{
    node->pre->nxt = node->nxt;
    node->nxt->pre = node->pre;
}

static inline void panel_list_add(panel_list_t *head, panel_list_t *node)
{
    node->nxt = head->nxt;
    node->pre = head;
    head->nxt->pre = node;
    head->nxt = node;
}

static inline void panel_list_add_tail(panel_list_t *head, panel_list_t *node)
{
    node->nxt = head;
    node->pre = head->pre;
    head->pre->nxt = node;
    head->pre = node;
}

#define panel_list_for_each(node, head)\
for (panel_list_t *node = (head)->nxt; node != (head); node = node->nxt)

static panel_memory_node_t *panel_get_mem_node(void)
{
    panel_memory_node_t *node = NULL;

    for (uint32_t i = 0; i < PANEL_MEM_NODE_NUM; i++) {
        if (!panel_mem_nodes[i].is_used) {
            node = &panel_mem_nodes[i];
            node->is_used = 1;
            break;
        }
    }
    return node;
}

static void panel_put_mem_node(panel_memory_node_t *node)
{
    node->is_used = 0;
}

void *hpm_panel_memory_alloc(hpm_panel_mem_type_t type, uint32_t size)
{
    panel_list_t *mem_pool;
    panel_memory_node_t *_node = NULL;
    panel_memory_node_t *mem_node = NULL;
    panel_memory_node_t *split_mem_node;

    if (type > HPM_PANEL_MEM_TYPE_MAX)
        return NULL;

    mem_pool = &panel_mem_pool[type];
    if (!mem_pool->nxt || mem_pool->nxt == mem_pool)
        return NULL;

    size = PANEL_ROUNDUP(size, PANEL_MEM_ALIGN);
    panel_list_for_each(node, mem_pool) {
        _node = (panel_memory_node_t *)node;
        if (!_node->is_alloced && _node->size >= size) {
            mem_node = _node;
            mem_node->is_alloced = 1;
            break;
        }
    }

    if (!mem_node)
        return NULL;

    if (mem_node->size >= (size + PANEL_MEM_ALIGN)) {
        split_mem_node = panel_get_mem_node();
        if (split_mem_node) {
            split_mem_node->addr = mem_node->addr + size;
            split_mem_node->size = mem_node->size - size;
            split_mem_node->pool_addr = mem_node->pool_addr;
            split_mem_node->is_alloced = 0;
            panel_list_node_init(&split_mem_node->list);
            panel_list_add(&mem_node->list, &split_mem_node->list);
            mem_node->size = size;
        }
    }

    return (void *)mem_node->addr;
}

void hpm_panel_memory_free(void *addr)
{
    panel_list_t *mem_pool;
    panel_memory_node_t *_node = NULL;
    panel_memory_node_t *mem_node = NULL;
    panel_memory_node_t *pre_mem_node = NULL;
    panel_memory_node_t *nxt_mem_node = NULL;

    for (int i = 0; i < HPM_PANEL_MEM_TYPE_MAX; i++) {
        mem_pool = &panel_mem_pool[i];
        if (!mem_pool->nxt || mem_pool->nxt == mem_pool)
            continue;

        panel_list_for_each(node, mem_pool) {
            _node = (panel_memory_node_t *)node;
            if ((void *)_node->addr == addr && _node->is_alloced) {
                mem_node = _node;
                mem_node->is_alloced = 0;
                i = HPM_PANEL_MEM_TYPE_MAX;
                break;
            }
        }
    }

    if (!mem_node)
        return;

    nxt_mem_node = (panel_memory_node_t *)mem_node->list.nxt;
    if (&nxt_mem_node->list != mem_pool &&
        nxt_mem_node->pool_addr == mem_node->pool_addr &&
        !nxt_mem_node->is_alloced) {
        mem_node->size += nxt_mem_node->size;
        panel_list_del(&nxt_mem_node->list);
        panel_put_mem_node(nxt_mem_node);
    }

    /*
     * Merge to prev memory node if prev node is free.
     */
    pre_mem_node = (panel_memory_node_t *)mem_node->list.pre;
    if (&pre_mem_node->list != mem_pool &&
        pre_mem_node->pool_addr == mem_node->pool_addr &&
        !pre_mem_node->is_alloced) {
        pre_mem_node->size += mem_node->size;
        panel_list_del(&mem_node->list);
        panel_put_mem_node(mem_node);
    }
}

hpm_panel_mem_type_t hpm_panel_memory_get_type(void *addr)
{
    panel_memory_node_t *mem_node;
    hpm_panel_mem_type_t type = HPM_PANEL_MEM_TYPE_UNKNOWN;

    for (int i = 0; i < HPM_PANEL_MEM_TYPE_MAX; i++)
        panel_list_for_each(node, &panel_mem_pool[i]) {
            mem_node = (panel_memory_node_t *)node;
            if ((void *)mem_node->addr == addr) {
                type = (hpm_panel_mem_type_t)i;
                i = HPM_PANEL_MEM_TYPE_MAX;
                break;
            }
        }

    return type;
}

int hpm_panel_memory_register(hpm_panel_mem_type_t type, void *addr, uint32_t size)
{
    panel_list_t *mem_pool;
    panel_memory_node_t *mem_node;

    if (type > HPM_PANEL_MEM_TYPE_MAX)
        return -1;

    mem_pool = &panel_mem_pool[type];
    if (!mem_pool->nxt || !mem_pool->pre)
        panel_list_node_init(mem_pool);

    unsigned long aligned_addr = PANEL_ROUNDUP((unsigned long)addr, PANEL_MEM_ALIGN);
    unsigned long diff = aligned_addr - (unsigned long)addr;

    /*
     * addr exist
     */
    panel_list_for_each(node, mem_pool) {
        mem_node = (panel_memory_node_t *)node;
        if (mem_node->addr == (char *)aligned_addr) {
            return -1;
        }
    }

    mem_node = panel_get_mem_node();
    if (!mem_node)
        return -1;

    mem_node->pool_addr = addr;
    mem_node->addr = (char *)aligned_addr;
    mem_node->size = size - diff;
    mem_node->is_alloced = 0;
    panel_list_node_init(&mem_node->list);
    panel_list_add_tail(mem_pool, &mem_node->list);

    return 0;
}

