#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "chry_readline.h"

struct termios orig_termios;

static void disableRawMode(int fd)
{
    tcsetattr(fd, TCSAFLUSH, &orig_termios);
}

static void AtExit(void)
{
    disableRawMode(STDIN_FILENO);
}

/* Raw mode: 1960 magic shit. */
static int enableRawMode(int fd)
{
    struct termios raw;

    if (!isatty(STDIN_FILENO))
        goto fatal;

    atexit(AtExit);

    if (tcgetattr(fd, &orig_termios) == -1)
        goto fatal;

    raw = orig_termios; /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer.
     * We want read to return every single byte, without timeout. */
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0; /* 1 byte, no timer */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(fd, TCSAFLUSH, &raw) < 0)
        goto fatal;

    return 0;

fatal:
    errno = ENOTTY;
    return -1;
}

static uint16_t sput(chry_readline_t *rl, const void *data, uint16_t size)
{
    uint16_t i;
    (void)rl;
    for (i = 0; i < size; i++) {
        if (1 != write(STDOUT_FILENO, (uint8_t *)data + i, 1)) {
            break;
        }
    }

    return i;
}

static uint16_t sget(chry_readline_t *rl, void *data, uint16_t size)
{
    uint16_t i;
    (void)rl;
    for (i = 0; i < size; i++) {
        if (1 != read(STDIN_FILENO, (uint8_t *)data + i, 1)) {
            break;
        }
    }

    return i;
}

chry_readline_t rl;

static int ucb(chry_readline_t *rl, uint8_t exec)
{
    /*!< user event callback will not output newline automatically */
    chry_readline_newline(rl);

    switch (exec) {
        case CHRY_READLINE_EXEC_EOF:
            fprintf(stderr, "EOF\r\n");
            disableRawMode(STDIN_FILENO);
            exit(0);
            break;
        case CHRY_READLINE_EXEC_SIGINT:
            chry_readline_ignore(rl, false);
            fprintf(stderr, "SIGINT\r\n");
            break;
        case CHRY_READLINE_EXEC_SIGQUIT:
            fprintf(stderr, "SIGQUIT\r\n");
            break;
        case CHRY_READLINE_EXEC_SIGCONT:
            fprintf(stderr, "SIGCONT\r\n");
            break;
        case CHRY_READLINE_EXEC_SIGSTOP:
            fprintf(stderr, "SIGSTOP\r\n");
            break;
        case CHRY_READLINE_EXEC_SIGTSTP:
            fprintf(stderr, "SIGTSTP\r\n");
            break;
        case CHRY_READLINE_EXEC_F1 ... CHRY_READLINE_EXEC_F12:
            fprintf(stderr, "F%d event\r\n", exec - CHRY_READLINE_EXEC_F1 + 1);
            break;
        case CHRY_READLINE_EXEC_USER ... CHRY_READLINE_EXEC_END:
            fprintf(stderr, "U%d event\r\n", exec - CHRY_READLINE_EXEC_USER + 1);
            break;
        default:
            fprintf(stderr, "ERROR EXEC %d\r\n", exec);
            return -1;
    }

    /*!< return 1 will not refresh */
    /*!< return 0 to refresh whole line (include prompt) */
    /*!< return -1 to end readline (error) */
    return 0;
}

// clang-format off
static const char *clist[] = {
    "hello", "world", "hell", "/exit", "/mask", "/unmask", "/prompt","/ignore",
    "hello0","hello1","hello2","hello3","hello4","hello5","hello6","hello7","hello8","hello9",
    "hello10","hello11","hello12","hello13","hello14","hello15","hello16","hello17","hello18","hello19",
    "hello20","hello21","hello22","hello23","hello24","hello25","hello26","hello27","hello28","hello29",
    "hello30","hello31","hello32","hello33","hello344444","hello35","hello36","hello37","hello38","hello39",
    "hello40","hello41","hello42","hello43","hello44","hello45","hello46","hello47","hello48","hello49",
    "hello50","hello51","hello52","hello53","hello54","hello55","hello56","hello57","hello58","hello59",
    "hello60","hello61","hello62","hello63","hello64","hello65","hello66","hello67","hello68","hello69",
    "hello70","hello71","hello72","hello73","hello74","hello75","hello76","hello77","hello78","hello79",
    "hello80","hello81","hello82","hello83","hello84","hello85","hello86","hello87","hello88","hello89",
};
// clang-format on

static uint8_t acb(chry_readline_t *rl, char *pre, uint16_t *size, const char **argv, uint8_t *argl, uint8_t argcmax)
{
    (void)rl;
    uint8_t argc = 0;

    for (uint32_t i = 0; i < sizeof(clist) / sizeof(char *); i++) {
        if (strncmp(pre, clist[i], *size) == 0) {
            argv[argc] = clist[i];
            argl[argc] = strlen(clist[i]);
            argc++;
            if (argc >= argcmax) {
                break;
            }
        }
    }

    return argc;
}

int main(int argc, char **argv)
{
    char *prgname = argv[0];
    uint8_t keycode = 0;
    uint8_t xterm = 0;
    uint8_t repl = 0;
    (void)keycode;

    while (argc > 1) {
        argc--;
        argv++;
        if (!strcmp(*argv, "--keycodes")) {
            keycode = 1;
        } else if (!strcmp(*argv, "--xterm")) {
            xterm = 1;
        } else if (!strcmp(*argv, "--autosize")) {
            xterm = 1;
        } else if (!strcmp(*argv, "--repl")) {
            repl = 1;
        } else {
            fprintf(stderr, "Usage: %s [--keycodes] [--xterm] [--autosize] [--repl]\n", prgname);
            exit(1);
        }
    }

    enableRawMode(STDIN_FILENO);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return 1;
    }

    if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        return 1;
    }

    char prompt[256];
    char history[256];

    char linebuf[128];
    char *line;
    uint16_t linesize;

    memset(prompt, 0xff, 256);

    chry_readline_init_t rl_init = {
        .prompt = prompt,
        .pptsize = 34 + 11,
        .history = history,
        .histsize = sizeof(history), /*!< size must be power of 2 !!! */
        .sget = sget,
        .sput = sput
    };

    assert(0 == chry_readline_init(&rl, &rl_init));

    /*!< the segidx must be incremented at first */
    assert(0 == chry_readline_prompt_edit(&rl, 0, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_GREEN, .bold = 1 }.raw, "cherry"));
    assert(0 == chry_readline_prompt_edit(&rl, 1, 0, ":"));
    assert(0 == chry_readline_prompt_edit(&rl, 2, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_BLUE, .bold = 1 }.raw, "~"));
    assert(0 == chry_readline_prompt_edit(&rl, 3, 0, "$ "));

#if !CONFIG_READLINE_PROMPTEDIT
    memcpy(prompt, "nopptedit:~$ ", sizeof("nopptedit:~$ "));
#endif

#if CONFIG_READLINE_DEBUG
    if (keycode) {
        chry_readline_debug(&rl);
        disableRawMode(STDIN_FILENO);
        return 0;
    }
#endif

    (void)xterm;
    if (xterm) {
        chry_readline_detect(&rl);
    }

    chry_readline_set_completion_cb(&rl, acb);
    chry_readline_set_user_cb(&rl, ucb);

    /*!< mapping ctrl+q to exec user event 1 */
    chry_readline_set_ctrlmap(&rl, CHRY_READLINE_CTRLMAP_X, CHRY_READLINE_EXEC_USER);
    /*!< mapping alt+q to exec user event 2 */
    chry_readline_set_altmap(&rl, CHRY_READLINE_ALTMAP_X, CHRY_READLINE_EXEC_USER + 1);

    if (repl) {
        goto repl;
    }

    uint8_t i = 0;
    uint32_t taskcnt = 0;

    while (1) {
        line = chry_readline(&rl, linebuf, sizeof(linebuf), &linesize);
        if (line == NULL) {
            printf("chry_readline error\r\n");
            break;
        } else if (line == (void *)-1) {
            if (taskcnt++ > 1000 * 10000) {
                chry_readline_erase_line(&rl);
                printf("other task\r\n");
                chry_readline_edit_refresh(&rl);
                taskcnt = 0;
            }
        } else if (linesize) {
            printf("len = %2d <'%s'>\r\n", linesize, line);

            if (strncmp(line, "/mask", linesize) == 0 && linesize == strlen("/mask")) {
                chry_readline_mask(&rl, 1);

            } else if (strncmp(line, "/unmask", linesize) == 0 && linesize == strlen("/unmask")) {
                chry_readline_mask(&rl, 0);

            } else if (strncmp(line, "/prompt", linesize) == 0 && linesize == strlen("/prompt")) {
                int ret = chry_readline_prompt_edit(&rl, 2, (chry_readline_sgr_t){ .foreground = CHRY_READLINE_SGR_BLUE, .bold = 1 }.raw, "~/readline/%d", i++);

                if (ret == 1) {
                    printf("prompt not enouth space\r\n");
                } else if (ret == -1) {
                    /*!< check if the segidx is legal */
                    printf("prompt edit error\r\n");
                    goto end;
                }

            } else if (strncmp(line, "/exit", linesize) == 0 && linesize == strlen("/exit")) {
                disableRawMode(STDIN_FILENO);
                return 0;

            } else if (strncmp(line, "/ignore", linesize) == 0 && linesize == strlen("/ignore")) {
                chry_readline_ignore(&rl, true);
            }
        }
    }

    goto end;

repl:
    if (!CONFIG_READLINE_PROMPTEDIT) {
        printf("repl example, must enable CONFIG_READLINE_PROMPTEDIT\r\n");
        return 0;
    }

    printf("enter repl test mode, end with ':' to enter multiline mode\r\n");
    chry_readline_prompt_clear(&rl);
    assert(0 == chry_readline_prompt_edit(&rl, 0, 0, ">>> "));

    /*!< map ctrl+i(tab) to auto completion or space */
    chry_readline_set_ctrlmap(&rl, CHRY_READLINE_CTRLMAP_I, CHRY_READLINE_EXEC_ACPLT);

    bool multienable = false;
    int linecount = 0;
    char multibuff[32][128];

    while ((line = chry_readline(&rl, multibuff[linecount], 128, &linesize)) != NULL) {
        if (linesize) {
            if (line[linesize - 1] == ':') {
                multienable = true;
                assert(0 == chry_readline_prompt_edit(&rl, 0, 0, "... "));
            }

            if (++linecount > 32) {
                printf("too many line\r\n");
                break;
            }

            if (!multienable) {
                printf("len = %2d <'%s'>\r\n", linesize, line);

                if (strncmp(line, "/exit", linesize) == 0) {
                    if (linesize == strlen("/exit")) {
                        disableRawMode(STDIN_FILENO);
                        return 0;
                    }
                }
            }
        } else {
            /**
             * The first two bytes of the buffer store the linesize of type uint16_t, 
             * followed by the input content and ending \0. \0 is not included in the linesize
            */
            if (multienable) {
                assert(0 == chry_readline_prompt_edit(&rl, 0, 0, ">>> "));

                for (int i = 0; i < linecount; i++) {
                    printf("%3d %s\r\n", *((uint16_t *)multibuff[i]), &multibuff[i][2]);
                }

                multienable = false;
                linecount = 0;
            }
        }
    }

end:
    disableRawMode(STDIN_FILENO);
    return -1;
}
