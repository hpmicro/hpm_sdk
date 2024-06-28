/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "csh.h"

/*!< shsize */
static int shsize(int argc, char **argv)
{
    chry_shell_t *csh = (void *)argv[argc + 1];
    char *prgname = argv[0];

    if ((argc == 2) && !strcmp(argv[1], "--update")) {
        if (5 != csh->rl.sput(&csh->rl, "\033[18t", 5)) {
            csh_printf(csh, "Failed to request update\r\n");
            return -1;
        }
        return 0;
    } else if ((argc == 4) && !strcmp(argv[1], "--config")) {
        int row = atoi(argv[2]);
        int col = atoi(argv[3]);
        if (row > 10 && row < 4096) {
            csh->rl.term.row = row;
        } else {
            csh_printf(csh, "Illegal row %d\r\n", row);
        }
        if (col > 10 && col < 4096) {
            csh->rl.term.col = col;
        } else {
            csh_printf(csh, "Illegal col %d\r\n", col);
        }
    } else if (argc == 1) {
    } else {
        csh_printf(csh, "Usage: %s [--update | --config <row> <col>\r\n", prgname);
        return 0;
    }

    csh_printf(csh, "Window config to row:%d col:%d\r\n", csh->rl.term.row, csh->rl.term.col);

    return 0;
}

CSH_SCMD_EXPORT(shsize, );
