// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/KeyedString.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/keyed_string__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__KeyedString__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x20, 0xc0, 0x32, 0x0c, 0x41, 0x01, 0x3b, 0xc5,
      0x16, 0x12, 0x12, 0x3c, 0x18, 0xab, 0xad, 0xac,
      0x81, 0x94, 0x4a, 0x31, 0xdb, 0x59, 0xf1, 0xca,
      0xf8, 0x17, 0x82, 0x9e, 0x2f, 0x18, 0x60, 0x43,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char test_msgs__msg__KeyedString__TYPE_NAME[] = "test_msgs/msg/KeyedString";

// Define type names, field names, and default values
static char test_msgs__msg__KeyedString__FIELD_NAME__key[] = "key";
static char test_msgs__msg__KeyedString__FIELD_NAME__value[] = "value";

static rosidl_runtime_c__type_description__Field test_msgs__msg__KeyedString__FIELDS[] = {
  {
    {test_msgs__msg__KeyedString__FIELD_NAME__key, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__KeyedString__FIELD_NAME__value, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__KeyedString__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__KeyedString__TYPE_NAME, 25, 25},
      {test_msgs__msg__KeyedString__FIELDS, 2, 2},
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
  "    struct KeyedString {\n"
  "      @key string key;\n"
  "      string value;\n"
  "    };\n"
  "  };\n"
  "};";

static char idl_encoding[] = "idl";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__KeyedString__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__KeyedString__TYPE_NAME, 25, 25},
    {idl_encoding, 3, 3},
    {toplevel_type_raw_source, 116, 116},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__KeyedString__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__KeyedString__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
