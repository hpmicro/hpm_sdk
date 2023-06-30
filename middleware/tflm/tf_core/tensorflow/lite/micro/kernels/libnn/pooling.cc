/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.
   Copyright 2022 Andes Technology Corporation. All rights reserved.
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

#include "hpm_math.h"
#include "tensorflow/lite/kernels/internal/reference/pooling.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/pooling.h"

namespace tflite {

namespace {

struct OpData {
  OpDataPooling reference_op_data;

  // Index to buffer for optimizations if applicable.
  int buffer_idx;
};

void* Init(TfLiteContext* context, const char* buffer, size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(OpData));
}

void AverageEvalQuantized(TfLiteContext* context, const TfLiteNode* node,
                          const TfLitePoolParams* params, const OpData& data,
                          const TfLiteEvalTensor* input,
                          TfLiteEvalTensor* output) {
  TFLITE_DCHECK(input->type == kTfLiteUInt8 || input->type == kTfLiteInt8);

  PoolParams op_params;
  op_params.stride_height = params->stride_height;
  op_params.stride_width = params->stride_width;
  op_params.filter_height = params->filter_height;
  op_params.filter_width = params->filter_width;
  op_params.padding_values.height = data.reference_op_data.padding.height;
  op_params.padding_values.width = data.reference_op_data.padding.width;
  op_params.quantized_activation_min = data.reference_op_data.activation_min;
  op_params.quantized_activation_max = data.reference_op_data.activation_max;

  RuntimeShape input_shape = tflite::micro::GetTensorShape(input);
  RuntimeShape output_shape = tflite::micro::GetTensorShape(output);

  const int depth = MatchingDim(input_shape, 3, output_shape, 3);
  const int input_height = input_shape.Dims(1);
  const int input_width = input_shape.Dims(2);
  const int output_height = output_shape.Dims(1);
  const int output_width = output_shape.Dims(2);

hpm_nn_avepool_HWC_s8_any_act(
      input_height, input_width, output_height, output_width,
      op_params.stride_height, op_params.stride_width, op_params.filter_height, 
      op_params.filter_width, op_params.padding_values.height, op_params.padding_values.width,
      op_params.quantized_activation_min, op_params.quantized_activation_max, depth,
      (int8_t *)tflite::micro::GetTensorData<int8_t>(input),
      nullptr, tflite::micro::GetTensorData<int8_t>(output));
}

TfLiteStatus MaxPrepare(TfLiteContext* context, TfLiteNode* node) {
  TF_LITE_ENSURE_STATUS(PoolingPrepare(context, node));
  // Set buffer index to a reset value
  static_cast<OpData*>(node->user_data)->buffer_idx = -1;
  return kTfLiteOk;
}

TfLiteStatus AveragePrepare(TfLiteContext* context, TfLiteNode* node) {
  TF_LITE_ENSURE_STATUS(PoolingPrepare(context, node));

  const TfLiteTensor* input = GetInput(context, node, kPoolingInputTensor);
  TfLiteTensor* output = GetOutput(context, node, kPoolingOutputTensor);

  if (input->type == kTfLiteInt8) {
    RuntimeShape input_shape = GetTensorShape(input);
    TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);

    RuntimeShape output_shape = GetTensorShape(output);
    TFLITE_DCHECK_EQ(output_shape.DimensionsCount(), 4);

    const int depth = MatchingDim(input_shape, 3, output_shape, 3);
    const int output_width = output_shape.Dims(2);

    const int32_t buffer_size =
hpm_nn_avepool_HWC_s8_any_act_get_buffer_size(output_width, depth);

    auto* data = static_cast<OpData*>(node->user_data);
    if (buffer_size > 0) {
      TF_LITE_ENSURE_STATUS(context->RequestScratchBufferInArena(
          context, buffer_size, &data->buffer_idx));
    } else {
      data->buffer_idx = -1;
    }
  }
  return kTfLiteOk;
}


TfLiteStatus AverageEval(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->builtin_data != nullptr);
  auto* params = reinterpret_cast<TfLitePoolParams*>(node->builtin_data);

  TFLITE_DCHECK(node->user_data != nullptr);
  const OpData& data = *(static_cast<const OpData*>(node->user_data));

  const TfLiteEvalTensor* input =
      micro::GetEvalInput(context, node, kPoolingInputTensor);
  TfLiteEvalTensor* output =
      micro::GetEvalOutput(context, node, kPoolingOutputTensor);

  // Inputs and outputs share the same type, guaranteed by the converter.
  switch (input->type) {
    case kTfLiteFloat32:
      AveragePoolingEvalFloat(context, node, params, &data.reference_op_data, input, output);
      break;
    case kTfLiteInt8:
      AverageEvalQuantized(context, node, params, data, input, output);
      break;
    default:
      TF_LITE_KERNEL_LOG(context, "Input type %s is not currently supported",
                         TfLiteTypeGetName(input->type));
      return kTfLiteError;
  }
  return kTfLiteOk;
}

TfLiteStatus MaxEval(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->builtin_data != nullptr);
  auto* params = reinterpret_cast<TfLitePoolParams*>(node->builtin_data);

  TFLITE_DCHECK(node->user_data != nullptr);
  const OpDataPooling* data =
      static_cast<const OpDataPooling*>(node->user_data);

  const TfLiteEvalTensor* input =
      micro::GetEvalInput(context, node, kPoolingInputTensor);
  TfLiteEvalTensor* output =
      micro::GetEvalOutput(context, node, kPoolingOutputTensor);

  switch (input->type) {
    case kTfLiteFloat32:
      MaxPoolingEvalFloat(context, node, params, data, input, output);
      break;
    case kTfLiteInt8:
      MaxPoolingEvalQuantized(context, node, params, data, input, output);
      break;
    default:
      TF_LITE_KERNEL_LOG(context, "Type %s not currently supported.",
                         TfLiteTypeGetName(input->type));
      return kTfLiteError;
  }
  return kTfLiteOk;
}

}  // namespace 

TfLiteRegistration Register_AVERAGE_POOL_2D() {
  return {/*init=*/Init,
          /*free=*/nullptr,
          /*prepare=*/AveragePrepare,
          /*invoke=*/AverageEval,
          /*profiling_string=*/nullptr,
          /*builtin_code=*/0,
          /*custom_name=*/nullptr,
          /*version=*/0};
}

TfLiteRegistration Register_MAX_POOL_2D() {
  return {/*init=*/Init,
          /*free=*/nullptr,
          /*prepare=*/MaxPrepare,
          /*invoke=*/MaxEval,
          /*profiling_string=*/nullptr,
          /*builtin_code=*/0,
          /*custom_name=*/nullptr,
          /*version=*/0};
}

}  // namespace tflite
