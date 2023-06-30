/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.
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

#include "tensorflow/lite/micro/kernels/depthwise_conv.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/depthwiseconv_float.h"
#include "tensorflow/lite/kernels/internal/reference/integer_ops/depthwise_conv.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/padding.h"
#include "tensorflow/lite/micro/kernels/conv.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"

namespace tflite {
namespace {

struct OpData {
  OpDataConv reference_op_data;

  // Index to buffer for optimizations if applicable.
  int buffer_idx;
};

void* Init(TfLiteContext* context, const char* buffer, size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(OpData));
}

TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);
  TFLITE_DCHECK(node->builtin_data != nullptr);

  OpData* data = static_cast<OpData*>(node->user_data);
  const auto& params =
      *(reinterpret_cast<TfLiteDepthwiseConvParams*>(node->builtin_data));

  const TfLiteTensor* input =
      GetInput(context, node, kDepthwiseConvInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  const TfLiteTensor* filter =
      GetInput(context, node, kDepthwiseConvWeightsTensor);
  TF_LITE_ENSURE(context, filter != nullptr);
  TfLiteTensor* output = GetOutput(context, node, kDepthwiseConvOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);

  const TfLiteType data_type = input->type;
  int input_width = SizeOfDimension(input, 2);
  int input_height = SizeOfDimension(input, 1);
  int filter_width = SizeOfDimension(filter, 2);
  int filter_height = SizeOfDimension(filter, 1);
  int output_width = SizeOfDimension(output, 2);
  int output_height = SizeOfDimension(output, 1);

  if (input->type == kTfLiteInt8) {
    TF_LITE_ENSURE_EQ(context, filter->quantization.type,
                      kTfLiteAffineQuantization);

    // All per-channel quantized tensors need valid zero point and scale arrays.
    const auto* affine_quantization =
        reinterpret_cast<TfLiteAffineQuantization*>(
            filter->quantization.params);
    TF_LITE_ENSURE(context, affine_quantization);
    TF_LITE_ENSURE(context, affine_quantization->scale);
    TF_LITE_ENSURE(context, affine_quantization->zero_point);
    TF_LITE_ENSURE(
        context, affine_quantization->scale->size == 1 ||
                     affine_quantization->scale->size ==
                         filter->dims->data[kDepthwiseConvQuantizedDimension]);
    TF_LITE_ENSURE_EQ(context, affine_quantization->scale->size,
                      affine_quantization->zero_point->size);

    // Allocate memory for per-channel quantization parameters
    const int num_channels =
        filter->dims->data[kDepthwiseConvQuantizedDimension];

    data->reference_op_data.per_channel_output_multiplier =
        reinterpret_cast<int32_t*>(context->AllocatePersistentBuffer(
            context, num_channels * sizeof(int32_t)));
    data->reference_op_data.per_channel_output_shift =
        reinterpret_cast<int32_t*>(context->AllocatePersistentBuffer(
            context, num_channels * sizeof(int32_t)));
  }

  TF_LITE_ENSURE_STATUS(CalculateOpDataDepthwiseConv(
      context, node, params, input_width, input_height, filter_width,
      filter_height, output_width, output_height, data_type,
      &data->reference_op_data));

  if (input->type == kTfLiteInt8) {
     int32_t buf_size = 0;
     RuntimeShape input_shape = GetTensorShape(input);

     buf_size =hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(input_shape.Dims(3), filter_width, filter_height);
     if (buf_size > 0) {
       TF_LITE_ENSURE_STATUS(context->RequestScratchBufferInArena(
           context, buf_size, &data->buffer_idx));
     } else {
       data->buffer_idx = -1;
     }
  }

  return kTfLiteOk;
}

void EvalQuantizedPerChannel(TfLiteContext* context, TfLiteNode* node,
                             const TfLiteDepthwiseConvParams& params,
                             const OpData& data, const TfLiteEvalTensor* input,
                             const TfLiteEvalTensor* filter,
                             const TfLiteEvalTensor* bias,
                             TfLiteEvalTensor* output) {
  DepthwiseParams op_params;
  op_params.dilation_height_factor = params.dilation_height_factor;
  op_params.dilation_width_factor = params.dilation_width_factor;

  if (1 == op_params.dilation_height_factor && 1 == op_params.dilation_width_factor) {
    op_params.input_offset = -data.reference_op_data.input_zero_point;
    op_params.output_offset = data.reference_op_data.output_zero_point;
    op_params.stride_height = params.stride_height;
    op_params.stride_width = params.stride_width;
    op_params.padding_values.height = data.reference_op_data.padding.height;
    op_params.padding_values.width = data.reference_op_data.padding.width;
    op_params.quantized_activation_min = std::numeric_limits<int8_t>::min();
    op_params.quantized_activation_max = std::numeric_limits<int8_t>::max();
    op_params.depth_multiplier = params.depth_multiplier;

    //op_params.padding_type = PaddingType::kSame;
    //op_params.weights_offset = 0;

    RuntimeShape filter_shape = tflite::micro::GetTensorShape(filter);
    RuntimeShape input_shape = tflite::micro::GetTensorShape(input);
    RuntimeShape output_shape = tflite::micro::GetTensorShape(output);
    RuntimeShape bias_shape = tflite::micro::GetTensorShape(bias);

    const int filter_height = filter_shape.Dims(1);
    const int filter_width = filter_shape.Dims(2);
    const int input_height = input_shape.Dims(1);
    const int input_width = input_shape.Dims(2);
    const int input_depth = input_shape.Dims(3);
    const int output_height = output_shape.Dims(1);
    const int output_width = output_shape.Dims(2);
    const int output_depth = MatchingDim(filter_shape, 3, output_shape, 3);
    const int batch_size = MatchingDim(input_shape, 0, output_shape, 0);

    int16_t* buf = nullptr;
    if (data.buffer_idx > -1) {
        buf = (int16_t *)context->GetScratchBuffer(context, data.buffer_idx);
    }

    if (input_depth != output_depth) {
        reference_integer_ops::DepthwiseConvPerChannel(
            DepthwiseConvParamsQuantized(params, data.reference_op_data),
            data.reference_op_data.per_channel_output_multiplier,
            data.reference_op_data.per_channel_output_shift,
            tflite::micro::GetTensorShape(input),
            tflite::micro::GetTensorData<int8_t>(input),
            tflite::micro::GetTensorShape(filter),
            tflite::micro::GetTensorData<int8_t>(filter),
            tflite::micro::GetTensorShape(bias),
            tflite::micro::GetTensorData<int32_t>(bias),
            tflite::micro::GetTensorShape(output),
            tflite::micro::GetTensorData<int8_t>(output));
    }
    else {
        if ((op_params.depth_multiplier == 1) && (batch_size == 1)) {
hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_fast_any(
                tflite::micro::GetTensorData<int8_t>(input), input_width, input_height,
                input_depth, tflite::micro::GetTensorData<int8_t>(filter), output_depth,
                filter_width, filter_height, op_params.padding_values.width,
                op_params.padding_values.height, op_params.stride_width,
                op_params.stride_height, tflite::micro::GetTensorData<int32_t>(bias),
                tflite::micro::GetTensorData<int8_t>(output), data.reference_op_data.per_channel_output_shift,
                data.reference_op_data.per_channel_output_multiplier, output_width, output_height,
                op_params.output_offset, op_params.input_offset,
                op_params.quantized_activation_min,
                op_params.quantized_activation_max, op_params.dilation_width_factor,
                op_params.dilation_height_factor, buf);
        } else {
hpm_nn_conv_dw_HWC_s8_s8_s8_asym_bias_any(
                tflite::micro::GetTensorData<int8_t>(input), input_width, input_height,
                input_depth, tflite::micro::GetTensorData<int8_t>(filter),
                output_depth,
                op_params.depth_multiplier, filter_width, filter_height,
                op_params.padding_values.width, op_params.padding_values.height,
                op_params.stride_width, op_params.stride_height,
                tflite::micro::GetTensorData<int32_t>(bias), tflite::micro::GetTensorData<int8_t>(output),
                data.reference_op_data.per_channel_output_shift, data.reference_op_data.per_channel_output_multiplier,
                output_width, output_height, op_params.output_offset,
                op_params.input_offset, op_params.quantized_activation_min,
                op_params.quantized_activation_max, op_params.dilation_width_factor,
                op_params.dilation_height_factor, nullptr);
        }
      } 
  } else {
    reference_integer_ops::DepthwiseConvPerChannel(
        DepthwiseConvParamsQuantized(params, data.reference_op_data),
        data.reference_op_data.per_channel_output_multiplier,
        data.reference_op_data.per_channel_output_shift,
        tflite::micro::GetTensorShape(input),
        tflite::micro::GetTensorData<int8_t>(input),
        tflite::micro::GetTensorShape(filter),
        tflite::micro::GetTensorData<int8_t>(filter),
        tflite::micro::GetTensorShape(bias),
        tflite::micro::GetTensorData<int32_t>(bias),
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<int8_t>(output));
  }

}

TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);
  TFLITE_DCHECK(node->builtin_data != nullptr);

  const auto& params =
      *(reinterpret_cast<TfLiteDepthwiseConvParams*>(node->builtin_data));
  const OpData& data = *(static_cast<OpData*>(node->user_data));

  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kDepthwiseConvOutputTensor);
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kDepthwiseConvInputTensor);
  const TfLiteEvalTensor* filter =
      tflite::micro::GetEvalInput(context, node, kDepthwiseConvWeightsTensor);
  const TfLiteEvalTensor* bias =
      (NumInputs(node) == 3)
          ? tflite::micro::GetEvalInput(context, node, kDepthwiseConvBiasTensor)
          : nullptr;

  switch (input->type) {  // Already know in/out types are same.
    case kTfLiteFloat32: {
      tflite::reference_ops::DepthwiseConv(
          DepthwiseConvParamsFloat(params, data.reference_op_data),
          tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<float>(input),
          tflite::micro::GetTensorShape(filter),
          tflite::micro::GetTensorData<float>(filter),
          tflite::micro::GetTensorShape(bias),
          tflite::micro::GetTensorData<float>(bias),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<float>(output));
      break;
    }
    case kTfLiteInt8:
      EvalQuantizedPerChannel(context, node, params, data, input, filter, bias, output);
      break;
    default:
      TF_LITE_KERNEL_LOG(context, "Type %s (%d) not supported.",
                         TfLiteTypeGetName(input->type), input->type);
      return kTfLiteError;
  }
  return kTfLiteOk;
}

}  // namespace

TfLiteRegistration Register_DEPTHWISE_CONV_2D() {
  return {/*init=*/Init,
          /*free=*/nullptr,
          /*prepare=*/Prepare,
          /*invoke=*/Eval,
          /*profiling_string=*/nullptr,
          /*builtin_code=*/0,
          /*custom_name=*/nullptr,
          /*version=*/0};
}

}  // namespace tflite
