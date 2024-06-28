/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_MOTOR_H
#define HPM_MOTOR_H


#ifdef __cplusplus
extern "C" {
#endif

void sei_sample_position(void);
bool sei_data_valid(void);
uint32_t sei_get_position_data(void);

#ifdef __cplusplus
}
#endif

#endif /* HPM_MOTOR_H */
