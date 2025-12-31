/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 * copyright (c) 2025, HPMicro
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PORT_ESP_HOSTED_HOST_LOG_H
#define __PORT_ESP_HOSTED_HOST_LOG_H

#include "esp_log.h"
#include "hpm_common.h"
#ifndef DEFINE_LOG_TAG
#define DEFINE_LOG_TAG(sTr) static const char TAG[] = #sTr
#endif

#endif
