/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef BLDC_FOC_H
#define BLDC_FOC_H

void setup_moter(void);
float get_motor_current_speed(void);
void set_motor_speed(float speed);
#endif
