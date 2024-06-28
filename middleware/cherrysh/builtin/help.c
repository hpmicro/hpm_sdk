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

/*!< help    */
static int help(int argc, char **argv)
{
    chry_shell_t *csh = (void *)argv[argc + 1];
    uint16_t longest_name;
    bool enable_function = true;
    bool enable_variable = true;

    while (argc > 1) {
        argc--;
        argv++;
    }

    if (enable_function) {
        csh_printf(csh, "total function %d\r\n",
               ((uintptr_t)csh->cmd_tbl_end - (uintptr_t)csh->cmd_tbl_beg) / sizeof(chry_syscall_t));
    }

    longest_name = 0;
    for (const chry_syscall_t *call = csh->cmd_tbl_beg; call < csh->cmd_tbl_end; call++) {
        uint16_t len = strlen(call->name);
        longest_name = len > longest_name ? len : longest_name;
    }

    for (const chry_syscall_t *call = csh->cmd_tbl_beg; call < csh->cmd_tbl_end; call++) {
        uint16_t len = strlen(call->name);

        csh_printf(csh, "  \033[32m%s\033[m", call->name);

        for (int k = len; k < longest_name; k++) {
            csh_printf(csh, " ");
        }

        csh_printf(csh, " -> ");

        csh_printf(csh, call->path);

        csh_printf(csh, "\r\n");
    }

    if (enable_variable) {
        if (enable_function) {
            csh_printf(csh, "\r\n");
        }

        csh_printf(csh, "total variable %d\r\n",
               ((uintptr_t)csh->var_tbl_end - (uintptr_t)csh->var_tbl_beg) / sizeof(chry_sysvar_t));
    }

    longest_name = 0;
    for (const chry_sysvar_t *var = csh->var_tbl_beg; var < csh->var_tbl_end; var++) {
        uint16_t len = strlen(var->name);
        longest_name = len > longest_name ? len : longest_name;
    }

    for (const chry_sysvar_t *var = csh->var_tbl_beg; var < csh->var_tbl_end; var++) {
        uint16_t len = strlen(var->name);

        csh_printf(csh, "  $\033[33m%s\033[m", var->name);

        for (int k = len; k < longest_name; k++) {
            csh_printf(csh, " ");
        }

        char cr = var->attr & CSH_VAR_READ ? 'r' : '-';
        char cw = var->attr & CSH_VAR_WRITE ? 'w' : '-';
        csh_printf(csh, " %c%c %3d\r\n", cr, cw, var->attr & CSH_VAR_SIZE);
    }

    return 0;
}

CSH_SCMD_EXPORT(help, );
