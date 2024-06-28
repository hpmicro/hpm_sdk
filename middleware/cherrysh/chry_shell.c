/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include "csh.h"

#include "CherryRL/chry_readline.c"

#if defined(CONFIG_CSH_DEBUG) && CONFIG_CSH_DEBUG
#define CHRY_SHELL_PARAM_CHECK(__expr, __ret) \
    do {                                      \
        if (!(__expr)) {                      \
            return __ret;                     \
        }                                     \
    } while (0)

#else
#define CHRY_SHELL_PARAM_CHECK(__expr, __ret) ((void)0)
#endif

#if defined(__CC_ARM) || defined(__CLANG_ARM) || defined(__GNUC__) || defined(__ADSPBLACKFIN__) || defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCRISCV__)
#ifndef __weak
#define __weak __attribute__((weak))
#endif
#else
#ifndef __weak
#define __weak
#endif
#endif

#define chry_shell_offsetof(type, member)          ((size_t) & (((type *)0)->member))
#define chry_shell_container_of(ptr, type, member) ((type *)((char *)(ptr)-chry_shell_offsetof(type, member)))

/*!< prompt segment index */
#define CSH_PROMPT_SEG_USER 0
#define CSH_PROMPT_SEG_HOST 2
#define CSH_PROMPT_SEG_PATH 4

extern void chry_shell_port_default_handler(chry_shell_t *csh, int sig);
extern int chry_shell_port_create_context(chry_shell_t *csh, int argc, const char **argv);
extern int chry_shell_port_hash_strcmp(const char *hash, const char *str);

#if defined(CONFIG_CSH_MULTI_THREAD) && CONFIG_CSH_MULTI_THREAD
/* static const uint8_t sigmap[CSH_SIGNAL_COUNT] = { CSH_SIGINT, CSH_SIGQUIT, CSH_SIGKILL, CSH_SIGTERM, CSH_SIGSTOP, CSH_SIGTSTP, CSH_SIGCONT }; */
#if !defined(CONFIG_CSH_SIGNAL_HANDLER) || (CONFIG_CSH_SIGNAL_HANDLER == 0)
static chry_sighandler_t sighdl[CSH_SIGNAL_COUNT] = {
    chry_shell_port_default_handler,
    chry_shell_port_default_handler,
    chry_shell_port_default_handler,
    chry_shell_port_default_handler,
    chry_shell_port_default_handler,
    chry_shell_port_default_handler,
    chry_shell_port_default_handler,
};
#endif

/*****************************************************************************
* @brief        default signal handler
*
* @param[in]    signum      signal number
*
*****************************************************************************/
__weak void chry_shell_port_default_handler(chry_shell_t *csh, int sig)
{
    (void)csh;
    switch (sig) {
        case CSH_SIGINT:
            break;
        case CSH_SIGQUIT:
            break;
        case CSH_SIGKILL:
            break;
        case CSH_SIGTERM:
            break;
        case CSH_SIGSTOP:
            break;
        case CSH_SIGTSTP:
            break;
        case CSH_SIGCONT:
            break;
        default:
            break;
    }
}

/*****************************************************************************
* @brief        create context to execute
*
* @param[in]    csh         shell instance
* @param[in]    argc        argument count
* @param[in]    argv        argument value
*
* @retval                   0:success -1:error
*****************************************************************************/
__weak int chry_shell_port_create_context(chry_shell_t *csh, int argc, const char **argv)
{
    (void)csh;
    (void)argc;
    (void)argv;
    return -1;
}

/*****************************************************************************
* @brief        conversion signum
*
* @param[in]    signum      signal number
*
* @retval                   0<= signal index -1:not find
*****************************************************************************/
/*
static int chry_shell_conversion_signum(int signum)
{
    int sigidx = -1;

    for (uint8_t i = 0; i < CSH_SIGNAL_COUNT; i++) {
        if (sigmap[i] == signum) {
            sigidx = i;
            break;
        }
    }

    return sigidx;
}
*/
#endif

/*****************************************************************************
* @brief        hash str then compare
*
* @param[in]    hash        hash
* @param[in]    str         string
*
* @retval                   0:Equal 1:hash>str -1:str>hash
*****************************************************************************/
__weak int chry_shell_port_hash_strcmp(const char *hash, const char *str)
{
    /*!< simple demo */
    return strcmp(hash, str);
}

#if defined(CONFIG_CSH_COMPLETION) && CONFIG_CSH_COMPLETION
/*****************************************************************************
* @brief        completion callback
*
* @param[in]    rl          readline instance
* @param[in]    pre         pre string
* @param[in]    size        strlen(pre)
* @param[in]    argv        argument value (completion)
* @param[in]    argl        argument length (completion length)
* @param[in]    argcmax     argument max count (max completion count)
*
* @retval                   argc:argument count, max is argcmax
*****************************************************************************/
static uint8_t chry_shell_completion_callback(chry_readline_t *rl, char *pre, uint16_t *size, const char **argv, uint8_t *argl, uint8_t argcmax)
{
    chry_shell_t *csh = chry_shell_container_of(rl, chry_shell_t, rl);
    uint8_t argc = 0;

    if (*size == 0) {
        return argc;
    }

    /*!< environment variable */
    if (*size && pre[0] == '$') {
        pre++;             /*!< skip $ */
        *size = *size - 1; /*!< skip $ */
        for (const chry_sysvar_t *var = csh->var_tbl_beg; var < csh->var_tbl_end; var++) {
            if (strncmp(pre, var->name, *size) == 0) {
                argv[argc] = var->name;
                argl[argc] = strlen(var->name);
                argc++;
                if (argc >= argcmax) {
                    break;
                }
            }
        }

        return argc;
    }

    const char *PATH = NULL;
    uint8_t Pargc;
    uint8_t Pargl[CONFIG_CSH_MAXSEG_PATH + 1];
    const char *Pargv[CONFIG_CSH_MAXSEG_PATH + 1];

    if (((*size >= 1) && (pre[0] == '/')) ||
        ((*size >= 2) && (pre[0] == '.') && (pre[1] == '/')) ||
        ((*size >= 3) && (pre[0] == '.') && (pre[1] == '.') && (pre[2] == '/'))) {
        /*!< search from the current path */
        PATH = csh->path;
    } else {
        /*!< search from the environment PATH */
        for (const chry_sysvar_t *var = csh->var_tbl_beg; var < csh->var_tbl_end; var++) {
            if (strcmp("PATH", var->name) == 0) {
                PATH = var->var;
                break;
            }
        }
    }

    while (PATH != NULL) {
        Pargc = chry_shell_path_resolve(PATH, pre, Pargv, Pargl, CONFIG_CSH_MAXSEG_PATH + 1);
        *size = Pargl[Pargc - 1];

        /*!< match from system path */
        for (const chry_syscall_t *call = csh->cmd_tbl_beg; call < csh->cmd_tbl_end; call++) {
            uint8_t clen = 0;
            const char *cpath_next;
            const char *cpath_prev = NULL;
            const char *cpath = call->path;
            bool match = true;

            for (uint8_t Pargi = 0;; Pargi++) {
                if (Pargi >= Pargc) {
                    bool exist = false;

                    for (uint8_t i = 0; i < argc; i++) {
                        if ((clen == argl[i]) && (strncmp(argv[i], cpath_prev, clen) == 0)) {
                            exist = true;
                            break;
                        }
                    }

                    if (exist == false) {
                        argv[argc] = cpath_prev;
                        argl[argc] = clen;
                        argc++;
                        if (argc >= argcmax) {
                            return argc;
                        }
                    }

                    break;
                }

                if ((cpath == NULL) || (match == false)) {
                    break;
                }

                cpath++;

                if (strncmp(cpath, Pargv[Pargi], Pargl[Pargi]) != 0) {
                    break;
                }

                cpath_next = strchr(cpath, '/');

                if (cpath_next == NULL) {
                    clen = strlen(cpath);
                } else {
                    clen = (uint8_t)(cpath_next - cpath);
                }

                match = (clen == Pargl[Pargi]);
                cpath_prev = cpath;

                if (cpath != call->name) {
                    cpath = cpath_next == NULL ? (call->name - 1) : cpath_next;
                } else {
                    cpath = cpath_next;
                }
            }
        }

        /*!< match from port of file system */
        PATH = strchr(PATH, ':');
        PATH = (PATH != NULL) ? PATH + 1 : PATH;
    }

    return argc;
}
#endif

#if defined(CONFIG_CSH_USER_CALLBACK) && CONFIG_CSH_USER_CALLBACK
/*****************************************************************************
* @brief        user callback
*
* @param[in]    rl          readline instance
* @param[in]    exec        exec code
*
* @retval                   status
*****************************************************************************/
static int chry_shell_user_callback(chry_readline_t *rl, uint8_t exec)
{
    chry_shell_t *csh = chry_shell_container_of(rl, chry_shell_t, rl);
    (void)csh;
    /*!< user event callback will not output newline automatically */

    switch (exec) {
        case CHRY_READLINE_EXEC_EOF:
            chry_readline_newline(rl);
            break;

#if defined(CONFIG_CSH_MULTI_THREAD) && CONFIG_CSH_MULTI_THREAD
#if defined(CONFIG_CSH_SIGNAL_HANDLER) && CONFIG_CSH_SIGNAL_HANDLER
        case CHRY_READLINE_EXEC_SIGINT:
            csh->sighdl[0](csh, CSH_SIGINT);
            return 0;
        case CHRY_READLINE_EXEC_SIGQUIT:
            csh->sighdl[1](csh, CSH_SIGQUIT);
            return 0;
        case CHRY_READLINE_EXEC_SIGCONT:
            csh->sighdl[6](csh, CSH_SIGCONT);
            return 1;
        case CHRY_READLINE_EXEC_SIGSTOP:
            csh->sighdl[4](csh, CSH_SIGSTOP);
            return 1;
        case CHRY_READLINE_EXEC_SIGTSTP:
            csh->sighdl[5](csh, CSH_SIGTSTP);
            return 1;
#else
        case CHRY_READLINE_EXEC_SIGINT:
            sighdl[0](csh, CSH_SIGINT);
            return 0;
        case CHRY_READLINE_EXEC_SIGQUIT:
            sighdl[1](csh, CSH_SIGQUIT);
            return 0;
        case CHRY_READLINE_EXEC_SIGCONT:
            sighdl[6](csh, CSH_SIGCONT);
            return 1;
        case CHRY_READLINE_EXEC_SIGSTOP:
            sighdl[4](csh, CSH_SIGSTOP);
            return 1;
        case CHRY_READLINE_EXEC_SIGTSTP:
            sighdl[5](csh, CSH_SIGTSTP);
            return 1;
#endif
#else
        case CHRY_READLINE_EXEC_SIGINT:
            return 0;
        case CHRY_READLINE_EXEC_SIGQUIT:
            return 0;
        case CHRY_READLINE_EXEC_SIGCONT:
            return 1;
        case CHRY_READLINE_EXEC_SIGSTOP:
            return 1;
        case CHRY_READLINE_EXEC_SIGTSTP:
            return 1;
#endif
        case CHRY_READLINE_EXEC_F1 ... CHRY_READLINE_EXEC_F12:
            chry_readline_newline(rl);
            break;
        case CHRY_READLINE_EXEC_USER ... CHRY_READLINE_EXEC_END:
            chry_readline_newline(rl);
            break;
        default:
            return -1;
    }

    /*!< return 1 will not refresh */
    /*!< return 0 to refresh whole line (include prompt) */
    /*!< return -1 to end readline (error) */
    return 0;
}
#endif

/*****************************************************************************
* @brief        init shell
*
* @param[in]    csh         shell instance
* @param[in]    init        init config
*
* @retval                   0:Success -1:Error
*****************************************************************************/
int chry_shell_init(chry_shell_t *csh, const chry_shell_init_t *init)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, -1);
    CHRY_SHELL_PARAM_CHECK(NULL != init, -1);
    CHRY_SHELL_PARAM_CHECK(NULL != init->sget, -1);
    CHRY_SHELL_PARAM_CHECK(NULL != init->sput, -1);

#if defined(CONFIG_CSH_PROMPTEDIT) && CONFIG_CSH_PROMPTEDIT
    CHRY_SHELL_PARAM_CHECK(NULL != init->prompt_buffer, -1);
#endif

#if defined(CONFIG_CSH_HISTORY) && CONFIG_CSH_HISTORY
    CHRY_SHELL_PARAM_CHECK(NULL != init->history_buffer, -1);
#endif

#if defined(CONFIG_CSH_LNBUFF_STATIC) && CONFIG_CSH_LNBUFF_STATIC
    CHRY_SHELL_PARAM_CHECK(NULL != init->line_buffer, -1);
#endif

    int ret = 0;
    chry_readline_init_t rl_init;

    /*!< do readline init */
#if defined(CONFIG_CSH_PROMPTEDIT) && CONFIG_CSH_PROMPTEDIT
    rl_init.prompt = init->prompt_buffer;
    rl_init.pptsize = init->prompt_buffer_size;
#else
    rl_init.prompt = "$ ";
    rl_init.pptsize = 3;
#endif
    rl_init.history = init->history_buffer;
    rl_init.histsize = init->history_buffer_size;
    rl_init.sput = init->sput;
    rl_init.sget = init->sget;

    if (chry_readline_init(&csh->rl, &rl_init)) {
        return -1;
    }

    csh->cmd_tbl_beg = init->command_table_beg;
    csh->cmd_tbl_end = init->command_table_end;
    csh->var_tbl_beg = init->variable_table_beg;
    csh->var_tbl_end = init->variable_table_end;

#if defined(CONFIG_CSH_LNBUFF_STATIC) && CONFIG_CSH_LNBUFF_STATIC
    csh->linebuff = init->line_buffer;
    csh->buffsize = init->line_buffer_size;
    csh->linesize = 0;
#endif

    csh->exec_code = 0;
    csh->exec = CSH_STATUS_EXEC_IDLE;

    /*!< set default user id */
    if ((init->uid >= CONFIG_CSH_MAX_USER) || (init->uid < 0)) {
        return -1;
    }
    csh->uid = init->uid;

    /*!< set host name */
    if (strnlen(init->host, 255) == 255) {
        return -1;
    }
    csh->host = init->host;

    /*!< set users name */
    for (uint8_t i = 0; i < CONFIG_CSH_MAX_USER; i++) {
        if (strnlen(init->user[i], 255) == 255) {
            return -1;
        }
        if (strnlen(init->hash[i], 255) == 255) {
            return -1;
        }
        csh->user[i] = init->user[i];
        csh->hash[i] = init->hash[i];
    }

    /*!< set default path */
#if defined(CONFIG_CSH_MAXLEN_PATH) && CONFIG_CSH_MAXLEN_PATH
    csh->path[0] = '/';
    csh->path[1] = '\0';
#else
    csh->path = "/";
#endif

    csh->data = NULL;
    csh->user_data = init->user_data;

#if defined(CONFIG_CSH_COMPLETION) && CONFIG_CSH_COMPLETION
    chry_readline_set_completion_cb(&csh->rl, chry_shell_completion_callback);
#endif

#if defined(CONFIG_CSH_USER_CALLBACK) && CONFIG_CSH_USER_CALLBACK
    chry_readline_set_user_cb(&csh->rl, chry_shell_user_callback);
#endif

#if defined(CONFIG_CSH_PROMPTEDIT) && CONFIG_CSH_PROMPTEDIT
    ret |= chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_USER, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_GREEN, .bold = 1 }.raw, csh->user[csh->uid]);
    ret |= chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_USER + 1, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_GREEN, .bold = 1 }.raw, "@");
    ret |= chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_HOST, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_GREEN, .bold = 1 }.raw, csh->host);
    ret |= chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_HOST + 1, 0, ":");
    ret |= chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_PATH, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_BLUE, .bold = 1 }.raw, csh->path);
    ret |= chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_PATH + 1, 0, "$ ");
#else

#endif
    return ret;
}

/*****************************************************************************
* @brief        execute task internal
*
* @param[in]    csh         shell instance
*
*****************************************************************************/
static void chry_shell_task_exec_internal(chry_shell_t *csh, int argc, const char **argv)
{
    volatile uint8_t *pexec = (void *)&csh->exec;
    volatile int *pcode = (void *)&csh->exec_code;

    /*!< if stage find */
    if (*pexec == CSH_STATUS_EXEC_FIND) {
        /*!< stage prepare */
        *pexec = CSH_STATUS_EXEC_PREP;

        *pcode = ((chry_syscall_func_t)argv[argc + 2])(argc, (void *)argv);

        /*!< stage done */
        *pexec = CSH_STATUS_EXEC_DONE;
    }
}

/*****************************************************************************
* @brief        execute task
*
* @param[in]    csh         shell instance
*
*****************************************************************************/
void chry_shell_task_exec(chry_shell_t *csh)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, );
    (void)csh;
#if defined(CONFIG_CSH_MULTI_THREAD) && CONFIG_CSH_MULTI_THREAD
    chry_shell_task_exec_internal(csh, csh->exec_argc, &csh->exec_argv[0]);
#endif
}

/*****************************************************************************
* @brief        read eval print loop task
*
* @param[in]    csh         shell instance
*
* @retval                   0:Success -1:Error 1:Continue
*****************************************************************************/
int chry_shell_task_repl(chry_shell_t *csh)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, -1);
    char *csh_linebuff;
    uint16_t *csh_linesize;

#if defined(CONFIG_CSH_LNBUFF_STATIC) && CONFIG_CSH_LNBUFF_STATIC
    csh_linebuff = csh->linebuff;
    csh_linesize = &csh->linesize;

    char *line = chry_readline(&csh->rl, csh_linebuff, csh->buffsize, csh_linesize);
#else
    char linebuff[CONFIG_CSH_LNBUFF_SIZE];
    uint16_t linesize;

    csh_linebuff = linebuff;
    csh_linesize = &linesize;

    char *line = chry_readline(&csh->rl, csh_linebuff, CONFIG_CSH_LNBUFF_SIZE, csh_linesize);
#endif

    if (line == NULL) {
        return -1;
    } else if (line == (void *)-1) {
        return 1;
    } else if (*csh_linesize) {
        int *argc;
        const char **argv;
        volatile uint8_t *pexec = (void *)&csh->exec;
        volatile uint32_t *pcode = (void *)&csh->exec_code;
        *pcode = 0xBAD2BE8E; /*!< a magic number */

#if defined(CONFIG_CSH_MULTI_THREAD) && CONFIG_CSH_MULTI_THREAD
        argc = &csh->exec_argc;
        argv = &csh->exec_argv[0];
#else
        int csh_exec_argc;
        const char *csh_exec_argv[CONFIG_CSH_MAX_ARG + 3];
        argc = &csh_exec_argc;
        argv = &csh_exec_argv[0];
#endif

        *argc = chry_shell_parse(line, *csh_linesize, argv, CONFIG_CSH_MAX_ARG + 1);

        /*!< compile environment variable */
        for (uint8_t i = 0; i < *argc;) {
            if (argv[i][0] == '$') {
                const char *param = chry_shell_getenv(csh, argv[i] + 1);

                if (param == NULL) {
                    memmove(&argv[i], &argv[i + 1], (*argc - i) * sizeof(char *));
                    *argc = (*argc > 0) ? *argc - 1 : 0;
                } else {
                    argv[i] = param;
                    i++;
                }
            } else {
                i++;
            }
        }

        argv[*argc + 1] = (void *)csh;
        argv[*argc + 2] = NULL;

        if (*argc) {
            uint8_t size = strlen(argv[0]);
            const char *PATH = NULL;
            uint8_t Pargc;
            uint8_t Pargl[CONFIG_CSH_MAXSEG_PATH + 1];
            const char *Pargv[CONFIG_CSH_MAXSEG_PATH + 1];

            if (((size >= 1) && (argv[0][0] == '/')) ||
                ((size >= 2) && (argv[0][0] == '.') && (argv[0][1] == '/')) ||
                ((size >= 3) && (argv[0][0] == '.') && (argv[0][1] == '.') && (argv[0][2] == '/'))) {
                /*!< search from the current path */
                PATH = csh->path;
            } else {
                /*!< search from the environment PATH */
                for (const chry_sysvar_t *var = csh->var_tbl_beg; var < csh->var_tbl_end; var++) {
                    if (strcmp("PATH", var->name) == 0) {
                        PATH = var->var;
                        break;
                    }
                }
            }

            while (PATH != NULL) {
                Pargc = chry_shell_path_resolve(PATH, argv[0], Pargv, Pargl, CONFIG_CSH_MAXSEG_PATH + 1);

                /*!< match from system path */
                for (const chry_syscall_t *call = csh->cmd_tbl_beg; call < csh->cmd_tbl_end; call++) {
                    uint8_t clen = 0;
                    const char *cpath_next;
                    const char *cpath = call->path;

                    for (uint8_t Pargi = 0;; Pargi++) {
                        if (Pargi >= Pargc) {
                            if (cpath == NULL) {
                                /*!< match success */
                                argv[*argc + 2] = (void *)call->func;
                                goto found;
                            } else {
                                /*!< argv[0] end */
                                break;
                            }
                        }

                        if (cpath == NULL) {
                            /*!< path+name end */
                            break;
                        }

                        cpath++;

                        if (strncmp(cpath, Pargv[Pargi], Pargl[Pargi]) != 0) {
                            /*!< match failed */
                            break;
                        }

                        cpath_next = strchr(cpath, '/');

                        if (cpath_next == NULL) {
                            clen = strlen(cpath);
                        } else {
                            clen = (uint8_t)(cpath_next - cpath);
                        }

                        if (clen != Pargl[Pargi]) {
                            /*!< len not equal */
                            break;
                        }

                        if (cpath != call->name) {
                            cpath = cpath_next == NULL ? (call->name - 1) : cpath_next;
                        } else {
                            cpath = cpath_next;
                        }
                    }
                }

                /*!< match from port of file system */
                PATH = strchr(PATH, ':');
                PATH = (PATH != NULL) ? PATH + 1 : PATH;
            }

        found:

            if (NULL == argv[*argc + 2]) {
                *pexec = CSH_STATUS_EXEC_IDLE;

                csh->rl.sput(&csh->rl, argv[0], strlen(argv[0]));
                csh->rl.sput(&csh->rl, ": command not found" CONFIG_CSH_NEWLINE CONFIG_CSH_NEWLINE,
                             19 + (sizeof(CONFIG_CSH_NEWLINE) ? (sizeof(CONFIG_CSH_NEWLINE) - 1) * 2 : 0));
            } else {
#if defined(CONFIG_CSH_MULTI_THREAD) && CONFIG_CSH_MULTI_THREAD
                *pexec = CSH_STATUS_EXEC_FIND;

                chry_readline_ignore(&csh->rl, true); /*!< accepts only signal inputs */
                chry_readline_auto_refresh(&csh->rl, false);

                /*!< reset signal handler to default */
                for (uint8_t i = 0; i < CSH_SIGNAL_COUNT; i++) {
#if defined(CONFIG_CSH_SIGNAL_HANDLER) && CONFIG_CSH_SIGNAL_HANDLER
                    csh->sighdl[i] = chry_shell_port_default_handler;
#else
                    sighdl[i] = chry_shell_port_default_handler;
#endif
                }

                /*!< try to create execute context */
                if (0 != chry_shell_port_create_context(csh, *argc, argv)) {
                    *pexec = CSH_STATUS_EXEC_IDLE;

                    csh->rl.sput(&csh->rl, argv[0], strlen(argv[0]));
                    csh->rl.sput(&csh->rl, ": context creation error" CONFIG_CSH_NEWLINE CONFIG_CSH_NEWLINE,
                                 24 + (sizeof(CONFIG_CSH_NEWLINE) ? (sizeof(CONFIG_CSH_NEWLINE) - 1) * 2 : 0));
                }
#else
                *pexec = CSH_STATUS_EXEC_FIND;
                chry_shell_task_exec_internal(csh, *argc, argv);
                *pexec = CSH_STATUS_EXEC_IDLE;
#endif
            }
        }

        return 0;
    }

    return 0;
}

/*****************************************************************************
* @brief        parse line to argc,argv[]
*
* @param[in]    line        read line
* @param[in]    linesize    strlen(line)
* @param[out]   argv        argument value
* @param[out]   argcmax     max argument count (max segment count)
*
* @retval                   argc:argument count, max is argcmax
*****************************************************************************/
int chry_shell_parse(char *line, uint32_t linesize, const char **argv, uint8_t argcmax)
{
    int argc = 0;
    bool ignore = false;
    bool start = false;
    bool escape = false;

    argcmax = (argcmax > 0) ? argcmax - 1 : argcmax;

    while (linesize) {
        char c = *line;

        if (c == '\0') {
            break;
        } else if ((escape == false) && (c == '\\')) {
            escape = true;
            memmove(line, line + 1, linesize);
            line--;
        } else if ((escape == false) && (ignore == false) && (c == ' ')) {
            start = false;
            *line = '\0';
        } else if ((escape == false) && (c == '"')) {
            memmove(line, line + 1, linesize);
            line--;
            ignore = !ignore;
        } else if (!start) {
            argv[argc] = line;
            if (argc < argcmax) {
                argc++;
            } else {
                break;
            }
            start = true;
            escape = false;
        } else {
            escape = false;
        }

        line++;
        linesize--;
    }

    *line = '\0';
    argv[argc] = NULL;

    return argc;
}

/*****************************************************************************
* @brief        resolve path to argc,argv[],argl[]
*
* @param[in]    cur         current path
* @param[in]    path        path
* @param[out]   argv        argument value (path segment)
* @param[out]   argl        argument length (path segment length)
* @param[out]   argcmax     argument max count (max segment count)
*
* @retval                   argc:argument count, max is argcmax
*****************************************************************************/
int chry_shell_path_resolve(const char *cur, const char *path, const char **argv, uint8_t *argl, uint8_t argcmax)
{
    uint8_t len;
    uint8_t argc = 0;
    bool expr;

    if (cur == NULL || path == NULL) {
        return argc;
    }

    expr = (*path == '/') || ((cur[0] == '/') && (cur[1] == '\0'));

    if (expr) {
        cur = path;
    }

    argcmax = (argcmax > 0) ? argcmax - 1 : argcmax;

resolve:

    while ((cur != NULL) && (*cur != '\0') && (*cur != ':')) {
        while (*cur == '/') {
            cur++;
        }
        argv[argc] = cur;
        cur = strchr(cur, '/');

        if (cur == NULL) {
            len = strlen(argv[argc]);
        } else {
            len = (uint8_t)(cur - argv[argc]);
            const char *sep = memchr(argv[argc], ':', len);
            if (sep != NULL) {
                len = (uint8_t)(sep - argv[argc]);
                cur = NULL;
            }
        }

        if (len == 1) {
            bool match_dot1 = argv[argc][0] == '.';
            if (!match_dot1) {
                argl[argc] = len;
                argc++;
            }
        } else if (len == 2) {
            bool match_dot1 = argv[argc][0] == '.';
            bool match_dot2 = argv[argc][1] == '.';

            if (match_dot1 && match_dot2) {
                if (argc > 0) {
                    argc--;
                }
            } else {
                argl[argc] = len;
                argc++;
            }
        } else {
            argl[argc] = len;
            argc++;
        }

        if (argc >= argcmax) {
            argv[argc] = NULL;
            argl[argc] = 0;
            return argcmax;
        }
    }

    if (!expr) {
        cur = path;
        expr = true;
        goto resolve;
    }

    argv[argc] = NULL;
    argl[argc] = 0;
    return argc;
}

/*****************************************************************************
* @brief        set host
*
* @param[in]    csh         shell instance
* @param[in]    host        pointer of host
*
* @retval                   0:Success -1:Error
*****************************************************************************/
int chry_shell_set_host(chry_shell_t *csh, const char *host)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, -1);
    CHRY_SHELL_PARAM_CHECK(NULL != host, -1);

    if (strnlen(host, 255) == 255) {
        return -1;
    }

    csh->host = host;

    return chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_HOST, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_GREEN, .bold = 1 }.raw, csh->host);
}

/*****************************************************************************
* @brief        set user
*
* @param[in]    csh         shell instance
* @param[in]    uid         user id
* @param[in]    user        pointer of user
* @param[in]    user        pointer of user password hash
*
* @retval                   0:Success -1:Error
*****************************************************************************/
int chry_shell_set_user(chry_shell_t *csh, uint8_t uid, const char *user, const char *hash)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, -1);
    CHRY_SHELL_PARAM_CHECK(NULL != user, -1);

    if (uid < CONFIG_CSH_MAX_USER) {
        if (strnlen(user, 255) == 255) {
            return -1;
        }
        if (strnlen(hash, 255) == 255) {
            return -1;
        }

        csh->user[uid] = user;
        csh->hash[uid] = hash;
        if (uid == csh->uid) {
            return chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_USER, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_GREEN, .bold = 1 }.raw, csh->user[uid]);
        }
        return 0;
    }

    return -1;
}

/*****************************************************************************
* @brief        set path
*
* @param[in]    csh         shell instance
* @param[in]    size        strlen(path) + 1, include \0.
*                           ignore when CONFIG_CSH_MAXLEN_PATH = 0
* @param[in]    path        shell instance
*
* @retval                   0:Success -1:Error
*****************************************************************************/
int chry_shell_set_path(chry_shell_t *csh, uint8_t size, const char *path)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, -1);
    CHRY_SHELL_PARAM_CHECK(NULL != path, -1);

#if defined(CONFIG_CSH_MAXLEN_PATH) && CONFIG_CSH_MAXLEN_PATH
    if (size > CONFIG_CSH_MAXLEN_PATH) {
        return -1;
    }

    memcpy(csh->path, path, size);
    csh->path[size] = '\0';

#else
    if (strnlen(path, 255) == 255) {
        return -1;
    }
    csh->path = path;
#endif

    return chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_PATH, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_BLUE, .bold = 1 }.raw, csh->path);
}

/*****************************************************************************
* @brief        get path
*
* @param[in]    csh         shell instance
* @param[in]    size        path buffer size
* @param[in]    path        path buffer
*
*****************************************************************************/
void chry_shell_get_path(chry_shell_t *csh, uint8_t size, char *path)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, );
    CHRY_SHELL_PARAM_CHECK(NULL != path, );

#if defined(CONFIG_CSH_MAXLEN_PATH) && CONFIG_CSH_MAXLEN_PATH
    size = size < CONFIG_CSH_MAXLEN_PATH ? size : CONFIG_CSH_MAXLEN_PATH;
#endif

    strncpy(path, csh->path, size);
    path[size - 1] = '\0';
}

/*****************************************************************************
* @brief        substitute user
*
* @param[in]    csh         shell instance
* @param[in]    uid         user id
* @param[in]    password    password
*
* @retval                   0:Success -1:Error
*****************************************************************************/
int chry_shell_substitute_user(chry_shell_t *csh, uint8_t uid, const char *password)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, -1);

    if (uid < CONFIG_CSH_MAX_USER) {
        if (csh->hash[uid]) {
            if (chry_shell_port_hash_strcmp(csh->hash[uid], password) != 0) {
                return -1;
            }
        }

        csh->uid = uid;

        return chry_readline_prompt_edit(&csh->rl, CSH_PROMPT_SEG_USER, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_GREEN, .bold = 1 }.raw, csh->user[uid]);
    }

    return -1;
}

/*****************************************************************************
* @brief        get environment variable
*
* @param[in]    csh         shell instance
* @param[in]    name        env name
*
* @retval                   env string pointer or NULL
* @note                     env string maybe read-only
*****************************************************************************/
char *chry_shell_getenv(chry_shell_t *csh, const char *name)
{
    CHRY_SHELL_PARAM_CHECK(NULL != csh, NULL);

    char *env = NULL;
    for (const chry_sysvar_t *var = csh->var_tbl_beg; var < csh->var_tbl_end; var++) {
        if (strcmp(name, var->name) == 0) {
            env = var->var;
            break;
        }
    }

    return env;
}

/*****************************************************************************
* @brief        print from shell
*
* @param[in]    csh         shell instance
* @param[in]    fmt         string format
*
* @retval                   size that has printed
*****************************************************************************/
int csh_printf(chry_shell_t *csh, const char *fmt, ...)
{
  int     n;
  char  shell_printf_buffer[CONFIG_CSH_PRINT_BUFFER_SIZE];
  va_list args;

  va_start (args, fmt);
  n = vsnprintf(shell_printf_buffer, sizeof(shell_printf_buffer), fmt, args);
  if (n > (int)sizeof(shell_printf_buffer)) {
    csh->rl.sput(&csh->rl, shell_printf_buffer, sizeof(shell_printf_buffer));
  } else if (n > 0) {
    csh->rl.sput(&csh->rl, shell_printf_buffer, n);
  }
  va_end(args);
  return n;
}
