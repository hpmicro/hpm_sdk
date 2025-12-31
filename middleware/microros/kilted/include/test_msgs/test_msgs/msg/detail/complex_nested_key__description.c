// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from test_msgs:msg/ComplexNestedKey.idl
// generated code does not contain a copyright notice

#include "test_msgs/msg/detail/complex_nested_key__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__ComplexNestedKey__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x67, 0xde, 0x37, 0xf2, 0x51, 0xf9, 0x43, 0x1e,
      0xd5, 0xf2, 0x63, 0x05, 0x07, 0x93, 0x9f, 0xf5,
      0x53, 0x8e, 0xcf, 0xaa, 0xa4, 0x16, 0x98, 0x49,
      0xbe, 0x18, 0x87, 0x0c, 0x3f, 0xbe, 0x2d, 0xe8,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "test_msgs/msg/detail/keyed_string__functions.h"
#include "test_msgs/msg/detail/non_keyed_with_nested_key__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t test_msgs__msg__KeyedString__EXPECTED_HASH = {1, {
    0x20, 0xc0, 0x32, 0x0c, 0x41, 0x01, 0x3b, 0xc5,
    0x16, 0x12, 0x12, 0x3c, 0x18, 0xab, 0xad, 0xac,
    0x81, 0x94, 0x4a, 0x31, 0xdb, 0x59, 0xf1, 0xca,
    0xf8, 0x17, 0x82, 0x9e, 0x2f, 0x18, 0x60, 0x43,
  }};
static const rosidl_type_hash_t test_msgs__msg__NonKeyedWithNestedKey__EXPECTED_HASH = {1, {
    0x36, 0xba, 0xf7, 0xe9, 0x33, 0xc8, 0x64, 0x7c,
    0xcf, 0x6a, 0x96, 0xe2, 0x75, 0x49, 0xdf, 0x0d,
    0xb4, 0xc6, 0xd5, 0xbc, 0x02, 0x00, 0x81, 0x9d,
    0xab, 0x4c, 0x61, 0xe6, 0xb6, 0xd9, 0x27, 0xd2,
  }};
#endif

static char test_msgs__msg__ComplexNestedKey__TYPE_NAME[] = "test_msgs/msg/ComplexNestedKey";
static char test_msgs__msg__KeyedString__TYPE_NAME[] = "test_msgs/msg/KeyedString";
static char test_msgs__msg__NonKeyedWithNestedKey__TYPE_NAME[] = "test_msgs/msg/NonKeyedWithNestedKey";

// Define type names, field names, and default values
static char test_msgs__msg__ComplexNestedKey__FIELD_NAME__uint32_key[] = "uint32_key";
static char test_msgs__msg__ComplexNestedKey__FIELD_NAME__nested_keys[] = "nested_keys";
static char test_msgs__msg__ComplexNestedKey__FIELD_NAME__float64_value[] = "float64_value";

static rosidl_runtime_c__type_description__Field test_msgs__msg__ComplexNestedKey__FIELDS[] = {
  {
    {test_msgs__msg__ComplexNestedKey__FIELD_NAME__uint32_key, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__ComplexNestedKey__FIELD_NAME__nested_keys, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {test_msgs__msg__NonKeyedWithNestedKey__TYPE_NAME, 35, 35},
    },
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__ComplexNestedKey__FIELD_NAME__float64_value, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription test_msgs__msg__ComplexNestedKey__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {test_msgs__msg__KeyedString__TYPE_NAME, 25, 25},
    {NULL, 0, 0},
  },
  {
    {test_msgs__msg__NonKeyedWithNestedKey__TYPE_NAME, 35, 35},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__ComplexNestedKey__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {test_msgs__msg__ComplexNestedKey__TYPE_NAME, 30, 30},
      {test_msgs__msg__ComplexNestedKey__FIELDS, 3, 3},
    },
    {test_msgs__msg__ComplexNestedKey__REFERENCED_TYPE_DESCRIPTIONS, 2, 2},
  };
  if (!constructed) {
    assert(0 == memcmp(&test_msgs__msg__KeyedString__EXPECTED_HASH, test_msgs__msg__KeyedString__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = test_msgs__msg__KeyedString__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&test_msgs__msg__NonKeyedWithNestedKey__EXPECTED_HASH, test_msgs__msg__NonKeyedWithNestedKey__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[1].fields = test_msgs__msg__NonKeyedWithNestedKey__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "#include \"test_msgs/msg/NonKeyedWithNestedKey.idl\"\n"
  "\n"
  "module test_msgs{\n"
  "  module msg {\n"
  "    struct ComplexNestedKey {\n"
  "      @key uint32 uint32_key;\n"
  "      @key test_msgs::msg::NonKeyedWithNestedKey nested_keys;\n"
  "      double float64_value;\n"
  "    };\n"
  "  };\n"
  "};";

static char idl_encoding[] = "idl";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__ComplexNestedKey__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {test_msgs__msg__ComplexNestedKey__TYPE_NAME, 30, 30},
    {idl_encoding, 3, 3},
    {toplevel_type_raw_source, 250, 250},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__ComplexNestedKey__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[3];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 3, 3};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *test_msgs__msg__ComplexNestedKey__get_individual_type_description_source(NULL),
    sources[1] = *test_msgs__msg__KeyedString__get_individual_type_description_source(NULL);
    sources[2] = *test_msgs__msg__NonKeyedWithNestedKey__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
