/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __CUSTOM_MEMORY_MANAGER_H_
#define __CUSTOM_MEMORY_MANAGER_H_

void *pvPortRealloc(void *pv, size_t xWantedSize);
void *pvPortCalloc(size_t num, size_t xWantedSize);

#endif
