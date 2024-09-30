/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file klibc.c
 * \author fra11385
 * \date 12.04.2021
 * \brief <insert file brief description>
 *
 */

#include "klibc.h"
#include <kernel/base/kernel.h>

#include <stddef.h>

list_data_t *list_create()
{
    list_data_t *top = (list_data_t *)kallocz(sizeof(list_data_t));

    top->data = NULL;
    top->idx = 0;

    top->prev = top;
    top->next = top;

    return top;
}

void list_free(list_data_t *top, bool free_data)
{
    if (top == NULL)
        return;

    list_data_t *last = top->prev;
    while (last != top) {
        list_data_t *temp = last;
        last = last->prev;

        if (free_data)
            kfree(temp->data);
        kfree(temp);
    }

    if (free_data)
        kfree(top->data);
    kfree(top);
}

uint8_t list_size(list_data_t *top)
{
    return top == NULL ? 0 : top->prev->idx;
}

list_data_t *list_next(list_data_t *top, list_data_t *data)
{
    if (top == NULL || data == NULL)
        return NULL;

    list_data_t *ret = data->next;

    return ret == top ? NULL : ret;
}

list_data_t *list_prev(list_data_t *top, list_data_t *data)
{
    if (top == NULL || data == NULL)
        return NULL;

    list_data_t *ret = data->prev;

    return ret == top ? NULL : ret;
}

bool list_end(list_data_t *top, list_data_t *data)
{
    return (top == NULL) || (data == NULL) || (data == top);
}

list_data_t *list_add(list_data_t *top, void *data)
{
    if (top->data == NULL) {
        top->data = data;
        top->idx = 1;

        return top;
    }

    list_data_t *next = (list_data_t *)kallocz(sizeof(list_data_t));
    next->data = data;
    next->idx = top->prev->idx + 1;
    next->next = top;
    next->prev = top->prev;

    top->prev->next = next;
    top->prev = next;

    return next;
}

list_data_t *list_get(list_data_t *top, uint8_t idx)
{
    list_data_t *el = top;

    do {
        if (el->idx == idx)
            return el;

        el = el->next;
    } while (el != top);

    return NULL;
}

void list_del(list_data_t *top, void *data)
{
    if (top == NULL || data == NULL)
        return;
    list_data_t *el = top;

    do {
        if (el->data == data) {
            list_del_entry(top, el);
            return;
        }

        el = el->next;
    } while (el != top);

    return;
}

void list_del_entry(list_data_t *top, list_data_t *entry)
{
    if (top == NULL || entry == NULL)
        return;
    list_data_t *el = top;
    bool update_idx = false;

    if (entry == top) {
        if (entry->next == top) {
            // el == top && list_size == 1
            top->data = NULL;
            top->idx = 0;
            return;
        } else {
            // el == top && list_size > 1
            top->data = top->next->data;
            el = top->next;
            top->next->next->prev = top;
            top->next = top->next->next;
            kfree(el);
            top->idx = 1;
            update_idx = true;
        }
    }

    el = top->next;
    while (el != top) {
        if (el == entry) {
            el = el->next;
            entry->prev->next = entry->next;
            entry->next->prev = entry->prev;
            kfree(entry);
            update_idx = true;
            continue;
        }

        if (update_idx)
            el->idx = el->prev->idx + 1;

        el = el->next;
    }

    return;
}

char *ltrim(const char *str)
{
    char *ret = (char *)str;

    while (*ret == ' ')
        ret++;

    return ret;
}

list_data_t *split(const char *str, const char *delim)
{
    list_data_t *ret = list_create();

    char *part = strtok((char *)str, delim);

    while (part != NULL) {
        list_add(ret, part);
        part = strtok(NULL, delim);
    }

    return ret;
}
