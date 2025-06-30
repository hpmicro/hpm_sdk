/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_gpio_drv.h"
#include "hpm_i2c_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_dma_mgr.h"
#include "hpm_i2s_over_spi.h"
#include "hpm_wm8978.h"

#if (SPI_SOC_TRANSFER_COUNT_MAX == 512)
#define RX_SIZE_MAX             (512U)
#else
#define RX_SIZE_MAX             (4096U)
#endif
#define TX_SIZE_MAX             (90112U)

#define WM8978_I2C                  BOARD_APP_I2C_BASE
#define WM8978_I2C_CLOCK_NAME       BOARD_APP_I2C_CLK_NAME
#define I2S_OVER_SPI_CS_CONTROLLER  BOARD_I2S_SPI_CS_GPIO_PAD
typedef struct {
    uint8_t num;
    char *num_str;
} test_number_t;

typedef enum {
    start_play    = 1,
    stop_play,
    start_record,
    stop_record,
    none,
} i2s_test_e;

static void i2s_emulation_pins_init(void);
static void i2s_emulation_config(hpm_i2s_over_spi_t *i2s);
static void show_help(void);
static bool get_char(uint8_t *ch);
static void record_start(void);
static void record_stop(void);
static void play_start(void);
static void play_stop(void);

const test_number_t test_table[] = {
    {start_play,           "*        1 - start play                                       *\n"},
    {stop_play,            "*        2 - stop play                                        *\n"},
    {start_record,         "*        3 - start record                                     *\n"},
    {stop_record,          "*        4 - stop record                                      *\n"},
};

ATTR_PLACE_AT(".ahb_sram") dma_resource_t dma_resource_pools[2];
/* descriptor should be 8-byte aligned */
ATTR_PLACE_AT_WITH_ALIGNMENT(".ahb_sram", 8) dma_linked_descriptor_t rx_descriptors[2];
ATTR_PLACE_AT(".ahb_sram") uint8_t rx_buffer[2][RX_SIZE_MAX];
uint8_t tx_buffer[TX_SIZE_MAX];

volatile bool rx_flag;
volatile bool ready_play;
volatile uint8_t rx_index;
volatile uint32_t index_count;

hpm_i2s_over_spi_t i2s_device;
wm8978_context_t wm8978_device;

uint8_t protocol;
uint8_t audio_depth;
uint32_t sample_rate;

SDK_DECLARE_EXT_ISR_M(BOARD_GPTMR_I2S_FINSH_IRQ, i2s_gptmr_isr)
void i2s_gptmr_isr(void)
{
    if (i2s_device.transfer_complete) {
        i2s_device.transfer_complete(&i2s_device);
    }
}

void rx_callback(uint32_t addr)
{
    rx_flag = true;
    rx_index = addr;
}

int main(void)
{
    uint8_t ch = 0;
    hpm_stat_t stat;
    i2s_test_e num = none;
    i2c_config_t i2c_config;
    uint32_t freq;
    index_count = 0;
    protocol = I2S_PROTOCOL_MSB_JUSTIFIED;
    audio_depth = 16;
    sample_rate = 8000;
    ready_play = false;
    rx_flag = false;
    board_init();
    board_init_i2c_clock(WM8978_I2C);
    board_init_spi_clock(BOARD_APP_SPI_BASE);
    init_i2c_pins(WM8978_I2C);
    i2s_emulation_pins_init();
    dma_mgr_init();
    for (uint8_t i = 0; i < 2; i++) {
        dma_resource_t *resource = &dma_resource_pools[i];
        stat = dma_mgr_request_resource(resource);
        if (stat != status_success) {
            while (1) {
            };
        }
    }
    i2c_config.i2c_mode = i2c_mode_normal;
    i2c_config.is_10bit_addressing = false;
    freq = clock_get_frequency(WM8978_I2C_CLOCK_NAME);
    stat = i2c_init_master(WM8978_I2C, freq, &i2c_config);
    if (stat != status_success) {
        while (1) {
        };
    }
    wm8978_device.ptr = WM8978_I2C;
    i2s_emulation_config(&i2s_device);
    hpm_i2s_master_over_spi_init(&i2s_device);
    if (wm8979_init(&wm8978_device) != status_success) {
        printf("wm8979 init fail\n");
        while (1) {
        };
    }
    wm8978_set_out_mute(&wm8978_device, wm8978_out1_channel, false);
    wm8978_set_out_mute(&wm8978_device, wm8978_out2_channel, false);
    wm8978_set_line_gain(&wm8978_device, 7);
    wm8978_set_out_volume(&wm8978_device, wm8978_out1_channel, 60);
    wm8978_set_out_volume(&wm8978_device, wm8978_out2_channel, 60);
    wm8978_cfg_audio_interface(&wm8978_device, wm8978_left_justified, wm8978_16bits_length);
    show_help();
    while (1) {
        if (get_char(&ch) == true) {
            num = (ch - '0');
            if (num == start_record) {
                record_start();
            } else if (num == stop_record) {
                record_stop();
            } else if (num == start_play) {
                play_start();
            } else if (num == stop_play) {
                play_stop();
            } else {
                show_help();
            }
        }
    }
}

static void i2s_emulation_config(hpm_i2s_over_spi_t *i2s)
{
    i2s->mclk.ptr = BOARD_GPTMR_I2S_MCLK;
    i2s->mclk.channel = BOARD_GPTMR_I2S_MCLK_CHANNEL;
    i2s->mclk.clock_name = BOARD_GPTMR_I2S_MCLK_CLK_NAME;

    i2s->lrck.ptr = BOARD_GPTMR_I2S_LRCK;
    i2s->lrck.channel = BOARD_GPTMR_I2S_LRCK_CHANNEL;
    i2s->lrck.clock_name = BOARD_GPTMR_I2S_LRCK_CLK_NAME;

    i2s->bclk.ptr = BOARD_GPTMR_I2S_BCLK;
    i2s->bclk.channel = BOARD_GPTMR_I2S_BLCK_CHANNEL;
    i2s->bclk.clock_name = BOARD_GPTMR_I2S_BLCK_CLK_NAME;

    i2s->transfer_time.ptr = BOARD_GPTMR_I2S_FINSH;
    i2s->transfer_time.channel = BOARD_GPTMR_I2S_FINSH_CHANNEL;
    i2s->transfer_time.clock_name = BOARD_GPTMR_I2S_FINSH_CLK_NAME;
    intc_m_enable_irq_with_priority(BOARD_GPTMR_I2S_FINSH_IRQ, 5);

    i2s->spi_slave.ptr = BOARD_APP_SPI_BASE;
    i2s->spi_slave.clock_name = BOARD_APP_SPI_CLK_NAME;
    i2s->spi_slave.cs_pin = I2S_OVER_SPI_CS_CONTROLLER;
    i2s->spi_slave.write_cs = board_write_spi_cs;
    i2s->spi_slave.rxdma_src = BOARD_APP_SPI_RX_DMA;
    i2s->spi_slave.txdma_src = BOARD_APP_SPI_TX_DMA;

    i2s->rx_callback = rx_callback;

    i2s->rx_dma.descriptors = rx_descriptors;
    i2s->rx_dma.resource = &dma_resource_pools[0];
    i2s->tx_dma.descriptors = NULL;
    i2s->tx_dma.resource = &dma_resource_pools[1];

    i2s->has_done = false;
    i2s->i2s_rx = false;
}

static void i2s_emulation_pins_init(void)
{
    init_spi_pins(BOARD_APP_SPI_BASE);
    init_gptmr_pins(BOARD_GPTMR_I2S_MCLK);
    init_gptmr_pins(BOARD_GPTMR_I2S_LRCK);
    init_gptmr_pins(BOARD_GPTMR_I2S_BCLK);
    HPM_IOC->PAD[BOARD_I2S_SPI_CS_GPIO_PAD].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(0);
    gpio_set_pin_output(BOARD_I2S_SPI_CS_GPIO_CTRL, BOARD_I2S_SPI_CS_GPIO_INDEX, BOARD_I2S_SPI_CS_GPIO_PIN);
    gpio_write_pin(BOARD_I2S_SPI_CS_GPIO_CTRL, BOARD_I2S_SPI_CS_GPIO_INDEX, BOARD_I2S_SPI_CS_GPIO_PIN, true);
}

static bool get_char(uint8_t *ch)
{
    bool sta = uart_check_status(BOARD_CONSOLE_UART_BASE, uart_stat_data_ready);
    if (sta == true) {
        *ch = uart_read_byte(BOARD_CONSOLE_UART_BASE);
    }
    return sta;
}

static void play_start(void)
{
    uint8_t ch = 0;
    uint32_t size = 0;
    int remain_size = 0;
    int index = 0;
    uint32_t max_transfer_len = 102400;
    printf("play start enter....\n");
    wm8978_cfg_audio_channel(&wm8978_device, dac_on, spk_on | earphone_left_on | earphone_right_on);
    if (SPI_SOC_TRANSFER_COUNT_MAX == 512) {
        max_transfer_len = 512;
    }
    while (1) {
        if (get_char(&ch) == true) {
            if ((ch - '0') != start_play) {
                printf("play start exit...\n");
                break;
            }
        }
        if (ready_play == false) {
            printf("there is no recorded content. Please record first...\n");
            break;
        }
        size = TX_SIZE_MAX;
        while (size > 0) {
            remain_size = (size > max_transfer_len) ? max_transfer_len : size;
            hpm_i2s_master_over_spi_tx_buffer_nonblocking(&i2s_device, protocol, sample_rate, audio_depth, (uint8_t *)&tx_buffer[index], remain_size);
            while (hpm_i2s_master_over_spi_tx_is_busy(&i2s_device) == true) {
                if ((get_char(&ch) == true) && ((ch - '0') != start_play)) {
                    play_stop();
                    return;
                }
            };
            /* In order to continue the next transmission, it needs to be stopped */
            hpm_i2s_master_over_spi_tx_stop(&i2s_device);
            size -= remain_size;
            index += remain_size;
        }
        printf("play end, now exit....\n");
        break;
    }
}


static void play_stop(void)
{
    if (ready_play == false) {
        printf("there is no recorded content. Please record first...\n");
        return;
    }
    hpm_i2s_master_over_spi_tx_stop(&i2s_device);
    printf("play stop finish....\n");
}

static void record_start(void)
{
    uint8_t ch = 0;
    printf("record start enter....\n");
    wm8978_cfg_audio_channel(&wm8978_device, mic_left_on | mic_right_on | adc_on, output_off);
    hpm_i2s_master_over_spi_rx_config(&i2s_device, protocol, sample_rate, audio_depth, rx_buffer[0], rx_buffer[1], RX_SIZE_MAX);
    hpm_i2s_master_over_spi_rx_start(&i2s_device, rx_callback);
    memset(&tx_buffer, 0, TX_SIZE_MAX);
    while (1) {
        if (get_char(&ch) == true) {
            if ((ch - '0') != start_record) {
                record_stop();
                memset(&tx_buffer[index_count], 0, TX_SIZE_MAX - index_count);
                printf("start record exit...\n");
                break;
            }
        }
        if (rx_flag == true) {
            rx_flag = false;
            memcpy(&tx_buffer[index_count], rx_buffer[rx_index], RX_SIZE_MAX);
            index_count += RX_SIZE_MAX;
            if (index_count >= TX_SIZE_MAX) {
                ready_play = true;
                index_count = 0;
                hpm_i2s_master_over_spi_rx_stop(&i2s_device);
                printf("recording size overflow, stopping and exiting now...\n");
                break;
            }
        }
    }
}

static void record_stop(void)
{
    if (index_count > 0) {
        ready_play = true;
    } else {
        printf("no recording process has been initiated. Please start the recording procedure to proceed...\n");
        return;
    }
    hpm_i2s_master_over_spi_rx_stop(&i2s_device);
    printf("record stop finish....\n");
}

static void show_help(void)
{
    static const char help_info[] = "\n"
                                    "---------------------------------------------------------------\n"
                                    "*                                                             *\n"
                                    "*                   i2s_emulation demo                        *\n"
                                    "*                                                             *\n";

    static const char help_info_ending[] = "*-------------------------------------------------------------*\n";

    printf("%s", help_info);
    for (uint32_t i = 0; i < sizeof(test_table) / sizeof(test_number_t); i++) {
        printf("%s", test_table[i].num_str);
    }
    printf("%s", help_info_ending);
}


