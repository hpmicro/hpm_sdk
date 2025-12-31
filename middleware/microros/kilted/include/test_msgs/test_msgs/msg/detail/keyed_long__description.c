// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/KeyedLong.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/keyed_long__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__KeyedLong__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x66, 0x0d, 0xef, 0xa5, 0x56, 0xc3, 0xc4, 0xb1,
      0x99, 0x14, 0x98, 0xe6, 0x2b, 0xca, 0x58, 0xbf,
      0x71, 0xac, 0x6e, 0x13, 0xbc, 0xf9, 0xa4, 0x76,
      0xaa, 0x99, 0x8a, 0x8b, 0x66, 0xf6, 0x50, 0xf0,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char test_msgs__msg__KeyedLong__TYPE_NAME[] = "test_msgs/msg/KeyedLong";

// Define type names, field names, and default values
static char test_msgs__msg__KeyedLong__FIELD_NAME__key[] = "key";
static char test_msgs__msg__KeyedLong__FIELD_NAME__value[] = "value";

static rosidl_runtime_c__type_description__Field test_msgs__msg__KeyedLong__FIELDS[] = {
  {
    {test_msgs__msg__KeyedLong__FIELD_NAME__key, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__KeyedLong__FIELD_NAME__value, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__KeyedLong__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__KeyedLong__TYPE_NAME, 23, 23},
      {test_msgs__msg__KeyedLong__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "module test_msgs{\n"
  "  module msg {\n"
  "    struct KeyedLong {\n"
  "      @key long key;\n"
  "      long value;\n"
  "    };\n"
  "  };\n"
  "};";

static char idl_encoding[] = "idl";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__KeyedLong__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__KeyedLong__TYPE_NAME, 23, 23},
    {idl_encoding, 3, 3},
    {toplevel_type_raw_source, 110, 110},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__KeyedLong__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__KeyedLong__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
