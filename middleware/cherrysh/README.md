# CherryShell

[中文版](./README_zh.md)

## Brief

CherryShell is a tiny shell specifically designed for embedded applications.

## Features
- [x] Path completion support, with tab for path completion
- [x] Command completion support, with tab for command completion
- [x] Environment variable support, using `$` as a prefix, e.g., `$PATH`
- [x] Automatic command search in specified directories using the `$PATH` environment variable
- [x] History record support, with up and down arrows
- [x] Compatibility with VT100 and Xterm key values
- [x] Support for setting username, hostname, and path
- [x] Non-blocking mode support
- [x] User login support, requiring implementation of a hash function (default: strcmp)
- [ ] Shortcut key support for `Ctrl + <key>`, `Alt + <key>`, `F1-F12` to invoke commands
- [ ] Key function remapping support for `Ctrl + <key>`, `Alt + <key>`, `F1-F12`
- [ ] Support for exec function family to directly call commands
- [ ] File system support, including FatFS, FileX, LittleFS, RomFS, etc.
- [ ] Support for adding, modifying, deleting, and retrieving environment variables
- [ ] Signal handling support, capturing and handling different signals such as Ctrl+C ``SIGINT`` and Ctrl+Z ``SIGTSTP``
- [ ] Support for exit function to terminate command execution, return to the specified handler, using setjmp (barebonel)
- [ ] Input/output redirection support
- [ ] Multiple user command permission support
- [ ] Job control support, allowing commands to run in the foreground or background, and using control commands (e.g., fg, bg, jobs) to manage and manipulate jobs

## Demo

![test1](./doc/test1.png)

![test2](./doc/test2.png)

## Porting

Taking the example of the HPMicro hpm5301evklite board.

``` c
// Include header file
#include "csh.h"

// Create a shell instance
static chry_shell_t csh;

```

``` c
// Character output function, directly outputs characters to the serial port
static uint16_t csh_sput_cb(chry_readline_t *rl, const void *data, uint16_t size)
{
    uint16_t i;
    (void)rl;
    for (i = 0; i < size; i++) {
        if (status_success != uart_send_byte(HPM_UART0, ((uint8_t *)data)[i])) {
            break;
        }
    }

    // Return the number of successfully output characters
    return i;
}

```

``` c
// Character input function, reads characters directly from the serial port
// If the baud rate is high, a FIFO input buffer can be added
static uint16_t csh_sget_cb(chry_readline_t *rl, void *data, uint16_t size)
{
    uint16_t i;
    (void)rl;
    for (i = 0; i < size; i++) {
        if (status_success != uart_receive_byte(HPM_UART0, (uint8_t *)data + i)) {
            break;
        }
    }

    // Return the number of successfully read characters
    return i;
}

```

``` c
// Shell initialization
int shell_init(void)
{
    // Structure for configuring initialization parameters
    chry_shell_init_t csh_init;

    // Set the character input and output functions (must be implemented)
    csh_init.sput = csh_sput_cb;
    csh_init.sget = csh_sget_cb;

    // Symbols defined in the linkscript, used to store exported commands and variables
    extern const int __fsymtab_start;
    extern const int __fsymtab_end;
    extern const int __vsymtab_start;
    extern const int __vsymtab_end;

    // Configure the start and end addresses of the function and variable tables (must be implemented)
    csh_init.command_table_beg = &__fsymtab_start;
    csh_init.command_table_end = &__fsymtab_end;
    csh_init.variable_table_beg = &__vsymtab_start;
    csh_init.variable_table_end = &__vsymtab_end;

    // Define a prompt buffer
    static char csh_prompt_buffer[128];
    // Configure the prompt buffer (optional)
    // Depends on whether the editable prompt feature is enabled (CONFIG_CSH_PROMPTEDIT)
    csh_init.prompt_buffer = csh_prompt_buffer;
    csh_init.prompt_buffer_size = sizeof(csh_prompt_buffer);

    // Define a history buffer
    static char csh_history_buffer[128];

    // Configure the history buffer (optional)
    // Depends on whether the history record feature is enabled (CONFIG_CSH_LNBUFF_STATIC)
    csh_init.history_buffer = csh_history_buffer;
    csh_init.history_buffer_size = sizeof(csh_history_buffer);

    // Define a line input buffer
    static char csh_line_buffer[128];

    // Configure the line input buffer (optional)
    // Depends on whether the static line buffer feature is enabled (CONFIG_CSH_LNBUFF_STATIC)
    // If the static line buffer feature is not enabled, the line buffer will exist on the stack,
    //   and non-blocking mode (CONFIG_CSH_NOBLOCK) must be set to 0
    csh_init.line_buffer = csh_line_buffer;
    csh_init.line_buffer_size = sizeof(csh_line_buffer);

    // Default user count is 1
    csh_init.uid = 0; // Default user ID
    csh_init.user[0] = "cherry"; // Username for user ID 0
    csh_init.hash[0] = NULL; // Password hash value for user ID 0
    csh_init.host = "hpm5301evklite"; // Host name

    // Initialization
    int ret = chry_shell_init(&csh, &csh_init);
    if (ret) {
        return -1;
    }

    return 0;
}

```

``` c
// Main function of the shell, needs to be called in a while(1) loop
int shell_main(void)
{
    int ret;

restart:
    ret = chry_shell_task_repl(&csh);
    if (ret == -1) {
        // Execution failed or encountered an issue
        return -1;
    } else if (ret == 1) {
        // Non-blocking mode: Insufficient characters read during character input, return to perform other user tasks
        return 0;
    } else {
        // Execution successful, restart
        goto restart;
    }
}
```

``` c
// Used to avoid competition between printf and shell for the same serial port, called before printf
void shell_uart_lock(void)
{
    chry_readline_erase_line(&csh.rl);
}

// Used to avoid competition between printf and shell for the same serial port, called after printf
void shell_uart_unlock(void)
{
    chry_readline_edit_refresh(&csh.rl);
}
```

``` c
int main(void)
{
    board_init();
    board_init_led_pins();

    shell_init();

    uint32_t freq = clock_get_frequency(clock_mchtmr0);
    uint64_t time = mchtmr_get_count(HPM_MCHTMR) / (freq / 1000);

    while (1) {
        shell_main();

        // Print every 5 seconds
        uint64_t now = mchtmr_get_count(HPM_MCHTMR) / (freq / 1000);
        if (now > time + 5000) {
            time = now;
            // Call the lock to avoid printf interfering with shell input
            shell_uart_lock();
            printf("other task interval 5S\r\n");
            shell_uart_unlock();
        }
    }

    return 0;
}

```

``` c
// Implement the export of a command
// write_led 1 - Turn on the LED
// write_led 0 - Turn off the LED
static int write_led(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: write_led <status>\r\n\r\n");
        printf("  status    0 or 1\r\n\r\n");
        return -1;
    }

    board_led_write(atoi(argv[1]) == 0);
    return 0;
}
CSH_CMD_EXPORT(write_led, );

```
