/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_SOC_IP_FEATURE_H
#define HPM_SOC_IP_FEATURE_H

/* PLIC related feature */
#define HPM_IP_FEATURE_PLIC_HIGHEST_PRIORITY 31
#define HPM_IP_FEATURE_PLIC_SUPPORT_S_MODE 1
#define HPM_IP_FEATURE_PLIC_HAS_TRIGGER_TYPE 1

/* UART related feature */
#define HPM_IP_FEATURE_UART_RX_IDLE_DETECT 1
#define HPM_IP_FEATURE_UART_FCRR 1
#define HPM_IP_FEATURE_UART_RX_EN 1

/* I2C related feature */
#define HPM_IP_FEATURE_I2C_SUPPORT_RESET 1

/* SPI related feature */
#define HPM_IP_FEATURE_SPI_NEW_TRANS_COUNT 1
#define HPM_IP_FEATURE_SPI_CS_SELECT 1
#define HPM_IP_FEATURE_SPI_SUPPORT_DIRECTIO 1

/* GPTMR related feature */
#define HPM_IP_FEATURE_GPTMR_MONITOR 1
#define HPM_IP_FEATURE_GPTMR_OP_MODE 1

/* ADC16 related feature */
#define HPM_IP_FEATURE_ADC16_SEQ_HCFG_EN 1

/* DAO related feature */
#define HPM_IP_FEATURE_DAO_DATA_FORMAT_CONFIG 1

/* CAM related feature */
#define HPM_IP_FEATURE_CAM_INV_DEN 1

#endif /* HPM_SOC_IP_FEATURE_H */