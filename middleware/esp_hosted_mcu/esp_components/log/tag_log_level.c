/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <esp_log_level.h>
#include "esp_log.h"

#include "log_linked_list.h"

esp_log_level_t s_master_log_level = CONFIG_LOG_DEFAULT_LEVEL;

void esp_log_set_default_level(esp_log_level_t level)
{
    s_master_log_level = level;
}

static inline void log_level_set(const char *tag, esp_log_level_t level)
{
    if (tag == NULL) {
        // expected non-NULL zero terminated string.
        return;
    }
    // for wildcard tag, remove all linked list items and clear the cache
    if (strcmp(tag, "*") == 0) {
        esp_log_set_default_level(level);
        esp_log_linked_list_clean();
    } else {
        esp_log_linked_list_set_level(tag, level);
    }
}

/* timeout ignored */
static esp_log_level_t log_level_get(const char *tag, bool timeout)
{
    esp_log_level_t level_for_tag = esp_log_get_default_level();
    if (tag == NULL) {
        return level_for_tag;
    }
    esp_log_linked_list_get_level(tag, &level_for_tag);
    return level_for_tag;
}

void esp_log_level_set(const char *tag, esp_log_level_t level)
{
	log_level_set(tag, level);
}

esp_log_level_t esp_log_level_get(const char *tag)
{
    return log_level_get(tag, false);
}
