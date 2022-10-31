/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef RAMSTRESS_LIB_H
#define RAMSTRESS_LIB_H

#include <stdint.h>

int ramstress_run(uint32_t ram_base, uint32_t ram_size_bytes);

#endif
