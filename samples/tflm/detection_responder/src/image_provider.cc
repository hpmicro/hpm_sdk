/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.
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
==============================================================================*/

#include "image_provider.h"
#include "model_settings.h"
#include "hpm_camera.h"

extern uint16_t buffer[];
extern uint32_t tf_run_inference;
extern"C"{
void start_time(void);
}
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data) {
  uint32_t x, y;
  uint16_t i;
  uint32_t data_pos;
  (void)error_reporter;
  (void)image_width;
  (void)image_height;
  (void)channels;
  i = 0;
  for ( y = 0; y < 96; y++) {
      for ( x = 0; x < 96; x++) {
          data_pos = ((y) * 3 + 150) * 800 + 200 + 4 * x;
          image_data[i] = (((buffer[data_pos] & 0x7e0)>>1) + ((buffer[data_pos] & 0x1f)>>5) + ((buffer[data_pos] & 0xf800)>>3))&0xff;
          buffer[(y + 480) * 800 + x + 520] = image_data[i];
          i++;
      }
  }
  start_time();
  return kTfLiteOk;
}
