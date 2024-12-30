/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_common.h"
#include "uart_demo_conf.h"

#define TEST_UART                     BOARD_UART_IDLE

#define TEST_DMA                      BOARD_APP_HDMA
#define TEST_DMA_CHANNEL              (0U)

#define TEST_DMAMUX                   BOARD_APP_DMAMUX
#define TEST_DMAMUX_CHANNEL           DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_DMA, TEST_DMA_CHANNEL)
#define TEST_DMAMUX_SRC               BOARD_UART_IDLE_DMA_SRC

#define TEST_TRGM                     BOARD_UART_IDLE_TRGM
#define TEST_TRGM_PIN                 BOARD_UART_IDLE_TRGM_PIN
#define TEST_TRGM_INPUT_SRC           BOARD_UART_IDLE_TRGM_INPUT_SRC /* Corresponding to Pin setting */
#define TEST_TRGM_OUTPUT_GPTMR_IN     BOARD_UART_IDLE_TRGM_OUTPUT_GPTMR_IN /* Corresponding to GPTMR inst */
#define TEST_TRGM_OUTPUT_GPTMR_SYNCI  BOARD_UART_IDLE_TRGM_OUTPUT_GPTMR_SYNCI /* Corresponding to GPTMR inst */

#define TEST_GPTMR                    BOARD_UART_IDLE_GPTMR
#define TEST_GPTMR_CLK_NAME           BOARD_UART_IDLE_GPTMR_CLK_NAME
#define TEST_GPTMR_CMP_CHANNEL        BOARD_UART_IDLE_GPTMR_CMP_CH
#define TEST_GPTMR_CAPTURE_CHANNEL    BOARD_UART_IDLE_GPTMR_CAP_CH
#define TEST_GPTMR_IRQ                BOARD_UART_IDLE_GPTMR_IRQ

#define BUFF_SIZE                     (1024U)
#define UART_DMA_ADDR_ALIGNMENT       (8U)
ATTR_PLACE_AT_NONCACHEABLE uint8_t uart_rx_buff[BUFF_SIZE];
ATTR_PLACE_AT_NONCACHEABLE uint8_t dma_buf[BUFF_SIZE];

uart_rx_flexiable_data_context_t demo_config = {
    .uart_rx_idle = false,
    .uart_receive_data_size = 0,
    .uart_info = {
                .ptr = TEST_UART,
                .baudrate = 115200U,
                .buff_addr = uart_rx_buff,
    },
    .dma_info = {
                .ptr = TEST_DMA,
                .ch = TEST_DMA_CHANNEL,
                .dst_addr = dma_buf,
                .buff_size = BUFF_SIZE,
    },
    .dmamux_info = {
                .ptr = TEST_DMAMUX,
                .ch = TEST_DMAMUX_CHANNEL,
                .src = TEST_DMAMUX_SRC,
    },
    .trgm_info = {
                .ptr = TEST_TRGM,
                .pin_index = TEST_TRGM_PIN,
                .input_src = TEST_TRGM_INPUT_SRC,
                .output_gptmr_in = TEST_TRGM_OUTPUT_GPTMR_IN,
                .output_gptmr_synci = TEST_TRGM_OUTPUT_GPTMR_SYNCI,
    },
    .gptmr_info = {
                .ptr = TEST_GPTMR,
                .clock_name = TEST_GPTMR_CLK_NAME,
                .cmp_ch = TEST_GPTMR_CMP_CHANNEL,
                .cap_ch = TEST_GPTMR_CAPTURE_CHANNEL,
                .irq_index = TEST_GPTMR_IRQ,
    },
};

hpm_stat_t uart_rx_trigger_dma(DMA_Type *dma_ptr,
                                uint8_t ch_num,
                                UART_Type *uart_ptr,
                                uint32_t dst,
                                uint32_t size)
{
    dma_handshake_config_t config;

    dma_default_handshake_config(dma_ptr, &config);
    config.ch_index = ch_num;
    config.dst = dst;
    config.dst_fixed = false;
    config.src = (uint32_t)&uart_ptr->RBR;
    config.src_fixed = true;
    config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    config.size_in_byte = size;

    return dma_setup_handshake(dma_ptr, &config, true);
}

void config_uart(uart_info_t *uart_info)
{
    hpm_stat_t stat;
    uart_config_t config = {0};

    /* if uart_info->ptr is same as BOARD_CONSOLE_BASE, it has been initialized in board_init(); */
    board_init_uart(uart_info->ptr);
    board_init_uart_clock(uart_info->ptr);
    uart_default_config(uart_info->ptr, &config);
    config.baudrate = uart_info->baudrate;
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = board_init_uart_clock(uart_info->ptr);
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty; /* this config should not change */
    stat = uart_init(uart_info->ptr, &config);
    if (stat != status_success) {
        printf("failed to initialize uart\n");
        while (1) {
        }
    }
}

void configure_uart_dma_transfer(uart_info_t *uart_info, dma_info_t *dma_info, dmamux_info_t *dmamux_info)
{
    hpm_stat_t stat;

    dmamux_config(dmamux_info->ptr, dmamux_info->ch, dmamux_info->src, true);
    stat = uart_rx_trigger_dma(dma_info->ptr,
                            dma_info->ch,
                            uart_info->ptr,
                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_info->dst_addr),
                            dma_info->buff_size);
    if (stat != status_success) {
        printf("uart_rx_trigger_dma function failed\n");
        while (1) {
        }
    }
}

void config_gptmr_to_detect_uart_rx_idle(gptmr_info_t *gptmr_info, trgm_info_t *trgm_info, uint32_t uart_baudrate)
{
    uint32_t gptmr_freq, bits;
    gptmr_channel_config_t config;

    bits = 12; /* use max value: 1 start bit, 8 data bits, 2 stop bits, 1 parity bit */
    clock_add_to_group(gptmr_info->clock_name, 0);
    gptmr_channel_get_default_config(gptmr_info->ptr, &config);
    gptmr_freq = clock_get_frequency(gptmr_info->clock_name);
    config.cmp[0] = gptmr_freq / uart_baudrate * bits; /* Time to transmit a byte */
    config.cmp[1] = 0xFFFFFFFEUL;
    config.synci_edge = gptmr_synci_edge_both;
    gptmr_channel_config(gptmr_info->ptr, gptmr_info->cmp_ch, &config, false);
    gptmr_channel_reset_count(gptmr_info->ptr, gptmr_info->cmp_ch);

    /* config uart rx -> TRGM INPUT IO -> GPTMR SYNCI */
    trgm_output_update_source(trgm_info->ptr, trgm_info->output_gptmr_synci, trgm_info->input_src);

    gptmr_enable_irq(gptmr_info->ptr, GPTMR_CH_CMP_IRQ_MASK(gptmr_info->cmp_ch, 0)); /* enable compare irq */
    /* not start gptmr cmp channel counter, start this channel counter when rx reception start */
}

void config_gptmr_to_detect_uart_rx_start(gptmr_info_t *gptmr_info, trgm_info_t *trgm_info)
{
    gptmr_channel_config_t config;

    clock_add_to_group(gptmr_info->clock_name, 0);
    gptmr_channel_get_default_config(gptmr_info->ptr, &config);
    config.mode = gptmr_work_mode_capture_at_falling_edge;
    gptmr_channel_config(gptmr_info->ptr, gptmr_info->cap_ch, &config, false);
    gptmr_channel_reset_count(gptmr_info->ptr, gptmr_info->cap_ch);

    /* configure uart_rx -> TRGM INPUT IO -> GPTMR IN */
    trgm_output_update_source(trgm_info->ptr, trgm_info->output_gptmr_in, trgm_info->input_src);
    gptmr_enable_irq(gptmr_info->ptr, GPTMR_CH_CAP_IRQ_MASK(gptmr_info->cap_ch)); /* enable capture irq */
    gptmr_start_counter(gptmr_info->ptr, gptmr_info->cap_ch);
}

void uart_dma_rx_idle_callback(uart_rx_flexiable_data_context_t *demo_config)
{
    uart_info_t uart_info = demo_config->uart_info;
    dma_info_t dma_info = demo_config->dma_info;
    gptmr_info_t gptmr_info = demo_config->gptmr_info;
    demo_config->uart_rx_idle = true;
    demo_config->uart_receive_data_size = BUFF_SIZE - dma_get_remaining_transfer_size(dma_info.ptr, dma_info.ch);
    memcpy(uart_info.buff_addr, dma_info.dst_addr, demo_config->uart_receive_data_size);

    /* start grtmr capture for next rx reception start detection */
    gptmr_enable_irq(gptmr_info.ptr, GPTMR_CH_CAP_IRQ_MASK(gptmr_info.cap_ch));
    gptmr_start_counter(gptmr_info.ptr, gptmr_info.cap_ch);
}

/* this handler used to detect uart idle status */
/* when gptmr cap irq occurs, think as uart rx start, enable gptmr cmp irq for rx idle */
/* when gptmr cmp irq occurs, think as uart rx idle status, close gptmr cmp irq, close uart dma */
void gptmr_detect_uart_rx_idle_handler(uart_rx_flexiable_data_context_t *demo_config)
{
    gptmr_info_t gptmr_info = demo_config->gptmr_info;
    dma_info_t dma_info = demo_config->dma_info;

    /* cmp */
    if (gptmr_check_status(gptmr_info.ptr, GPTMR_CH_CMP_STAT_MASK(gptmr_info.cmp_ch, 0))) {
        /* clear cmp status and capture status, the rx idle set cmp status, the low level of data transmission set capture status */
        gptmr_clear_status(gptmr_info.ptr, GPTMR_CH_CMP_STAT_MASK(gptmr_info.cmp_ch, 0) | GPTMR_CH_CAP_STAT_MASK(gptmr_info.cap_ch));
        gptmr_stop_counter(gptmr_info.ptr, gptmr_info.cmp_ch); /* stop counter */
        gptmr_channel_reset_count(gptmr_info.ptr, gptmr_info.cmp_ch); /* clear counter */
        gptmr_disable_irq(gptmr_info.ptr, GPTMR_CH_CMP_IRQ_MASK(gptmr_info.cmp_ch, 0)); /* disable cmp irq */

        /* disable dma channel */
        dma_disable_channel(dma_info.ptr, dma_info.ch);

        /* uart rx idle callback */
        uart_dma_rx_idle_callback(demo_config);
    }

    /* capture */
    if (gptmr_check_status(gptmr_info.ptr, GPTMR_CH_CAP_STAT_MASK(gptmr_info.cap_ch))) {
        gptmr_clear_status(gptmr_info.ptr, GPTMR_CH_CAP_STAT_MASK(gptmr_info.cap_ch)); /* clear capture status */
        gptmr_stop_counter(gptmr_info.ptr, gptmr_info.cap_ch);        /* stop counter */
        gptmr_channel_reset_count(gptmr_info.ptr, gptmr_info.cap_ch); /* clear counter */
        gptmr_disable_irq(gptmr_info.ptr, GPTMR_CH_CAP_IRQ_MASK(gptmr_info.cap_ch)); /* disable capture irq */

        /* config dma to transfer uart rx data */
        configure_uart_dma_transfer(&demo_config->uart_info, &demo_config->dma_info, &demo_config->dmamux_info);

        /* start gptmr cmp for rx idle detection */
        gptmr_enable_irq(gptmr_info.ptr, GPTMR_CH_CMP_IRQ_MASK(gptmr_info.cmp_ch, 0));
        gptmr_start_counter(gptmr_info.ptr, gptmr_info.cmp_ch);
    }
}

SDK_DECLARE_EXT_ISR_M(TEST_GPTMR_IRQ, gptmr_isr)
void gptmr_isr(void)
{
    gptmr_detect_uart_rx_idle_handler(&demo_config);
}

void setup_uart_flexible_data_reception(uart_rx_flexiable_data_context_t *config)
{
    /* config gptmr to detect uart rx idle */
    config_gptmr_to_detect_uart_rx_start(&config->gptmr_info, &config->trgm_info);
    config_gptmr_to_detect_uart_rx_idle(&config->gptmr_info, &config->trgm_info, config->uart_info.baudrate);
    intc_m_enable_irq_with_priority(config->gptmr_info.irq_index, 1); /* enable gptmr irq */
}

int main(void)
{
    board_init();
    printf("uart software rx idle detection.\n");
    config_uart(&demo_config.uart_info);
    init_trgmux_pins(demo_config.trgm_info.pin_index);
    setup_uart_flexible_data_reception(&demo_config);

    while (1) {
        while (!demo_config.uart_rx_idle) {
            __asm("nop");
        }

        demo_config.uart_rx_idle = false;
        printf("uart receive %d bytes, the received data are:", demo_config.uart_receive_data_size);
        for (uint32_t i = 0; i < demo_config.uart_receive_data_size; i++) {
            if ((i % 10) == 0) {
                printf("\n");
            }
            printf("%c ", *((uint8_t *) demo_config.uart_info.buff_addr + i));
        }
        printf("\n");
    }

    return 0;
}
