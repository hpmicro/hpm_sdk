/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_i2s_drv.h"
#include "hpm_dao_drv.h"
#include "hpm_smix_drv.h"
#include "smix_config.h"
#include "hpm_gpio_drv.h"
#include "hpm_mchtmr_drv.h"

#if defined(TEST_AUDIO_MONO) && TEST_AUDIO_MONO
#include "audio_mono.h"
#if !(defined(HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX) && HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX)
#error DAO_I2S not support mono audio!!!
#endif
#else
#include "audio_stereo.h"
#endif

/* DAO_I2S play status */
volatile bool dao_i2s_playing;
volatile bool dao_i2s_status_changed;
/* DAO_I2S play gain */
volatile smix_mixer_gain_t smix_gain;
volatile bool smix_gain_changed;

/* Button debounce */
uint32_t debounce_threshold;          /* button debounce time threshold */
volatile uint64_t key1_gpio_isr_curr_time; /* mark the current time for key1 gpio_isr */
volatile uint64_t key1_gpio_isr_pre_time;  /* mark the last time for key1 gpio_isr */
volatile uint64_t key2_gpio_isr_curr_time; /* mark the current time for key2 gpio_isr */
volatile uint64_t key2_gpio_isr_pre_time;  /* mark the last time for key2 gpio_isr */

/* SMIX DMA */
volatile bool dma_transfer_done;

typedef struct {
    uint32_t sample_rate;
    uint8_t channel_num;
    uint8_t audio_depth;
    const uint8_t *data;
    uint32_t length;
} audio_data_t;

SDK_DECLARE_EXT_ISR_M(SMIX_DMA_IRQ, smix_dma_isr)
void smix_dma_isr(void)
{
    if (smix_dma_check_transfer_complete(SMIX, SMIX_DMA_CH_FOR_DST)) {
        dma_transfer_done = true;
    }
}

static hpm_stat_t smix_get_sample_rate_ratio(uint32_t src_sample_rate, uint32_t dst_sample_rate, smix_mixer_rate_convert_t *ratio)
{
    if (dst_sample_rate == src_sample_rate) {
        *ratio = smix_mixer_no_rate_convert;
    } else if (dst_sample_rate == 2 * src_sample_rate) {
        *ratio = smix_mixer_upper_2x_sample;
    } else if (dst_sample_rate == 3 * src_sample_rate) {
        *ratio = smix_mixer_upper_3x_sample;
    } else if (dst_sample_rate == 4 * src_sample_rate) {
        *ratio = smix_mixer_upper_4x_sample;
    } else if (dst_sample_rate == 6 * src_sample_rate) {
        *ratio = smix_mixer_upper_6x_sample;
    } else if (dst_sample_rate == 8 * src_sample_rate) {
        *ratio = smix_mixer_upper_8x_sample;
    } else if (dst_sample_rate == 12 * src_sample_rate) {
        *ratio = smix_mixer_upper_12x_sample;
    } else if (2 * dst_sample_rate == src_sample_rate) {
        *ratio = smix_mixer_lower_2x_sample;
    } else {
        return status_fail;
    }

    return status_success;
}



hpm_stat_t dao_i2s_init(audio_data_t *audio_data, uint32_t mclk_freq)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;
    hpm_stat_t stat;

    assert(mclk_freq > 0);
    assert(audio_data->sample_rate == DAO_SOC_SAMPLE_RATE_IN_HZ);

    /* Config I2S interface to CODEC */
    i2s_get_default_config(TEST_I2S, &i2s_config);
    i2s_init(TEST_I2S, &i2s_config);

    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.sample_rate = audio_data->sample_rate;
    transfer.audio_depth = audio_data->audio_depth;
#if defined(TEST_AUDIO_MONO) && TEST_AUDIO_MONO
    transfer.channel_slot_mask = BOARD_DAO_SINGLE_CHANNEL_MASK; /* Optional parameter values: 0x1 - left channel; 0x2 - right channel */
#else
    transfer.channel_slot_mask = 3; /* Optional parameter values: 0x3 - dual-channel; */
#endif
    stat = i2s_config_tx(TEST_I2S, mclk_freq, &transfer);
    if (stat != status_success) {
        return status_fail;
    }

    /* DAO */
    dao_get_default_config(HPM_DAO, &dao_config);
    dao_config.audio_depth = audio_data->audio_depth;
    dao_config.channel_slot_mask = 3; /* DAO must be set to dual channel */
    stat = dao_init(HPM_DAO, &dao_config);
    if (stat != status_success) {
        return status_fail;
    }

    i2s_enable_tx_dma_request(TEST_I2S); /* enable I2S TX DMA transfer */

    return status_success;
}

hpm_stat_t mixer_play_one_sound(audio_data_t *sound)
{
    smix_dma_ch_config_t source_dma, dst_dma;
    smix_mixer_source_config_t mixer_source;
    smix_mixer_dst_config_t mixer_dst;
    dma_transfer_done = false;

    assert((sound->audio_depth == 16) || (sound->audio_depth == 32));

    uint32_t ratio = SMIX_OUTPUT_SAMPLE_RATE / sound->sample_rate;
    smix_mixer_rate_convert_t smix_rate_convert;
    uint32_t addr_shift = (32 - sound->audio_depth) / 8;
    uint32_t dma_width = sound->audio_depth / 8 - 1;
    uint32_t source_count = sound->length / (sound->audio_depth / 8);
    uint32_t dst_data_bytes = sound->length * ratio;

    if (status_success != smix_get_sample_rate_ratio(sound->sample_rate, SMIX_OUTPUT_SAMPLE_RATE, &smix_rate_convert)) {
        printf("not support convert src %d to dst %d\n", sound->sample_rate, SMIX_OUTPUT_SAMPLE_RATE);
        return status_fail;
    }

    printf("mixer play one sound\n");
    printf("input sound sample rate: %dHz\n", sound->sample_rate);
    printf("output sound sample rate: %dHz\n", SMIX_OUTPUT_SAMPLE_RATE);

    /* config source data DMA */
    smix_get_dma_default_ch_config(SMIX, &source_dma);
    source_dma.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)sound->data);
    source_dma.src_width = dma_width;
    source_dma.src_burst_size = (sound->channel_num == 2) ? smix_dma_transfer_burst_2t : smix_dma_transfer_burst_1t;
    source_dma.dst_addr = (uint32_t)&SMIX->SOURCE_CH[SMIX_SOURCE_CH].DATA + addr_shift; /* move valid data to high bit of register */
    source_dma.dst_width = dma_width;
    source_dma.dst_addr_ctrl = smix_dma_address_fixed;
    source_dma.dst_mode = smix_dma_mode_handshake;
    source_dma.dst_req_sel = smix_dma_req_mixer_src_ch0; /* align with dma smix source_dma channel */
    source_dma.trans_bytes = sound->length;
    smix_config_dma_channel(SMIX, SMIX_DMA_CH_FOR_SOURCE, &source_dma, true);

    /* config dst data DMA */
    smix_get_dma_default_ch_config(SMIX, &dst_dma);
    dst_dma.src_addr = (uint32_t)&SMIX->DST_CH[SMIX_DST_CH].DATA + addr_shift;
    dst_dma.src_width = dma_width;
    dst_dma.src_addr_ctrl = smix_dma_address_fixed;
    dst_dma.src_burst_size = (sound->channel_num == 2) ? smix_dma_transfer_burst_2t : smix_dma_transfer_burst_1t;
    dst_dma.dst_addr = (uint32_t)&TEST_I2S->TXD[TEST_I2S_DATA_LINE] + addr_shift;
    dst_dma.dst_width = dma_width;
    dst_dma.dst_addr_ctrl = smix_dma_address_fixed;
    dst_dma.dst_req_sel = smix_dma_req_i2s1_tx; /* !!! align with I2s which use to play sound */
    dst_dma.dst_mode = smix_dma_mode_handshake;
    dst_dma.src_burst_size = smix_dma_transfer_burst_8t;
    dst_dma.trans_bytes = dst_data_bytes;
    dst_dma.complete_int_en = true;
    smix_config_dma_channel(SMIX, SMIX_DMA_CH_FOR_DST, &dst_dma, true);

    /* config mixer source */
    smix_get_mixer_source_ch_default_config(SMIX, &mixer_source);
    mixer_source.convert_rate = smix_rate_convert;
    mixer_source.length = (source_count - 4) * ratio;
    smix_mixer_config_source_ch(SMIX, SMIX_SOURCE_CH, &mixer_source);

    /* config mixer dst */
    smix_get_mixer_dst_ch_default_config(SMIX, &mixer_dst);
    mixer_dst.gain = smix_mixer_gain_decrease_12db;

    mixer_dst.src_ch_mask = (1 << SMIX_SOURCE_CH); /* enable source channel to destination channel */
    smix_mixer_config_dst_ch(SMIX, SMIX_DST_CH, &mixer_dst);

    i2s_reset_tx(TEST_I2S);
    dao_software_reset(HPM_DAO); /* reset DAO */
    if (i2s_fill_tx_dummy_data(TEST_I2S, TEST_I2S_DATA_LINE, sound->channel_num) != status_success) {
        printf("I2S error occurred during playing\n");
        return status_fail;
    }

    i2s_start(TEST_I2S);
    dao_start(HPM_DAO);

    /* Enable GPIO interrupt to stop/resume audio play or change gain */
    dao_i2s_playing = true;
    smix_gain = mixer_dst.gain;
    gpio_enable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN);
    gpio_enable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN);

    while (!dma_transfer_done) {
        if (dao_i2s_status_changed) {
            dao_i2s_status_changed = false;
            if (dao_i2s_playing) {
                printf("dao_i2s resume\n");
            } else {
                printf("dao_i2s pause\n");
            }
        }
        if (smix_gain_changed) {
            smix_gain_changed = false;
            if (smix_gain == smix_mixer_gain_0db) {
                printf("set SMIX gain to 0dB\n");
            } else if (smix_gain == smix_mixer_gain_decrease_12db) {
                printf("set SMIX gain to -12dB\n");
            }
        }
    }

    /* Disable GPIO interrupt */
    gpio_disable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN);
    gpio_disable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN);

    /* disable source channel 0 to dst channel 0*/
    smix_mixer_dst_disable(SMIX);
    smix_mixer_dst_disable_source_channel(SMIX, SMIX_DST_CH, (1 << SMIX_SOURCE_CH));
    i2s_stop(TEST_I2S); /* stop I2S */
    dao_stop(HPM_DAO);

    if (smix_mixer_check_dst_cal_saturation_error(SMIX, SMIX_DST_CH)
        || smix_mixer_check_source_cal_saturation_error(SMIX, SMIX_SOURCE_CH)
        || smix_mixer_check_dst_data_underflew(SMIX, SMIX_DST_CH)) {
        printf("SMIX error occurred during playing\n");
        return status_fail;
    }

    return status_success;
}

/*
 * GPIO interrupt handler
 */
SDK_DECLARE_EXT_ISR_M(BOARD_KEY_GPIO_IRQ, button_gpio_isr)
void button_gpio_isr(void)
{
    /* KEY1 */
    if (gpio_check_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN)) {
        key1_gpio_isr_curr_time = mchtmr_get_count(HPM_MCHTMR);
        if ((key1_gpio_isr_curr_time - key1_gpio_isr_pre_time > debounce_threshold)) {
            /* change DAO_I2S status */
            if (dao_i2s_playing) {
                /* stop DAO and I2S */
                smix_mixer_dst_disable(SMIX);
                smix_mixer_dst_disable_source_channel(SMIX, SMIX_DST_CH, (1 << SMIX_SOURCE_CH));
                i2s_disable(DAO_I2S);
                dao_stop(HPM_DAO);
                dao_i2s_playing = false;
                dao_i2s_status_changed = true;
            } else {
                /* reset and reopen DAO and I2S */
                i2s_reset_tx(DAO_I2S);
                dao_software_reset(HPM_DAO);
                smix_mixer_dst_enable(SMIX);
                smix_mixer_dst_enable_source_channel(SMIX, SMIX_DST_CH, (1 << SMIX_SOURCE_CH));
                i2s_enable(DAO_I2S);
                dao_start(HPM_DAO);
                dao_i2s_playing = true;
                dao_i2s_status_changed = true;
            }
        }

        key1_gpio_isr_pre_time = key1_gpio_isr_curr_time;
        gpio_clear_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN);
    }

    /* KEY2 */
    if (gpio_check_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN)) {
        key2_gpio_isr_curr_time = mchtmr_get_count(HPM_MCHTMR);
        if ((key2_gpio_isr_curr_time - key2_gpio_isr_pre_time > debounce_threshold)) {
            /* change DAO_I2S status */
            if (smix_gain == smix_mixer_gain_decrease_12db) {
                smix_set_dst_gain(HPM_SMIX, SMIX_DST_CH, smix_mixer_gain_0db);
                smix_gain = smix_mixer_gain_0db;
            } else {
                smix_set_dst_gain(HPM_SMIX, SMIX_DST_CH, smix_mixer_gain_decrease_12db);
                smix_gain = smix_mixer_gain_decrease_12db;
            }
            smix_gain_changed = true;
        }

        key2_gpio_isr_pre_time = key2_gpio_isr_curr_time;
        gpio_clear_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN);
    }
}

/* Button GPIO interrupt configuration */
void button_gpio_config(void)
{
    board_init_gpio_pins();

    gpio_interrupt_trigger_t trigger = gpio_interrupt_trigger_edge_falling;

    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN);
    gpio_config_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN, trigger);

    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN);
    gpio_config_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN, trigger);

    intc_m_enable_irq_with_priority(BOARD_KEY_GPIO_IRQ, 1);

    /* button debounce time threshold */
    uint32_t mchtmr_freq;
    mchtmr_freq = clock_get_frequency(clock_mchtmr0);
    debounce_threshold = mchtmr_freq / 2; /* 500ms */
}

int main(void)
{
    uint32_t i2s_mclk_freq;
    hpm_stat_t stat;

    board_init();
    clock_add_to_group(clock_smix, 0);
    printf("Sound mixer change DAO volume example\n");

    /* Button GPIO configuration */
    button_gpio_config();
    printf("Press Key1 will pause/resume audio play, Press Key2 will change audio play gain.");

    audio_data_t source_data;
    source_data.sample_rate = hpm_desc_sample_rate;
    source_data.audio_depth = hpm_desc_audio_depth;
    source_data.channel_num = hpm_desc_channel_num;
    source_data.data = hpm_desc_audio;
    source_data.length = sizeof(hpm_desc_audio);

    audio_data_t target_data;
    target_data.sample_rate = SMIX_OUTPUT_SAMPLE_RATE;
    target_data.audio_depth = source_data.audio_depth;
    target_data.channel_num = source_data.channel_num;

    board_init_dao_clock();
    init_dao_pins();
    i2s_mclk_freq = board_config_i2s_clock(TEST_I2S, SMIX_OUTPUT_SAMPLE_RATE);

    /* config I2S peripehral */
    stat = dao_i2s_init(&target_data, i2s_mclk_freq);
    if (stat != status_success) {
        printf("Init I2S failed\n");
        return 1;
    }
    intc_m_enable_irq_with_priority(IRQn_SMIX_DMA, 1);

    char option;
    while (1) {
        printf("\n-- Select audio play action --\n");
        printf(" 1 - audio play\n");
        option = getchar();
        putchar(option);
        putchar('\n');

        switch (option) {
        case '1':
            mixer_play_one_sound(&source_data);
            break;
        default:
            break;
        }
    }

    return 0;
}
