/******************************************************************************
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.*
 * Copyright (C) 2018-2023 Andes Technology Corporation. All rights reserved. *
 *                                                                            *
 * SPDX-License-Identifier: Apache-2.0                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the License); you may      *
 * not use this file except in compliance with the License.                   *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 * www.apache.org/licenses/LICENSE-2.0                                        *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT    *
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.           *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 ******************************************************************************/

 /**
  * @defgroup libvec config header file
  */

  /**
   * This header file will be included for each libvec's C file
   * with while compiling. So any configuration or common
   * definition can be written here.
   */

#ifndef __INTERNAL_CONFIG_H__
#define __INTERNAL_CONFIG_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include "internal_nds_config.h"
//#include "riscv_dsp_math_types.h"

//For those algorithms which have worse performance when using stride load/store instructions.
//Enable this flag for stride load/store experiments. Otherwise, disable this
//flag to use the alternative algorithm(without stride ls) to get better performance.
//#define EXP_WORSE_PERF_STRIDE_LS

//Define the input pointers are restricted.
#define ENA_FUNC_RESTRICT

#ifdef ENA_FUNC_RESTRICT
#define FUNC_RESTRICT       __restrict
#else
#define FUNC_RESTRICT
#endif

#ifdef  __cplusplus
}
#endif


#endif      //__INTERNAL_CONFIG_H__
