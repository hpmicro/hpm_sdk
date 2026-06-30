/*
 * Copyright (c) 2022, Egahp
 * Copyright (c) 2024-2026, HPMicro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CSH_CONFIG_H
#define CSH_CONFIG_H

#ifdef CONFIG_CSH_HAS_EXTRA_CONFIG
#include CONFIG_CSH_HAS_EXTRA_CONFIG
#endif

/*!< argument check */
#ifndef CONFIG_CSH_DEBUG
#define CONFIG_CSH_DEBUG 0
#endif

/*!< default row */
#ifndef CONFIG_CSH_DFTROW
#define CONFIG_CSH_DFTROW 25
#endif

/*!< default column */
#ifndef CONFIG_CSH_DFTCOL
#define CONFIG_CSH_DFTCOL 80
#endif

/*!< history support <+550byte> */
#ifndef CONFIG_CSH_HISTORY
#define CONFIG_CSH_HISTORY 1
#endif

/*!< history buffer size */
#ifndef CONFIG_CSH_HISTORY_BUF_SIZE
#define CONFIG_CSH_HISTORY_BUF_SIZE 256
#endif

/*!< completion support <+1100byte> */
#ifndef CONFIG_CSH_COMPLETION
#define CONFIG_CSH_COMPLETION 1
#endif

/*!< max completion item list count (use stack 4 x count byte) */
#ifndef CONFIG_CSH_MAX_COMPLETION
#define CONFIG_CSH_MAX_COMPLETION 40
#endif

/*!< prompt edit support <+1000byte>  */
#ifndef CONFIG_CSH_PROMPTEDIT
#define CONFIG_CSH_PROMPTEDIT 1
#endif

#ifndef CONFIG_CSH_PROMPTEDIT_BUF_SIZE
#define CONFIG_CSH_PROMPTEDIT_BUF_SIZE 128
#endif

/*!< prompt segment count */
#ifndef CONFIG_CSH_PROMPTSEG
#define CONFIG_CSH_PROMPTSEG 7
#endif

/*!< xterm support */
#ifndef CONFIG_CSH_XTERM
#define CONFIG_CSH_XTERM 0
#endif

/*!< newline */
#ifndef CONFIG_CSH_NEWLINE
#define CONFIG_CSH_NEWLINE "\r\n"
#endif

/*!< tab space count */
#ifndef CONFIG_CSH_SPACE
#define CONFIG_CSH_SPACE 4
#endif

/*!< independent ctrl map */
#ifndef CONFIG_CSH_CTRLMAP
#define CONFIG_CSH_CTRLMAP 0
#endif

/*!< independent alt map */
#ifndef CONFIG_CSH_ALTMAP
#define CONFIG_CSH_ALTMAP 0
#endif

/*!< refresh prompt */
#ifndef CONFIG_CSH_REFRESH_PROMPT
#define CONFIG_CSH_REFRESH_PROMPT 1
#endif

/*!< no waiting for sget */
#ifndef CONFIG_CSH_NOBLOCK
#define CONFIG_CSH_NOBLOCK 1
#endif

/*!< help information */
#ifndef CONFIG_CSH_HELP
#define CONFIG_CSH_HELP ""
#endif

/*!< path length 0:const path, <=255:variable path */
#ifndef CONFIG_CSH_MAXLEN_PATH
#define CONFIG_CSH_MAXLEN_PATH 128
#endif

/*!< path segment count */
#ifndef CONFIG_CSH_MAXSEG_PATH
#define CONFIG_CSH_MAXSEG_PATH 16
#endif

/*!< user count */
#ifndef CONFIG_CSH_MAX_USER
#define CONFIG_CSH_MAX_USER 1
#endif

/*!< max argument count */
#ifndef CONFIG_CSH_MAX_ARG
#define CONFIG_CSH_MAX_ARG 8
#endif

/*!< linebuffer static or on stack */
#ifndef CONFIG_CSH_LNBUFF_STATIC
#define CONFIG_CSH_LNBUFF_STATIC 1
#endif

/*!< linebuffer size (valid only if lnbuff on stack) */
#ifndef CONFIG_CSH_LNBUFF_SIZE
#define CONFIG_CSH_LNBUFF_SIZE 256
#endif

/*!< multi-thread mode */
#ifndef CONFIG_CSH_MULTI_THREAD
#define CONFIG_CSH_MULTI_THREAD 1
#endif

/*!< independent signal handler (for multi instances) */
#ifndef CONFIG_CSH_SIGNAL_HANDLER
#define CONFIG_CSH_SIGNAL_HANDLER 0
#endif

/*!< Ctrl+c/d/q/s/z/\ F1-F12 UE <+120byte> */
#ifndef CONFIG_CSH_USER_CALLBACK
#define CONFIG_CSH_USER_CALLBACK 1
#endif

/*!< enable macro export symbol table */
#ifndef CONFIG_CSH_SYMTAB
#define CONFIG_CSH_SYMTAB 1
#endif

/*!< print buffer size */
#ifndef CONFIG_CSH_PRINT_BUFFER_SIZE
#define CONFIG_CSH_PRINT_BUFFER_SIZE 512
#endif

#endif
