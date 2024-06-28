/*
 * Copyright (c) 2022, Egahp
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SHELL_H
#define SHELL_H

#include "csh.h"

extern int shell_init(uint8_t busid, uint32_t regbase, bool need_login);
extern void shell_lock(void);
extern void shell_unlock(void);

#endif
