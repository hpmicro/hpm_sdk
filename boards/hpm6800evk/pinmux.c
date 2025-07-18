/*
 * Copyright (c) 2023-2024 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Note:
 *   PY and PZ IOs: if any SOC pin function needs to be routed to these IOs,
 *  besides of IOC, PIOC/BIOC needs to be configured SOC_GPIO_X_xx, so that
 *  expected SoC function can be enabled on these IOs.
 *
 */
#include "board.h"

void init_uart_pins(UART_Type *ptr)
{
    if (ptr == HPM_UART0) {
        HPM_IOC->PAD[IOC_PAD_PA00].FUNC_CTL = IOC_PA00_FUNC_CTL_UART0_TXD;
        HPM_IOC->PAD[IOC_PAD_PA01].FUNC_CTL = IOC_PA01_FUNC_CTL_UART0_RXD;
    } else if (ptr == HPM_UART3) {
        HPM_IOC->PAD[IOC_PAD_PE14].FUNC_CTL = IOC_PE14_FUNC_CTL_UART3_RXD;
        HPM_IOC->PAD[IOC_PAD_PE15].FUNC_CTL = IOC_PE15_FUNC_CTL_UART3_TXD;
    } else if (ptr == HPM_PUART) {
        HPM_PIOC->PAD[IOC_PAD_PY00].FUNC_CTL = PIOC_PY00_FUNC_CTL_PURT_TXD;
        HPM_PIOC->PAD[IOC_PAD_PY01].FUNC_CTL = PIOC_PY01_FUNC_CTL_PURT_RXD;
    } else {
        ;
    }
}

/* for uart_lin case, need to configure pin as gpio to sent break signal */
void init_uart_pin_as_gpio(UART_Type *ptr)
{
    /* pull-up */
    uint32_t pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);

    if (ptr == HPM_UART3) {
        HPM_IOC->PAD[IOC_PAD_PE14].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PE15].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PE14].FUNC_CTL = IOC_PE14_FUNC_CTL_GPIO_E_14;
        HPM_IOC->PAD[IOC_PAD_PE15].FUNC_CTL = IOC_PE15_FUNC_CTL_GPIO_E_15;
    }
}

void init_cap_pins(void)
{
    /* CAP_INT */
    HPM_IOC->PAD[IOC_PAD_PY06].PAD_CTL = IOC_PAD_PAD_CTL_PE_MASK;
    HPM_IOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY06_FUNC_CTL_GPIO_Y_06;
    HPM_PIOC->PAD[IOC_PAD_PY06].FUNC_CTL = PIOC_PY06_FUNC_CTL_SOC_PY_06;
    /* CAP_RST */
    HPM_IOC->PAD[IOC_PAD_PY07].PAD_CTL = IOC_PAD_PAD_CTL_PE_MASK;
    HPM_IOC->PAD[IOC_PAD_PY07].FUNC_CTL = IOC_PY07_FUNC_CTL_GPIO_Y_07;
    HPM_PIOC->PAD[IOC_PAD_PY07].FUNC_CTL = PIOC_PY07_FUNC_CTL_SOC_PY_07;
}

void init_i2c_pins(I2C_Type *ptr)
{
    if (ptr == HPM_I2C3) {            /* Audio */
        HPM_IOC->PAD[IOC_PAD_PD28].FUNC_CTL = IOC_PD28_FUNC_CTL_I2C3_SDA
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PD29].FUNC_CTL = IOC_PD29_FUNC_CTL_I2C3_SCL
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PD28].PAD_CTL = IOC_PAD_PAD_CTL_OD_MASK;
        HPM_IOC->PAD[IOC_PAD_PD29].PAD_CTL = IOC_PAD_PAD_CTL_OD_MASK;
    } else if (ptr == HPM_I2C1) {         /* Storage */
        HPM_IOC->PAD[IOC_PAD_PE12].FUNC_CTL = IOC_PE12_FUNC_CTL_I2C1_SDA
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PE13].FUNC_CTL = IOC_PE13_FUNC_CTL_I2C1_SCL
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PE12].PAD_CTL = IOC_PAD_PAD_CTL_OD_MASK;
        HPM_IOC->PAD[IOC_PAD_PE13].PAD_CTL = IOC_PAD_PAD_CTL_OD_MASK;
    } else if (ptr == HPM_I2C0) {         /* Touch Panel/ Camera */
        HPM_IOC->PAD[IOC_PAD_PF09].FUNC_CTL = IOC_PF09_FUNC_CTL_I2C0_SDA
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PF08].FUNC_CTL = IOC_PF08_FUNC_CTL_I2C0_SCL
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PF09].PAD_CTL = IOC_PAD_PAD_CTL_OD_MASK;
        HPM_IOC->PAD[IOC_PAD_PF08].PAD_CTL = IOC_PAD_PAD_CTL_OD_MASK;
    } else {
        ;
    }
}

void init_cam_pins(void)
{
    /* configure rst pin function */
    HPM_IOC->PAD[IOC_PAD_PA22].FUNC_CTL = IOC_PA22_FUNC_CTL_GPIO_A_22;
    /* configure pwdn pin function */
    HPM_PIOC->PAD[IOC_PAD_PA21].FUNC_CTL = IOC_PA21_FUNC_CTL_GPIO_A_21;

    HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PA29_FUNC_CTL_CAM0_XCLK;
    HPM_IOC->PAD[IOC_PAD_PA26].FUNC_CTL = IOC_PA26_FUNC_CTL_CAM0_PIXCLK;
    HPM_IOC->PAD[IOC_PAD_PB21].FUNC_CTL = IOC_PB21_FUNC_CTL_CAM0_VSYNC;
    HPM_IOC->PAD[IOC_PAD_PB20].FUNC_CTL = IOC_PB20_FUNC_CTL_CAM0_HSYNC;
    HPM_IOC->PAD[IOC_PAD_PA27].FUNC_CTL = IOC_PA27_FUNC_CTL_CAM0_D_2;
    HPM_IOC->PAD[IOC_PAD_PA25].FUNC_CTL = IOC_PA25_FUNC_CTL_CAM0_D_3;
    HPM_IOC->PAD[IOC_PAD_PA23].FUNC_CTL = IOC_PA23_FUNC_CTL_CAM0_D_4;
    HPM_IOC->PAD[IOC_PAD_PA24].FUNC_CTL = IOC_PA24_FUNC_CTL_CAM0_D_5;
    HPM_IOC->PAD[IOC_PAD_PB16].FUNC_CTL = IOC_PB16_FUNC_CTL_CAM0_D_6;
    HPM_IOC->PAD[IOC_PAD_PB17].FUNC_CTL = IOC_PB17_FUNC_CTL_CAM0_D_7;
    HPM_IOC->PAD[IOC_PAD_PB19].FUNC_CTL = IOC_PB19_FUNC_CTL_CAM0_D_8;
    HPM_IOC->PAD[IOC_PAD_PB18].FUNC_CTL = IOC_PB18_FUNC_CTL_CAM0_D_9;
}

void init_cam_mipi_csi_pins(void)
{
    /* configure rst pin function */
    HPM_IOC->PAD[IOC_PAD_PB00].FUNC_CTL = IOC_PB00_FUNC_CTL_GPIO_B_00;
    HPM_IOC->PAD[IOC_PAD_PB00].PAD_CTL = IOC_PAD_PAD_CTL_OD_MASK;
}

void init_sdm_pins(void)
{

}

void init_gpio_pins(void)
{
    /* configure pad setting: pull enable and pull up, schmitt trigger enable */
    /* enable schmitt trigger to eliminate jitter of pin used as button */
    HPM_IOC->PAD[IOC_PAD_PF06].FUNC_CTL = IOC_PF06_FUNC_CTL_GPIO_F_06;
    HPM_IOC->PAD[IOC_PAD_PF06].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_MASK | IOC_PAD_PAD_CTL_PRS_SET(3);

    HPM_IOC->PAD[IOC_PAD_PF07].FUNC_CTL = IOC_PF07_FUNC_CTL_GPIO_F_07;
    HPM_IOC->PAD[IOC_PAD_PF07].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_MASK | IOC_PAD_PAD_CTL_PRS_SET(3);
}

void init_spi_pins(SPI_Type *ptr)
{
    if (ptr == HPM_SPI3) {
        HPM_IOC->PAD[IOC_PAD_PE05].FUNC_CTL = IOC_PE05_FUNC_CTL_SPI3_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
        HPM_IOC->PAD[IOC_PAD_PE06].FUNC_CTL = IOC_PE06_FUNC_CTL_SPI3_MISO;
        HPM_IOC->PAD[IOC_PAD_PE07].FUNC_CTL = IOC_PE07_FUNC_CTL_SPI3_MOSI;
        HPM_IOC->PAD[IOC_PAD_PE04].FUNC_CTL = IOC_PE04_FUNC_CTL_SPI3_CS_0;

        /* set max frequency slew rate(200M) */
        HPM_IOC->PAD[IOC_PAD_PE05].PAD_CTL = IOC_PAD_PAD_CTL_SR_MASK | IOC_PAD_PAD_CTL_SPD_SET(3) | IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_PRS_SET(1);
        HPM_IOC->PAD[IOC_PAD_PE06].PAD_CTL = IOC_PAD_PAD_CTL_SR_MASK | IOC_PAD_PAD_CTL_SPD_SET(3);
        HPM_IOC->PAD[IOC_PAD_PE07].PAD_CTL = IOC_PAD_PAD_CTL_SR_MASK | IOC_PAD_PAD_CTL_SPD_SET(3);
        HPM_IOC->PAD[IOC_PAD_PE04].PAD_CTL = IOC_PAD_PAD_CTL_SR_MASK | IOC_PAD_PAD_CTL_SPD_SET(3);
    }
}

void init_spi_pins_with_gpio_as_cs(SPI_Type *ptr)
{
    if (ptr == HPM_SPI3) {
        HPM_IOC->PAD[IOC_PAD_PE05].FUNC_CTL = IOC_PE05_FUNC_CTL_SPI3_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
        HPM_IOC->PAD[IOC_PAD_PE06].FUNC_CTL = IOC_PE06_FUNC_CTL_SPI3_MISO;
        HPM_IOC->PAD[IOC_PAD_PE07].FUNC_CTL = IOC_PE07_FUNC_CTL_SPI3_MOSI;
        HPM_IOC->PAD[IOC_PAD_PE04].FUNC_CTL = IOC_PE04_FUNC_CTL_GPIO_E_04;

        /* set max frequency slew rate(200M) */
        HPM_IOC->PAD[IOC_PAD_PE05].PAD_CTL = IOC_PAD_PAD_CTL_SR_MASK | IOC_PAD_PAD_CTL_SPD_SET(3) | IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_PRS_SET(1);
        HPM_IOC->PAD[IOC_PAD_PE06].PAD_CTL = IOC_PAD_PAD_CTL_SR_MASK | IOC_PAD_PAD_CTL_SPD_SET(3);
        HPM_IOC->PAD[IOC_PAD_PE07].PAD_CTL = IOC_PAD_PAD_CTL_SR_MASK | IOC_PAD_PAD_CTL_SPD_SET(3);
        HPM_IOC->PAD[IOC_PAD_PE04].PAD_CTL = IOC_PAD_PAD_CTL_SR_MASK | IOC_PAD_PAD_CTL_SPD_SET(3);
    }
}

void init_pins(void)
{
#ifdef BOARD_CONSOLE_UART_BASE
    init_uart_pins(BOARD_CONSOLE_UART_BASE);
#endif
}

void init_gptmr_pins(GPTMR_Type *ptr)
{
    if (ptr == HPM_GPTMR2) {
        HPM_IOC->PAD[IOC_PAD_PE22].FUNC_CTL = IOC_PE22_FUNC_CTL_GPTMR2_CAPT_0;
        HPM_IOC->PAD[IOC_PAD_PE23].FUNC_CTL = IOC_PE23_FUNC_CTL_GPTMR2_COMP_0;
        HPM_IOC->PAD[IOC_PAD_PE24].FUNC_CTL = IOC_PE24_FUNC_CTL_GPTMR2_COMP_1;
        HPM_IOC->PAD[IOC_PAD_PE26].FUNC_CTL = IOC_PE26_FUNC_CTL_GPTMR2_COMP_2;
    }
}

void init_butn_pins(void)
{

}

void init_acmp_pins(void)
{

}

void init_sdxc_cmd_pin(SDXC_Type *ptr, bool open_drain, bool is_1v8)
{
    (void) is_1v8;
    /* Pull-up */
    uint32_t pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_PRS_SET(2) | \
                       IOC_PAD_PAD_CTL_SR_SET(1) | IOC_PAD_PAD_CTL_SPD_SET(3) | IOC_PAD_PAD_CTL_DS_SET(6);
    if (ptr == HPM_SDXC0) {
        if (open_drain) {
            pad_ctl |= IOC_PAD_PAD_CTL_OD_MASK;
        }
        HPM_IOC->PAD[IOC_PAD_PC01].FUNC_CTL = IOC_PC01_FUNC_CTL_SDC0_CMD | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
        HPM_IOC->PAD[IOC_PAD_PC01].PAD_CTL = pad_ctl;
    }
    if (ptr == HPM_SDXC1) {
        HPM_IOC->PAD[IOC_PAD_PC13].FUNC_CTL = IOC_PC13_FUNC_CTL_SDC1_CMD | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
        HPM_IOC->PAD[IOC_PAD_PC13].PAD_CTL = pad_ctl;
    }
}


void init_sdxc_ds_pin(SDXC_Type *ptr)
{
#define SDXC_DS_PIN_SETTING (IOC_PAD_PAD_CTL_PE_SET(1) \
                                | IOC_PAD_PAD_CTL_SPD_SET(3) \
                                | IOC_PAD_PAD_CTL_SR_SET(1))
    if (ptr == HPM_SDXC0) {
        HPM_IOC->PAD[IOC_PAD_PC00].FUNC_CTL = IOC_PC00_FUNC_CTL_SDC0_DS;
        HPM_IOC->PAD[IOC_PAD_PC00].PAD_CTL = SDXC_DS_PIN_SETTING;
    }
}

void init_sdxc_pwr_pin(SDXC_Type *ptr, bool as_gpio)
{
    if (ptr == HPM_SDXC1) {
        if (as_gpio) {
            /* SD_PWR */
            HPM_IOC->PAD[IOC_PAD_PD07].FUNC_CTL = IOC_PD07_FUNC_CTL_GPIO_D_07;
            HPM_IOC->PAD[IOC_PAD_PD07].PAD_CTL =
                    IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_DS_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
            HPM_GPIO0->OE[GPIO_OE_GPIOD].SET = 1UL << 7;
        }
    }
}

void init_sdxc_vsel_pin(SDXC_Type *ptr, bool as_gpio)
{
    if (ptr == HPM_SDXC1) {
        if (as_gpio) {
            /* VSEL */
            HPM_IOC->PAD[IOC_PAD_PD12].FUNC_CTL = IOC_PD12_FUNC_CTL_GPIO_D_12;
            HPM_IOC->PAD[IOC_PAD_PD12].PAD_CTL =
                    IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_DS_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
            HPM_GPIO0->OE[GPIO_OE_GPIOD].SET = 1UL << 12;
        }
    }
}

void init_sdxc_cd_pin(SDXC_Type *ptr, bool as_gpio)
{
    if (ptr == HPM_SDXC1) {
        if (as_gpio) {
            /* CDN */
            HPM_IOC->PAD[IOC_PAD_PD05].FUNC_CTL = IOC_PD05_FUNC_CTL_GPIO_D_05;
            HPM_IOC->PAD[IOC_PAD_PD05].PAD_CTL =
                    IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_DS_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
            HPM_GPIO0->OE[GPIO_OE_GPIOD].CLEAR = 1UL << 5;
        }
    }
}

void init_sdxc_clk_data_pins(SDXC_Type *ptr, uint32_t width, bool is_1v8)
{
    (void) is_1v8;
#define SDXC_PIN_SETTING_COMMON (IOC_PAD_PAD_CTL_PE_SET(1) \
                                | IOC_PAD_PAD_CTL_SPD_SET(3) \
                                | IOC_PAD_PAD_CTL_SR_SET(1))
#define SDXC_PIN_SETTING        (IOC_PAD_PAD_CTL_DS_SET(7) \
                                | SDXC_PIN_SETTING_COMMON \
                                | IOC_PAD_PAD_CTL_PS_SET(1) \
                                | IOC_PAD_PAD_CTL_PRS_SET(3))

    uint32_t pad_ctl = SDXC_PIN_SETTING;
    if (ptr == HPM_SDXC0) {
        /*CLK*/
        HPM_IOC->PAD[IOC_PAD_PC02].FUNC_CTL = IOC_PC02_FUNC_CTL_SDC0_CLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
        HPM_IOC->PAD[IOC_PAD_PC02].PAD_CTL = pad_ctl;

        /* DAT0-DATA7 */
        HPM_IOC->PAD[IOC_PAD_PC06].FUNC_CTL = IOC_PC06_FUNC_CTL_SDC0_DATA_0;
        HPM_IOC->PAD[IOC_PAD_PC06].PAD_CTL = pad_ctl;
        if ((width == 4) || (width == 8)) {
            HPM_IOC->PAD[IOC_PAD_PC03].FUNC_CTL = IOC_PC03_FUNC_CTL_SDC0_DATA_1;
            HPM_IOC->PAD[IOC_PAD_PC03].PAD_CTL = pad_ctl;
            HPM_IOC->PAD[IOC_PAD_PC04].FUNC_CTL = IOC_PC04_FUNC_CTL_SDC0_DATA_2;
            HPM_IOC->PAD[IOC_PAD_PC04].PAD_CTL = pad_ctl;
            HPM_IOC->PAD[IOC_PAD_PC05].FUNC_CTL = IOC_PC05_FUNC_CTL_SDC0_DATA_3;
            HPM_IOC->PAD[IOC_PAD_PC05].PAD_CTL = pad_ctl;
        }
        if (width == 8) {
            HPM_IOC->PAD[IOC_PAD_PC08].FUNC_CTL = IOC_PC08_FUNC_CTL_SDC0_DATA_4;
            HPM_IOC->PAD[IOC_PAD_PC08].PAD_CTL = pad_ctl;
            HPM_IOC->PAD[IOC_PAD_PC09].FUNC_CTL = IOC_PC09_FUNC_CTL_SDC0_DATA_5;
            HPM_IOC->PAD[IOC_PAD_PC09].PAD_CTL = pad_ctl;
            HPM_IOC->PAD[IOC_PAD_PC10].FUNC_CTL = IOC_PC10_FUNC_CTL_SDC0_DATA_6;
            HPM_IOC->PAD[IOC_PAD_PC10].PAD_CTL = pad_ctl;
            HPM_IOC->PAD[IOC_PAD_PC11].FUNC_CTL = IOC_PC11_FUNC_CTL_SDC0_DATA_7;
            HPM_IOC->PAD[IOC_PAD_PC11].PAD_CTL = pad_ctl;
        }
    }
    if (ptr == HPM_SDXC1) {
        /*CLK*/
        HPM_IOC->PAD[IOC_PAD_PC16].FUNC_CTL = IOC_PC16_FUNC_CTL_SDC1_CLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
        HPM_IOC->PAD[IOC_PAD_PC16].PAD_CTL = pad_ctl;

        /* DAT0 -DATA3 */
        HPM_IOC->PAD[IOC_PAD_PC17].FUNC_CTL = IOC_PC17_FUNC_CTL_SDC1_DATA_0;
        HPM_IOC->PAD[IOC_PAD_PC17].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PC15].FUNC_CTL = IOC_PC15_FUNC_CTL_SDC1_DATA_1;
        HPM_IOC->PAD[IOC_PAD_PC15].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PC14].FUNC_CTL = IOC_PC14_FUNC_CTL_SDC1_DATA_2;
        HPM_IOC->PAD[IOC_PAD_PC14].PAD_CTL = pad_ctl;
        HPM_IOC->PAD[IOC_PAD_PC12].FUNC_CTL = IOC_PC12_FUNC_CTL_SDC1_DATA_3;
        HPM_IOC->PAD[IOC_PAD_PC12].PAD_CTL = pad_ctl;
    }
}

void init_usb_pins(USB_Type *ptr)
{
    if (ptr == HPM_USB0) {
        /* USB0_ID */
        HPM_IOC->PAD[IOC_PAD_PF04].FUNC_CTL = IOC_PF04_FUNC_CTL_USB0_ID;
        /* USB0_OC */
        HPM_IOC->PAD[IOC_PAD_PF03].FUNC_CTL = IOC_PF03_FUNC_CTL_USB0_OC;
        /* USB0_PWR */
        HPM_IOC->PAD[IOC_PAD_PF00].FUNC_CTL = IOC_PF00_FUNC_CTL_USB0_PWR;
    }
}

void init_can_pins(MCAN_Type *ptr)
{
    if (ptr == HPM_MCAN3) {
        HPM_IOC->PAD[IOC_PAD_PD15].FUNC_CTL = IOC_PD15_FUNC_CTL_MCAN3_TXD;
        HPM_IOC->PAD[IOC_PAD_PD14].FUNC_CTL = IOC_PD14_FUNC_CTL_MCAN3_RXD;
        HPM_IOC->PAD[IOC_PAD_PD13].FUNC_CTL = IOC_PD13_FUNC_CTL_MCAN3_STBY;
    }
}

void init_clk_obs_pins(void)
{
    /* HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PB02_FUNC_CTL_SYSCTL_CLK_OBS_0; */
}

void init_led_pins_as_gpio(void)
{
    HPM_IOC->PAD[IOC_PAD_PF01].FUNC_CTL = IOC_PF01_FUNC_CTL_GPIO_F_01;
    HPM_IOC->PAD[IOC_PAD_PF02].FUNC_CTL = IOC_PF02_FUNC_CTL_GPIO_F_02;
    HPM_IOC->PAD[IOC_PAD_PF05].FUNC_CTL = IOC_PF05_FUNC_CTL_GPIO_F_05;
}

void init_mipi_lvds_tx_phy0_pin(void)
{
    HPM_IOC->PAD[IOC_PAD_PB04].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB05].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB06].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB07].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB08].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB09].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB10].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB11].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB14].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; /*REXT*/
}

void init_mipi_lvds_tx_phy1_pin(void)
{
    HPM_IOC->PAD[IOC_PAD_PB22].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB23].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB24].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB25].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB26].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB27].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB28].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB29].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB30].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB31].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB15].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; /*REXT*/
}

void init_mipi_lvds_rx_phy0_pin(void)
{
    HPM_IOC->PAD[IOC_PAD_PA30].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PA31].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB00].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB01].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB03].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PA28].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; /*REXT*/
}

void init_mipi_lvds_rx_phy1_pin(void)
{
    HPM_IOC->PAD[IOC_PAD_PB16].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB17].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB18].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB19].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB20].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PB21].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; /*REXT*/
}

void init_lcd_mipi_ctl_pins(void)
{
    /* RESET */
    HPM_IOC->PAD[IOC_PAD_PB01].FUNC_CTL = IOC_PB01_FUNC_CTL_GPIO_B_01;
}

void init_lcd_lvds_double_ctl_pins(void)
{
    /* RESET */
    HPM_IOC->PAD[IOC_PAD_PA31].FUNC_CTL = IOC_PA31_FUNC_CTL_GPIO_A_31;
}

void init_lcd_lvds_single_ctl_pins(void)
{
    /* LED-EN */
    HPM_IOC->PAD[IOC_PAD_PA30].FUNC_CTL = IOC_PA30_FUNC_CTL_GPIO_A_30;

    /* PWM */
    HPM_IOC->PAD[IOC_PAD_PA31].FUNC_CTL = IOC_PA31_FUNC_CTL_GPIO_A_31;
}

void init_lcd_rgb_ctl_pins(void)
{
    /* PWM */
    HPM_IOC->PAD[IOC_PAD_PA09].FUNC_CTL = IOC_PA09_FUNC_CTL_GPIO_A_09;
    /* RST */
    HPM_IOC->PAD[IOC_PAD_PA14].FUNC_CTL = IOC_PA14_FUNC_CTL_GPIO_A_14;

    HPM_IOC->PAD[IOC_PAD_PY05].PAD_CTL = IOC_PAD_PAD_CTL_PE_MASK;
    HPM_IOC->PAD[IOC_PAD_PY05].FUNC_CTL = IOC_PY05_FUNC_CTL_GPIO_Y_05;
    HPM_PIOC->PAD[IOC_PAD_PY05].FUNC_CTL = PIOC_PY05_FUNC_CTL_SOC_PY_05;
}

void init_lcd_rgb_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PA10].FUNC_CTL = IOC_PA10_FUNC_CTL_DIS0_G_4;
    HPM_IOC->PAD[IOC_PAD_PA11].FUNC_CTL = IOC_PA11_FUNC_CTL_DIS0_G_3;
    HPM_IOC->PAD[IOC_PAD_PA12].FUNC_CTL = IOC_PA12_FUNC_CTL_DIS0_G_6;
    HPM_IOC->PAD[IOC_PAD_PA13].FUNC_CTL = IOC_PA13_FUNC_CTL_DIS0_G_5;
    HPM_IOC->PAD[IOC_PAD_PA15].FUNC_CTL = IOC_PA15_FUNC_CTL_DIS0_R_3;
    HPM_IOC->PAD[IOC_PAD_PA16].FUNC_CTL = IOC_PA16_FUNC_CTL_DIS0_R_5;
    HPM_IOC->PAD[IOC_PAD_PA17].FUNC_CTL = IOC_PA17_FUNC_CTL_DIS0_R_4;
    HPM_IOC->PAD[IOC_PAD_PA18].FUNC_CTL = IOC_PA18_FUNC_CTL_DIS0_R_7;
    HPM_IOC->PAD[IOC_PAD_PA19].FUNC_CTL = IOC_PA19_FUNC_CTL_DIS0_R_6;
    HPM_IOC->PAD[IOC_PAD_PA20].FUNC_CTL = IOC_PA20_FUNC_CTL_DIS0_G_2;
    HPM_IOC->PAD[IOC_PAD_PA28].FUNC_CTL = IOC_PA28_FUNC_CTL_DIS0_R_0;
    HPM_IOC->PAD[IOC_PAD_PA30].FUNC_CTL = IOC_PA30_FUNC_CTL_DIS0_R_2;
    HPM_IOC->PAD[IOC_PAD_PA31].FUNC_CTL = IOC_PA31_FUNC_CTL_DIS0_R_1;

    HPM_IOC->PAD[IOC_PAD_PB00].FUNC_CTL = IOC_PB00_FUNC_CTL_DIS0_G_1;
    HPM_IOC->PAD[IOC_PAD_PB01].FUNC_CTL = IOC_PB01_FUNC_CTL_DIS0_G_0;
    HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PB02_FUNC_CTL_DIS0_B_1;
    HPM_IOC->PAD[IOC_PAD_PB03].FUNC_CTL = IOC_PB03_FUNC_CTL_DIS0_B_0;
    HPM_IOC->PAD[IOC_PAD_PB04].FUNC_CTL = IOC_PB04_FUNC_CTL_DIS0_B_2;
    HPM_IOC->PAD[IOC_PAD_PB05].FUNC_CTL = IOC_PB05_FUNC_CTL_DIS0_G_7;
    HPM_IOC->PAD[IOC_PAD_PB06].FUNC_CTL = IOC_PB06_FUNC_CTL_DIS0_B_3;
    HPM_IOC->PAD[IOC_PAD_PB07].FUNC_CTL = IOC_PB07_FUNC_CTL_DIS0_B_4;
    HPM_IOC->PAD[IOC_PAD_PB08].FUNC_CTL = IOC_PB08_FUNC_CTL_DIS0_B_6;
    HPM_IOC->PAD[IOC_PAD_PB09].FUNC_CTL = IOC_PB09_FUNC_CTL_DIS0_B_5;
    HPM_IOC->PAD[IOC_PAD_PB10].FUNC_CTL = IOC_PB10_FUNC_CTL_DIS0_EN;
    HPM_IOC->PAD[IOC_PAD_PB11].FUNC_CTL = IOC_PB11_FUNC_CTL_DIS0_B_7;
    HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = IOC_PB12_FUNC_CTL_DIS0_HSYNC;
    HPM_IOC->PAD[IOC_PAD_PB14].FUNC_CTL = IOC_PB14_FUNC_CTL_DIS0_VSYNC;

    HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PB13_FUNC_CTL_DIS0_CLK; /*A.CLK*/
}

void init_i2s_pins(I2S_Type *ptr)
{
    if (ptr == HPM_I2S3) {
        HPM_IOC->PAD[IOC_PAD_PD30].FUNC_CTL = IOC_PD30_FUNC_CTL_I2S3_MCLK;
        HPM_IOC->PAD[IOC_PAD_PD24].FUNC_CTL = IOC_PD24_FUNC_CTL_I2S3_BCLK;
        HPM_IOC->PAD[IOC_PAD_PD25].FUNC_CTL = IOC_PD25_FUNC_CTL_I2S3_FCLK;
        HPM_IOC->PAD[IOC_PAD_PD27].FUNC_CTL = IOC_PD27_FUNC_CTL_I2S3_TXD_2;
        HPM_IOC->PAD[IOC_PAD_PD23].FUNC_CTL = IOC_PD23_FUNC_CTL_I2S3_RXD_2;
    }
}

void init_dao_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PC31].FUNC_CTL = IOC_PC31_FUNC_CTL_DAO_LP;
    HPM_IOC->PAD[IOC_PAD_PC30].FUNC_CTL = IOC_PC30_FUNC_CTL_DAO_LN;
    HPM_IOC->PAD[IOC_PAD_PD01].FUNC_CTL = IOC_PD01_FUNC_CTL_DAO_RP;
    HPM_IOC->PAD[IOC_PAD_PD00].FUNC_CTL = IOC_PD00_FUNC_CTL_DAO_RN;
}

void init_pdm_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PE00].FUNC_CTL = IOC_PE00_FUNC_CTL_PDM0_CLK;
    HPM_IOC->PAD[IOC_PAD_PE01].FUNC_CTL = IOC_PE01_FUNC_CTL_PDM0_D_1;
}

void init_enet_pins(ENET_Type *ptr)
{
    if (ptr == HPM_ENET0) {
        HPM_IOC->PAD[IOC_PAD_PD18].FUNC_CTL = IOC_PD18_FUNC_CTL_GPIO_D_18;

        HPM_IOC->PAD[IOC_PAD_PD02].FUNC_CTL = IOC_PD02_FUNC_CTL_ETH0_MDC;
        HPM_IOC->PAD[IOC_PAD_PD03].FUNC_CTL = IOC_PD03_FUNC_CTL_ETH0_MDIO;

        HPM_IOC->PAD[IOC_PAD_PC29].FUNC_CTL = IOC_PC29_FUNC_CTL_ETH0_RXD_0;
        HPM_IOC->PAD[IOC_PAD_PC28].FUNC_CTL = IOC_PC28_FUNC_CTL_ETH0_RXD_1;
        HPM_IOC->PAD[IOC_PAD_PC24].FUNC_CTL = IOC_PC24_FUNC_CTL_ETH0_RXD_2;
        HPM_IOC->PAD[IOC_PAD_PC25].FUNC_CTL = IOC_PC25_FUNC_CTL_ETH0_RXD_3;
        HPM_IOC->PAD[IOC_PAD_PC26].FUNC_CTL = IOC_PC26_FUNC_CTL_ETH0_RXCK;
        HPM_IOC->PAD[IOC_PAD_PC27].FUNC_CTL = IOC_PC27_FUNC_CTL_ETH0_RXDV;

        HPM_IOC->PAD[IOC_PAD_PC22].FUNC_CTL = IOC_PC22_FUNC_CTL_ETH0_TXD_0;
        HPM_IOC->PAD[IOC_PAD_PC23].FUNC_CTL = IOC_PC23_FUNC_CTL_ETH0_TXD_1;
        HPM_IOC->PAD[IOC_PAD_PC19].FUNC_CTL = IOC_PC19_FUNC_CTL_ETH0_TXD_2;
        HPM_IOC->PAD[IOC_PAD_PC18].FUNC_CTL = IOC_PC18_FUNC_CTL_ETH0_TXD_3;
        HPM_IOC->PAD[IOC_PAD_PC20].FUNC_CTL = IOC_PC20_FUNC_CTL_ETH0_TXCK;
        HPM_IOC->PAD[IOC_PAD_PC21].FUNC_CTL = IOC_PC21_FUNC_CTL_ETH0_TXEN;
    }
}

void init_enet_pps_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PD31].FUNC_CTL = IOC_PD31_FUNC_CTL_ETH0_EVTO_0;
    HPM_IOC->PAD[IOC_PAD_PE10].FUNC_CTL = IOC_PE10_FUNC_CTL_ETH0_EVTO_1;
}

void init_enet_pps_capture_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PE17].FUNC_CTL = IOC_PE17_FUNC_CTL_ETH0_EVTI_0;
    HPM_IOC->PAD[IOC_PAD_PE19].FUNC_CTL = IOC_PE19_FUNC_CTL_ETH0_EVTI_1;
}

void init_adc_pins(void)
{
    /* ADC0.INA12 */
    HPM_IOC->PAD[IOC_PAD_PE24].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
}

void init_tamper_pins(void)
{
    HPM_BIOC->PAD[IOC_PAD_PZ04].FUNC_CTL = BIOC_PZ04_FUNC_CTL_TAMP_PZ_04 | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
    HPM_BIOC->PAD[IOC_PAD_PZ05].FUNC_CTL = BIOC_PZ05_FUNC_CTL_TAMP_PZ_05;
    HPM_BIOC->PAD[IOC_PAD_PZ06].FUNC_CTL = BIOC_PZ06_FUNC_CTL_TAMP_PZ_06;
}

/* for uart_rx_line_status case, need to a gpio pin to sent break signal */
void init_uart_break_signal_pin(void)
{
    HPM_IOC->PAD[IOC_PAD_PE04].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
    HPM_IOC->PAD[IOC_PAD_PE04].FUNC_CTL = IOC_PE04_FUNC_CTL_GPIO_E_04;
}

void init_gptmr_channel_pin(GPTMR_Type *ptr, uint32_t channel, bool as_comp)
{
    if (ptr == HPM_GPTMR2) {
        if (as_comp) {
            switch (channel) {
            case 0:
                HPM_IOC->PAD[IOC_PAD_PE23].FUNC_CTL = IOC_PE23_FUNC_CTL_GPTMR2_COMP_0;
                break;
            case 1:
                HPM_IOC->PAD[IOC_PAD_PE24].FUNC_CTL = IOC_PE24_FUNC_CTL_GPTMR2_COMP_1;
                break;
            case 2:
                HPM_IOC->PAD[IOC_PAD_PE26].FUNC_CTL = IOC_PE26_FUNC_CTL_GPTMR2_COMP_2;
            default:
                break;
            }
        } else {
            switch (channel) {
            case 0:
                HPM_IOC->PAD[IOC_PAD_PE22].FUNC_CTL = IOC_PE22_FUNC_CTL_GPTMR2_CAPT_0;
                break;
            default:
                break;
            }
        }
    }
}

void init_clk_ref_pin(void)
{
    HPM_IOC->PAD[IOC_PAD_PD31].FUNC_CTL = IOC_PD31_FUNC_CTL_SOC_REF1;
}
