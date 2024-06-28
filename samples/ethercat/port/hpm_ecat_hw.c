
/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "board.h"
#include "hpm_ecat_hw.h"
#include "hpm_gptmr_drv.h"
#include "hpm_ppor_drv.h"
#include "hpm_sysctl_drv.h"

MEM_ADDR ESCMEM * pEsc;
static UINT32 ecat_time_ms = 0;

void ecat_pdi_isr(void);
void ecat_sync0_isr(void);
void ecat_sync1_isr(void);
void ecat_time_ms_isr(void);
void ecat_timer_ms_enable(void);

/* ecat hwardware init */
hpm_stat_t ecat_hardware_init(ESC_Type *esc_ptr)
{
    hpm_stat_t stat;

    esc_eeprom_clock_config_t esc_config;
    esc_config.core_clock_en = true;
    esc_config.phy_refclk_en = true;

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
    stat = ecat_flash_eeprom_init();
    if (stat != status_success) {
        printf("Flash simulation EEPROM initialization failed.\n");
        return stat;
    }

    esc_config.eeprom_emulation = true;
    esc_config.eeprom_size_over_16kbit = false; /* eeprom emulation not use this */
    esc_config_eeprom_and_clock(esc_ptr, &esc_config);

    /* eeprom reload */
    ecat_eeprom_emulation_reload_response();

#else

    esc_config.eeprom_emulation = false;
    /* esc_config.eeprom_size_over_16kbit = true; */
    esc_config.eeprom_size_over_16kbit = (ECAT_EEPROM_SIZE_BYTE > 256) ? true : false; /* single eeprom usage */
    esc_config_eeprom_and_clock(esc_ptr, &esc_config);
#endif


    /* Wait Loading ESC EEPROM and check */
    stat = esc_check_eeprom_loading(esc_ptr);
    if (stat != status_success) {
        if (stat == status_esc_eeprom_checksum_error) {
            printf("EEPROM loading with checksum error.\n");
            printf("EtherCAT communication is possible even if the EEPROM is blank(checksum error), but PDI not operational, please update eeprom context.\n");
            /* not return */
        } else {
            printf("EEPROM loading failed, please check EEPROM setting.\n");
            return status_fail;
        }
    } else {
        printf("EEPROM loading successful, no checksum error.\n");
    }

    /* PHY reset */
    ecat_phy_reset();

    /* PHY LED mode configuration */
    stat = ecat_phy_config(esc_ptr);
    if (stat != status_success) {
        printf("PHY configuration failed.\n");
        return stat;
    }

    /* assign ESC_CTRLx to specific function, check with hardware connection */
    esc_config_ctrl_signal_function(esc_ptr, BOARD_ECAT_NMII_LINK0_CTRL_INDEX, esc_ctrl_signal_func_alt_nmii_link0, true); /* NMII_LINK0 */
#if defined(BOARD_ECAT_SUPPORT_PORT1) && BOARD_ECAT_SUPPORT_PORT1
    esc_config_ctrl_signal_function(esc_ptr, BOARD_ECAT_NMII_LINK1_CTRL_INDEX, esc_ctrl_signal_func_alt_nmii_link1, true); /* NMII_LINK1 */
#endif
#if defined(BOARD_ECAT_SUPPORT_PORT2) && BOARD_ECAT_SUPPORT_PORT2
    esc_config_ctrl_signal_function(esc_ptr, BOARD_ECAT_NMII_LINK2_CTRL_INDEX, esc_ctrl_signal_func_alt_nmii_link2, false); /* NMII_LINK2 */
#endif
    esc_config_nmii_link_source(esc_ptr, true, BOARD_ECAT_SUPPORT_PORT1, BOARD_ECAT_SUPPORT_PORT2); /* configure nmii_link source */

    esc_config_ctrl_signal_function(esc_ptr, BOARD_ECAT_LED_RUN_CTRL_INDEX, esc_ctrl_signal_func_alt_led_run, false);   /* LED_RUN */
    esc_config_ctrl_signal_function(esc_ptr, BOARD_ECAT_LED_ERROR_CTRL_INDEX, esc_ctrl_signal_func_alt_led_err, false); /* LED_ERROR */

    if (status_success == stat) {
        /* Open Timer driver */
        ecat_timer_ms_enable();

        /* Initialize the EtherCAT Slave Interface of the SSC */
        HW_Init();

        /* Enable EtherCAT interrupts */
        ENABLE_ESC_INT();
    } else {
        /* Close operation */
    }

    return stat;
}


/* enable gptmr 1ms tick for ecat */
void ecat_timer_ms_enable(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(ECAT_TIMER_GPTMR, &config);
    gptmr_freq = clock_get_frequency(ECAT_TIMER_GPTMR_CLK);
    config.reload = gptmr_freq / 1000; /* 1ms */
    gptmr_channel_config(ECAT_TIMER_GPTMR, ECAT_TIMER_GPTMR_CH, &config, false);
    gptmr_start_counter(ECAT_TIMER_GPTMR, ECAT_TIMER_GPTMR_CH);

    gptmr_enable_irq(ECAT_TIMER_GPTMR, GPTMR_CH_RLD_IRQ_MASK(ECAT_TIMER_GPTMR_CH));
    intc_m_enable_irq_with_priority(ECAT_TIMER_GPTRM_IRQ, 1);
}

/* gptmr irq handler */
void ecat_time_ms_isr(void)
{
    if (gptmr_check_status(HPM_GPTMR0, GPTMR_CH_RLD_STAT_MASK(ECAT_TIMER_GPTMR_CH))) {
        gptmr_clear_status(HPM_GPTMR0, GPTMR_CH_RLD_STAT_MASK(ECAT_TIMER_GPTMR_CH));
        ecat_time_ms += ECAT_TIMER_INC_P_MS;
#if defined(ECAT_TIMER_INT) && ECAT_TIMER_INT
        ECAT_CheckTimer();
#endif
    }
}
SDK_DECLARE_EXT_ISR_M(ECAT_TIMER_GPTRM_IRQ, ecat_time_ms_isr);

/* ECAT PDI IRQ handler */
void ecat_pdi_isr(void)
{
    /* call handler in SSC */
    PDI_Isr();

    /* callback */
}
SDK_DECLARE_EXT_ISR_M(IRQn_ESC, ecat_pdi_isr)

/* ECAT SYNC0 IRQ handler */
void ecat_sync0_isr(void)
{
    UINT8 SyncState = 0;
    HW_EscReadByte(SyncState, 0x098E); /* Sync0 acknowledge action */
    (void) SyncState;

    /* call handler in SSC */
    Sync0_Isr();

    /* callback */
}
SDK_DECLARE_EXT_ISR_M(IRQn_ESC_SYNC0, ecat_sync0_isr)

/* ECAT SYNC0 IRQ handler */
void ecat_sync1_isr(void)
{
    UINT8 SyncState = 0;
    HW_EscReadByte(SyncState, 0x098F); /* Sync1 acknowledge action */
    (void) SyncState;

    /* call handler in SSC */
    Sync1_Isr();

    /* callback */
}
SDK_DECLARE_EXT_ISR_M(IRQn_ESC_SYNC1, ecat_sync1_isr)

/* ECAT reset handler, please do necessary protection before resetting!!!! */
void ecat_reset_isr(void)
{
    /* SSC ESC reset Interrupt handler */
#if defined(ECAT_RESET_ESC_PERIPHERAL) && ECAT_RESET_ESC_PERIPHERAL
    /* keep phy in reset before reset esc peripheral */
    ecat_phy_keep_reset();
    /* reset OTN module by sysctl */
    sysctl_resource_target_set_mode(HPM_SYSCTL, sysctl_resource_rst_otn, sysctl_resource_mode_force_off);
    while (sysctl_resource_target_is_busy(HPM_SYSCTL, sysctl_resource_rst_otn)) {
    }
    sysctl_resource_target_set_mode(HPM_SYSCTL, sysctl_resource_rst_otn, sysctl_resource_mode_force_on);
    while (sysctl_resource_target_is_busy(HPM_SYSCTL, sysctl_resource_rst_otn)) {
    }

    esc_hardware_init(HPM_ESC); /* reconfig ESC peripheral */
#elif defined(ECAT_RESET_ESC_WITH_MCU) && ECAT_RESET_ESC_WITH_MCU
    /* keep phy in reset before reset esc peripheral */
    ecat_phy_keep_reset();
    ppor_sw_reset(HPM_PPOR, 2); /* software reset */
#else
    #error please specify ECAT reset type
#endif
}
SDK_DECLARE_EXT_ISR_M(IRQn_ESC_RESET, ecat_reset_isr)


UINT32 HW_GetTimer(void)
{
    return ecat_time_ms;
}

void HW_ClearTimer(void)
{
    ecat_time_ms = 0;
}

UINT16 HW_Init(void)
{
    /* set esc register base address */
    pEsc = (MEM_ADDR ESCMEM *) HPM_ESC_BASE;

    return 0;
}

void HW_Release(void)
{
}

/* if ESC_SUPPORT_ECAT_LED = 1, no need to implement this function */
void HW_SetLed(BOOL RunLed, BOOL ErrLed)
{
    UINT8 runled, errled;

    if (RunLed) {
        runled = ESC_LED_ON;
    } else {
        runled = ESC_LED_OFF;
    }
    HW_EscWriteByte(runled, ESC_REG_RUN_LED_OVERRIDE);

    if (ErrLed) {
        errled = ESC_LED_ON;
    } else {
        errled = ESC_LED_OFF;
    }

    HW_EscWriteByte(errled, ESC_REG_ERROR_LED_OVERRIDE);
}

void DISABLE_ESC_INT(void)
{
    HPM_ESC->GPR_CFG1 &= ~(ESC_GPR_CFG1_SYNC0_IRQ_EN_MASK | ESC_GPR_CFG1_SYNC1_IRQ_EN_MASK | ESC_GPR_CFG1_RSTO_IRQ_EN_MASK);
    intc_m_disable_irq(IRQn_ESC_SYNC0);
    intc_m_disable_irq(IRQn_ESC_SYNC1);
    intc_m_disable_irq(IRQn_ESC);
    intc_m_disable_irq(IRQn_ESC_RESET);
}


void ENABLE_ESC_INT(void)
{
    HPM_ESC->GPR_CFG1 |= ESC_GPR_CFG1_SYNC0_IRQ_EN_MASK | ESC_GPR_CFG1_SYNC1_IRQ_EN_MASK | ESC_GPR_CFG1_RSTO_IRQ_EN_MASK;

    intc_m_enable_irq_with_priority(IRQn_ESC_SYNC0, 2);
    intc_m_enable_irq_with_priority(IRQn_ESC_SYNC1, 2);
    intc_m_enable_irq_with_priority(IRQn_ESC, 1);
    intc_m_enable_irq_with_priority(IRQn_ESC_RESET, 1);
}

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
ATTR_RAMFUNC
UINT16 ecat_eeprom_emulation_read(UINT32 wordaddr)
{
    if ((EepromReadSize == 4) || (EepromReadSize == 8)) {
        hpm_stat_t stat;
        uint16_t data[4] = {0};
        uint64_t data_total = 0;
        for (uint8_t i = 0; i < (EepromReadSize / EEPROM_WRITE_SIZE); i++) {
            stat = ecat_flash_eeprom_read(wordaddr + i, (uint8_t *)&data[i]);
            if (status_success != stat) {
                return ESC_EEPROM_EMULATION_ACK_ERROR;
            }
            data_total = data_total + (data[i] << i * (EEPROM_WRITE_SIZE * 8));
        }
        esc_write_eeprom_data(HPM_ESC, data_total); /* write to ESC register */
    } else {
        return ESC_EEPROM_EMULATION_ACK_ERROR;
    }

    return ESC_EEPROM_EMULATION_SUCCESS;
}

ATTR_RAMFUNC
UINT16 ecat_eeprom_emulation_write(UINT32 wordaddr)
{
    assert(EEPROM_WRITE_SIZE == 0x2);
    hpm_stat_t stat;
    uint16_t data, data_stored;
    data = (uint16_t)(esc_read_eeprom_data(HPM_ESC) & 0xFFFF); /* read from ESC register */
    if ((status_success != ecat_flash_eeprom_read(wordaddr, (uint8_t *)&data_stored)) || (data != data_stored)) {
        stat = ecat_flash_eeprom_write(wordaddr, (uint8_t *)&data);
        if (status_success != stat) {
            return ESC_EEPROM_EMULATION_ACK_ERROR;
        }
    }
    return ESC_EEPROM_EMULATION_SUCCESS;
}

ATTR_RAMFUNC
static uint8_t calc_eeprom_checksum(const uint8_t *data, size_t size)
{
    uint8_t remainder = 0xFF; /* Initial remainder */
    uint8_t polynomial = 0x07; /* CRC-8 polynomial x^8 + x^2 + x + 1 */

    for (size_t i = 0; i < size; i++) {
        remainder ^= data[i]; /* XOR with byte */

        /* Perform polynomial division */
        for (int j = 0; j < 8; j++) {
            if (remainder & 0x80) { /* If high bit is set */
                remainder = (remainder << 1) ^ polynomial; /* XOR with polynomial */
            } else {
                remainder = (remainder << 1); /* Shift left */
            }
        }
    }

    return remainder;
}

ATTR_RAMFUNC
UINT16 ecat_eeprom_emulation_reload(void)
{
    hpm_stat_t stat;
    uint32_t reload_data = 0;
    uint16_t config_data[8] = {0xff};
    for (uint8_t i = 0; i < 8; i++) {
        stat = ecat_flash_eeprom_read(i, (uint8_t *)&config_data[i]);
        if (status_success != stat) {
            break;
        }
    }

    if (status_success != stat) {
        return ESC_EEPROM_EMULATION_ACK_ERROR; /* ack error */
    }

    uint8_t checksum = calc_eeprom_checksum((uint8_t *)config_data, 14); /* word0 - word6 */
    if (checksum != config_data[7]) {
        return ESC_EEPROM_EMULATION_CHECKSUM_ERROR; /* checksum error */
    }

    /* build the value of eeprom emulation data */
    reload_data += config_data[4];                                    /* bit[15:0] */
    reload_data += ESC_ESC_CFG_ELDAP_GET(config_data[0] >> 8) << 16;  /* bit[16] */
    reload_data += ESC_ESC_CFG_ELP0_GET(config_data[0] >> 8) << 17;   /* bit[17] */
    reload_data += ESC_ESC_CFG_ELP1_GET(config_data[0] >> 8) << 18;   /* bit[18] */
    reload_data += ESC_ESC_CFG_ELP2_GET(config_data[0] >> 8) << 19;   /* bit[19] */
    reload_data += ESC_ESC_CFG_ELP3_GET(config_data[0] >> 8) << 20;   /* bit[20] */
    esc_write_eeprom_data(HPM_ESC, reload_data); /* write to ESC register */

    return ESC_EEPROM_EMULATION_SUCCESS;
}

ATTR_RAMFUNC
void ecat_eeprom_emulation_reload_response(void)
{
    if ((HPM_ESC->EEPROM_CTRL_STAT & ESC_EEPROM_CTRL_STAT_EE_EMU_MASK) == ESC_EEPROM_CTRL_STAT_EE_EMU_MASK) {
        if ((HPM_ESC->EEPROM_CTRL_STAT & ESC_EEPROM_CTRL_STAT_BUSY_MASK) == ESC_EEPROM_CTRL_STAT_BUSY_MASK) {
            uint16_t stat;
            uint8_t cmd = esc_get_eeprom_cmd(HPM_ESC);
            if (cmd == esc_eeprom_reload_cmd) {
                stat = ecat_eeprom_emulation_reload();
                if (stat == ESC_EEPROM_EMULATION_SUCCESS) {
                    esc_eeprom_emulation_ack(HPM_ESC, cmd, false, false);
                } else if (stat == ESC_EEPROM_EMULATION_CHECKSUM_ERROR) {
                    esc_eeprom_emulation_ack(HPM_ESC, cmd, false, true); /* checksum error */
                } else {
                    esc_eeprom_emulation_ack(HPM_ESC, cmd, true, false); /* ack error */
                }
            }
        }
    }
}

/* no need to implement if the written data is stored directly
during the EEPROM Write commands(pAPPL_EEPROM_Write) to the permanent memory  */
void ecat_eeprom_emulation_store(void)
{
}

/* In case that pAPPL_EEPROM_Reload is implemented the function
“HW_EepromReload()” is not used.*/
UINT16 HW_EepromReload(void)
{
    return ESC_EEPROM_EMULATION_SUCCESS;
}
#endif /* ESC_EEPROM_EMULATION */

