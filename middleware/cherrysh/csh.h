/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CSH_H
#define CSH_H

#include <stdint.h>
#include "csh_config.h"

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

/*!< completion support <+1100byte> */
#ifndef CONFIG_CSH_COMPLETION
#define CONFIG_CSH_COMPLETION 1
#endif

/*!< max completion item list count (use stack 4 x count byte) */
#ifndef CONFIG_CSH_MAX_COMPLETION
#define CONFIG_CSH_MAX_COMPLETION 48
#endif

/*!< prompt edit support <+1000byte>  */
#ifndef CONFIG_CSH_PROMPTEDIT
#define CONFIG_CSH_PROMPTEDIT 1
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
#define CONFIG_CSH_NOBLOCK 0
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
#define CONFIG_CSH_MULTI_THREAD 0
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

#define CONFIG_READLINE_DEBUG          CONFIG_CSH_DEBUG
#define CONFIG_READLINE_DFTROW         CONFIG_CSH_DFTROW
#define CONFIG_READLINE_DFTCOL         CONFIG_CSH_DFTCOL
#define CONFIG_READLINE_HISTORY        CONFIG_CSH_HISTORY
#define CONFIG_READLINE_COMPLETION     CONFIG_CSH_COMPLETION
#define CONFIG_READLINE_MAX_COMPLETION CONFIG_CSH_MAX_COMPLETION
#define CONFIG_READLINE_PROMPTEDIT     CONFIG_CSH_PROMPTEDIT
#define CONFIG_READLINE_PROMPTSEG      CONFIG_CSH_PROMPTSEG
#define CONFIG_READLINE_XTERM          CONFIG_CSH_XTERM
#define CONFIG_READLINE_NEWLINE        CONFIG_CSH_NEWLINE
#define CONFIG_READLINE_SPACE          CONFIG_CSH_SPACE
#define CONFIG_READLINE_CTRLMAP        CONFIG_CSH_CTRLMAP
#define CONFIG_READLINE_ALTMAP         CONFIG_CSH_ALTMAP
#define CONFIG_READLINE_REFRESH_PROMPT CONFIG_CSH_REFRESH_PROMPT
#define CONFIG_READLINE_NOBLOCK        CONFIG_CSH_NOBLOCK
#define CONFIG_READLINE_HELP           CONFIG_CSH_HELP

#include "CherryRL/chry_readline.h"
#include "chry_shell.h"

#if defined(__CC_ARM) || defined(__CLANG_ARM) || defined(__GNUC__) || defined(__ADSPBLACKFIN__)
#define __CSH_SECTION(x) __attribute__((section(x)))
#define __CSH_USED       __attribute__((used))
#elif defined(__IAR_SYSTEMS_ICC__)
#define __CSH_SECTION(x) @x
#define __CSH_USED       __root
#else
#define __CSH_SECTION(x)
#define __CSH_USED
#endif

#if defined(CONFIG_CSH_SYMTAB) && CONFIG_CSH_SYMTAB

#if defined(_MSC_VER)
#pragma section("FSymTab$f", read)
#pragma section("VSymTab", read)
#endif

#if defined(__TI_COMPILER_VERSION__)
#defien __TI_CSH_PRAGMA(x) _Pragma(#x)
#endif

#endif

#if defined(CONFIG_CSH_SYMTAB) && CONFIG_CSH_SYMTAB
#if defined(_MSC_VER)

#define CSH_EXPORT_CALL(name, func, path)            \
    __declspec(allocate("FSymTab$f"))                \
        const chry_syscall_t __fsym_##name##func = { \
            path,                                    \
            #name,                                   \
            (chry_syscall_func_t)func,               \
        };

#pragma comment(linker, "/merge:FSymTab=mytext")

#define CSH_EXPORT_VAR(name, var, attr)                                        \
    __declspec(allocate("VSymTab")) const chry_sysvar_t __vsym_##name##var = { \
        #name,                                                                 \
        (void *)&var,                                                          \
        attr,                                                                  \
    };

#elif defined(__TI_COMPILER_VERSION__)

#define CSH_EXPORT_CALL(name, func, path)                              \
    __TI_CSH_PRAGMA(DATA_SECTION(__fsym_##name##func, "FSymTab"));     \
    __attribute__((used)) const chry_syscall_t __fsym_##name##func = { \
        path,                                                          \
        #name,                                                         \
        (chry_syscall_func_t)func,                                     \
    };

#define CSH_EXPORT_VAR(name, var, attr)                              \
    __TI_CSH_PRAGMA(DATA_SECTION(__vsym_##name##var, "VSymTab"));    \
    __attribute__((used)) const chry_sysvar_t __vsym_##name##var = { \
        #name,                                                       \
        (void *)&var,                                                \
        attr,                                                        \
    };

#else

#define CSH_EXPORT_CALL(name, func, path)                                            \
    __CSH_USED const chry_syscall_t __fsym_##name##func __CSH_SECTION("FSymTab") = { \
        path,                                                                        \
        #name,                                                                       \
        (chry_syscall_func_t)func,                                                   \
    };

#define CSH_EXPORT_VAR(name, var, attr)                                            \
    __CSH_USED const chry_sysvar_t __vsym_##name##var __CSH_SECTION("VSymTab") = { \
        #name,                                                                     \
        (void *)var,                                                               \
        attr,                                                                      \
    };

#endif
#else
#define CSH_EXPORT_CALL(name, func, path)
#define CSH_EXPORT_VAR(name, var, attr)
#endif

/*****************************************************************************
* @brief        export csh command (default path "/bin")
*
* @param[in]    func        function pointer
* @param[in]    _dummy      (compatible descriptions)
*
*****************************************************************************/
#define CSH_CMD_EXPORT(func, _dummy) \
    CSH_EXPORT_CALL(func, func, "/bin")

/*****************************************************************************
* @brief        export csh command with alias  (default path "/bin" )
*
* @param[in]    func        function pointer
* @param[in]    name        command name
* @param[in]    _dummy      (compatible descriptions)
*
*****************************************************************************/
#define CSH_CMD_EXPORT_ALIAS(func, name, _dummy) \
    CSH_EXPORT_CALL(name, func, "/bin")

/*****************************************************************************
* @brief        export csh command (default path "/sbin")
*
* @param[in]    func        function pointer
* @param[in]    _dummy      (compatible descriptions)
*
*****************************************************************************/
#define CSH_SCMD_EXPORT(func, _dummy) \
    CSH_EXPORT_CALL(func, func, "/sbin")

/*****************************************************************************
* @brief        export csh command with alias  (default path "/sbin" )
*
* @param[in]    func        function pointer
* @param[in]    name        command name
* @param[in]    _dummy      (compatible descriptions)
*
*****************************************************************************/
#define CSH_SCMD_EXPORT_ALIAS(func, name, _dummy) \
    CSH_EXPORT_CALL(name, func, "/sbin")

/*****************************************************************************
* @brief        export csh command with path
*
* @param[in]    func        function pointer
* @param[in]    name        command name
* @param[in]    path        command path
*
* @example      CSH_CMD_EXPORT_PATH(test_func1, test1, "/bin");       // ok
*               CSH_CMD_EXPORT_PATH(test_func2, test2, "/sbin/test"); // ok
*               CSH_CMD_EXPORT_PATH(test_func2, test2, "/sbin/test/");// error, cannot end with '/'
*               CSH_CMD_EXPORT_PATH(test_func2, test2, "/");          // error, cannot be the root path "/"
*
*****************************************************************************/
#define CSH_CMD_EXPORT_PATH(func, name, path) \
    CSH_EXPORT_CALL(name, func, path)

/*****************************************************************************
* @brief        export csh read-only variable
*
* @param[in]    var         variable
* @param[in]    name        variable name
* @param[in]    size        variable size
*
*****************************************************************************/
#define CSH_RVAR_EXPORT(var, name, size) \
    CSH_EXPORT_VAR(name, var, 0x80000000 | (size))

/*****************************************************************************
* @brief        export csh write-only variable
*
* @param[in]    var         variable
* @param[in]    name        variable name
* @param[in]    size        variable size
*
*****************************************************************************/
#define CSH_WVAR_EXPORT(var, name, size) \
    CSH_EXPORT_VAR(name, var, 0x40000000 | (size))

/*****************************************************************************
* @brief        export csh read-write variable
*
* @param[in]    var         variable
* @param[in]    name        variable name
* @param[in]    size        variable size
*
*****************************************************************************/
#define CSH_RWVAR_EXPORT(var, name, size) \
    CSH_EXPORT_VAR(name, var, 0xc0000000 | (size))

#endif
