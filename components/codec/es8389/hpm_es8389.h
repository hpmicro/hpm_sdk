/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_ES8389_H
#define HPM_ES8389_H

#include "hpm_i2c_drv.h"
#include "hpm_common.h"
#include "hpm_es8389_regs.h"
#include "hpm_codec_control.h"

/** @def ES8389_I2C_ADDR
 * @brief ES8389 I2C device address
 */
#define ES8389_I2C_ADDR 0x10

/**
 * @name Parameters definitions
 * @{
 */

/** @def EVEREST_MASTER_MODE
 * @brief Master mode selection value
 */
#define EVEREST_MASTER_MODE 1

/** @def EVEREST_SLAVE_MODE
 * @brief Slave mode selection value
 */
#define EVEREST_SLAVE_MODE  0

/** @def EVEREST_INVERT
 * @brief Invert signal selection value
 */
#define EVEREST_INVERT     1

/** @def EVEREST_NO_INVERT
 * @brief No invert signal selection value
 */
#define EVEREST_NO_INVERT   0

/** @def EVEREST_ON
 * @brief On state selection value
 */
#define EVEREST_ON         1

/** @def EVEREST_OFF
 * @brief Off state selection value
 */
#define EVEREST_OFF        0

/** @def EVEREST_ENABLE
 * @brief Enable selection value
 */
#define EVEREST_ENABLE     1

/** @def EVEREST_DISABLE
 * @brief Disable selection value
 */
#define EVEREST_DISABLE    0

/** @def EVEREST_MUTE
 * @brief Mute selection value
 */
#define EVEREST_MUTE       1

/** @def EVEREST_UNMUTE
 * @brief Unmute selection value
 */
#define EVEREST_UNMUTE     0

/** @def EVEREST_TRI
 * @brief Tri-state selection value
 */
#define EVEREST_TRI        1

/** @def EVEREST_NO_TRI
 * @brief No tri-state selection value
 */
#define EVEREST_NO_TRI      0

/** @def EVEREST_SCLK_PIN
 * @brief SCLK pin selection value
 */
#define EVEREST_SCLK_PIN   1

/** @def EVEREST_MCLK_PIN
 * @brief MCLK pin selection value
 */
#define EVEREST_MCLK_PIN   0

/** @def EVEREST_VOLTAGE_1V8
 * @brief 1.8V voltage selection value
 */
#define EVEREST_VOLTAGE_1V8 1

/** @def EVEREST_VOLTAGE_3V3
 * @brief 3.3V voltage selection value
 */
#define EVEREST_VOLTAGE_3V3 0

/** @def EVEREST_DMIC_SEL
 * @brief DMIC selection value
 */
#define EVEREST_DMIC_SEL    1

/** @def EVEREST_AMIC_SEL
 * @brief AMIC selection value
 */
#define EVEREST_AMIC_SEL    0

/** @def EVEREST_DRIVE_SEL_NORMAL
 * @brief Normal drive selection value
 */
#define EVEREST_DRIVE_SEL_NORMAL    0

/** @def EVEREST_DRIVE_SEL_LOW_POWER
 * @brief Low power drive selection value
 */
#define EVEREST_DRIVE_SEL_LOW_POWER  1

/** @def EVEREST_DRIVE_SEL_HEADPHONE
 * @brief Headphone drive selection value
 */
#define EVEREST_DRIVE_SEL_HEADPHONE 2

/** @} */

/**
 * @name Parameters configuration
 * @{
 */

/** @def ES8389_DVDD_VoltageSel
 * @brief DVDD voltage selection
 * @note Select according to actual voltage
 */
#define ES8389_DVDD_VoltageSel EVEREST_VOLTAGE_3V3

/** @def ES8389_MainCLK_Source
 * @brief Main clock source selection
 * @note When selecting SCLK_PIN, need to synchronously modify ratio of MCLK/LRCK to be equal to SCLK/LRCK
 */
#define ES8389_MainCLK_Source  EVEREST_MCLK_PIN

/** @def ES8389_Analog_DriveSel
 * @brief Analog drive selection
 * @details Normal (normal LOUT/PA mode); LowPower (low power mode); HeadPhone (headphone drive mode)
 */
#define ES8389_Analog_DriveSel EVEREST_DRIVE_SEL_NORMAL

/** @def ES8389_DMICSel
 * @brief DMIC function enable configuration
 */
#define ES8389_DMICSel    EVEREST_DISABLE

/** @def ES8389_MicTypeSel
 * @brief Microphone type selection
 * @details EVEREST_AMIC_SEL=0; EVEREST_DMIC_SEL=1
 */
#define ES8389_MicTypeSel EVEREST_AMIC_SEL

/** @def ES8389_MCLK_Invert
 * @brief MCLK invert configuration
 */
#define ES8389_MCLK_Invert EVEREST_NO_INVERT

/** @def ES8389_BCLK_Invert
 * @brief SCLK invert configuration
 * @note Set to 1 when external SCLK/LRCK rising edge is required
 */
#define ES8389_BCLK_Invert EVEREST_NO_INVERT

/**
 * @name DMIC/MIX/SLOT configuration
 * @note Do not modify the following if DMIC/MIX/SLOT functions are not used
 * @{
 */

/** @def ES8389_DMIC_Invert
 * @brief DMIC invert configuration - swap DMIC data
 */
#define ES8389_DMIC_Invert        EVEREST_NO_INVERT

/** @def ES8389_ADC_DMICGainSel
 * @brief DMIC gain setting
 * @details Range: Min0~Max255, 0.5dB per step, 191 represents 0dB
 */
#define ES8389_ADC_DMICGainSel    191

/** @def ES8389_ADC2DAC_MIX_Volume
 * @brief MIX gain setting
 * @details Range: Min0~Max127, 1dB per step, 95 represents 0dB
 */
#define ES8389_ADC2DAC_MIX_Volume 95

/** @def ES8389_ADCPTDM_SLOTSel
 * @brief ADC SLOT data channel selection
 * @details Range: Min0~Max7
 */
#define ES8389_ADCPTDM_SLOTSel    0

/** @def ES8389_DACSTDM_SLOTSel
 * @brief DAC SLOT data channel selection
 * @details Range: Min0~Max7
 */
#define ES8389_DACSTDM_SLOTSel    0

/** @} */

/** @def ES8389_BCLK_TRI
 * @brief Tri-state configuration for SCLK pin
 */
#define ES8389_BCLK_TRI   EVEREST_NO_TRI

/** @def ES8389_LRCK_TRI
 * @brief Tri-state configuration for LRCK pin
 */
#define ES8389_LRCK_TRI   EVEREST_NO_TRI

/** @def ES8389_ASDOUT_TRI
 * @brief Tri-state configuration for ASDOUT pin
 */
#define ES8389_ASDOUT_TRI EVEREST_NO_TRI

/** @def ES8389_ADC1_Invert
 * @brief ADC1 invert configuration - 180° phase inversion
 */
#define ES8389_ADC1_Invert EVEREST_NO_INVERT

/** @def ES8389_ADC2_Invert
 * @brief ADC2 invert configuration - 180° phase inversion
 */
#define ES8389_ADC2_Invert EVEREST_NO_INVERT

/** @def ES8389_DAC1_Invert
 * @brief DAC1 invert configuration - 180° phase inversion
 */
#define ES8389_DAC1_Invert EVEREST_NO_INVERT

/** @def ES8389_DAC2_Invert
 * @brief DAC2 invert configuration - 180° phase inversion
 */
#define ES8389_DAC2_Invert EVEREST_NO_INVERT

/** @def ES8389_ADC1_PGAGainSel
 * @brief ADC1 analog PGA gain setting
 * @details Range: Min0~Max14
 */
#define ES8389_ADC1_PGAGainSel 10

/** @def ES8389_ADC2_PGAGainSel
 * @brief ADC2 analog PGA gain setting
 * @details Range: Min0~Max14
 */
#define ES8389_ADC2_PGAGainSel 10

/** @def ES8389_ADC1_DigVolume
 * @brief ADC1 digital gain setting
 * @details Range: Min0~Max255, 0.5dB per step, 191 represents 0dB, 215 represents +12dB
 */
#define ES8389_ADC1_DigVolume  215

/** @def ES8389_ADC2_DigVolume
 * @brief ADC2 digital gain setting
 * @details Range: Min0~Max255, 0.5dB per step, 191 represents 0dB, 215 represents +12dB
 */
#define ES8389_ADC2_DigVolume  215

/** @def ES8389_DAC1_DigVolume
 * @brief DAC1 digital gain setting
 * @details Range: Min0~Max255, 0.5dB per step, 191 represents 0dB, 215 represents +12dB
 */
#define ES8389_DAC1_DigVolume  191

/** @def ES8389_DAC2_DigVolume
 * @brief DAC2 digital gain setting
 * @details Range: Min0~Max255, 0.5dB per step, 191 represents 0dB, 215 represents +12dB
 */
#define ES8389_DAC2_DigVolume  191

/** @def ES8389_ADC1_MUTE
 * @brief ADC1 mute configuration - all data set to 0
 */
#define ES8389_ADC1_MUTE EVEREST_UNMUTE

/** @def ES8389_ADC2_MUTE
 * @brief ADC2 mute configuration - all data set to 0
 */
#define ES8389_ADC2_MUTE EVEREST_UNMUTE

/** @def ES8389_DAC1_MUTE
 * @brief DAC1 mute configuration - all data set to 0
 */
#define ES8389_DAC1_MUTE EVEREST_UNMUTE

/** @def ES8389_DAC2_MUTE
 * @brief DAC2 mute configuration - all data set to 0
 */
#define ES8389_DAC2_MUTE EVEREST_UNMUTE

/** @def ES8389_MIX_DAC1AndADC1ToADC1
 * @brief Mix function: DAC1 mixed with ADC1 data then output to ADC1
 */
#define ES8389_MIX_DAC1AndADC1ToADC1 EVEREST_DISABLE

/** @def ES8389_MIX_DAC2AndADC2ToADC2
 * @brief Mix function: DAC2 mixed with ADC2 data then output to ADC2
 */
#define ES8389_MIX_DAC2AndADC2ToADC2 EVEREST_DISABLE

/** @def ES8389_MIX_DAC1AndDAC2ToDAC1
 * @brief Mix function: DAC1 mixed with DAC2 data then output to DAC1
 */
#define ES8389_MIX_DAC1AndDAC2ToDAC1 EVEREST_DISABLE

/** @def ES8389_MIX_DAC1AndDAC2ToDAC2
 * @brief Mix function: DAC1 mixed with DAC2 data then output to DAC2
 */
#define ES8389_MIX_DAC1AndDAC2ToDAC2 EVEREST_DISABLE

/** @def ES8389_MIX_ADC1AndDAC1ToDAC1
 * @brief Mix function: ADC1 mixed with DAC1 data then output to DAC1
 */
#define ES8389_MIX_ADC1AndDAC1ToDAC1 EVEREST_DISABLE

/** @def ES8389_MIX_ADC2AndDAC2ToDAC2
 * @brief Mix function: ADC2 mixed with DAC2 data then output to DAC2
 */
#define ES8389_MIX_ADC2AndDAC2ToDAC2 EVEREST_DISABLE

/** @def ES8389_LOOP_ADCToDAC
 * @brief Loop function: ADC data internally looped to DAC data
 */
#define ES8389_LOOP_ADCToDAC EVEREST_DISABLE

/** @def ES8389_LOOP_DACToADC
 * @brief Loop function: ADC data followed by DAC data output through ASDOUT (similar to TDMIN)
 */
#define ES8389_LOOP_DACToADC EVEREST_DISABLE

/**
 * @name ES8389_ADCL_InputSel
 * @brief ADC left channel input selection
 * @{
 */

/** @def ES8389_ADCL_InputSel
 * @brief ADC left channel input selection configuration
 */
#define ES8389_ADCL_InputSel         ES8389_InputSelL_DiffMIC1P1N

/** @def ES8389_InputSelL_DiffMIC1P1N
 * @brief Differential input MIC1P-MIC1N
 */
#define ES8389_InputSelL_DiffMIC1P1N 1

/** @def ES8389_InputSelL_SingleMIC1P
 * @brief Single-ended input MIC1P
 */
#define ES8389_InputSelL_SingleMIC1P 5

/** @def ES8389_InputSelL_SingleMIC2P
 * @brief Single-ended input MIC2P
 */
#define ES8389_InputSelL_SingleMIC2P 6

/** @} */

/**
 * @name ES8389_ADCR_InputSel
 * @brief ADC right channel input selection
 * @{
 */

/** @def ES8389_ADCR_InputSel
 * @brief ADC right channel input selection configuration
 */
#define ES8389_ADCR_InputSel         ES8389_InputSelR_SingleMIC2N

/** @def ES8389_InputSelR_DiffMIC2N2P
 * @brief Differential input MIC2N-MIC1P
 */
#define ES8389_InputSelR_DiffMIC2N2P 1

/** @def ES8389_InputSelR_SingleMIC2N
 * @brief Single-ended input MIC2N
 */
#define ES8389_InputSelR_SingleMIC2N 5

/** @def ES8389_InputSelR_SingleMIC1N
 * @brief Single-ended input MIC1N
 */
#define ES8389_InputSelR_SingleMIC1N 6

/** @} */

/** @} */

/**
 * @brief ES8389 module enumeration
 */
typedef enum es8389_module {
    es8389_adc1 = 0,  /**< ADC1 module */
    es8389_adc2 = 1,  /**< ADC2 module */
    es8389_dac1 = 2,  /**< DAC1 module */
    es8389_dac2 = 3,  /**< DAC2 module */
} es8389_module_t;

/**
 * @brief ES8389 audio protocol enumeration
 */
typedef enum es8389_protocol {
    es8389_protocol_i2s = 0,            /**< I2S protocol */
    es8389_protocol_left_justified = 1, /**< Left justified */
    es8389_protocol_pcma = 2,           /**< DSP/PCM mode A */
    es8389_protocol_pcmb = 3,           /**< DSP/PCM mode B */
} es8389_protocol_t;

/**
 * @brief ES8389 LRCLK polarity enumeration
 */
typedef enum es8389_lrclk_polarity {
    es8389_lrclk_polarity_default = 0U,               /**< LRCLK default polarity: Left/Right=High/Low (Left Justified), Left/Right=Low/High (I2S) */
    es8389_lrclk_polarity_low_for_left_channel = 1U,  /**< LRCLK low for left channel for I2S(Philips)/Left Justified */
    es8389_lrclk_polarity_high_for_left_channel = 2U, /**< LRCLK high for left channel for I2S(Philips)/Left Justified */
} es8389_lrclk_polarity_t;

/**
 * @brief ES8389 configuration structure
 */
typedef struct es8389_config {
    bool master_mode;                    /**< Master mode enable flag */
    uint32_t mclk_hz;                   /**< Master clock frequency in Hz */
    uint32_t sample_rate;               /**< Audio sample rate in Hz */
    uint8_t data_width;                 /**< Audio data width in bits */
    es8389_protocol_t protocol;          /**< Audio transfer protocol */
    es8389_lrclk_polarity_t lrclk_polarity; /**< LRCLK polarity */
} es8389_config_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get default configuration for ES8389.
 *
 * This function initializes the configuration structure with default values.
 * The default configuration:
 * - master_mode: false (slave mode)
 * - mclk_hz: 0 (to be set later based on actual clock)
 * - sample_rate: 48000 Hz
 * - data_width: 32 bits
 * - protocol: es8389_protocol_left_justified
 * - lrclk_polarity: es8389_lrclk_polarity_low_for_left_channel
 *
 * @param config Pointer to ES8389 configuration structure.
 */
void es8389_get_default_config(es8389_config_t *config);

/**
 * @brief ES8389 initialize function.
 *
 * This function initializes the ES8389 codec with the provided configuration.
 * It configures the codec registers for audio playback and recording, including
 * protocol, data format, clock settings, and power management.
 *
 * @param control ES8389 control structure.
 * @param config ES8389 configuration structure.
 * @return status_success if initialization successful, status_fail otherwise.
 */
hpm_stat_t es8389_init(codec_control_t *control, es8389_config_t *config);

/**
 * @brief Set the audio transfer protocol and LRCLK polarity.
 *
 * This function configures the audio serial port protocol (I2S, Left Justified,
 * PCM A, PCM B) and LRCLK polarity for both ADC and DAC paths.
 *
 * @param control ES8389 control structure.
 * @param protocol Audio transfer protocol (I2S, Left Justified, PCM A, PCM B).
 * @param lrclk_polarity LRCLK polarity setting.
 * @return status_success if configuration successful, status_fail otherwise.
 */
hpm_stat_t es8389_set_protocol(codec_control_t *control, es8389_protocol_t protocol, es8389_lrclk_polarity_t lrclk_polarity);

/**
 * @brief Set the audio data width.
 *
 * This function configures the audio data width for both ADC and DAC paths.
 * ES8389 supports 16, 18, 20, 24, and 32 bits data width.
 *
 * @param control ES8389 control structure.
 * @param data_width Audio data width in bits (16, 18, 20, 24, or 32).
 * @return status_success if configuration successful, status_invalid_argument if data_width is not supported.
 */
hpm_stat_t es8389_set_data_width(codec_control_t *control, uint8_t data_width);

/**
 * @brief Configure the data format of audio data.
 *
 * This function configures the master clock, sample rate, and data width for the codec.
 * It internally calls es8389_config_clock and es8389_set_data_width.
 *
 * @param control ES8389 control structure pointer.
 * @param mclk Master clock frequency in Hz for the codec.
 * @param sample_rate Sample rate of audio data. ES8389 supports various sample rates
 *                    including 8k, 11.025k, 12k, 16k, 22.05k, 24k, 32k, 44.1k, 48k, 96k.
 * @param data_width Bit depth of audio data (16, 18, 20, 24, or 32 bits).
 * @return status_success if configuration successful, status_fail otherwise.
 */
hpm_stat_t es8389_set_data_format(codec_control_t *control, uint32_t mclk, uint32_t sample_rate, uint8_t data_width);

/**
 * @brief Set the volume of different modules in ES8389.
 *
 * This function sets the digital volume for the specified module.
 * Volume range: 0-255, where 191 (0xBF) represents 0dB.
 * Volume setting uses 0.5dB per step: 191 represents 0dB, 215 represents +12dB.
 *
 * Module: es8389_adc1, volume range: 0-255 (0.5dB per step, 191 = 0dB)
 * Module: es8389_adc2, volume range: 0-255 (0.5dB per step, 191 = 0dB)
 * Module: es8389_dac1, volume range: 0-255 (0.5dB per step, 191 = 0dB)
 * Module: es8389_dac2, volume range: 0-255 (0.5dB per step, 191 = 0dB)
 *
 * @param control ES8389 control structure.
 * @param module Module to set volume, it can be es8389_adc1, es8389_adc2, es8389_dac1, or es8389_dac2.
 * @param volume Volume value to be set (0-255).
 * @return status_success if successful, status_invalid_argument if module is invalid.
 */
hpm_stat_t es8389_set_volume(codec_control_t *control, es8389_module_t module, uint8_t volume);

/**
 * @brief Enable/disable mute for the specified module.
 *
 * This function sets or clears the mute bit for the specified module.
 * When muted, all data is set to 0 for the module.
 *
 * @param control ES8389 control structure.
 * @param module Module to set mute, it can be es8389_adc1, es8389_adc2, es8389_dac1, or es8389_dac2.
 * @param mute true to mute the module, false to unmute.
 * @return status_success if successful, status_invalid_argument if module is invalid.
 */
hpm_stat_t es8389_mute(codec_control_t *control, es8389_module_t module, bool mute);

/**
 * @brief Enter power on state.
 *
 * This function powers on the ES8389 codec and enables all analog circuits.
 * The codec enters bias_on state and is ready for audio operation.
 *
 * @param control ES8389 control structure.
 * @return status_success if successful, status_fail otherwise.
 */
hpm_stat_t es8389_enter_power_on(codec_control_t *control);

/**
 * @brief Enter standby state.
 *
 * This function puts the ES8389 codec into standby mode to save power.
 * The codec maintains its configuration but reduces power consumption.
 * Before entering standby, DAC is muted to prevent pop noise.
 *
 * @param control ES8389 control structure.
 * @return status_success if successful, status_fail otherwise.
 */
hpm_stat_t es8389_enter_standby(codec_control_t *control);

/**
 * @brief Enter power down state.
 *
 * This function powers down the ES8389 codec to minimize power consumption.
 * The codec enters bias_off state. All analog circuits are disabled.
 * Configuration is maintained but the codec is not operational.
 *
 * @param control ES8389 control structure.
 * @return status_success if successful, status_fail otherwise.
 */
hpm_stat_t es8389_enter_power_down(codec_control_t *control);

#ifdef __cplusplus
}
#endif

#endif /* HPM_ES8389_H */