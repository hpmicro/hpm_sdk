/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_DFU_TRIGGER_H
#define HPM_DFU_TRIGGER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool hpm_dfu_check_and_clear_trigger(void);
void hpm_dfu_reboot_to_dfu(void);
bool cherryusb_dfu_jump_app(void);
#ifdef __cplusplus
}
#endif

#endif
