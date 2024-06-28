/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
 #include "csh.h"

/*!< login */
int csh_login(chry_shell_t *csh)
{
    int ret = 0;
    char *password;
    char first = 0;

#if defined(CONFIG_CSH_LNBUFF_STATIC) && CONFIG_CSH_LNBUFF_STATIC
#define BUFFER_SIZE csh->buffsize
    char *buffer = csh->linebuff;
    uint16_t *linesize = &csh->linesize;
#else
#define BUFFER_SIZE 64
    static char buffer[BUFFER_SIZE];
    uint16_t _linesize;
    uint16_t *linesize = &_linesize;
#endif

#if defined(CONFIG_CSH_NOBLOCK) && CONFIG_CSH_NOBLOCK
    if (csh->rl.noblock && !csh->rl.block) {
        goto restore;
    }
#endif

retry:
    csh_printf(csh, "login as: %s\r\n", csh->user[csh->uid]);
    csh_printf(csh, "%s@%s's password:\r\n", csh->user[csh->uid], csh->host);
    chry_readline_mask(&csh->rl, true);
    first = csh->rl.prompt[0];
    csh->rl.prompt[0] = '\0';

#if defined(CONFIG_CSH_NOBLOCK) && CONFIG_CSH_NOBLOCK
restore:
#endif

    password = chry_readline(&csh->rl, buffer, BUFFER_SIZE, linesize);

#if defined(CONFIG_CSH_NOBLOCK) && CONFIG_CSH_NOBLOCK
    if (csh->rl.noblock && !csh->rl.block) {
        return 1;
    }
#endif

    csh->rl.prompt[0] = first;
    chry_readline_mask(&csh->rl, false);

    if (password == NULL) {
        return -1;
    } else {
        ret = chry_shell_substitute_user(csh, csh->uid, password);
        if (ret != 0) {
            csh_printf(csh, "\033[31mincorrect password\033[0m\r\n\r\n");
            goto retry;
        }

        csh_printf(csh, "welcome to cherry shell\r\n");

        return ret;
    }
}
