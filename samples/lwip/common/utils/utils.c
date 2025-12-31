/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "hpm_debug_console.h"
#include "utils.h"
#include "netinfo.h"

static bool isIPv4(const char *s)
{
    if (s == NULL || strlen(s) == 0 || strlen(s) > 15) {
        return false;
    }

    int segments = 0;

    while (*s != '\0') {
        /* check if the current section begins with a number */
        if (!isdigit((uint8_t)(*s))) {
            return false;
        }

        const char *end = s;
        /* move to the end of the segment (the end of a number or string) */
        while (isdigit((uint8_t)(*end))) {
            end++;
        }

        /* the length of the segment exceeds 3 characters , which is not valid */
        if (end - s > 3) {
            return false;
        }

        /* leading zero check (length > 1 and the first character is '0') */
        if (end - s > 1 && *s == '0') {
            return false;
        }

        /* convert to integer and check the range (0 - 255) */
        int num = 0;
        for (const char *p = s; p < end; p++) {
            num = num * 10 + (*p - '0');
        }
        if (num > 255) {
            return false;
        }

        segments++;
        s = end;

        /* When the number of segment is 4, it is necessary to reach the end of the string */
        if (segments == 4) {
            return *s == '\0';
        }

        /* There must be a '.' to separte the segments. */
        if (*s != '.') {
            return false;
        }
        s++; /* skip '.'  */
    }

    /* Check if the number of segment is less than 4. */
    return segments == 4;
}

static bool fetch_user_command(uint8_t *buffer, uint32_t length, uint8_t *idx)
{
    uint8_t cmd_str_idx = 0;
    bool ret = true;

    while (1) {
        uint8_t ch = console_try_receive_byte();

        if (ch != 0) {
            if (ch == 0x7f) {
                if (cmd_str_idx != 0) {
                    console_send_byte(ch);
                }
            } else {
                console_send_byte(ch);
                if (ch == '\r') {
                    console_send_byte('\n');
                }
            }

            if (ch == 0x7f) {
                if (cmd_str_idx != 0) {
                    buffer[--cmd_str_idx] = '\0';
                }
            } else {
                if (cmd_str_idx < length) {
                    if (isprint(ch)) {
                        buffer[cmd_str_idx++] = ch;
                    } else if (ch == '\r') {
                        buffer[cmd_str_idx] = '\0';
                        break;
                    }
                } else  {
                    break;
                }
            }
        }
    }

    *idx = cmd_str_idx;

    if (cmd_str_idx != 0) {
        ret = isIPv4((char *)buffer);

        if (!ret) {
            printf("\"%s\": %s\n", buffer, "invalid");
        }
    }

    return ret;
}

bool fetch_ip_addr_from_serial_terminal(uint8_t netif_num, uint8_t *buff, uint32_t length)
{
    uint8_t idx;
    char *str;

    if (buff == NULL) {
        return false;
    }

    str = (netif_num == 0) ? HPM_STRINGIFY(REMOTE_IP0_CONFIG) : HPM_STRINGIFY(REMOTE_IP1_CONFIG);

    printf("Please enter remote server IP (or press the Enter key to use the default remote server IP(%s)):\n", str);

    if (!fetch_user_command(buff, length, &idx)) {
        memset(buff, 0x00, length);
        return false;
    }

    if (idx == 0) {
        printf("No any character input! The default remote server IP will be used.\n");
        memcpy(buff, str, sizeof(HPM_STRINGIFY(REMOTE_IP0_CONFIG)));
    }

    return true;
}