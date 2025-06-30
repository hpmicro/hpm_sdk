/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_EUI_H
#define HPM_EUI_H

typedef struct {
    __RW uint32_t CTRL;                        /* 0x0: Control Register */
    __RW uint32_t DIV_CFG;                     /* 0x4:  */
    __RW uint32_t PAD_CFG;                     /* 0x8:  */
    __RW uint32_t POL_CFG;                     /* 0xC:  */
    __RW uint32_t OUT_CFG;                     /* 0x10:  */
    __RW uint32_t IN_CFG;                      /* 0x14:  */
    __R  uint8_t  RESERVED0[8];                /* 0x18 - 0x1F: Reserved */
    __W  uint32_t IRQ_STS;                     /* 0x20:  */
    __RW uint32_t IRQ_EN;                      /* 0x24:  */
    __R  uint8_t  RESERVED1[24];               /* 0x28 - 0x3F: Reserved */
    __R  uint32_t DATA_IN0;                    /* 0x40:  */
    __R  uint32_t DATA_IN1;                    /* 0x44:  */
    __R  uint32_t DATA_IN2;                    /* 0x48:  */
    __R  uint32_t DATA_IN3;                    /* 0x4C:  */
    __R  uint32_t DATA_IN4;                    /* 0x50:  */
    __R  uint32_t DATA_IN5;                    /* 0x54:  */
    __R  uint32_t DATA_IN6;                    /* 0x58:  */
    __R  uint32_t DATA_IN7;                    /* 0x5C:  */
    __R  uint8_t  RESERVED2[32];               /* 0x60 - 0x7F: Reserved */
    __RW uint32_t DAB_CTRL;                    /* 0x80:  */
    __R  uint8_t  RESERVED3[12];               /* 0x84 - 0x8F: Reserved */
    __RW uint32_t DCD_CTRL;                    /* 0x90:  */
    __R  uint8_t  RESERVED4[12];               /* 0x94 - 0x9F: Reserved */
    __RW uint32_t DEF_CTRL;                    /* 0xA0:  */
    __R  uint8_t  RESERVED5[12];               /* 0xA4 - 0xAF: Reserved */
    __RW uint32_t DGH_CTRL;                    /* 0xB0:  */
    __R  uint8_t  RESERVED6[76];               /* 0xB4 - 0xFF: Reserved */
    __RW uint32_t DA_00;                       /* 0x100:  */
    __RW uint32_t DA_01;                       /* 0x104:  */
    __R  uint8_t  RESERVED7[56];               /* 0x108 - 0x13F: Reserved */
    __RW uint32_t DB_00;                       /* 0x140:  */
    __RW uint32_t DB_01;                       /* 0x144:  */
    __R  uint8_t  RESERVED8[56];               /* 0x148 - 0x17F: Reserved */
    __RW uint32_t DC_00;                       /* 0x180:  */
    __RW uint32_t DC_01;                       /* 0x184:  */
    __R  uint8_t  RESERVED9[56];               /* 0x188 - 0x1BF: Reserved */
    __RW uint32_t DD_00;                       /* 0x1C0:  */
    __RW uint32_t DD_01;                       /* 0x1C4:  */
    __R  uint8_t  RESERVED10[56];              /* 0x1C8 - 0x1FF: Reserved */
    __RW uint32_t DE_00;                       /* 0x200:  */
    __RW uint32_t DE_01;                       /* 0x204:  */
    __R  uint8_t  RESERVED11[56];              /* 0x208 - 0x23F: Reserved */
    __RW uint32_t DF_00;                       /* 0x240:  */
    __RW uint32_t DF_01;                       /* 0x244:  */
    __R  uint8_t  RESERVED12[56];              /* 0x248 - 0x27F: Reserved */
    __RW uint32_t DG_00;                       /* 0x280:  */
    __RW uint32_t DG_01;                       /* 0x284:  */
    __R  uint8_t  RESERVED13[56];              /* 0x288 - 0x2BF: Reserved */
    __RW uint32_t DH_00;                       /* 0x2C0:  */
    __RW uint32_t DH_01;                       /* 0x2C4:  */
} EUI_Type;


/* Bitfield definition for register: CTRL */
/*
 * CLR_OUTPUT (RW)
 *
 * write 1 to send 24bit 0 to data output, Auto clear after clear finishd;
 * only can be set when clock_enable(in eut_state)  is 0
 */
#define EUI_CTRL_CLR_OUTPUT_MASK (0x80000000UL)
#define EUI_CTRL_CLR_OUTPUT_SHIFT (31U)
#define EUI_CTRL_CLR_OUTPUT_SET(x) (((uint32_t)(x) << EUI_CTRL_CLR_OUTPUT_SHIFT) & EUI_CTRL_CLR_OUTPUT_MASK)
#define EUI_CTRL_CLR_OUTPUT_GET(x) (((uint32_t)(x) & EUI_CTRL_CLR_OUTPUT_MASK) >> EUI_CTRL_CLR_OUTPUT_SHIFT)

/*
 * CLR_DATA_IN (WO)
 *
 * write 1 to clear data_in register to all 0.
 * only can be set when clock_enable(in eut_state)  is 0
 */
#define EUI_CTRL_CLR_DATA_IN_MASK (0x40000000UL)
#define EUI_CTRL_CLR_DATA_IN_SHIFT (30U)
#define EUI_CTRL_CLR_DATA_IN_SET(x) (((uint32_t)(x) << EUI_CTRL_CLR_DATA_IN_SHIFT) & EUI_CTRL_CLR_DATA_IN_MASK)
#define EUI_CTRL_CLR_DATA_IN_GET(x) (((uint32_t)(x) & EUI_CTRL_CLR_DATA_IN_MASK) >> EUI_CTRL_CLR_DATA_IN_SHIFT)

/*
 * COL_LOOP_MODE (RW)
 *
 * 0:  all configured LED on at same time in one column;
 * 1:  only one LED on in one column, need
 */
#define EUI_CTRL_COL_LOOP_MODE_MASK (0x4U)
#define EUI_CTRL_COL_LOOP_MODE_SHIFT (2U)
#define EUI_CTRL_COL_LOOP_MODE_SET(x) (((uint32_t)(x) << EUI_CTRL_COL_LOOP_MODE_SHIFT) & EUI_CTRL_COL_LOOP_MODE_MASK)
#define EUI_CTRL_COL_LOOP_MODE_GET(x) (((uint32_t)(x) & EUI_CTRL_COL_LOOP_MODE_MASK) >> EUI_CTRL_COL_LOOP_MODE_SHIFT)

/*
 * DISP_MODE (RW)
 *
 * 0 for 8x8(8bit data,8bit select, need 2 external 595 chip, 1 for data, 1 for select)
 * 1 for 16x4(16bit data, 4bit select, need 3 external 595 chip, 2 for data, 4bit for select,
 * the other 4bit  can be used as dedicate led control)
 */
#define EUI_CTRL_DISP_MODE_MASK (0x2U)
#define EUI_CTRL_DISP_MODE_SHIFT (1U)
#define EUI_CTRL_DISP_MODE_SET(x) (((uint32_t)(x) << EUI_CTRL_DISP_MODE_SHIFT) & EUI_CTRL_DISP_MODE_MASK)
#define EUI_CTRL_DISP_MODE_GET(x) (((uint32_t)(x) & EUI_CTRL_DISP_MODE_MASK) >> EUI_CTRL_DISP_MODE_SHIFT)

/*
 * ENABLE (RW)
 *
 * set to start block; clr to stop(hw will finish current loop)
 */
#define EUI_CTRL_ENABLE_MASK (0x1U)
#define EUI_CTRL_ENABLE_SHIFT (0U)
#define EUI_CTRL_ENABLE_SET(x) (((uint32_t)(x) << EUI_CTRL_ENABLE_SHIFT) & EUI_CTRL_ENABLE_MASK)
#define EUI_CTRL_ENABLE_GET(x) (((uint32_t)(x) & EUI_CTRL_ENABLE_MASK) >> EUI_CTRL_ENABLE_SHIFT)

/* Bitfield definition for register: DIV_CFG */
/*
 * CLKO_ADJ (RW)
 *
 * used to generate pulse signal for  internal slow clock base, such as data change time.
 * default 2 for data change 3 system clock cycles after negedge of coutput clock
 */
#define EUI_DIV_CFG_CLKO_ADJ_MASK (0xF00000UL)
#define EUI_DIV_CFG_CLKO_ADJ_SHIFT (20U)
#define EUI_DIV_CFG_CLKO_ADJ_SET(x) (((uint32_t)(x) << EUI_DIV_CFG_CLKO_ADJ_SHIFT) & EUI_DIV_CFG_CLKO_ADJ_MASK)
#define EUI_DIV_CFG_CLKO_ADJ_GET(x) (((uint32_t)(x) & EUI_DIV_CFG_CLKO_ADJ_MASK) >> EUI_DIV_CFG_CLKO_ADJ_SHIFT)

/*
 * HOLD_TIME (RW)
 *
 * for display hold time, in unit of slot time(32 output clock cycles)
 * shift 16 or 24 cycles in one slot time, then hold output for hold_time slots
 * it need N*(hold_time+1) slot to display all LEDs, N is LED column select signal bits,
 * valid value is 1 to 8 (default is 1024us in 8x8 mode, 512 us in 16x4 mode)
 */
#define EUI_DIV_CFG_HOLD_TIME_MASK (0xF0000UL)
#define EUI_DIV_CFG_HOLD_TIME_SHIFT (16U)
#define EUI_DIV_CFG_HOLD_TIME_SET(x) (((uint32_t)(x) << EUI_DIV_CFG_HOLD_TIME_SHIFT) & EUI_DIV_CFG_HOLD_TIME_MASK)
#define EUI_DIV_CFG_HOLD_TIME_GET(x) (((uint32_t)(x) & EUI_DIV_CFG_HOLD_TIME_MASK) >> EUI_DIV_CFG_HOLD_TIME_SHIFT)

/*
 * CLK_DIV (RW)
 *
 * output clock divider of system clock.
 * 0 for div2, 1 for div4;  default 0x63 for 1MHz output clock at 200MHz system clock
 */
#define EUI_DIV_CFG_CLK_DIV_MASK (0xFFFFU)
#define EUI_DIV_CFG_CLK_DIV_SHIFT (0U)
#define EUI_DIV_CFG_CLK_DIV_SET(x) (((uint32_t)(x) << EUI_DIV_CFG_CLK_DIV_SHIFT) & EUI_DIV_CFG_CLK_DIV_MASK)
#define EUI_DIV_CFG_CLK_DIV_GET(x) (((uint32_t)(x) & EUI_DIV_CFG_CLK_DIV_MASK) >> EUI_DIV_CFG_CLK_DIV_SHIFT)

/* Bitfield definition for register: PAD_CFG */
/*
 * FILT_LEN (RW)
 *
 * switch filter length, use to remove jitter. In unit of disp_time.
 * will assert interrupt if scan input data stable for more than this time.
 * Default 5ms in 8x8 mode
 */
#define EUI_PAD_CFG_FILT_LEN_MASK (0xFFFU)
#define EUI_PAD_CFG_FILT_LEN_SHIFT (0U)
#define EUI_PAD_CFG_FILT_LEN_SET(x) (((uint32_t)(x) << EUI_PAD_CFG_FILT_LEN_SHIFT) & EUI_PAD_CFG_FILT_LEN_MASK)
#define EUI_PAD_CFG_FILT_LEN_GET(x) (((uint32_t)(x) & EUI_PAD_CFG_FILT_LEN_MASK) >> EUI_PAD_CFG_FILT_LEN_SHIFT)

/* Bitfield definition for register: POL_CFG */
/*
 * DI_POL (RW)
 *
 * polarity at pad_do_eut_in
 */
#define EUI_POL_CFG_DI_POL_MASK (0x8000000UL)
#define EUI_POL_CFG_DI_POL_SHIFT (27U)
#define EUI_POL_CFG_DI_POL_SET(x) (((uint32_t)(x) << EUI_POL_CFG_DI_POL_SHIFT) & EUI_POL_CFG_DI_POL_MASK)
#define EUI_POL_CFG_DI_POL_GET(x) (((uint32_t)(x) & EUI_POL_CFG_DI_POL_MASK) >> EUI_POL_CFG_DI_POL_SHIFT)

/*
 * DO_POL (RW)
 *
 * polarity at pad_do_eut_out
 */
#define EUI_POL_CFG_DO_POL_MASK (0x4000000UL)
#define EUI_POL_CFG_DO_POL_SHIFT (26U)
#define EUI_POL_CFG_DO_POL_SET(x) (((uint32_t)(x) << EUI_POL_CFG_DO_POL_SHIFT) & EUI_POL_CFG_DO_POL_MASK)
#define EUI_POL_CFG_DO_POL_GET(x) (((uint32_t)(x) & EUI_POL_CFG_DO_POL_MASK) >> EUI_POL_CFG_DO_POL_SHIFT)

/*
 * CK_POL (RW)
 *
 * polarity at pad_do_eut_ck
 */
#define EUI_POL_CFG_CK_POL_MASK (0x2000000UL)
#define EUI_POL_CFG_CK_POL_SHIFT (25U)
#define EUI_POL_CFG_CK_POL_SET(x) (((uint32_t)(x) << EUI_POL_CFG_CK_POL_SHIFT) & EUI_POL_CFG_CK_POL_MASK)
#define EUI_POL_CFG_CK_POL_GET(x) (((uint32_t)(x) & EUI_POL_CFG_CK_POL_MASK) >> EUI_POL_CFG_CK_POL_SHIFT)

/*
 * SH_POL (RW)
 *
 * polarity at pad_do_eut_sh
 */
#define EUI_POL_CFG_SH_POL_MASK (0x1000000UL)
#define EUI_POL_CFG_SH_POL_SHIFT (24U)
#define EUI_POL_CFG_SH_POL_SET(x) (((uint32_t)(x) << EUI_POL_CFG_SH_POL_SHIFT) & EUI_POL_CFG_SH_POL_MASK)
#define EUI_POL_CFG_SH_POL_GET(x) (((uint32_t)(x) & EUI_POL_CFG_SH_POL_MASK) >> EUI_POL_CFG_SH_POL_SHIFT)

/*
 * DATA_POL (RW)
 *
 * output polarity, set to invert output data at output shift register.
 * NOTE: high 16bits are used in 8x8 mode since shift is always 24 cycles
 */
#define EUI_POL_CFG_DATA_POL_MASK (0xFFFFFFUL)
#define EUI_POL_CFG_DATA_POL_SHIFT (0U)
#define EUI_POL_CFG_DATA_POL_SET(x) (((uint32_t)(x) << EUI_POL_CFG_DATA_POL_SHIFT) & EUI_POL_CFG_DATA_POL_MASK)
#define EUI_POL_CFG_DATA_POL_GET(x) (((uint32_t)(x) & EUI_POL_CFG_DATA_POL_MASK) >> EUI_POL_CFG_DATA_POL_SHIFT)

/* Bitfield definition for register: OUT_CFG */
/*
 * DO_CFG (RW)
 *
 * define how many dedicate led controls
 * in 8x8 mode:
 * 0:  no dedicate led control, use 8bits(A~H) for led line select
 * 1: do_val[7]/H is use as dedicate led control, 7bits(A~G) for led line select
 * 2: do_val[7:6]/GH are use as dedicate led control, 6bits(A~F) for led line select
 * …...
 * 6: do_val[7:2]/C~H are use as dedicate led control, 2bit/AB for led line select
 * 7:  24 bits  are used as dedicate led control(do_val[23:0] is used)
 * in 16x4 mode, if set to 0~4, 4bit(A~D)for led line select
 * others are same as 8x8 mode
 */
#define EUI_OUT_CFG_DO_CFG_MASK (0x7000000UL)
#define EUI_OUT_CFG_DO_CFG_SHIFT (24U)
#define EUI_OUT_CFG_DO_CFG_SET(x) (((uint32_t)(x) << EUI_OUT_CFG_DO_CFG_SHIFT) & EUI_OUT_CFG_DO_CFG_MASK)
#define EUI_OUT_CFG_DO_CFG_GET(x) (((uint32_t)(x) & EUI_OUT_CFG_DO_CFG_MASK) >> EUI_OUT_CFG_DO_CFG_SHIFT)

/*
 * DO_VAL (RW)
 *
 * dedicate led control values, this value will override output data(after do_pol)
 */
#define EUI_OUT_CFG_DO_VAL_MASK (0xFFFFFFUL)
#define EUI_OUT_CFG_DO_VAL_SHIFT (0U)
#define EUI_OUT_CFG_DO_VAL_SET(x) (((uint32_t)(x) << EUI_OUT_CFG_DO_VAL_SHIFT) & EUI_OUT_CFG_DO_VAL_MASK)
#define EUI_OUT_CFG_DO_VAL_GET(x) (((uint32_t)(x) & EUI_OUT_CFG_DO_VAL_MASK) >> EUI_OUT_CFG_DO_VAL_SHIFT)

/* Bitfield definition for register: IN_CFG */
/*
 * DI_CFG (RW)
 *
 * set for input used as dedicate switch, will use dedicate filter logic and interrupt logic
 */
#define EUI_IN_CFG_DI_CFG_MASK (0xFFFF0000UL)
#define EUI_IN_CFG_DI_CFG_SHIFT (16U)
#define EUI_IN_CFG_DI_CFG_SET(x) (((uint32_t)(x) << EUI_IN_CFG_DI_CFG_SHIFT) & EUI_IN_CFG_DI_CFG_MASK)
#define EUI_IN_CFG_DI_CFG_GET(x) (((uint32_t)(x) & EUI_IN_CFG_DI_CFG_MASK) >> EUI_IN_CFG_DI_CFG_SHIFT)

/*
 * DI_VAL (RO)
 *
 * 16bit input data
 */
#define EUI_IN_CFG_DI_VAL_MASK (0xFFFFU)
#define EUI_IN_CFG_DI_VAL_SHIFT (0U)
#define EUI_IN_CFG_DI_VAL_GET(x) (((uint32_t)(x) & EUI_IN_CFG_DI_VAL_MASK) >> EUI_IN_CFG_DI_VAL_SHIFT)

/* Bitfield definition for register: IRQ_STS */
/*
 * IRQ_SINGLE_STS (W1C)
 *
 */
#define EUI_IRQ_STS_IRQ_SINGLE_STS_MASK (0x2U)
#define EUI_IRQ_STS_IRQ_SINGLE_STS_SHIFT (1U)
#define EUI_IRQ_STS_IRQ_SINGLE_STS_SET(x) (((uint32_t)(x) << EUI_IRQ_STS_IRQ_SINGLE_STS_SHIFT) & EUI_IRQ_STS_IRQ_SINGLE_STS_MASK)
#define EUI_IRQ_STS_IRQ_SINGLE_STS_GET(x) (((uint32_t)(x) & EUI_IRQ_STS_IRQ_SINGLE_STS_MASK) >> EUI_IRQ_STS_IRQ_SINGLE_STS_SHIFT)

/*
 * IRQ_AREA_STS (W1C)
 *
 */
#define EUI_IRQ_STS_IRQ_AREA_STS_MASK (0x1U)
#define EUI_IRQ_STS_IRQ_AREA_STS_SHIFT (0U)
#define EUI_IRQ_STS_IRQ_AREA_STS_SET(x) (((uint32_t)(x) << EUI_IRQ_STS_IRQ_AREA_STS_SHIFT) & EUI_IRQ_STS_IRQ_AREA_STS_MASK)
#define EUI_IRQ_STS_IRQ_AREA_STS_GET(x) (((uint32_t)(x) & EUI_IRQ_STS_IRQ_AREA_STS_MASK) >> EUI_IRQ_STS_IRQ_AREA_STS_SHIFT)

/* Bitfield definition for register: IRQ_EN */
/*
 * IRQ_SINGLE_EN (RW)
 *
 */
#define EUI_IRQ_EN_IRQ_SINGLE_EN_MASK (0x2U)
#define EUI_IRQ_EN_IRQ_SINGLE_EN_SHIFT (1U)
#define EUI_IRQ_EN_IRQ_SINGLE_EN_SET(x) (((uint32_t)(x) << EUI_IRQ_EN_IRQ_SINGLE_EN_SHIFT) & EUI_IRQ_EN_IRQ_SINGLE_EN_MASK)
#define EUI_IRQ_EN_IRQ_SINGLE_EN_GET(x) (((uint32_t)(x) & EUI_IRQ_EN_IRQ_SINGLE_EN_MASK) >> EUI_IRQ_EN_IRQ_SINGLE_EN_SHIFT)

/*
 * IRQ_AREA_EN (RW)
 *
 */
#define EUI_IRQ_EN_IRQ_AREA_EN_MASK (0x1U)
#define EUI_IRQ_EN_IRQ_AREA_EN_SHIFT (0U)
#define EUI_IRQ_EN_IRQ_AREA_EN_SET(x) (((uint32_t)(x) << EUI_IRQ_EN_IRQ_AREA_EN_SHIFT) & EUI_IRQ_EN_IRQ_AREA_EN_MASK)
#define EUI_IRQ_EN_IRQ_AREA_EN_GET(x) (((uint32_t)(x) & EUI_IRQ_EN_IRQ_AREA_EN_MASK) >> EUI_IRQ_EN_IRQ_AREA_EN_SHIFT)

/* Bitfield definition for register: DATA_IN0 */
/*
 * DATA (RO)
 *
 * read value when select bit 0(0x01 for output value in 8x8 mode)
 */
#define EUI_DATA_IN0_DATA_MASK (0xFFFFU)
#define EUI_DATA_IN0_DATA_SHIFT (0U)
#define EUI_DATA_IN0_DATA_GET(x) (((uint32_t)(x) & EUI_DATA_IN0_DATA_MASK) >> EUI_DATA_IN0_DATA_SHIFT)

/* Bitfield definition for register: DATA_IN1 */
/*
 * DATA (RO)
 *
 */
#define EUI_DATA_IN1_DATA_MASK (0xFFFFU)
#define EUI_DATA_IN1_DATA_SHIFT (0U)
#define EUI_DATA_IN1_DATA_GET(x) (((uint32_t)(x) & EUI_DATA_IN1_DATA_MASK) >> EUI_DATA_IN1_DATA_SHIFT)

/* Bitfield definition for register: DATA_IN2 */
/*
 * DATA (RO)
 *
 */
#define EUI_DATA_IN2_DATA_MASK (0xFFFFU)
#define EUI_DATA_IN2_DATA_SHIFT (0U)
#define EUI_DATA_IN2_DATA_GET(x) (((uint32_t)(x) & EUI_DATA_IN2_DATA_MASK) >> EUI_DATA_IN2_DATA_SHIFT)

/* Bitfield definition for register: DATA_IN3 */
/*
 * DATA (RO)
 *
 */
#define EUI_DATA_IN3_DATA_MASK (0xFFFFU)
#define EUI_DATA_IN3_DATA_SHIFT (0U)
#define EUI_DATA_IN3_DATA_GET(x) (((uint32_t)(x) & EUI_DATA_IN3_DATA_MASK) >> EUI_DATA_IN3_DATA_SHIFT)

/* Bitfield definition for register: DATA_IN4 */
/*
 * DATA (RO)
 *
 */
#define EUI_DATA_IN4_DATA_MASK (0xFFFFU)
#define EUI_DATA_IN4_DATA_SHIFT (0U)
#define EUI_DATA_IN4_DATA_GET(x) (((uint32_t)(x) & EUI_DATA_IN4_DATA_MASK) >> EUI_DATA_IN4_DATA_SHIFT)

/* Bitfield definition for register: DATA_IN5 */
/*
 * DATA (RO)
 *
 */
#define EUI_DATA_IN5_DATA_MASK (0xFFFFU)
#define EUI_DATA_IN5_DATA_SHIFT (0U)
#define EUI_DATA_IN5_DATA_GET(x) (((uint32_t)(x) & EUI_DATA_IN5_DATA_MASK) >> EUI_DATA_IN5_DATA_SHIFT)

/* Bitfield definition for register: DATA_IN6 */
/*
 * DATA (RO)
 *
 */
#define EUI_DATA_IN6_DATA_MASK (0xFFFFU)
#define EUI_DATA_IN6_DATA_SHIFT (0U)
#define EUI_DATA_IN6_DATA_GET(x) (((uint32_t)(x) & EUI_DATA_IN6_DATA_MASK) >> EUI_DATA_IN6_DATA_SHIFT)

/* Bitfield definition for register: DATA_IN7 */
/*
 * DATA (RO)
 *
 */
#define EUI_DATA_IN7_DATA_MASK (0xFFFFU)
#define EUI_DATA_IN7_DATA_SHIFT (0U)
#define EUI_DATA_IN7_DATA_GET(x) (((uint32_t)(x) & EUI_DATA_IN7_DATA_MASK) >> EUI_DATA_IN7_DATA_SHIFT)

/* Bitfield definition for register: DAB_CTRL */
/*
 * DB_CTRL (RW)
 *
 * 00:  output data B directly;
 * 01:  A&B;   10:  A|B;   11:  A^B
 */
#define EUI_DAB_CTRL_DB_CTRL_MASK (0xC0000000UL)
#define EUI_DAB_CTRL_DB_CTRL_SHIFT (30U)
#define EUI_DAB_CTRL_DB_CTRL_SET(x) (((uint32_t)(x) << EUI_DAB_CTRL_DB_CTRL_SHIFT) & EUI_DAB_CTRL_DB_CTRL_MASK)
#define EUI_DAB_CTRL_DB_CTRL_GET(x) (((uint32_t)(x) & EUI_DAB_CTRL_DB_CTRL_MASK) >> EUI_DAB_CTRL_DB_CTRL_SHIFT)

/*
 * LOOPAB (RW)
 *
 * display AB loop
 * display data A for prda, then B for prdB, then lopp AB;
 * then display C, D(loop CD), EF, GH, then back to AB
 * 0 for no loop, just display once
 */
#define EUI_DAB_CTRL_LOOPAB_MASK (0xFF0000UL)
#define EUI_DAB_CTRL_LOOPAB_SHIFT (16U)
#define EUI_DAB_CTRL_LOOPAB_SET(x) (((uint32_t)(x) << EUI_DAB_CTRL_LOOPAB_SHIFT) & EUI_DAB_CTRL_LOOPAB_MASK)
#define EUI_DAB_CTRL_LOOPAB_GET(x) (((uint32_t)(x) & EUI_DAB_CTRL_LOOPAB_MASK) >> EUI_DAB_CTRL_LOOPAB_SHIFT)

/*
 * PRDB (RW)
 *
 * display data B time(in unit of 16*disp_time), default 65ms(65536us)
 */
#define EUI_DAB_CTRL_PRDB_MASK (0xFF00U)
#define EUI_DAB_CTRL_PRDB_SHIFT (8U)
#define EUI_DAB_CTRL_PRDB_SET(x) (((uint32_t)(x) << EUI_DAB_CTRL_PRDB_SHIFT) & EUI_DAB_CTRL_PRDB_MASK)
#define EUI_DAB_CTRL_PRDB_GET(x) (((uint32_t)(x) & EUI_DAB_CTRL_PRDB_MASK) >> EUI_DAB_CTRL_PRDB_SHIFT)

/*
 * PRDA (RW)
 *
 * display data A time(in unit of 16*disp_time)
 * dab_ctrl.prda must be none-zero value, others can be set to 0 if no data
 */
#define EUI_DAB_CTRL_PRDA_MASK (0xFFU)
#define EUI_DAB_CTRL_PRDA_SHIFT (0U)
#define EUI_DAB_CTRL_PRDA_SET(x) (((uint32_t)(x) << EUI_DAB_CTRL_PRDA_SHIFT) & EUI_DAB_CTRL_PRDA_MASK)
#define EUI_DAB_CTRL_PRDA_GET(x) (((uint32_t)(x) & EUI_DAB_CTRL_PRDA_MASK) >> EUI_DAB_CTRL_PRDA_SHIFT)

/* Bitfield definition for register: DCD_CTRL */
/*
 * DD_CTRL (RW)
 *
 */
#define EUI_DCD_CTRL_DD_CTRL_MASK (0xC0000000UL)
#define EUI_DCD_CTRL_DD_CTRL_SHIFT (30U)
#define EUI_DCD_CTRL_DD_CTRL_SET(x) (((uint32_t)(x) << EUI_DCD_CTRL_DD_CTRL_SHIFT) & EUI_DCD_CTRL_DD_CTRL_MASK)
#define EUI_DCD_CTRL_DD_CTRL_GET(x) (((uint32_t)(x) & EUI_DCD_CTRL_DD_CTRL_MASK) >> EUI_DCD_CTRL_DD_CTRL_SHIFT)

/*
 * LOOPCD (RW)
 *
 */
#define EUI_DCD_CTRL_LOOPCD_MASK (0xFF0000UL)
#define EUI_DCD_CTRL_LOOPCD_SHIFT (16U)
#define EUI_DCD_CTRL_LOOPCD_SET(x) (((uint32_t)(x) << EUI_DCD_CTRL_LOOPCD_SHIFT) & EUI_DCD_CTRL_LOOPCD_MASK)
#define EUI_DCD_CTRL_LOOPCD_GET(x) (((uint32_t)(x) & EUI_DCD_CTRL_LOOPCD_MASK) >> EUI_DCD_CTRL_LOOPCD_SHIFT)

/*
 * PRDD (RW)
 *
 */
#define EUI_DCD_CTRL_PRDD_MASK (0xFF00U)
#define EUI_DCD_CTRL_PRDD_SHIFT (8U)
#define EUI_DCD_CTRL_PRDD_SET(x) (((uint32_t)(x) << EUI_DCD_CTRL_PRDD_SHIFT) & EUI_DCD_CTRL_PRDD_MASK)
#define EUI_DCD_CTRL_PRDD_GET(x) (((uint32_t)(x) & EUI_DCD_CTRL_PRDD_MASK) >> EUI_DCD_CTRL_PRDD_SHIFT)

/*
 * PRDC (RW)
 *
 */
#define EUI_DCD_CTRL_PRDC_MASK (0xFFU)
#define EUI_DCD_CTRL_PRDC_SHIFT (0U)
#define EUI_DCD_CTRL_PRDC_SET(x) (((uint32_t)(x) << EUI_DCD_CTRL_PRDC_SHIFT) & EUI_DCD_CTRL_PRDC_MASK)
#define EUI_DCD_CTRL_PRDC_GET(x) (((uint32_t)(x) & EUI_DCD_CTRL_PRDC_MASK) >> EUI_DCD_CTRL_PRDC_SHIFT)

/* Bitfield definition for register: DEF_CTRL */
/*
 * DF_CTRL (RW)
 *
 */
#define EUI_DEF_CTRL_DF_CTRL_MASK (0xC0000000UL)
#define EUI_DEF_CTRL_DF_CTRL_SHIFT (30U)
#define EUI_DEF_CTRL_DF_CTRL_SET(x) (((uint32_t)(x) << EUI_DEF_CTRL_DF_CTRL_SHIFT) & EUI_DEF_CTRL_DF_CTRL_MASK)
#define EUI_DEF_CTRL_DF_CTRL_GET(x) (((uint32_t)(x) & EUI_DEF_CTRL_DF_CTRL_MASK) >> EUI_DEF_CTRL_DF_CTRL_SHIFT)

/*
 * LOOPEF (RW)
 *
 */
#define EUI_DEF_CTRL_LOOPEF_MASK (0xFF0000UL)
#define EUI_DEF_CTRL_LOOPEF_SHIFT (16U)
#define EUI_DEF_CTRL_LOOPEF_SET(x) (((uint32_t)(x) << EUI_DEF_CTRL_LOOPEF_SHIFT) & EUI_DEF_CTRL_LOOPEF_MASK)
#define EUI_DEF_CTRL_LOOPEF_GET(x) (((uint32_t)(x) & EUI_DEF_CTRL_LOOPEF_MASK) >> EUI_DEF_CTRL_LOOPEF_SHIFT)

/*
 * PRDF (RW)
 *
 */
#define EUI_DEF_CTRL_PRDF_MASK (0xFF00U)
#define EUI_DEF_CTRL_PRDF_SHIFT (8U)
#define EUI_DEF_CTRL_PRDF_SET(x) (((uint32_t)(x) << EUI_DEF_CTRL_PRDF_SHIFT) & EUI_DEF_CTRL_PRDF_MASK)
#define EUI_DEF_CTRL_PRDF_GET(x) (((uint32_t)(x) & EUI_DEF_CTRL_PRDF_MASK) >> EUI_DEF_CTRL_PRDF_SHIFT)

/*
 * PRDE (RW)
 *
 */
#define EUI_DEF_CTRL_PRDE_MASK (0xFFU)
#define EUI_DEF_CTRL_PRDE_SHIFT (0U)
#define EUI_DEF_CTRL_PRDE_SET(x) (((uint32_t)(x) << EUI_DEF_CTRL_PRDE_SHIFT) & EUI_DEF_CTRL_PRDE_MASK)
#define EUI_DEF_CTRL_PRDE_GET(x) (((uint32_t)(x) & EUI_DEF_CTRL_PRDE_MASK) >> EUI_DEF_CTRL_PRDE_SHIFT)

/* Bitfield definition for register: DGH_CTRL */
/*
 * DH_CTRL (RW)
 *
 */
#define EUI_DGH_CTRL_DH_CTRL_MASK (0xC0000000UL)
#define EUI_DGH_CTRL_DH_CTRL_SHIFT (30U)
#define EUI_DGH_CTRL_DH_CTRL_SET(x) (((uint32_t)(x) << EUI_DGH_CTRL_DH_CTRL_SHIFT) & EUI_DGH_CTRL_DH_CTRL_MASK)
#define EUI_DGH_CTRL_DH_CTRL_GET(x) (((uint32_t)(x) & EUI_DGH_CTRL_DH_CTRL_MASK) >> EUI_DGH_CTRL_DH_CTRL_SHIFT)

/*
 * LOOPGH (RW)
 *
 */
#define EUI_DGH_CTRL_LOOPGH_MASK (0xFF0000UL)
#define EUI_DGH_CTRL_LOOPGH_SHIFT (16U)
#define EUI_DGH_CTRL_LOOPGH_SET(x) (((uint32_t)(x) << EUI_DGH_CTRL_LOOPGH_SHIFT) & EUI_DGH_CTRL_LOOPGH_MASK)
#define EUI_DGH_CTRL_LOOPGH_GET(x) (((uint32_t)(x) & EUI_DGH_CTRL_LOOPGH_MASK) >> EUI_DGH_CTRL_LOOPGH_SHIFT)

/*
 * PRDH (RW)
 *
 */
#define EUI_DGH_CTRL_PRDH_MASK (0xFF00U)
#define EUI_DGH_CTRL_PRDH_SHIFT (8U)
#define EUI_DGH_CTRL_PRDH_SET(x) (((uint32_t)(x) << EUI_DGH_CTRL_PRDH_SHIFT) & EUI_DGH_CTRL_PRDH_MASK)
#define EUI_DGH_CTRL_PRDH_GET(x) (((uint32_t)(x) & EUI_DGH_CTRL_PRDH_MASK) >> EUI_DGH_CTRL_PRDH_SHIFT)

/*
 * PRDG (RW)
 *
 */
#define EUI_DGH_CTRL_PRDG_MASK (0xFFU)
#define EUI_DGH_CTRL_PRDG_SHIFT (0U)
#define EUI_DGH_CTRL_PRDG_SET(x) (((uint32_t)(x) << EUI_DGH_CTRL_PRDG_SHIFT) & EUI_DGH_CTRL_PRDG_MASK)
#define EUI_DGH_CTRL_PRDG_GET(x) (((uint32_t)(x) & EUI_DGH_CTRL_PRDG_MASK) >> EUI_DGH_CTRL_PRDG_SHIFT)

/* Bitfield definition for register: DA_00 */
/*
 * DATA (RW)
 *
 */
#define EUI_DA_00_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DA_00_DATA_SHIFT (0U)
#define EUI_DA_00_DATA_SET(x) (((uint32_t)(x) << EUI_DA_00_DATA_SHIFT) & EUI_DA_00_DATA_MASK)
#define EUI_DA_00_DATA_GET(x) (((uint32_t)(x) & EUI_DA_00_DATA_MASK) >> EUI_DA_00_DATA_SHIFT)

/* Bitfield definition for register: DA_01 */
/*
 * DATA (RW)
 *
 */
#define EUI_DA_01_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DA_01_DATA_SHIFT (0U)
#define EUI_DA_01_DATA_SET(x) (((uint32_t)(x) << EUI_DA_01_DATA_SHIFT) & EUI_DA_01_DATA_MASK)
#define EUI_DA_01_DATA_GET(x) (((uint32_t)(x) & EUI_DA_01_DATA_MASK) >> EUI_DA_01_DATA_SHIFT)

/* Bitfield definition for register: DB_00 */
/*
 * DATA (RW)
 *
 */
#define EUI_DB_00_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DB_00_DATA_SHIFT (0U)
#define EUI_DB_00_DATA_SET(x) (((uint32_t)(x) << EUI_DB_00_DATA_SHIFT) & EUI_DB_00_DATA_MASK)
#define EUI_DB_00_DATA_GET(x) (((uint32_t)(x) & EUI_DB_00_DATA_MASK) >> EUI_DB_00_DATA_SHIFT)

/* Bitfield definition for register: DB_01 */
/*
 * DATA (RW)
 *
 */
#define EUI_DB_01_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DB_01_DATA_SHIFT (0U)
#define EUI_DB_01_DATA_SET(x) (((uint32_t)(x) << EUI_DB_01_DATA_SHIFT) & EUI_DB_01_DATA_MASK)
#define EUI_DB_01_DATA_GET(x) (((uint32_t)(x) & EUI_DB_01_DATA_MASK) >> EUI_DB_01_DATA_SHIFT)

/* Bitfield definition for register: DC_00 */
/*
 * DATA (RW)
 *
 */
#define EUI_DC_00_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DC_00_DATA_SHIFT (0U)
#define EUI_DC_00_DATA_SET(x) (((uint32_t)(x) << EUI_DC_00_DATA_SHIFT) & EUI_DC_00_DATA_MASK)
#define EUI_DC_00_DATA_GET(x) (((uint32_t)(x) & EUI_DC_00_DATA_MASK) >> EUI_DC_00_DATA_SHIFT)

/* Bitfield definition for register: DC_01 */
/*
 * DATA (RW)
 *
 */
#define EUI_DC_01_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DC_01_DATA_SHIFT (0U)
#define EUI_DC_01_DATA_SET(x) (((uint32_t)(x) << EUI_DC_01_DATA_SHIFT) & EUI_DC_01_DATA_MASK)
#define EUI_DC_01_DATA_GET(x) (((uint32_t)(x) & EUI_DC_01_DATA_MASK) >> EUI_DC_01_DATA_SHIFT)

/* Bitfield definition for register: DD_00 */
/*
 * DATA (RW)
 *
 */
#define EUI_DD_00_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DD_00_DATA_SHIFT (0U)
#define EUI_DD_00_DATA_SET(x) (((uint32_t)(x) << EUI_DD_00_DATA_SHIFT) & EUI_DD_00_DATA_MASK)
#define EUI_DD_00_DATA_GET(x) (((uint32_t)(x) & EUI_DD_00_DATA_MASK) >> EUI_DD_00_DATA_SHIFT)

/* Bitfield definition for register: DD_01 */
/*
 * DATA (RW)
 *
 */
#define EUI_DD_01_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DD_01_DATA_SHIFT (0U)
#define EUI_DD_01_DATA_SET(x) (((uint32_t)(x) << EUI_DD_01_DATA_SHIFT) & EUI_DD_01_DATA_MASK)
#define EUI_DD_01_DATA_GET(x) (((uint32_t)(x) & EUI_DD_01_DATA_MASK) >> EUI_DD_01_DATA_SHIFT)

/* Bitfield definition for register: DE_00 */
/*
 * DATA (RW)
 *
 */
#define EUI_DE_00_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DE_00_DATA_SHIFT (0U)
#define EUI_DE_00_DATA_SET(x) (((uint32_t)(x) << EUI_DE_00_DATA_SHIFT) & EUI_DE_00_DATA_MASK)
#define EUI_DE_00_DATA_GET(x) (((uint32_t)(x) & EUI_DE_00_DATA_MASK) >> EUI_DE_00_DATA_SHIFT)

/* Bitfield definition for register: DE_01 */
/*
 * DATA (RW)
 *
 */
#define EUI_DE_01_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DE_01_DATA_SHIFT (0U)
#define EUI_DE_01_DATA_SET(x) (((uint32_t)(x) << EUI_DE_01_DATA_SHIFT) & EUI_DE_01_DATA_MASK)
#define EUI_DE_01_DATA_GET(x) (((uint32_t)(x) & EUI_DE_01_DATA_MASK) >> EUI_DE_01_DATA_SHIFT)

/* Bitfield definition for register: DF_00 */
/*
 * DATA (RW)
 *
 */
#define EUI_DF_00_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DF_00_DATA_SHIFT (0U)
#define EUI_DF_00_DATA_SET(x) (((uint32_t)(x) << EUI_DF_00_DATA_SHIFT) & EUI_DF_00_DATA_MASK)
#define EUI_DF_00_DATA_GET(x) (((uint32_t)(x) & EUI_DF_00_DATA_MASK) >> EUI_DF_00_DATA_SHIFT)

/* Bitfield definition for register: DF_01 */
/*
 * DATA (RW)
 *
 */
#define EUI_DF_01_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DF_01_DATA_SHIFT (0U)
#define EUI_DF_01_DATA_SET(x) (((uint32_t)(x) << EUI_DF_01_DATA_SHIFT) & EUI_DF_01_DATA_MASK)
#define EUI_DF_01_DATA_GET(x) (((uint32_t)(x) & EUI_DF_01_DATA_MASK) >> EUI_DF_01_DATA_SHIFT)

/* Bitfield definition for register: DG_00 */
/*
 * DATA (RW)
 *
 */
#define EUI_DG_00_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DG_00_DATA_SHIFT (0U)
#define EUI_DG_00_DATA_SET(x) (((uint32_t)(x) << EUI_DG_00_DATA_SHIFT) & EUI_DG_00_DATA_MASK)
#define EUI_DG_00_DATA_GET(x) (((uint32_t)(x) & EUI_DG_00_DATA_MASK) >> EUI_DG_00_DATA_SHIFT)

/* Bitfield definition for register: DG_01 */
/*
 * DATA (RW)
 *
 */
#define EUI_DG_01_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DG_01_DATA_SHIFT (0U)
#define EUI_DG_01_DATA_SET(x) (((uint32_t)(x) << EUI_DG_01_DATA_SHIFT) & EUI_DG_01_DATA_MASK)
#define EUI_DG_01_DATA_GET(x) (((uint32_t)(x) & EUI_DG_01_DATA_MASK) >> EUI_DG_01_DATA_SHIFT)

/* Bitfield definition for register: DH_00 */
/*
 * DATA (RW)
 *
 */
#define EUI_DH_00_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DH_00_DATA_SHIFT (0U)
#define EUI_DH_00_DATA_SET(x) (((uint32_t)(x) << EUI_DH_00_DATA_SHIFT) & EUI_DH_00_DATA_MASK)
#define EUI_DH_00_DATA_GET(x) (((uint32_t)(x) & EUI_DH_00_DATA_MASK) >> EUI_DH_00_DATA_SHIFT)

/* Bitfield definition for register: DH_01 */
/*
 * DATA (RW)
 *
 */
#define EUI_DH_01_DATA_MASK (0xFFFFFFFFUL)
#define EUI_DH_01_DATA_SHIFT (0U)
#define EUI_DH_01_DATA_SET(x) (((uint32_t)(x) << EUI_DH_01_DATA_SHIFT) & EUI_DH_01_DATA_MASK)
#define EUI_DH_01_DATA_GET(x) (((uint32_t)(x) & EUI_DH_01_DATA_MASK) >> EUI_DH_01_DATA_SHIFT)




#endif /* HPM_EUI_H */
