/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdbool.h>
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
static inline esp_log_level_t esp_log_get_default_level(void)
{
    return (esp_log_level_t) CONFIG_LOG_DEFAULT_LEVEL;
}

/**
 * @brief Set the default log level.
 *
 * This function sets the default log level used by the ESP-IDF log library.
 * The default log level is used by the definition of ESP_LOGx macros and
 * can be overridden for specific tags using `esp_log_level_set("*", level)`.
 * This is for internal use only.
 *
 * If CONFIG_LOG_DYNAMIC_LEVEL_CONTROL is not set, this function does not change the default log level.
 *
 * @param level The new default log level to set.
 */
void esp_log_set_default_level(esp_log_level_t level);

/**
 * @brief Set log level for given tag
 *
 * If logging for given component has already been enabled, changes previous setting.
 *
 * @note Note that this function can not raise log level above the level set using
 *       CONFIG_LOG_MAXIMUM_LEVEL setting in menuconfig.
 *
 * To raise log level above the default one for a given file, define
 * LOG_LOCAL_LEVEL to one of the ESP_LOG_* values, before including esp_log.h in this file.
 *
 * If CONFIG_LOG_DYNAMIC_LEVEL_CONTROL is not selected the static (no-op) implementation of log level is used.
 * Changing the log level is not possible, esp_log_level_set does not work.
 *
 * @param tag   Tag of the log entries to enable. Must be a non-NULL zero terminated string.
 *              Value "*" resets log level for all tags to the given value.
 *              If the tag is NULL then a silent return happens.
 * @param level Selects log level to enable.
 *              Only logs at this and lower verbosity levels will be shown.
 */
void esp_log_level_set(const char* tag, esp_log_level_t level);


/**
 * @brief Get log level for a given tag, can be used to avoid expensive log statements
 *
 * If CONFIG_LOG_DYNAMIC_LEVEL_CONTROL is not selected the static (no-op) implementation of log level is used.
 * Changing the log level is not possible, esp_log_level_set does not work. This function returns the default log level.
 *
 * @param tag   Tag of the log to query current level. Must be a zero terminated string.
 *              If tag is NULL then the default log level is returned (see esp_log_get_default_level()).
 * @return      The current log level for the given tag.
 */
esp_log_level_t esp_log_level_get(const char* tag);

#ifdef __cplusplus
}
#endif
