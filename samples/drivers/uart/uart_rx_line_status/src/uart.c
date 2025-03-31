/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_gpio_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_dma_mgr.h"

#define TEST_UART                     BOARD_APP_UART_BASE
#define TEST_UART_IRQ                 BOARD_APP_UART_IRQ
#define TEST_UART_CLK_NAME            BOARD_APP_UART_CLK_NAME
#define TEST_UART_RX_DMA_REQ          BOARD_APP_UART_RX_DMA_REQ
#define TEST_APP_UART_BREAK_GPIO_CTRL GPIO_GET_PORT_INDEX(BOARD_APP_UART_BREAK_SIGNAL_PIN)
#define TEST_APP_UART_BREAK_PIN       GPIO_GET_PIN_INDEX(BOARD_APP_UART_BREAK_SIGNAL_PIN)
#define TEST_BUFFER_SIZE              (16U)
typedef enum {
    none_error = 0,
    overrun_error,
    parity_error,
    framing_error,
    line_break,
} rxline_status_test_e;

typedef struct {
    uint8_t num;
    char *num_str;
} test_number_t;

static void show_help(void);
static void rxline_status_test(rxline_status_test_e num);
static void overrun_error_test(void);
static void parity_error_test(void);
static void framing_error_test(void);
static void line_break_test(void);
static char *parity2string(parity_setting_t parity);
static char *stop2string(num_of_stop_bits_t parity);
static hpm_stat_t rxdma_config(void);

const test_number_t test_table[] = {
    {overrun_error,                "*        1 - overrun error test for rx line status            *\n"},
    {parity_error,                 "*        2 - parity error test for rx line status             *\n"},
    {framing_error,                "*        3 - framing error test for rx line status            *\n"},
    {line_break,                   "*        4 - line break test for rx line status               *\n"},
};

#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
/* The lower eight bits of the UART RBR register are data
 * and the upper eight bits can be used to check whether a reception error occurs for each data.
 */
ATTR_PLACE_AT_NONCACHEABLE uint16_t uart_buff[TEST_BUFFER_SIZE];
#else
ATTR_PLACE_AT_NONCACHEABLE uint8_t uart_buff[TEST_BUFFER_SIZE];
#endif

uart_config_t uart_config;
volatile bool rxline_status_detection;
volatile uint32_t rxline_status;
dma_resource_t rxdma_resource;

SDK_DECLARE_EXT_ISR_M(TEST_UART_IRQ, uart_isr)
void uart_isr(void)
{
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    if ((uart_rx_is_fifo_error(TEST_UART) == true) && (uart_get_enabled_irq(TEST_UART) & uart_intr_errf)) {
        uart_clear_rx_fifo_error_flag(TEST_UART);
        rxline_status_detection = true;
    }
    if ((uart_rx_is_break(TEST_UART) == true) && (uart_get_enabled_irq(TEST_UART) & uart_intr_break_err)) {
        uart_clear_rx_break_flag(TEST_UART);
        rxline_status_detection = true;
    }
    if ((uart_rx_is_framing_error(TEST_UART) == true) && (uart_get_enabled_irq(TEST_UART) & uart_intr_framing_err)) {
        uart_clear_rx_framing_error_flag(TEST_UART);
        rxline_status_detection = true;
    }
    if ((uart_rx_is_parity_error(TEST_UART) == true) && (uart_get_enabled_irq(TEST_UART) & uart_intr_parity_err)) {
        uart_clear_rx_parity_error_flag(TEST_UART);
        rxline_status_detection = true;
    }
    if ((uart_rx_is_overrun(TEST_UART) == true) && (uart_get_enabled_irq(TEST_UART) & uart_intr_overrun)) {
        uart_clear_rx_overrun_flag(TEST_UART);
        rxline_status_detection = true;
    }
#else
    uint8_t irq_id = uart_get_irq_id(TEST_UART);
    if (irq_id == uart_intr_id_rx_line_stat) {
        rxline_status = uart_get_status(TEST_UART);
        rxline_status_detection = true;
    }
#endif
}

int main(void)
{
    int i;
    char option;
    int table_count = sizeof(test_table) / sizeof(test_number_t);
    rxline_status_test_e num = none_error;
    rxline_status_detection = false;
    board_init();
    board_init_uart(TEST_UART);
    init_uart_break_signal_pin();
    gpio_set_pin_output(HPM_GPIO0, TEST_APP_UART_BREAK_GPIO_CTRL, TEST_APP_UART_BREAK_PIN);
    gpio_write_pin(HPM_GPIO0, TEST_APP_UART_BREAK_GPIO_CTRL, TEST_APP_UART_BREAK_PIN, 1);
    dma_mgr_init();
    show_help();
    uart_default_config(TEST_UART, &uart_config);
    uart_config.fifo_enable = true;
    uart_config.dma_enable = false;
    uart_config.src_freq_in_hz = clock_get_frequency(TEST_UART_CLK_NAME);
    uart_config.tx_fifo_level = uart_tx_fifo_trg_lt_three_quarters;
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    uart_config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
#else
    /*if need to use rxline status interrupt in DMA mode, RXFIFO threshold must be set more than 1 byte */
    uart_config.rx_fifo_level = uart_rx_fifo_trg_gt_half;
#endif
    intc_m_enable_irq_with_priority(TEST_UART_IRQ, 2);
    dma_mgr_request_resource(&rxdma_resource);
    while (1) {
        if (uart_check_status(BOARD_CONSOLE_UART_BASE, uart_stat_data_ready) == true) {
            option = uart_read_byte(BOARD_CONSOLE_UART_BASE);
            putchar(option);
            putchar('\n');
            num = (option - '0');
            for (i = 0; i < table_count; i++) {
                if (num == test_table[i].num) {
                    rxline_status_test(num);
                    show_help();
                    break;
                }
            }
            if (i >= table_count) {
                show_help();
            }
        }
    }
    return 0;
}

static void show_help(void)
{
    static const char help_info[] = "\n"
                                    "---------------------------------------------------------------\n"
                                    "*                                                             *\n"
                                    "*                   uart rxline status demo                   *\n"
                                    "*                                                             *\n";

    static const char help_info_ending[] = "*-------------------------------------------------------------*\n";

    printf("%s", help_info);
    for (uint32_t i = 0; i < sizeof(test_table) / sizeof(test_number_t); i++) {
        printf("%s", test_table[i].num_str);
    }
    printf("%s", help_info_ending);
}


static void rxline_status_test(rxline_status_test_e num)
{
    switch (num) {
    case overrun_error:
        overrun_error_test();
        break;
    case parity_error:
        parity_error_test();
        break;
    case framing_error:
        framing_error_test();
        break;
    case line_break:
        line_break_test();
        break;
    default:
        break;
    }
}

static void overrun_error_test(void)
{
    hpm_stat_t stat;
    uart_config.parity = parity_none;
    uart_config.dma_enable = false;
    uart_config.num_of_stop_bits = stop_bits_1;
    stat = uart_init(TEST_UART, &uart_config);
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    uart_enable_irq(TEST_UART, uart_intr_overrun);
#else
    uart_enable_irq(TEST_UART, uart_intr_rx_line_stat);
#endif
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }
    printf("overrun error test start.......\n");
    printf("##############################################################\n");
    printf("step.1......\n");
    printf("For the sender, please set the following uart configuration:\n");
    printf("baudrate:%d\n", uart_config.baudrate);
    printf("data bits:%d\n", uart_config.word_length + 5);
    printf("parity bits:%s\n", parity2string(uart_config.parity));
    printf("stop bits:%s\n", stop2string(uart_config.num_of_stop_bits));
    printf("##############################################################\n");
    printf("step.2......\n");
    /* Due to UART characteristics, detecting an overrun requires exceeding the RXFIFO depth by at least two bytes. */
    printf("Please send length more than %d bytes\n", uart_get_fifo_size(TEST_UART) + 1);
    printf("##############################################################\n");
    printf("step.3......\n");
    printf("waiting the rxline status detection......\n");
    rxline_status_detection = false;
    while (rxline_status_detection == false) {
    };
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    printf("overrun error status: true\n");
#else
    printf("overrun error status: %s\n", UART_LSR_OE_GET(rxline_status) ? "true" : "false");
#endif
    printf("##############################################################\n\n");
}

static void parity_error_test(void)
{
    hpm_stat_t stat;
    uart_config.parity = parity_odd;
    uart_config.dma_enable = true;
    uart_config.num_of_stop_bits = stop_bits_1;
    stat = uart_init(TEST_UART, &uart_config);
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    uart_enable_irq(TEST_UART, uart_intr_parity_err | uart_intr_errf);
#else
    uart_enable_irq(TEST_UART, uart_intr_rx_line_stat);
#endif
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }
#if defined(HPM_IP_FEATURE_UART_DMA_STOP) && (HPM_IP_FEATURE_UART_DMA_STOP == 1)
    uart_rx_enable_dma_auto_stop(TEST_UART);
#endif
    rxdma_config();
    printf("parity error test start.......\n");
    printf("##############################################################\n");
    printf("step.1......\n");
    printf("For the sender, please set the following uart configuration:\n");
    printf("baudrate:%d\n", uart_config.baudrate);
    printf("data bits:%d\n", uart_config.word_length + 5);
    printf("parity bits:%s\n", parity2string(parity_even));
    printf("stop bits:%s\n", stop2string(uart_config.num_of_stop_bits));
    printf("##############################################################\n");
    printf("step.2......\n");
    printf("Please send some bytes\n");
    printf("##############################################################\n");
    printf("step.3......\n");
    printf("waiting the rxline status detection......\n");
    rxline_status_detection = false;
    while (rxline_status_detection == false) {
    };
#if defined(HPM_IP_FEATURE_UART_DMA_STOP) && (HPM_IP_FEATURE_UART_DMA_STOP == 1)
    uart_clear_rx_dma_auto_stop_flag(TEST_UART);
#endif

#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    printf("parity error status: true\n");
#else
    if (UART_LSR_ERRF_GET(rxline_status)) {
        printf("parity error status: %s\n", UART_LSR_PE_GET(rxline_status) ? "true" : "false");
    }
#endif
    printf("##############################################################\n\n");
}

static void framing_error_test(void)
{
    hpm_stat_t stat;
    uart_config.parity = parity_none;
    uart_config.dma_enable = true;
    uart_config.num_of_stop_bits = stop_bits_2;
    stat = uart_init(TEST_UART, &uart_config);
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    uart_enable_irq(TEST_UART, uart_intr_framing_err);
#else
    uart_enable_irq(TEST_UART, uart_intr_rx_line_stat);
#endif
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }
    rxdma_config();
    printf("framing error test start.......\n");
    printf("##############################################################\n");
    printf("step.1......\n");
    printf("For the sender, please set the following uart configuration:\n");
    printf("baudrate:%d\n", uart_config.baudrate);
    printf("data bits:%d\n", uart_config.word_length + 5);
    printf("parity bits:%s\n", parity2string(uart_config.parity));
    printf("stop bits:%s\n", stop2string(stop_bits_1));
    printf("##############################################################\n");
    printf("step.2......\n");
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    printf("Please send some bytes\n");
#else
    printf("Please send length more than %d bytes\n", TEST_BUFFER_SIZE);
#endif
    printf("##############################################################\n");
    printf("step.3......\n");
    printf("waiting the rxline status detection......\n");
    rxline_status_detection = false;
    while (rxline_status_detection == false) {
    };
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    printf("framing error status: true\n");
#else
    if (UART_LSR_ERRF_GET(rxline_status)) {
        printf("framing error status: %s\n", UART_LSR_FE_GET(rxline_status) ? "true" : "false");
    }
#endif
    printf("##############################################################\n\n");
}

static void line_break_test(void)
{
    hpm_stat_t stat;
    char c;
    uint32_t bit_period_us, break_period_us;
    rxline_status_detection = false;
    uart_config.parity = parity_none;
    uart_config.dma_enable = true;
    uart_config.num_of_stop_bits = stop_bits_1;
    stat = uart_init(TEST_UART, &uart_config);
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    uart_enable_irq(TEST_UART, uart_intr_break_err);
#else
    uart_enable_irq(TEST_UART, uart_intr_rx_line_stat);
#endif
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }
    rxdma_config();
    printf("line break test start.......\n");
    printf("##############################################################\n");
    printf("step.1......\n");
    printf("Please connect uart rx pin to break signal pin. enter 't' to confirm.......\n");
    c = getchar();
    putchar(c);
    putchar('\n');
    if (c != 't') {
        printf("input error, exit\n");
        return;
    }
    bit_period_us = 1000000 / uart_config.baudrate;
    break_period_us = bit_period_us * 13; /* more than 13bits */
    uart_clear_rx_fifo(TEST_UART);
    gpio_write_pin(HPM_GPIO0, TEST_APP_UART_BREAK_GPIO_CTRL, TEST_APP_UART_BREAK_PIN, 0);
    board_delay_us(break_period_us);
    gpio_write_pin(HPM_GPIO0, TEST_APP_UART_BREAK_GPIO_CTRL, TEST_APP_UART_BREAK_PIN, 1);
    board_delay_us(bit_period_us);
    printf("##############################################################\n");
    printf("waiting the rxline status detection......\n");
    while (rxline_status_detection == false) {
    };
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    printf("line break status: true\n");
#else
    if (UART_LSR_ERRF_GET(rxline_status)) {
        printf("line break status: %s\n", UART_LSR_LBREAK_GET(rxline_status) ? "true" : "false");
    }
#endif
    printf("##############################################################\n\n");
}

static char *parity2string(parity_setting_t parity)
{
    switch (parity) {
    case parity_none:
        return "none";
    case parity_odd:
        return "odd";
    case parity_even:
        return "even";
    case parity_always_0:
        return "always_0";
    case parity_always_1:
        return "always_1";
    default:
        return "null";
    }
}

static char *stop2string(num_of_stop_bits_t parity)
{
    switch (parity) {
    case stop_bits_1:
        return "1bits";
    case stop_bits_1_5:
        return "1.5bits";
    case stop_bits_2:
        return "2bits";
    default:
        return "null";
    }
}

static hpm_stat_t rxdma_config(void)
{
    hpm_stat_t stat;
    dma_mgr_chn_conf_t chg_config;
    memset(uart_buff, 0, sizeof(uart_buff));
    dma_mgr_get_default_chn_config(&chg_config);
#if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
    chg_config.src_width = DMA_MGR_TRANSFER_WIDTH_HALF_WORD;
    chg_config.dst_width = DMA_MGR_TRANSFER_WIDTH_HALF_WORD;
#endif
    chg_config.src_mode = DMA_MGR_HANDSHAKE_MODE_HANDSHAKE;
    chg_config.src_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_FIXED;
    chg_config.src_addr = (uint32_t)&TEST_UART->RBR;
    chg_config.dst_mode = DMA_MGR_HANDSHAKE_MODE_NORMAL;
    chg_config.dst_addr_ctrl = DMA_MGR_ADDRESS_CONTROL_INCREMENT;
    chg_config.dst_addr = (uint32_t)uart_buff;
    chg_config.size_in_byte = sizeof(uart_buff);
    chg_config.en_dmamux = true;
    chg_config.dmamux_src = TEST_UART_RX_DMA_REQ;
    stat = dma_mgr_setup_channel(&rxdma_resource, &chg_config);
    if (stat == status_success) {
        dma_mgr_enable_channel(&rxdma_resource);
    }
    return stat;
}
