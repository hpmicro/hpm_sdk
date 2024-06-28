/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CHRY_READLINE_CONFIG_H
#define CHRY_READLINE_CONFIG_H

#ifdef CONFIG_READLINE_USER_DEFINE_FILE
#include "chry_readline_user.h"
#endif

/*!< argument check */
#ifndef CONFIG_READLINE_DEBUG
#define CONFIG_READLINE_DEBUG 0
#endif

/*!< default row */
#ifndef CONFIG_READLINE_DFTROW
#define CONFIG_READLINE_DFTROW 25
#endif

/*!< default column */
#ifndef CONFIG_READLINE_DFTCOL
#define CONFIG_READLINE_DFTCOL 80
#endif

/*!< history support <+600byte> */
#ifndef CONFIG_READLINE_HISTORY
#define CONFIG_READLINE_HISTORY 1
#endif

/*!< completion support <+800byte> */
#ifndef CONFIG_READLINE_COMPLETION
#define CONFIG_READLINE_COMPLETION 1
#endif

/*!< max completion item list count (use stack 4 x count byte) */
#ifndef CONFIG_READLINE_MAX_COMPLETION
#define CONFIG_READLINE_MAX_COMPLETION 40
#endif

/*!< prompt edit support <+900byte>  */
#ifndef CONFIG_READLINE_PROMPTEDIT
#define CONFIG_READLINE_PROMPTEDIT 1
#endif

/*!< prompt segment count */
#ifndef CONFIG_READLINE_PROMPTSEG
#define CONFIG_READLINE_PROMPTSEG 7
#endif

/*!< xterm support */
#ifndef CONFIG_READLINE_XTERM
#define CONFIG_READLINE_XTERM 0
#endif

/*!< newline */
#ifndef CONFIG_READLINE_NEWLINE
#define CONFIG_READLINE_NEWLINE "\r\n"
#endif

/*!< tab space count */
#ifndef CONFIG_READLINE_SPACE
#define CONFIG_READLINE_SPACE 4
#endif

/*!< independent ctrl map */
#ifndef CONFIG_READLINE_CTRLMAP
#define CONFIG_READLINE_CTRLMAP 0
#endif

/*!< independent alt map */
#ifndef CONFIG_READLINE_ALTMAP
#define CONFIG_READLINE_ALTMAP 0
#endif

/*!< refresh prompt */
#ifndef CONFIG_READLINE_REFRESH_PROMPT
#define CONFIG_READLINE_REFRESH_PROMPT 1
#endif

/*!< no waiting for sget */
#ifndef CONFIG_READLINE_NOBLOCK
#define CONFIG_READLINE_NOBLOCK 0
#endif

/*!< help information */
#ifndef CONFIG_READLINE_HELP
#define CONFIG_READLINE_HELP ""
#endif

/*
"\r\n"
"\t+-------------------------------------+\r\n"
"\t|           \e[1;31mCherry ReadLine \e[m          |\r\n"
"\t|                                     |\r\n"
"\t| -> CTRL + ^  return normal screen   |\r\n"
"\t| -> CTRL + -  show this help         |\r\n"
"\t| -> CTRL + C  abort line             |\r\n"
"\t| -> CTRL + K  delete cursor to end   |\r\n"
"\t| -> CTRL + L  clear screen           |\r\n"
"\t| -> CTRL + U  delete whole line      |\r\n"
"\t| -> CTRL + W  delete prev word       |\r\n"
"\t| -> CTRL + Z  abort line             |\r\n"
"\t+-------------------------------------+\r\n"
*/

#endif
