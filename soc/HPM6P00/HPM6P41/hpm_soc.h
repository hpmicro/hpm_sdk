/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_SOC_H
#define HPM_SOC_H


#include "hpm_soc_irq.h"
#include "hpm_common.h"

#include "hpm_gpio_regs.h"
/* Address of GPIO instances */
/* FGPIO base address */
#define HPM_FGPIO_BASE (0x300000UL)
/* FGPIO base pointer */
#define HPM_FGPIO ((GPIO_Type *) HPM_FGPIO_BASE)
/* GPIO0 base address */
#define HPM_GPIO0_BASE (0xF00D0000UL)
/* GPIO0 base pointer */
#define HPM_GPIO0 ((GPIO_Type *) HPM_GPIO0_BASE)
/* GPIO1 base address */
#define HPM_GPIO1_BASE (0xF00D4000UL)
/* GPIO1 base pointer */
#define HPM_GPIO1 ((GPIO_Type *) HPM_GPIO1_BASE)
/* PGPIO base address */
#define HPM_PGPIO_BASE (0xF411C000UL)
/* PGPIO base pointer */
#define HPM_PGPIO ((GPIO_Type *) HPM_PGPIO_BASE)

/* Address of DM instances */
/* DM base address */
#define HPM_DM_BASE (0x30000000UL)

#include "hpm_plic_regs.h"
/* Address of PLIC instances */
/* PLIC base address */
#define HPM_PLIC_BASE (0xE4000000UL)
/* PLIC base pointer */
#define HPM_PLIC ((PLIC_Type *) HPM_PLIC_BASE)

#include "hpm_mchtmr_regs.h"
/* Address of MCHTMR instances */
/* MCHTMR base address */
#define HPM_MCHTMR_BASE (0xE6000000UL)
/* MCHTMR base pointer */
#define HPM_MCHTMR ((MCHTMR_Type *) HPM_MCHTMR_BASE)

#include "hpm_plic_sw_regs.h"
/* Address of PLICSW instances */
/* PLICSW base address */
#define HPM_PLICSW_BASE (0xE6400000UL)
/* PLICSW base pointer */
#define HPM_PLICSW ((PLIC_SW_Type *) HPM_PLICSW_BASE)

#include "hpm_gptmr_regs.h"
/* Address of GPTMR instances */
/* GPTMR0 base address */
#define HPM_GPTMR0_BASE (0xF0000000UL)
/* GPTMR0 base pointer */
#define HPM_GPTMR0 ((GPTMR_Type *) HPM_GPTMR0_BASE)
/* GPTMR1 base address */
#define HPM_GPTMR1_BASE (0xF0004000UL)
/* GPTMR1 base pointer */
#define HPM_GPTMR1 ((GPTMR_Type *) HPM_GPTMR1_BASE)
/* GPTMR2 base address */
#define HPM_GPTMR2_BASE (0xF0008000UL)
/* GPTMR2 base pointer */
#define HPM_GPTMR2 ((GPTMR_Type *) HPM_GPTMR2_BASE)
/* GPTMR3 base address */
#define HPM_GPTMR3_BASE (0xF000C000UL)
/* GPTMR3 base pointer */
#define HPM_GPTMR3 ((GPTMR_Type *) HPM_GPTMR3_BASE)
/* NTMR0 base address */
#define HPM_NTMR0_BASE (0xF1410000UL)
/* NTMR0 base pointer */
#define HPM_NTMR0 ((GPTMR_Type *) HPM_NTMR0_BASE)
/* PTMR base address */
#define HPM_PTMR_BASE (0xF4120000UL)
/* PTMR base pointer */
#define HPM_PTMR ((GPTMR_Type *) HPM_PTMR_BASE)

#include "hpm_uart_regs.h"
/* Address of UART instances */
/* UART0 base address */
#define HPM_UART0_BASE (0xF0040000UL)
/* UART0 base pointer */
#define HPM_UART0 ((UART_Type *) HPM_UART0_BASE)
/* UART1 base address */
#define HPM_UART1_BASE (0xF0044000UL)
/* UART1 base pointer */
#define HPM_UART1 ((UART_Type *) HPM_UART1_BASE)
/* UART2 base address */
#define HPM_UART2_BASE (0xF0048000UL)
/* UART2 base pointer */
#define HPM_UART2 ((UART_Type *) HPM_UART2_BASE)
/* UART3 base address */
#define HPM_UART3_BASE (0xF004C000UL)
/* UART3 base pointer */
#define HPM_UART3 ((UART_Type *) HPM_UART3_BASE)
/* PUART base address */
#define HPM_PUART_BASE (0xF4124000UL)
/* PUART base pointer */
#define HPM_PUART ((UART_Type *) HPM_PUART_BASE)

#include "hpm_i2c_regs.h"
/* Address of I2C instances */
/* I2C0 base address */
#define HPM_I2C0_BASE (0xF0060000UL)
/* I2C0 base pointer */
#define HPM_I2C0 ((I2C_Type *) HPM_I2C0_BASE)
/* I2C1 base address */
#define HPM_I2C1_BASE (0xF0064000UL)
/* I2C1 base pointer */
#define HPM_I2C1 ((I2C_Type *) HPM_I2C1_BASE)
/* I2C2 base address */
#define HPM_I2C2_BASE (0xF0068000UL)
/* I2C2 base pointer */
#define HPM_I2C2 ((I2C_Type *) HPM_I2C2_BASE)

#include "hpm_spi_regs.h"
/* Address of SPI instances */
/* SPI0 base address */
#define HPM_SPI0_BASE (0xF0070000UL)
/* SPI0 base pointer */
#define HPM_SPI0 ((SPI_Type *) HPM_SPI0_BASE)
/* SPI1 base address */
#define HPM_SPI1_BASE (0xF0074000UL)
/* SPI1 base pointer */
#define HPM_SPI1 ((SPI_Type *) HPM_SPI1_BASE)
/* SPI2 base address */
#define HPM_SPI2_BASE (0xF0078000UL)
/* SPI2 base pointer */
#define HPM_SPI2 ((SPI_Type *) HPM_SPI2_BASE)

#include "hpm_crc_regs.h"
/* Address of CRC instances */
/* CRC base address */
#define HPM_CRC_BASE (0xF0080000UL)
/* CRC base pointer */
#define HPM_CRC ((CRC_Type *) HPM_CRC_BASE)

#include "hpm_tsns_regs.h"
/* Address of TSNS instances */
/* TSNS base address */
#define HPM_TSNS_BASE (0xF0090000UL)
/* TSNS base pointer */
#define HPM_TSNS ((TSNS_Type *) HPM_TSNS_BASE)

#include "hpm_mbx_regs.h"
/* Address of MBX instances */
/* MBX0A base address */
#define HPM_MBX0A_BASE (0xF00A0000UL)
/* MBX0A base pointer */
#define HPM_MBX0A ((MBX_Type *) HPM_MBX0A_BASE)
/* MBX0B base address */
#define HPM_MBX0B_BASE (0xF00A4000UL)
/* MBX0B base pointer */
#define HPM_MBX0B ((MBX_Type *) HPM_MBX0B_BASE)
/* MBX1A base address */
#define HPM_MBX1A_BASE (0xF00A8000UL)
/* MBX1A base pointer */
#define HPM_MBX1A ((MBX_Type *) HPM_MBX1A_BASE)
/* MBX1B base address */
#define HPM_MBX1B_BASE (0xF00AC000UL)
/* MBX1B base pointer */
#define HPM_MBX1B ((MBX_Type *) HPM_MBX1B_BASE)

#include "hpm_ewdg_regs.h"
/* Address of EWDG instances */
/* EWDG0 base address */
#define HPM_EWDG0_BASE (0xF00B0000UL)
/* EWDG0 base pointer */
#define HPM_EWDG0 ((EWDG_Type *) HPM_EWDG0_BASE)
/* EWDG1 base address */
#define HPM_EWDG1_BASE (0xF00B4000UL)
/* EWDG1 base pointer */
#define HPM_EWDG1 ((EWDG_Type *) HPM_EWDG1_BASE)
/* PEWDG base address */
#define HPM_PEWDG_BASE (0xF4128000UL)
/* PEWDG base pointer */
#define HPM_PEWDG ((EWDG_Type *) HPM_PEWDG_BASE)

#include "hpm_dmamux_regs.h"
/* Address of DMAMUX instances */
/* DMAMUX base address */
#define HPM_DMAMUX_BASE (0xF00C4000UL)
/* DMAMUX base pointer */
#define HPM_DMAMUX ((DMAMUX_Type *) HPM_DMAMUX_BASE)

#include "hpm_dmav2_regs.h"
/* Address of DMAV2 instances */
/* HDMA base address */
#define HPM_HDMA_BASE (0xF00C8000UL)
/* HDMA base pointer */
#define HPM_HDMA ((DMAV2_Type *) HPM_HDMA_BASE)
/* XDMA base address */
#define HPM_XDMA_BASE (0xF3100000UL)
/* XDMA base pointer */
#define HPM_XDMA ((DMAV2_Type *) HPM_XDMA_BASE)

#include "hpm_gpiom_regs.h"
/* Address of GPIOM instances */
/* GPIOM base address */
#define HPM_GPIOM_BASE (0xF00D8000UL)
/* GPIOM base pointer */
#define HPM_GPIOM ((GPIOM_Type *) HPM_GPIOM_BASE)

#include "hpm_lobs_regs.h"
/* Address of LOBS instances */
/* LOBS base address */
#define HPM_LOBS_BASE (0xF00DC000UL)
/* LOBS base pointer */
#define HPM_LOBS ((LOBS_Type *) HPM_LOBS_BASE)

#include "hpm_adc16_regs.h"
/* Address of ADC16 instances */
/* ADC0 base address */
#define HPM_ADC0_BASE (0xF0100000UL)
/* ADC0 base pointer */
#define HPM_ADC0 ((ADC16_Type *) HPM_ADC0_BASE)
/* ADC1 base address */
#define HPM_ADC1_BASE (0xF0104000UL)
/* ADC1 base pointer */
#define HPM_ADC1 ((ADC16_Type *) HPM_ADC1_BASE)
/* ADC2 base address */
#define HPM_ADC2_BASE (0xF0108000UL)
/* ADC2 base pointer */
#define HPM_ADC2 ((ADC16_Type *) HPM_ADC2_BASE)

#include "hpm_dac_regs.h"
/* Address of DAC instances */
/* DAC0 base address */
#define HPM_DAC0_BASE (0xF0120000UL)
/* DAC0 base pointer */
#define HPM_DAC0 ((DAC_Type *) HPM_DAC0_BASE)
/* DAC1 base address */
#define HPM_DAC1_BASE (0xF0124000UL)
/* DAC1 base pointer */
#define HPM_DAC1 ((DAC_Type *) HPM_DAC1_BASE)

#include "hpm_acmp_regs.h"
/* Address of ACMP instances */
/* ACMP0 base address */
#define HPM_ACMP0_BASE (0xF0130000UL)
/* ACMP0 base pointer */
#define HPM_ACMP0 ((ACMP_Type *) HPM_ACMP0_BASE)
/* ACMP1 base address */
#define HPM_ACMP1_BASE (0xF0134000UL)
/* ACMP1 base pointer */
#define HPM_ACMP1 ((ACMP_Type *) HPM_ACMP1_BASE)
/* ACMP2 base address */
#define HPM_ACMP2_BASE (0xF0138000UL)
/* ACMP2 base pointer */
#define HPM_ACMP2 ((ACMP_Type *) HPM_ACMP2_BASE)
/* ACMP3 base address */
#define HPM_ACMP3_BASE (0xF013C000UL)
/* ACMP3 base pointer */
#define HPM_ACMP3 ((ACMP_Type *) HPM_ACMP3_BASE)

#include "hpm_mcan_regs.h"
/* Address of MCAN instances */
/* MCAN0 base address */
#define HPM_MCAN0_BASE (0xF0300000UL)
/* MCAN0 base pointer */
#define HPM_MCAN0 ((MCAN_Type *) HPM_MCAN0_BASE)
/* MCAN1 base address */
#define HPM_MCAN1_BASE (0xF0304000UL)
/* MCAN1 base pointer */
#define HPM_MCAN1 ((MCAN_Type *) HPM_MCAN1_BASE)

#include "hpm_ptpc_regs.h"
/* Address of PTPC instances */
/* PTPC base address */
#define HPM_PTPC_BASE (0xF037C000UL)
/* PTPC base pointer */
#define HPM_PTPC ((PTPC_Type *) HPM_PTPC_BASE)

#include "hpm_qeiv2_regs.h"
/* Address of QEIV2 instances */
/* QEI0 base address */
#define HPM_QEI0_BASE (0xF0400000UL)
/* QEI0 base pointer */
#define HPM_QEI0 ((QEIV2_Type *) HPM_QEI0_BASE)
/* QEI1 base address */
#define HPM_QEI1_BASE (0xF0404000UL)
/* QEI1 base pointer */
#define HPM_QEI1 ((QEIV2_Type *) HPM_QEI1_BASE)

#include "hpm_qeov2_regs.h"
/* Address of QEOV2 instances */
/* QEO0 base address */
#define HPM_QEO0_BASE (0xF0410000UL)
/* QEO0 base pointer */
#define HPM_QEO0 ((QEOV2_Type *) HPM_QEO0_BASE)
/* QEO1 base address */
#define HPM_QEO1_BASE (0xF0414000UL)
/* QEO1 base pointer */
#define HPM_QEO1 ((QEOV2_Type *) HPM_QEO1_BASE)

#include "hpm_pwmv2_regs.h"
/* Address of PWMV2 instances */
/* PWM0 base address */
#define HPM_PWM0_BASE (0xF0420000UL)
/* PWM0 base pointer */
#define HPM_PWM0 ((PWMV2_Type *) HPM_PWM0_BASE)
/* PWM1 base address */
#define HPM_PWM1_BASE (0xF0424000UL)
/* PWM1 base pointer */
#define HPM_PWM1 ((PWMV2_Type *) HPM_PWM1_BASE)
/* PWM2 base address */
#define HPM_PWM2_BASE (0xF0428000UL)
/* PWM2 base pointer */
#define HPM_PWM2 ((PWMV2_Type *) HPM_PWM2_BASE)

#include "hpm_rdc_regs.h"
/* Address of RDC instances */
/* RDC0 base address */
#define HPM_RDC0_BASE (0xF0440000UL)
/* RDC0 base pointer */
#define HPM_RDC0 ((RDC_Type *) HPM_RDC0_BASE)

#include "hpm_sdm_regs.h"
/* Address of SDM instances */
/* SDM0 base address */
#define HPM_SDM0_BASE (0xF0450000UL)
/* SDM0 base pointer */
#define HPM_SDM0 ((SDM_Type *) HPM_SDM0_BASE)

#include "hpm_plb_regs.h"
/* Address of PLB instances */
/* PLB base address */
#define HPM_PLB_BASE (0xF0460000UL)
/* PLB base pointer */
#define HPM_PLB ((PLB_Type *) HPM_PLB_BASE)

#include "hpm_synt_regs.h"
/* Address of SYNT instances */
/* SYNT base address */
#define HPM_SYNT_BASE (0xF0464000UL)
/* SYNT base pointer */
#define HPM_SYNT ((SYNT_Type *) HPM_SYNT_BASE)

#include "hpm_sei_regs.h"
/* Address of SEI instances */
/* SEI base address */
#define HPM_SEI_BASE (0xF0470000UL)
/* SEI base pointer */
#define HPM_SEI ((SEI_Type *) HPM_SEI_BASE)

#include "hpm_trgm_regs.h"
/* Address of TRGM instances */
/* TRGM0 base address */
#define HPM_TRGM0_BASE (0xF047C000UL)
/* TRGM0 base pointer */
#define HPM_TRGM0 ((TRGM_Type *) HPM_TRGM0_BASE)

#include "hpm_mtgv2_regs.h"
/* Address of MTGV2 instances */
/* MTG0 base address */
#define HPM_MTG0_BASE (0xF0490000UL)
/* MTG0 base pointer */
#define HPM_MTG0 ((MTGV2_Type *) HPM_MTG0_BASE)

#include "hpm_vsc_regs.h"
/* Address of VSC instances */
/* VSC0 base address */
#define HPM_VSC0_BASE (0xF04A0000UL)
/* VSC0 base pointer */
#define HPM_VSC0 ((VSC_Type *) HPM_VSC0_BASE)

#include "hpm_clc_regs.h"
/* Address of CLC instances */
/* CLC0 base address */
#define HPM_CLC0_BASE (0xF04B0000UL)
/* CLC0 base pointer */
#define HPM_CLC0 ((CLC_Type *) HPM_CLC0_BASE)

#include "hpm_usb_regs.h"
/* Address of USB instances */
/* USB0 base address */
#define HPM_USB0_BASE (0xF1420000UL)
/* USB0 base pointer */
#define HPM_USB0 ((USB_Type *) HPM_USB0_BASE)

#include "hpm_sdp_regs.h"
/* Address of SDP instances */
/* SDP base address */
#define HPM_SDP_BASE (0xF3140000UL)
/* SDP base pointer */
#define HPM_SDP ((SDP_Type *) HPM_SDP_BASE)

#include "hpm_psec_regs.h"
/* Address of PSEC instances */
/* PSEC base address */
#define HPM_PSEC_BASE (0xF3144000UL)
/* PSEC base pointer */
#define HPM_PSEC ((PSEC_Type *) HPM_PSEC_BASE)

#include "hpm_pmon_regs.h"
/* Address of PMON instances */
/* PMON base address */
#define HPM_PMON_BASE (0xF3148000UL)
/* PMON base pointer */
#define HPM_PMON ((PMON_Type *) HPM_PMON_BASE)

#include "hpm_rng_regs.h"
/* Address of RNG instances */
/* RNG base address */
#define HPM_RNG_BASE (0xF314C000UL)
/* RNG base pointer */
#define HPM_RNG ((RNG_Type *) HPM_RNG_BASE)

#include "hpm_keym_regs.h"
/* Address of KEYM instances */
/* KEYM base address */
#define HPM_KEYM_BASE (0xF3154000UL)
/* KEYM base pointer */
#define HPM_KEYM ((KEYM_Type *) HPM_KEYM_BASE)

#include "hpm_otp_regs.h"
/* Address of OTP instances */
/* OTP base address */
#define HPM_OTP_BASE (0xF3158000UL)
/* OTP base pointer */
#define HPM_OTP ((OTP_Type *) HPM_OTP_BASE)

#include "hpm_sysctl_regs.h"
/* Address of SYSCTL instances */
/* SYSCTL base address */
#define HPM_SYSCTL_BASE (0xF4000000UL)
/* SYSCTL base pointer */
#define HPM_SYSCTL ((SYSCTL_Type *) HPM_SYSCTL_BASE)

#include "hpm_ioc_regs.h"
/* Address of IOC instances */
/* IOC base address */
#define HPM_IOC_BASE (0xF4040000UL)
/* IOC base pointer */
#define HPM_IOC ((IOC_Type *) HPM_IOC_BASE)
/* PIOC base address */
#define HPM_PIOC_BASE (0xF4118000UL)
/* PIOC base pointer */
#define HPM_PIOC ((IOC_Type *) HPM_PIOC_BASE)

#include "hpm_pllctlv2_regs.h"
/* Address of PLLCTLV2 instances */
/* PLLCTLV2 base address */
#define HPM_PLLCTLV2_BASE (0xF40C0000UL)
/* PLLCTLV2 base pointer */
#define HPM_PLLCTLV2 ((PLLCTLV2_Type *) HPM_PLLCTLV2_BASE)

#include "hpm_ppor_regs.h"
/* Address of PPOR instances */
/* PPOR base address */
#define HPM_PPOR_BASE (0xF4100000UL)
/* PPOR base pointer */
#define HPM_PPOR ((PPOR_Type *) HPM_PPOR_BASE)

#include "hpm_pcfg_regs.h"
/* Address of PCFG instances */
/* PCFG base address */
#define HPM_PCFG_BASE (0xF4104000UL)
/* PCFG base pointer */
#define HPM_PCFG ((PCFG_Type *) HPM_PCFG_BASE)

#include "hpm_pdgo_regs.h"
/* Address of PDGO instances */
/* PDGO base address */
#define HPM_PDGO_BASE (0xF4134000UL)
/* PDGO base pointer */
#define HPM_PDGO ((PDGO_Type *) HPM_PDGO_BASE)

#include "hpm_pgpr_regs.h"
/* Address of PGPR instances */
/* PGPR0 base address */
#define HPM_PGPR0_BASE (0xF4138000UL)
/* PGPR0 base pointer */
#define HPM_PGPR0 ((PGPR_Type *) HPM_PGPR0_BASE)
/* PGPR1 base address */
#define HPM_PGPR1_BASE (0xF413C000UL)
/* PGPR1 base pointer */
#define HPM_PGPR1 ((PGPR_Type *) HPM_PGPR1_BASE)


#include "riscv/riscv_core.h"
#include "hpm_csr_regs.h"
#include "hpm_interrupt.h"
#include "hpm_misc.h"
#include "hpm_otp_table.h"
#include "hpm_dmamux_src.h"
#include "hpm_trgmmux_src.h"
#include "hpm_iomux.h"
#include "hpm_pmic_iomux.h"
#endif /* HPM_SOC_H */
