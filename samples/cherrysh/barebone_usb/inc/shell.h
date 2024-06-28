/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SHELL_H
#define SHELL_H

#include "csh.h"

extern int shell_init(uint8_t busid, uint32_t regbase, bool need_login);
extern int shell_main(void);
extern void shell_lock(void);
extern void shell_unlock(void);

#endif
