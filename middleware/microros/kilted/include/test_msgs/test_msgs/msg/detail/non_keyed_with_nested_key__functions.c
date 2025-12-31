// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from test_msgs:msg/NonKeyedWithNestedKey.idl
// generated code does not contain a copyright notice
#include "test_msgs/msg/detail/non_keyed_with_nested_key__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `nested_data`
#include "test_msgs/msg/detail/keyed_string__functions.h"

bool
test_msgs__msg__NonKeyedWithNestedKey__init(test_msgs__msg__NonKeyedWithNestedKey * msg)
{
  if (!msg) {
    return false;
  }
  // nested_data
  if (!test_msgs__msg__KeyedString__init(&msg->nested_data)) {
    test_msgs__msg__NonKeyedWithNestedKey__fini(msg);
    return false;
  }
  // some_int
  return true;
}

void
test_msgs__msg__NonKeyedWithNestedKey__fini(test_msgs__msg__NonKeyedWithNestedKey * msg)
{
  if (!msg) {
    return;
  }
  // nested_data
  test_msgs__msg__KeyedString__fini(&msg->nested_data);
  // some_int
}

bool
test_msgs__msg__NonKeyedWithNestedKey__are_equal(const test_msgs__msg__NonKeyedWithNestedKey * lhs, const test_msgs__msg__NonKeyedWithNestedKey * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // nested_data
  if (!test_msgs__msg__KeyedString__are_equal(
      &(lhs->nested_data), &(rhs->nested_data)))
  {
    return false;
  }
  // some_int
  if (lhs->some_int != rhs->some_int) {
    return false;
  }
  return true;
}

bool
test_msgs__msg__NonKeyedWithNestedKey__copy(
  const test_msgs__msg__NonKeyedWithNestedKey * input,
  test_msgs__msg__NonKeyedWithNestedKey * output)
{
  if (!input || !output) {
    return false;
  }
  // nested_data
  if (!test_msgs__msg__KeyedString__copy(
      &(input->nested_data), &(output->nested_data)))
  {
    return false;
  }
  // some_int
  output->some_int = input->some_int;
  return true;
}

test_msgs__msg__NonKeyedWithNestedKey *
test_msgs__msg__NonKeyedWithNestedKey__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__NonKeyedWithNestedKey * msg = (test_msgs__msg__NonKeyedWithNestedKey *)allocator.allocate(sizeof(test_msgs__msg__NonKeyedWithNestedKey), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(test_msgs__msg__NonKeyedWithNestedKey));
  bool success = test_msgs__msg__NonKeyedWithNestedKey__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
test_msgs__msg__NonKeyedWithNestedKey__destroy(test_msgs__msg__NonKeyedWithNestedKey * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    test_msgs__msg__NonKeyedWithNestedKey__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
test_msgs__msg__NonKeyedWithNestedKey__Sequence__init(test_msgs__msg__NonKeyedWithNestedKey__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__NonKeyedWithNestedKey * data = NULL;

  if (size) {
    data = (test_msgs__msg__NonKeyedWithNestedKey *)allocator.zero_allocate(size, sizeof(test_msgs__msg__NonKeyedWithNestedKey), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = test_msgs__msg__NonKeyedWithNestedKey__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        test_msgs__msg__NonKeyedWithNestedKey__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
test_msgs__msg__NonKeyedWithNestedKey__Sequence__fini(test_msgs__msg__NonKeyedWithNestedKey__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      test_msgs__msg__NonKeyedWithNestedKey__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

test_msgs__msg__NonKeyedWithNestedKey__Sequence *
test_msgs__msg__NonKeyedWithNestedKey__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  test_msgs__msg__NonKeyedWithNestedKey__Sequence * array = (test_msgs__msg__NonKeyedWithNestedKey__Sequence *)allocator.allocate(sizeof(test_msgs__msg__NonKeyedWithNestedKey__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = test_msgs__msg__NonKeyedWithNestedKey__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
test_msgs__msg__NonKeyedWithNestedKey__Sequence__destroy(test_msgs__msg__NonKeyedWithNestedKey__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    test_msgs__msg__NonKeyedWithNestedKey__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
test_msgs__msg__NonKeyedWithNestedKey__Sequence__are_equal(const test_msgs__msg__NonKeyedWithNestedKey__Sequence * lhs, const test_msgs__msg__NonKeyedWithNestedKey__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!test_msgs__msg__NonKeyedWithNestedKey__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
test_msgs__msg__NonKeyedWithNestedKey__Sequence__copy(
  const test_msgs__msg__NonKeyedWithNestedKey__Sequence * input,
  test_msgs__msg__NonKeyedWithNestedKey__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(test_msgs__msg__NonKeyedWithNestedKey);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    test_msgs__msg__NonKeyedWithNestedKey * data =
      (test_msgs__msg__NonKeyedWithNestedKey *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!test_msgs__msg__NonKeyedWithNestedKey__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          test_msgs__msg__NonKeyedWithNestedKey__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!test_msgs__msg__NonKeyedWithNestedKey__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
