/*
Copyright 2020 EEMBC and The MLPerf Authors. All Rights Reserved.
Copyright (c) 2022 HPMicro
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file reflects a modified version of th_lib from EEMBC. The reporting logic
in th_results is copied from the original in EEMBC.
==============================================================================*/
/// \file
/// \brief C++ implementations of submitter_implemented.h

#include "./api/submitter_implemented.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdarg.h>

#include "api/internally_implemented.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "util/tf_micro_model_runner.h"
#include "util/quantization_helpers.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "./ad/micro_inputs.h"
#include "./ad/model.h"

#include <stdio.h>
extern "C" {
#include "board.h"
}
volatile unsigned char gstr[0x100] __attribute__((aligned(4))) = {0};
volatile unsigned char gstr1[0x100] __attribute__((aligned(4))) = {0};

#define timestamp_in_usec ((unsigned long)(((unsigned long long )hpm_csr_get_core_cycle() * (unsigned long long) 1000000) / (unsigned long long) BOARD_CPU_FREQ))

tflite::MicroModelRunner<int8_t, int8_t, 3> *runner;

constexpr int kTensorArenaSize = 10 * 1024;
uint8_t tensor_arena_local[kTensorArenaSize];

typedef int8_t model_input_t;
typedef int8_t model_output_t;


tflite::ErrorReporter* error_reporter_local = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
float input_float[kInputSize];


// Implement this method to prepare for inference and preprocess inputs.
void th_load_tensor() {
  int8_t input_quantized[kInputSize];
  size_t bytes;

  bytes = ee_get_buffer(reinterpret_cast<uint8_t *>(input_float),
                         kInputSize * sizeof(float));

  if (bytes / sizeof(float) != kInputSize) {
    th_printf("Input db has %d elemented, expected %d\n", bytes / sizeof(float),
              kInputSize);
    return;
  }

  float input_scale = runner->input_scale();
  int input_zero_point = runner->input_zero_point();
  for (int i = 0; i < kInputSize; i++) {
    input_quantized[i] = QuantizeFloatToInt8(
        input_float[i], input_scale, input_zero_point);
  }

  runner->SetInput(input_quantized);

}


// Add to this method to return real inference results.
void th_results() {
  float result;
  int8_t *output_buf;
  float diffsum = 0;
  float output_scale;
  int output_zero_point;

  output_scale = runner->output_scale();
  output_zero_point = runner->output_zero_point();

  output_buf = runner->GetOutput();
  for (size_t i = 0; i < kFeatureElementCount; i++) {
    float converted = DequantizeInt8ToFloat(output_buf[i], output_scale, output_zero_point);
    float diff = converted - input_float[i];
    diffsum += diff * diff;
  }
  diffsum /= kFeatureElementCount;

  result = diffsum;

  th_printf("m-results-[%0.3f]\r\n", (double) result);
}

// Implement this method with the logic to perform one inference cycle.
void th_infer() { runner->Invoke(); }

/// \brief optional API.
void th_final_initialize(void) {

  static tflite::MicroMutableOpResolver<3> resolver;

  resolver.AddFullyConnected();
  resolver.AddQuantize();
  resolver.AddDequantize();

  static tflite::MicroModelRunner<int8_t, int8_t, 3> model_runner(
         g_model, resolver, tensor_arena_local, kTensorArenaSize);
  runner = &model_runner;

  th_printf("Initialized\r\n");
}

void th_pre() {}
void th_post() {}

void th_command_ready(char volatile *p_command) {
  p_command = p_command;
  ee_serial_command_parser_callback((char *)p_command);
}

// th_libc implementations.
int th_strncmp(const char *str1, const char *str2, size_t n) {
  return strncmp(str1, str2, n);
}

char *th_strncpy(char *dest, const char *src, size_t n) {
  return strncpy(dest, src, n);
}

size_t th_strnlen(const char *sstr, size_t maxlen) {
  return strlen(sstr);
}

char *th_strcat(char *dest, const char *src) { return strcat(dest, src); }

char *th_strtok(char *str1, const char *sep) { return strtok(str1, sep); }

int th_atoi(const char *str) { return atoi(str); }

void *th_memset(void *b, int c, size_t len) { return memset(b, c, len); }

void *th_memcpy(void *dst, const void *src, size_t n) {
  return memcpy(dst, src, n);
}


int th_vprintf(const char *format, va_list ap) {
	vsprintf((char *)gstr1, format, ap);
	printf((char *)gstr1);
	return 0;
}

void th_printf(const char *p_fmt, ...) {
  va_list args;
  va_start(args, p_fmt);
  (void)th_vprintf(p_fmt, args); /* ignore return */
  va_end(args);
}

char th_getchar() { return (char) getchar(); }

void th_serialport_initialize(void) {
  board_init();

  tflite::InitializeTarget();
}

void th_timestamp(void) {
# if EE_CFG_ENERGY_MODE==1

# else
  unsigned long microSeconds = 0ul;
  /* USER CODE 2 BEGIN */
  //microSeconds = us_ticker_read();
  microSeconds = timestamp_in_usec;
  /* USER CODE 2 END */
  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP, microSeconds);
# endif
}

void th_timestamp_initialize(void) {
  /* USER CODE 1 BEGIN */
  // Setting up BOTH perf and energy here
  /* USER CODE 1 END */
  /* This message must NOT be changed. */
  th_printf(EE_MSG_TIMESTAMP_MODE);
  /* Always call the timestamp on initialize so that the open-drain output
     is set to "1" (so that we catch a falling edge) */
  th_timestamp();
}
