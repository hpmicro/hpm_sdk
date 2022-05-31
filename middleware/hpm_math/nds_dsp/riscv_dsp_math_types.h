/**
 * Copyright (C) 2020-2021 Andes Technology Corporation
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __RISCV_DSP_MATH_TYPES_H__
#define __RISCV_DSP_MATH_TYPES_H__
#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef int8_t    q7_t;
typedef uint8_t   u8_t;
typedef int16_t   q15_t;
typedef uint16_t  u16_t;
typedef int32_t   q31_t;
typedef uint32_t  u32_t;
typedef int64_t   q63_t;
typedef uint64_t  u64_t;
typedef float     float32_t;
typedef double    float64_t;


#ifdef  __cplusplus
}
#endif
#endif // __RISCV_DSP_MATH_TYPES_H__
