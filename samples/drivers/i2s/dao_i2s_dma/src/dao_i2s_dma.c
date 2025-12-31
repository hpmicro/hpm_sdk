/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_i2s_drv.h"
#include "hpm_dao_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_gpio_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif

#if defined(TEST_AUDIO_MONO) && TEST_AUDIO_MONO
#include "audio_mono.h"
#if !(defined(HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX) && HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX)
#error DAO_I2S not support mono audio!!!
#endif
#else
#include "audio_stereo.h"
#endif

/* DMA configuration definitions */
#define TEST_DMA             BOARD_APP_XDMA
#define TEST_DMA_CHANNEL     (2U)
#define TEST_I2S_DMA_IRQ     BOARD_APP_XDMA_IRQ
#define TEST_DMA_SRC_REQ     BOARD_DAO_I2S_DMA_REQ
#define TEST_DMAMUX_CH       DMA_SOC_CHN_TO_DMAMUX_CHN(TEST_DMA, TEST_DMA_CHANNEL)

/* Audio data configuration structure */
typedef struct {
    uint32_t sample_rate;    /* Sampling frequency in Hz */
    uint8_t channel_num;     /* Number of audio channels (1: mono, 2: stereo) */
    uint8_t audio_depth;     /* Bit depth of audio samples (16 or 32 bits) */
    const uint8_t *data;     /* Pointer to audio data buffer */
    uint32_t length;         /* Length of audio data in bytes */
} audio_data_t;

/* DMA transfer status flags */
volatile bool dma_transfer_done;    /* Set when DMA transfer completes */
volatile bool dma_transfer_error;   /* Set if DMA transfer encounters an error */

/* DAO_I2S status */
volatile bool dao_i2s_playing; /* true when audio is playing */
volatile bool dao_i2s_status_changed;

/* GPIO button debounce */
uint32_t debounce_threshold;          /* button debounce time threshold */
volatile uint64_t gpio_isr_curr_time; /* mark the current time for gpio_isr */
volatile uint64_t gpio_isr_pre_time;  /* mark the last time for gpio_isr */

/*
 * DMA interrupt handler
 * Monitors DMA transfer status and updates completion flags
 */
SDK_DECLARE_EXT_ISR_M(TEST_I2S_DMA_IRQ, dma_isr)
void dma_isr(void)
{
    hpm_stat_t stat;

    stat = dma_check_transfer_status(TEST_DMA, TEST_DMA_CHANNEL);
    if (0 != (stat & DMA_CHANNEL_STATUS_ERROR)) {
        dma_transfer_error = true;
    }
    if (0 != (stat & DMA_CHANNEL_STATUS_TC)) {
        dma_transfer_done = true;
    }
}

/*
 * GPIO interrupt handler
 * Stop/Start DAO_I2S based on button press
 */
SDK_DECLARE_EXT_ISR_M(BOARD_APP_GPIO_IRQ, button_gpio_isr)
void button_gpio_isr(void)
{
    gpio_isr_curr_time = mchtmr_get_count(HPM_MCHTMR);
    if ((gpio_isr_curr_time - gpio_isr_pre_time > debounce_threshold)) {

        /* change DAO_I2S status */
        if (dao_i2s_playing) {
            /* stop DAO and I2S */
            i2s_disable(DAO_I2S);
            dao_stop(HPM_DAO);
            dao_i2s_playing = false;
            dao_i2s_status_changed = true;
        } else {
            /* reset and reopen DAO and I2S */
            i2s_reset_tx(DAO_I2S);
            dao_software_reset(HPM_DAO);
            i2s_enable(DAO_I2S);
            dao_start(HPM_DAO);
            dao_i2s_playing = true;
            dao_i2s_status_changed = true;
        }
    }

    gpio_isr_pre_time = gpio_isr_curr_time;
    gpio_clear_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
}

/*
 * Configure DMA channel for I2S data transfer
 * Parameters:
 * - audio_data: Audio configuration and data buffer
 * Returns:
 * - status_success: Configuration successful
 * - Other: Configuration failed
 */
hpm_stat_t config_dma_transfer_i2s_data(audio_data_t *audio_data)
{
    hpm_stat_t stat;
    dma_channel_config_t ch_config;
    uint8_t data_width;
    uint8_t data_shift_byte;

    /* Verify audio depth is either 16 or 32 bits */
    assert((audio_data->audio_depth == 16U) || (audio_data->audio_depth == 32U));

    /* Configure DMA transfer width based on audio depth */
    if (audio_data->audio_depth == 16U) {
        data_width = DMA_TRANSFER_WIDTH_HALF_WORD;
        data_shift_byte = 2U;
    } else {
        data_width = DMA_TRANSFER_WIDTH_WORD;
        data_shift_byte = 0U;
    }

    /* Set up DMA channel configuration */
    dma_default_channel_config(TEST_DMA, &ch_config);
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)audio_data->data);
    ch_config.dst_addr = (uint32_t)&DAO_I2S->TXD[I2S_DAO_DATA_LINE] + data_shift_byte;
    ch_config.src_burst_size = (audio_data->channel_num == 2) ? DMA_NUM_TRANSFER_PER_BURST_2T : DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.src_width = data_width;
    ch_config.dst_width = data_width;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = audio_data->length;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;

    /* Apply DMA channel configuration */
    stat = dma_setup_channel(TEST_DMA, TEST_DMA_CHANNEL, &ch_config, true);
    return stat;
}

/*
 * Initialize I2S interface
 * Parameters:
 * - audio_data: Audio configuration parameters
 * - mclk_freq: Master clock frequency for I2S
 * Returns:
 * - status_success: Initialization successful
 * - status_fail: Initialization failed
 */
hpm_stat_t dao_i2s_init(audio_data_t *audio_data, uint32_t mclk_freq)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    hpm_stat_t stat;

    assert(mclk_freq > 0);

    /* Config I2S interface to CODEC */
    i2s_get_default_config(DAO_I2S, &i2s_config);
    i2s_init(DAO_I2S, &i2s_config);

    /* Configure I2S transfer parameters */
    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.sample_rate = audio_data->sample_rate;
    transfer.audio_depth = audio_data->audio_depth;
#if defined(TEST_AUDIO_MONO) && TEST_AUDIO_MONO
    transfer.channel_slot_mask = BOARD_DAO_SINGLE_CHANNEL_MASK; /* Optional parameter values: 0x1 - left channel; 0x2 - right channel */
#else
    transfer.channel_slot_mask = 3; /* Optional parameter values: 0x3 - dual-channel; */
#endif
    transfer.data_line = I2S_DAO_DATA_LINE;
    transfer.master_mode = true;

    /* Apply I2S tx configuration */
    stat = i2s_config_tx(DAO_I2S, mclk_freq, &transfer);
    if (stat != status_success) {
        return status_fail;
    }
    i2s_enable_tx_dma_request(DAO_I2S); /* enable I2S TX DMA request */

    /* Initialize DAO with default configuration */
    dao_config_t dao_config;
    dao_get_default_config(HPM_DAO, &dao_config);
#if defined(HPM_IP_FEATURE_DAO_DATA_FORMAT_CONFIG) && (HPM_IP_FEATURE_DAO_DATA_FORMAT_CONFIG == 1)
    dao_config.audio_depth = audio_data->audio_depth;
#endif
    dao_config.channel_slot_mask = 3; /* DAO must be set to dual channel */
    stat = dao_init(HPM_DAO, &dao_config);
    if (stat != status_success) {
        return status_fail;
    }

    return status_success;
}


/* Button GPIO interrupt configuration */
void button_gpio_config(void)
{
    board_init_gpio_pins();

    gpio_interrupt_trigger_t trigger = gpio_interrupt_trigger_edge_falling;
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
    gpio_config_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN, trigger);
    intc_m_enable_irq_with_priority(BOARD_APP_GPIO_IRQ, 1);

    /* button debounce time threshold */
    uint32_t mchtmr_freq;
    mchtmr_freq = clock_get_frequency(clock_mchtmr0);
    debounce_threshold = mchtmr_freq / 2; /* 500ms */
}

/*
 * Test I2S DMA playback
 * Performs multiple playback cycles using DMA transfer
 * Parameters:
 * - audio_data: Audio data and configuration
 * Returns:
 * - status_success: Playback successful
 * - status_fail: Playback failed
 */
hpm_stat_t dao_i2s_dma_play(audio_data_t *audio_data)
{
    hpm_stat_t stat;

    /* Button GPIO configuration */
    button_gpio_config();

    /* DMAMUX configuration */
    dmamux_config(BOARD_APP_DMAMUX, TEST_DMAMUX_CH, TEST_DMA_SRC_REQ, true);

    /* DMA interrupt enable */
    intc_m_enable_irq_with_priority(BOARD_APP_XDMA_IRQ, 1);

    printf("DAO_I2S with DMA play test start, press button can pause/resume audio play.\n");

    /* Perform multiple playback cycles */
    for (uint8_t i = 0; i < 4; i++) {
        dma_transfer_done = false;
        dma_transfer_error = false;

        /* Configure DMA peripheral */
        stat = config_dma_transfer_i2s_data(audio_data);
        if (stat != status_success) {
            printf("DMA transfer config failed\n");
            return status_fail;
        }

        /* reset I2S TX function and DAO peripharal */
        i2s_reset_tx(DAO_I2S);
        dao_software_reset(HPM_DAO);

        /* fill tx dummy data to tx fifo to prevent underflow when TX starts */
        if (i2s_fill_tx_dummy_data(DAO_I2S, I2S_DAO_DATA_LINE, audio_data->channel_num) != status_success) {
            return status_fail;
        }

        /* start */
        i2s_enable(DAO_I2S);
        dao_start(HPM_DAO);

        /* Enable GPIO interrupt to stop/resume audio play */
        dao_i2s_playing = true;
        gpio_enable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

        /* Wait for DMA transfer completion */
        while ((!dma_transfer_error) && (!dma_transfer_done)) {
            if (dao_i2s_status_changed) {
                dao_i2s_status_changed = false;
                if (dao_i2s_playing) {
                    printf("dao_i2s resume\n");
                } else {
                    printf("dao_i2s pause\n");
                }
            }
        }

        /* Disable GPIO interrupt */
        gpio_disable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

        /* stop */
        i2s_disable(DAO_I2S);
        dao_stop(HPM_DAO);

        if (dma_transfer_error) {
            printf("dma transfer i2s data failed\n");
            return status_fail;
        } else {
            printf("DAO_I2S with DMA play one cycle finished\n");
        }
    }

    return status_success;
}


/*
 * Main function
 * Initializes system and performs I2S DMA playback test
 */
int main(void)
{
    uint32_t i2s_mclk_freq;
    hpm_stat_t stat;

    /* Initialize board basic peripherals */
    board_init();
    printf("DAO_I2S with DMA example\n");

    /* Configure audio parameters */
    audio_data_t wave_data;
    wave_data.sample_rate = hpm_desc_sample_rate;
    wave_data.audio_depth = hpm_desc_audio_depth;
    wave_data.channel_num = hpm_desc_channel_num;
    wave_data.data = hpm_desc_audio;
    wave_data.length = sizeof(hpm_desc_audio);

    /* Initialize I2S pins and clock */
    init_dao_pins();
    board_init_dao_clock();
    i2s_mclk_freq = board_config_i2s_clock(DAO_I2S, wave_data.sample_rate); /* Configure I2S clock according to sample rate */

    /* Initialize DAO_I2S peripheral */
    stat = dao_i2s_init(&wave_data, i2s_mclk_freq);
    if (stat != status_success) {
        printf("Init I2S and DAO failed\n");
        while (1) {
        }
    }

    /* Perform DAO_I2S + DMA play with pause control */
    stat = dao_i2s_dma_play(&wave_data);
    if (stat == status_success) {
        printf("Test DAO_I2S DMA play success\n");
    } else {
        printf("Test DAO_I2S DMA play failed\n");
    }

    while (1) {
    }

    return 0;
}
