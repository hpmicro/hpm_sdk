/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <allocators.h>
#include "custom_memory_manager.h"
#include "FreeRTOS.h"
#include "task.h"

void * __freertos_allocate(size_t size, void *state)
{
  (void) state;

  return pvPortMalloc(size);
}

void __freertos_deallocate(void *pointer, void *state)
{
  (void) state;

  if (NULL != pointer){
    vPortFree(pointer);
  }
}

void * __freertos_reallocate(void *pointer, size_t size, void *state)
{
  (void) state;

  if (NULL == pointer){
    return pvPortMalloc(size);
  } else {
    return pvPortRealloc(pointer,size);
  }
}

void * __freertos_zero_allocate(size_t number_of_elements, size_t size_of_element, void *state)
{
  (void) state;

  return pvPortCalloc(number_of_elements,size_of_element);
}
