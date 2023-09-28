/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __IPERF_TCP_CLIENT_H__
#define __IPERF_TCP_CLIENT_H__

#include "hpm_common.h"

hpm_stat_t iperf(char *target_name, uint16_t port);

#endif
