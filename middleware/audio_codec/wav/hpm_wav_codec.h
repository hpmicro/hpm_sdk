/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_WAV_DECODER_H
#define HPM_WAV_DECODER_H
 
#include "hpm_audio_codec.h" 
/**
 * @brief riff header
 * 
 */
typedef struct
{
    uint32_t groupid;               /** fix "RIFF" */
    uint32_t size ;                 /** file size sub 8 */
    uint32_t riff_type;             /** fix "WAVE" */
} wav_riff;

/**
 * @brief fmt header
 * 
 */
typedef struct
{
    uint32_t id;            /** fix "fmt" */
    uint32_t size ;         /** sizeof(wav_formatchunk) - id - size */
    uint16_t audioformat;   /** 0X01:PCM ;0X11:IMA ADPCM */
    uint16_t channels;   /** channels num 1 or 2 */
    uint32_t samplerate;    /**  sample rate */
    uint32_t byterate;      /** byterate=samplerate*channels*bitspersample/8 */ 
    uint16_t blockalign;    /** bitspersample*channels/8 */
    uint16_t bitspersample; /** 8 16 24 32*/
} wav_formatchunk;

/**
 * @brief data header
 * 
 */
typedef struct
{
    uint32_t id;            /** fix "data" */
    uint32_t size;          /** data size */     
} wav_data;

/**
 * @brief wav head describe
 * 
 */
typedef struct
{ 
    wav_riff riff_chunk;  /** riff chunk */
    wav_formatchunk fmt_chunk;    /** fmt chunk */
    wav_data data_chunk;  /** data chunk */      
} hpm_wav_head; 

/**
 * @brief wav control
 * 
 */
typedef struct
{ 
    char * file_name;
    hpm_wav_head wav_head;
    uint32_t sec_total;     /** song total seconds */
    uint32_t sec_cur;       /** song current seconds */
    uint32_t data_pos;     /**  data position */
    uint32_t remaining_data;    /**  The amount of data remaining, according to which the music has been played */
    hpm_audiocodec_callback func; /** callback function */
}hpm_wav_ctrl;

/**
 * @brief Init wav decode function
 *
 * @param[in] fname file path and name string
 * @param[inout] wav_ctrl @ref hpm_wav_ctrl
 * @param[in] pbuf buffer, minimum 512 bytes
 *
 * @return @ref hpm_stat_t
 */
hpm_stat_t hpm_wav_decode_init(char *fname, hpm_wav_ctrl *wav_ctrl, uint8_t (*pbuf)[512]);

/**
 * @brief hpm wav file decode
 *
 * @param[in] wav_ctrl @ref hpm_wav_ctrl
 * @param[out] buf output data
 * @param size buf size
 * @return uint32_t data size
 */
uint32_t hpm_wav_decode(hpm_wav_ctrl *wav_ctrl, uint8_t *buf, uint32_t size);
#endif
