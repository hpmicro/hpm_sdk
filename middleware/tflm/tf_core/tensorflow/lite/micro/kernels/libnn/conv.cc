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

#include "tensorflow/lite/micro/kernels/conv.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/conv.h"
#include "tensorflow/lite/kernels/internal/reference/integer_ops/conv.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/padding.h"
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

  int32_t buf_size = 0;
  const auto& params =
      *(static_cast<const TfLiteConvParams*>(node->builtin_data));
  OpData* data = static_cast<OpData*>(node->user_data);

  const TfLiteTensor* input = GetInput(context, node, kConvInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  const TfLiteTensor* filter = GetInput(context, node, kConvWeightsTensor);
  TF_LITE_ENSURE(context, filter != nullptr);
  const TfLiteTensor* output = GetOutput(context, node, kConvOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);

  RuntimeShape input_shape = GetTensorShape(input);
  RuntimeShape output_shape = GetTensorShape(output);

  int input_width = input->dims->data[2];
  int input_height = input->dims->data[1];
  int filter_width = filter->dims->data[2];
  int filter_height = filter->dims->data[1];
  int output_width = output->dims->data[2];
  int output_height = output->dims->data[1];

  if (input->type == kTfLiteInt8) {
    const int num_channels = filter->dims->data[kConvQuantizedDimension];
    data->reference_op_data.per_channel_output_multiplier =
        static_cast<int32_t*>(context->AllocatePersistentBuffer(
            context, num_channels * sizeof(int32_t)));
    data->reference_op_data.per_channel_output_shift =
        static_cast<int32_t*>(context->AllocatePersistentBuffer(
            context, num_channels * sizeof(int32_t)));
  }

  TF_LITE_ENSURE_STATUS(CalculateOpDataConv(
      context, node, params, input_width, input_height, filter_width,
      filter_height, output_width, output_height, input->type,
      &data->reference_op_data));

  if (input->type == kTfLiteInt8) {
     if ((data->reference_op_data.padding.width == 0) &&
         (data->reference_op_data.padding.height == 0) &&
         (input_shape.Dims(3) % 4 == 0) &&
         (params.stride_width == 1) &&
         (params.stride_height == 1) &&
         (filter_width == 1) &&
         (filter_height == 1)) {
        buf_size =hpm_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any_get_buffer_size(input_shape.Dims(3));
     }
     else if ((output_height == 1) &&
             (input_height == 1) &&
             (filter_height == 1) &&
             (output_width % 4 == 0) &&
             (MatchingDim(input_shape, 0, output_shape, 0) == 1)) {
        buf_size =hpm_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(input_shape.Dims(3), filter_width, filter_height);
     }
     else {
        buf_size =hpm_nn_conv_HWC_s8_s8_s8_asym_bias_any_get_buffer_size(input_shape.Dims(3), filter_width, filter_height);
     }
     if (buf_size > 0) {
       TF_LITE_ENSURE_STATUS(context->RequestScratchBufferInArena(
           context, buf_size, &data->buffer_idx));
     } else {
       data->buffer_idx = -1;
     }
  }

  return kTfLiteOk;
}  // namespace conv

TfLiteStatus EvalQuantizedPerChannel(
    TfLiteContext* context, TfLiteNode* node, const TfLiteConvParams& params,
    const OpData& data, const TfLiteEvalTensor* input,
    const TfLiteEvalTensor* filter, const TfLiteEvalTensor* bias,
    TfLiteEvalTensor* output, TfLiteEvalTensor* im2col) 
{

  ConvParams op_params;
  op_params.input_offset = -data.reference_op_data.input_zero_point;
  op_params.output_offset = data.reference_op_data.output_zero_point;
  op_params.stride_height = params.stride_height;
  op_params.stride_width = params.stride_width;
  op_params.dilation_height_factor = params.dilation_height_factor;
  op_params.dilation_width_factor = params.dilation_width_factor;
  op_params.padding_values.height = data.reference_op_data.padding.height;
  op_params.padding_values.width = data.reference_op_data.padding.width;
  op_params.quantized_activation_min = data.reference_op_data.output_activation_min;
  op_params.quantized_activation_max = data.reference_op_data.output_activation_max;


  if ((op_params.dilation_height_factor != 1) || (op_params.dilation_width_factor != 1)) {
    reference_integer_ops::ConvPerChannel(
        ConvParamsQuantized(params, data.reference_op_data),
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
      q15_t *buffer_a = nullptr;

      if (data.buffer_idx > -1) {
         buffer_a = (q15_t *)context->GetScratchBuffer(context, data.buffer_idx);
      }

      int input_width = input->dims->data[2];
      int input_height = input->dims->data[1];
      int filter_width = filter->dims->data[2];
      int filter_height = filter->dims->data[1];
      int output_width = output->dims->data[2];
      int output_height = output->dims->data[1];

      RuntimeShape filter_shape = tflite::micro::GetTensorShape(filter);
      RuntimeShape input_shape = tflite::micro::GetTensorShape(input);
      RuntimeShape output_shape = tflite::micro::GetTensorShape(output);
      RuntimeShape bias_shape = tflite::micro::GetTensorShape(bias);

      const int input_batches = MatchingDim(input_shape, 0, output_shape, 0);
      const int input_ch = MatchingDim(input_shape, 3, filter_shape, 3);
      const int output_ch = MatchingDim(filter_shape, 0, output_shape, 3);

      if ((data.reference_op_data.padding.width == 0) &&
         (data.reference_op_data.padding.height == 0) &&
         (input_shape.Dims(3) % 4 == 0) &&
         (params.stride_width == 1) &&
         (params.stride_height == 1) &&
         (filter_width == 1) &&
         (filter_height == 1)) {
hpm_nn_conv_1x1_HWC_s8_s8_s8_asym_bias_fast_any(
	         tflite::micro::GetTensorData<int8_t>(input),
                 input_width,
                 input_height,
                 input_ch,
                 input_batches,
                 tflite::micro::GetTensorData<int8_t>(filter),
                 output_ch,
                 op_params.padding_values.width,
                 op_params.padding_values.height,
                 params.stride_width,
                 params.stride_height,
                 tflite::micro::GetTensorData<int32_t>(bias),
                 tflite::micro::GetTensorData<int8_t>(output),
                 data.reference_op_data.per_channel_output_shift,
                 data.reference_op_data.per_channel_output_multiplier,
                 op_params.output_offset,
                 op_params.input_offset,
                 op_params.quantized_activation_min,
                 op_params.quantized_activation_max,
                 output_width,
                 output_height,
                 buffer_a);
      }
      else if ((output_height == 1) &&
             (input_height == 1) &&
             (filter_height == 1) &&
             (output_width % 4 == 0) &&
             (MatchingDim(input_shape, 0, output_shape, 0) == 1)) {
hpm_nn_conv_1xn_HWC_s8_s8_s8_asym_bias_any(
		     tflite::micro::GetTensorData<int8_t>(input),
                     input_width,
                     input_ch,
                     input_batches,
                     tflite::micro::GetTensorData<int8_t>(filter),
                     output_ch,
                     filter_width,
                     op_params.padding_values.width,
                     params.stride_width,
                     tflite::micro::GetTensorData<int32_t>(bias),
                     tflite::micro::GetTensorData<int8_t>(output),
                     data.reference_op_data.per_channel_output_shift,
                     data.reference_op_data.per_channel_output_multiplier,
                     op_params.output_offset,
                     op_params.input_offset,
                     op_params.quantized_activation_min,
                     op_params.quantized_activation_max,
                     output_width,
                     buffer_a);
      }
      else {
hpm_nn_conv_HWC_s8_s8_s8_asym_bias_any(
	      tflite::micro::GetTensorData<int8_t>(input),
              input_width,
              input_height,
              input_ch,
              input_batches,
              tflite::micro::GetTensorData<int8_t>(filter),
              output_ch,
              filter_width,
              filter_height,
              op_params.padding_values.width,
              op_params.padding_values.height,
              params.stride_width,
              params.stride_height,
              tflite::micro::GetTensorData<int32_t>(bias),
              tflite::micro::GetTensorData<int8_t>(output),
              data.reference_op_data.per_channel_output_shift,
              data.reference_op_data.per_channel_output_multiplier,
              op_params.output_offset,
              op_params.input_offset,
              op_params.quantized_activation_min,
              op_params.quantized_activation_max,
              output_width,
              output_height,
              buffer_a);
      }
  }

  return kTfLiteOk;
}

TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kConvInputTensor);
  const TfLiteEvalTensor* filter =
      tflite::micro::GetEvalInput(context, node, kConvWeightsTensor);
  const TfLiteEvalTensor* bias =
      (NumInputs(node) == 3)
          ? tflite::micro::GetEvalInput(context, node, kConvBiasTensor)
          : nullptr;
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kConvOutputTensor);

  TFLITE_DCHECK(node->builtin_data != nullptr);
  const auto& params =
      *(reinterpret_cast<TfLiteConvParams*>(node->builtin_data));
  TFLITE_DCHECK(node->user_data != nullptr);
  const auto& data = *(static_cast<const OpData*>(node->user_data));

  TF_LITE_ENSURE_EQ(context, input->type, output->type);
  TF_LITE_ENSURE_MSG(
      context,
      input->type == filter->type ||
          (input->type == kTfLiteInt16 && filter->type == kTfLiteInt8),
      "Hybrid models are not supported on TFLite Micro.");

  switch (input->type) {  // Already know in/out types are same.
    case kTfLiteFloat32: {
      tflite::reference_ops::Conv(
          ConvParamsFloat(params, data.reference_op_data), 
	  tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<float>(input),
          tflite::micro::GetTensorShape(filter),
          tflite::micro::GetTensorData<float>(filter),
          tflite::micro::GetTensorShape(bias),
          tflite::micro::GetTensorData<float>(bias),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<float>(output),
          tflite::micro::GetTensorShape(nullptr), nullptr);
      break;
    }
    case kTfLiteInt16: {
      reference_integer_ops::ConvPerChannel(
          ConvParamsQuantized(params, data.reference_op_data), 
	  data.reference_op_data.per_channel_output_multiplier,
          data.reference_op_data.per_channel_output_shift, tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<int16_t>(input),
          tflite::micro::GetTensorShape(filter),
          tflite::micro::GetTensorData<int8_t>(filter),
          tflite::micro::GetTensorShape(bias),
          tflite::micro::GetTensorData<std::int64_t>(bias),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int16_t>(output));
      break;
    }
    case kTfLiteInt8: {
      EvalQuantizedPerChannel(context, node, params, data, input, filter, bias, output, nullptr);
      break;
    }
    default:
      TF_LITE_KERNEL_LOG(context, "Type %s (%d) not supported.",
                         TfLiteTypeGetName(input->type), input->type);
      return kTfLiteError;
  }
  return kTfLiteOk;
}

}  // namespace

TfLiteRegistration Register_CONV_2D() {
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
