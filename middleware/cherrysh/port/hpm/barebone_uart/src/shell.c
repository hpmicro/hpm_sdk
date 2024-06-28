#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "hpm_uart_drv.h"
#include "chry_ringbuffer.h"
#include "csh.h"

static chry_shell_t csh;
static UART_Type *shell_uart = NULL;
static bool login = false;
static chry_ringbuffer_t shell_rb;
static uint8_t mempool[1024];

void shell_uart_isr(void)
{
    uint8_t irq_id = uart_get_irq_id(shell_uart);
    if (irq_id == uart_intr_id_rx_data_avail) {
        while (uart_check_status(shell_uart, uart_stat_data_ready)) {
            uint8_t byte = uart_read_byte(shell_uart);
            chry_ringbuffer_write_byte(&shell_rb, byte);
        }
    }
}

static uint16_t csh_sput_cb(chry_readline_t *rl, const void *data, uint16_t size)
{
    uint16_t i;
    (void)rl;
    for (i = 0; i < size; i++) {
        if (status_success != uart_send_byte(shell_uart, ((uint8_t *)data)[i])) {
            break;
        }
    }

    return i;
}

static uint16_t csh_sget_cb(chry_readline_t *rl, void *data, uint16_t size)
{
    (void)rl;
    return chry_ringbuffer_read(&shell_rb, data, size);
}

int shell_init(UART_Type *uart, bool need_login)
{
    chry_shell_init_t csh_init;

    if (uart == NULL) {
        return -1;
    }

    if (chry_ringbuffer_init(&shell_rb, mempool, sizeof(mempool))) {
        return -1;
    }

    if (need_login) {
        login = false;
    } else {
        login = true;
    }

    shell_uart = uart;

    /*!< I/O callback */
    csh_init.sput = csh_sput_cb;
    csh_init.sget = csh_sget_cb;

#if defined(CONFIG_CSH_SYMTAB) && CONFIG_CSH_SYMTAB
#if defined(__GNUC__)
    extern const int __fsymtab_start;
    extern const int __fsymtab_end;
    extern const int __vsymtab_start;
    extern const int __vsymtab_end;

    /*!< get table from ld symbol */
    csh_init.command_table_beg = &__fsymtab_start;
    csh_init.command_table_end = &__fsymtab_end;
    csh_init.variable_table_beg = &__vsymtab_start;
    csh_init.variable_table_end = &__vsymtab_end;
#elif defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCRISCV__)
#pragma section="FSymTab"
#pragma section="VSymTab"
    csh_init.command_table_beg = __section_begin("FSymTab");
    csh_init.command_table_end = __section_end("FSymTab");
    csh_init.variable_table_beg = __section_begin("VSymTab");
    csh_init.variable_table_end = __section_end("VSymTab");
#endif
#endif

#if defined(CONFIG_CSH_PROMPTEDIT) && CONFIG_CSH_PROMPTEDIT
    static char csh_prompt_buffer[128];

    /*!< set prompt buffer */
    csh_init.prompt_buffer = csh_prompt_buffer;
    csh_init.prompt_buffer_size = sizeof(csh_prompt_buffer);
#endif

#if defined(CONFIG_CSH_HISTORY) && CONFIG_CSH_HISTORY
    static char csh_history_buffer[128];

    /*!< set history buffer */
    csh_init.history_buffer = csh_history_buffer;
    csh_init.history_buffer_size = sizeof(csh_history_buffer);
#endif

#if defined(CONFIG_CSH_LNBUFF_STATIC) && CONFIG_CSH_LNBUFF_STATIC
    static char csh_line_buffer[128];

    /*!< set linebuffer */
    csh_init.line_buffer = csh_line_buffer;
    csh_init.line_buffer_size = sizeof(csh_line_buffer);
#endif

    csh_init.uid = 0;
    csh_init.user[0] = "cherry";

    /*!< The port hash function is required,
         and the strcmp attribute is used weakly by default,
         int chry_shell_port_hash_strcmp(const char *hash, const char *str); */
    csh_init.hash[0] = "12345678"; /*!< If there is no password, set to NULL */
    csh_init.host = BOARD_NAME;
    csh_init.user_data = NULL;

    int ret = chry_shell_init(&csh, &csh_init);
    if (ret) {
        return -1;
    }

    return 0;
}

int shell_main(void)
{
    int ret;

restart:

    if (login) {
        goto restart2;
    }

    ret = csh_login(&csh);
    if (ret == 0) {
        login = true;
    } else {
        return 0;
    }

restart2:
    chry_shell_task_exec(&csh);

    ret = chry_shell_task_repl(&csh);

    if (ret == -1) {
        /*!< error */
        return -1;
    } else if (ret == 1) {
        /*!< continue */
        return 0;
    } else {
        /*!< restart */
        goto restart;
    }

    return 0;
}

void shell_lock(void)
{
    chry_readline_erase_line(&csh.rl);
}

void shell_unlock(void)
{
    chry_readline_edit_refresh(&csh.rl);
}

static int csh_exit(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    login = false;
    return 0;
}
CSH_SCMD_EXPORT_ALIAS(csh_exit, exit, );

#define __ENV_PATH "/sbin:/bin"
const char ENV_PATH[] = __ENV_PATH;
CSH_RVAR_EXPORT(ENV_PATH, PATH, sizeof(__ENV_PATH));

#define __ENV_ZERO ""
const char ENV_ZERO[] = __ENV_ZERO;
CSH_RVAR_EXPORT(ENV_ZERO, ZERO, sizeof(__ENV_ZERO));
