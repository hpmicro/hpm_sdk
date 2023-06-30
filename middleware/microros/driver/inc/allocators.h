/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _ALLOCATORS_H_
#define _ALLOCATORS_H_

#include <stdlib.h>

void * __freertos_allocate(size_t size, void *state);
void __freertos_deallocate(void *pointer, void *state);
void * __freertos_reallocate(void *pointer, size_t size, void *state);
void * __freertos_zero_allocate(size_t number_of_elements, size_t size_of_element, void *state);

#endif // _ALLOCATORS_H_
