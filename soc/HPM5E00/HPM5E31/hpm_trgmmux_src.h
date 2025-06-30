/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_TRGMMUX_SRC_H
#define HPM_TRGMMUX_SRC_H

/* trgm0_input mux definitions */
#define HPM_TRGM0_INPUT_SRC_VSS                            (0x0UL)   /* low level voltage */
#define HPM_TRGM0_INPUT_SRC_VDD                            (0x1UL)   /* high level voltage */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP0                      (0x2UL)   /* PTPC compare output0 */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP1                      (0x3UL)   /* PTPC compare output1 */
#define HPM_TRGM0_INPUT_SRC_USB0_SOF                       (0x4UL)   /* USB0 start of frame marker */
#define HPM_TRGM0_INPUT_SRC_DEBUG_FLAG                     (0x5UL)   /* debug mode flag */
#define HPM_TRGM0_INPUT_SRC_ESC_SYNC1                      (0x6UL)   /* ethercat sync1 event */
#define HPM_TRGM0_INPUT_SRC_ESC_SYNC0                      (0x7UL)   /* ethercat sync0 event */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH00                      (0x8UL)   /* SYNT channel0 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH01                      (0x9UL)   /* SYNT channel1 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH02                      (0xAUL)   /* SYNT channel2 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH03                      (0xBUL)   /* SYNT channel3 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH04                      (0xCUL)   /* SYNT channel4 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH05                      (0xDUL)   /* SYNT channel5 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH06                      (0xEUL)   /* SYNT channel6 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH07                      (0xFUL)   /* SYNT channel7 pulse output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT2                    (0x10UL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT3                    (0x11UL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT2                    (0x12UL)  /* GPTMR1 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT3                    (0x13UL)  /* GPTMR1 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR2_OUT2                    (0x14UL)  /* GPTMR2 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR2_OUT3                    (0x15UL)  /* GPTMR2 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR3_OUT2                    (0x16UL)  /* GPTMR3 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR3_OUT3                    (0x17UL)  /* GPTMR3 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_0                    (0x18UL)  /* PWM0 trigger out0 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_1                    (0x19UL)  /* PWM0 trigger out1 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_2                    (0x1AUL)  /* PWM0 trigger out2 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_3                    (0x1BUL)  /* PWM0 trigger out3 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_4                    (0x1CUL)  /* PWM0 trigger out4 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_5                    (0x1DUL)  /* PWM0 trigger out5 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_6                    (0x1EUL)  /* PWM0 trigger out6 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_7                    (0x1FUL)  /* PWM0 trigger out7 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_0                    (0x20UL)  /* PWM1 trigger out0 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_1                    (0x21UL)  /* PWM1 trigger out1 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_2                    (0x22UL)  /* PWM1 trigger out2 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_3                    (0x23UL)  /* PWM1 trigger out3 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_4                    (0x24UL)  /* PWM1 trigger out4 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_5                    (0x25UL)  /* PWM1 trigger out5 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_6                    (0x26UL)  /* PWM1 trigger out6 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_7                    (0x27UL)  /* PWM1 trigger out7 */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P00                      (0x28UL)  /* TRGM input data0(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P01                      (0x29UL)  /* TRGM input data1(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P02                      (0x2AUL)  /* TRGM input data2(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P03                      (0x2BUL)  /* TRGM input data3(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P04                      (0x2CUL)  /* TRGM input data4(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P05                      (0x2DUL)  /* TRGM input data5(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P06                      (0x2EUL)  /* TRGM input data6(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P07                      (0x2FUL)  /* TRGM input data7(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P08                      (0x30UL)  /* TRGM input data8(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P09                      (0x31UL)  /* TRGM input data9(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P10                      (0x32UL)  /* TRGM input data10(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P11                      (0x33UL)  /* TRGM input data11(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P12                      (0x34UL)  /* TRGM input data12(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P13                      (0x35UL)  /* TRGM input data13(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P14                      (0x36UL)  /* TRGM input data14(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P15                      (0x37UL)  /* TRGM input data15(from IO) */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ0                   (0x38UL)  /* sdm0 channel0 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ1                   (0x39UL)  /* sdm0 channel1 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ2                   (0x3AUL)  /* sdm0 channel2 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ3                   (0x3BUL)  /* sdm0 channel3 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL0                    (0x3CUL)  /* sdm0 channel0 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL1                    (0x3DUL)  /* sdm0 channel1 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL2                    (0x3EUL)  /* sdm0 channel2 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL3                    (0x3FUL)  /* sdm0 channel3 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA0                   (0x40UL)  /* sdm0 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA1                   (0x41UL)  /* sdm0 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA2                   (0x42UL)  /* sdm0 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA3                   (0x43UL)  /* sdm0 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT00                      (0x44UL)  /* PLB trigger out0 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT01                      (0x45UL)  /* PLB trigger out1 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT02                      (0x46UL)  /* PLB trigger out2 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT03                      (0x47UL)  /* PLB trigger out3 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT04                      (0x48UL)  /* PLB trigger out4 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT05                      (0x49UL)  /* PLB trigger out5 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT06                      (0x4AUL)  /* PLB trigger out6 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT07                      (0x4BUL)  /* PLB trigger out7 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT08                      (0x4CUL)  /* PLB trigger out8 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT09                      (0x4DUL)  /* PLB trigger out9 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT10                      (0x4EUL)  /* PLB trigger out10 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT11                      (0x4FUL)  /* PLB trigger out11 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT12                      (0x50UL)  /* PLB trigger out12 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT13                      (0x51UL)  /* PLB trigger out13 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT14                      (0x52UL)  /* PLB trigger out14 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT15                      (0x53UL)  /* PLB trigger out15 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT16                      (0x54UL)  /* PLB trigger out16 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT17                      (0x55UL)  /* PLB trigger out17 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT18                      (0x56UL)  /* PLB trigger out18 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT19                      (0x57UL)  /* PLB trigger out19 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT20                      (0x58UL)  /* PLB trigger out20 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT21                      (0x59UL)  /* PLB trigger out21 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT22                      (0x5AUL)  /* PLB trigger out22 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT23                      (0x5BUL)  /* PLB trigger out23 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT24                      (0x5CUL)  /* PLB trigger out24 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT25                      (0x5DUL)  /* PLB trigger out25 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT26                      (0x5EUL)  /* PLB trigger out26 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT27                      (0x5FUL)  /* PLB trigger out27 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT28                      (0x60UL)  /* PLB trigger out28 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT29                      (0x61UL)  /* PLB trigger out29 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT30                      (0x62UL)  /* PLB trigger out30 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT31                      (0x63UL)  /* PLB trigger out31 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT32                      (0x64UL)  /* PLB trigger out32 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT33                      (0x65UL)  /* PLB trigger out33 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT34                      (0x66UL)  /* PLB trigger out34 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT35                      (0x67UL)  /* PLB trigger out35 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT36                      (0x68UL)  /* PLB trigger out36 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT37                      (0x69UL)  /* PLB trigger out37 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT38                      (0x6AUL)  /* PLB trigger out38 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT39                      (0x6BUL)  /* PLB trigger out39 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT40                      (0x6CUL)  /* PLB trigger out40 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT41                      (0x6DUL)  /* PLB trigger out41 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT42                      (0x6EUL)  /* PLB trigger out42 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT43                      (0x6FUL)  /* PLB trigger out43 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT44                      (0x70UL)  /* PLB trigger out44 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT45                      (0x71UL)  /* PLB trigger out45 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT46                      (0x72UL)  /* PLB trigger out46 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT47                      (0x73UL)  /* PLB trigger out47 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT48                      (0x74UL)  /* PLB trigger out48 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT49                      (0x75UL)  /* PLB trigger out49 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT50                      (0x76UL)  /* PLB trigger out50 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT51                      (0x77UL)  /* PLB trigger out51 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT52                      (0x78UL)  /* PLB trigger out52 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT53                      (0x79UL)  /* PLB trigger out53 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT54                      (0x7AUL)  /* PLB trigger out54 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT55                      (0x7BUL)  /* PLB trigger out55 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT56                      (0x7CUL)  /* PLB trigger out56 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT57                      (0x7DUL)  /* PLB trigger out57 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT58                      (0x7EUL)  /* PLB trigger out58 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT59                      (0x7FUL)  /* PLB trigger out59 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT60                      (0x80UL)  /* PLB trigger out60 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT61                      (0x81UL)  /* PLB trigger out61 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT62                      (0x82UL)  /* PLB trigger out62 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT63                      (0x83UL)  /* PLB trigger out63 */
#define HPM_TRGM0_INPUT_SRC_QEI0_TRIG_OUT                  (0x84UL)  /* QEI0 trigger out */
#define HPM_TRGM0_INPUT_SRC_QEI1_TRIG_OUT                  (0x85UL)  /* QEI1 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC0_TRIG_OUT                  (0x86UL)  /* ADC0 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC1_TRIG_OUT                  (0x87UL)  /* ADC1 trigger out */
#define HPM_TRGM0_INPUT_SRC_ACMP0_OUT_CH0                  (0x88UL)  /* ACMP0 CH0 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP0_OUT_CH1                  (0x89UL)  /* ACMP0 CH1 compare output */
#define HPM_TRGM0_INPUT_SRC_ENET0_PTP_3                    (0x8AUL)  /* ENET0 PTP output bit3 */
#define HPM_TRGM0_INPUT_SRC_QEI0_POS_VALID                 (0x8BUL)  /* QEI0 position valid */
#define HPM_TRGM0_INPUT_SRC_QEI1_POS_VALID                 (0x8CUL)  /* QEI1 position valid */
#define HPM_TRGM0_INPUT_SRC_ADC0_ADC_VALID                 (0x8DUL)  /* ADC0 ADC data valid */
#define HPM_TRGM0_INPUT_SRC_ADC1_ADC_VALID                 (0x8EUL)  /* ADC1 ADC data valid */
#define HPM_TRGM0_INPUT_SRC_SDM0_ADC0_VALID                (0x8FUL)  /* SDM0 ADC0 data valid */
#define HPM_TRGM0_INPUT_SRC_SDM0_ADC1_VALID                (0x90UL)  /* SDM0 ADC1 data valid */
#define HPM_TRGM0_INPUT_SRC_SDM0_ADC2_VALID                (0x91UL)  /* SDM0 ADC2 data valid */
#define HPM_TRGM0_INPUT_SRC_SDM0_ADC3_VALID                (0x92UL)  /* SDM0 ADC3 data valid */
#define HPM_TRGM0_INPUT_SRC_QEO0_DAC0_VALID                (0x93UL)  /* QEO0 DAC0 data valid */
#define HPM_TRGM0_INPUT_SRC_QEO1_DAC0_VALID                (0x94UL)  /* QEO1 DAC1 data valid */

/* trgm0_output mux definitions */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P00                     (0x0UL)   /* TRGM output data0(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P01                     (0x1UL)   /* TRGM output data1(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P02                     (0x2UL)   /* TRGM output data2(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P03                     (0x3UL)   /* TRGM output data3(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P04                     (0x4UL)   /* TRGM output data4(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P05                     (0x5UL)   /* TRGM output data5(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P06                     (0x6UL)   /* TRGM output data6(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P07                     (0x7UL)   /* TRGM output data7(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P08                     (0x8UL)   /* TRGM output data8(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P09                     (0x9UL)   /* TRGM output data9(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P10                     (0xAUL)   /* TRGM output data10(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P11                     (0xBUL)   /* TRGM output data11(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P12                     (0xCUL)   /* TRGM output data12(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P13                     (0xDUL)   /* TRGM output data13(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P14                     (0xEUL)   /* TRGM output data14(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P15                     (0xFUL)   /* TRGM output data15(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN_0                (0x10UL)  /* PWM0 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN_1                (0x11UL)  /* PWM0 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN_2                (0x12UL)  /* PWM0 trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN_3                (0x13UL)  /* PWM0 trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN_4                (0x14UL)  /* PWM0 trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN_5                (0x15UL)  /* PWM0 trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN_6                (0x16UL)  /* PWM0 trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN_7                (0x17UL)  /* PWM0 trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN_0                (0x18UL)  /* PWM1 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN_1                (0x19UL)  /* PWM1 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN_2                (0x1AUL)  /* PWM1 trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN_3                (0x1BUL)  /* PWM1 trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN_4                (0x1CUL)  /* PWM1 trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN_5                (0x1DUL)  /* PWM1 trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN_6                (0x1EUL)  /* PWM1 trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN_7                (0x1FUL)  /* PWM1 trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_ADC0_STRGI                    (0x20UL)  /* ADC0 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC1_STRGI                    (0x21UL)  /* ADC1 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0A                  (0x22UL)  /* ADC preemption trigger0A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0B                  (0x23UL)  /* ADC preemption trigger0B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0C                  (0x24UL)  /* ADC preemption trigger0C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1A                  (0x25UL)  /* ADC preemption trigger1A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1B                  (0x26UL)  /* ADC preemption trigger1B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1C                  (0x27UL)  /* ADC preemption trigger1C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2A                  (0x28UL)  /* ADC preemption trigger2A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2B                  (0x29UL)  /* ADC preemption trigger2B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2C                  (0x2AUL)  /* ADC preemption trigger2C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3A                  (0x2BUL)  /* ADC preemption trigger3A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3B                  (0x2CUL)  /* ADC preemption trigger3B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3C                  (0x2DUL)  /* ADC preemption trigger3C */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_TRIG_IN                  (0x2EUL)  /* QEI0 trigger in */
#define HPM_TRGM0_OUTPUT_SRC_QEI1_TRIG_IN                  (0x2FUL)  /* QEI1 trigger in */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_PAUSE                    (0x30UL)  /* QEI0 pause */
#define HPM_TRGM0_OUTPUT_SRC_QEI1_PAUSE                    (0x31UL)  /* QEI1 pause */
#define HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN_0                (0x32UL)  /* QEO0 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN_1                (0x33UL)  /* QEO0 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_QEO1_TRIG_IN_0                (0x34UL)  /* QEO1 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO1_TRIG_IN_1                (0x35UL)  /* QEO1 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_WIN_CH_0                (0x36UL)  /* ACMP0 CH0 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_WIN_CH_1                (0x37UL)  /* ACMP0 CH1 window */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_CAPT_2                 (0x38UL)  /* GPTMR0 channel2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_CAPT_3                 (0x39UL)  /* GPTMR0 channel3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_CAPT_2                 (0x3AUL)  /* GPTMR1 channel2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_CAPT_3                 (0x3BUL)  /* GPTMR1 channel3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_CAPT_2                 (0x3CUL)  /* GPTMR2 channel2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_CAPT_3                 (0x3DUL)  /* GPTMR2 channel3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_CAPT_2                 (0x3EUL)  /* GPTMR3 channel2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_CAPT_3                 (0x3FUL)  /* GPTMR3 channel3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_SYNC                   (0x40UL)  /* GPTMR0 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_SYNC                   (0x41UL)  /* GPTMR1 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_SYNC                   (0x42UL)  /* GPTMR2 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_SYNC                   (0x43UL)  /* GPTMR3 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_PTPC_CAPT_0                   (0x44UL)  /* PTPC capture in0 */
#define HPM_TRGM0_OUTPUT_SRC_PTPC_CAPT_1                   (0x45UL)  /* PTPC capture in1 */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG0                    (0x46UL)  /* UART0 ~ UART3 trigger event */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG1                    (0x47UL)  /* UART4 ~ UART7 trigger event */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC00                (0x48UL)  /* SDM0 sync event input0 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC01                (0x49UL)  /* SDM0 sync event input1 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC02                (0x4AUL)  /* SDM0 sync event input2 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC03                (0x4BUL)  /* SDM0 sync event input3 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC04                (0x4CUL)  /* SDM0 sync event input4 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC05                (0x4DUL)  /* SDM0 sync event input5 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC06                (0x4EUL)  /* SDM0 sync event input6 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC07                (0x4FUL)  /* SDM0 sync event input7 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC08                (0x50UL)  /* SDM0 sync event input8 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC09                (0x51UL)  /* SDM0 sync event input9 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC10                (0x52UL)  /* SDM0 sync event input10 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC11                (0x53UL)  /* SDM0 sync event input11 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC12                (0x54UL)  /* SDM0 sync event input12 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC13                (0x55UL)  /* SDM0 sync event input13 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC14                (0x56UL)  /* SDM0 sync event input14 */
#define HPM_TRGM0_OUTPUT_SRC_SDM0_PWM_SOC15                (0x57UL)  /* SDM0 sync event input15 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_00                     (0x58UL)  /* PLB trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_01                     (0x59UL)  /* PLB trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_02                     (0x5AUL)  /* PLB trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_03                     (0x5BUL)  /* PLB trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_04                     (0x5CUL)  /* PLB trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_05                     (0x5DUL)  /* PLB trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_06                     (0x5EUL)  /* PLB trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_07                     (0x5FUL)  /* PLB trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_08                     (0x60UL)  /* PLB trigger in8 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_09                     (0x61UL)  /* PLB trigger in9 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_10                     (0x62UL)  /* PLB trigger in10 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_11                     (0x63UL)  /* PLB trigger in11 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_12                     (0x64UL)  /* PLB trigger in12 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_13                     (0x65UL)  /* PLB trigger in13 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_14                     (0x66UL)  /* PLB trigger in14 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_15                     (0x67UL)  /* PLB trigger in15 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_16                     (0x68UL)  /* PLB trigger in16 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_17                     (0x69UL)  /* PLB trigger in17 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_18                     (0x6AUL)  /* PLB trigger in18 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_19                     (0x6BUL)  /* PLB trigger in19 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_20                     (0x6CUL)  /* PLB trigger in20 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_21                     (0x6DUL)  /* PLB trigger in21 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_22                     (0x6EUL)  /* PLB trigger in22 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_23                     (0x6FUL)  /* PLB trigger in23 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_24                     (0x70UL)  /* PLB trigger in24 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_25                     (0x71UL)  /* PLB trigger in25 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_26                     (0x72UL)  /* PLB trigger in26 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_27                     (0x73UL)  /* PLB trigger in27 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_28                     (0x74UL)  /* PLB trigger in28 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_29                     (0x75UL)  /* PLB trigger in29 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_30                     (0x76UL)  /* PLB trigger in30 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_31                     (0x77UL)  /* PLB trigger in31 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_32                     (0x78UL)  /* PLB trigger in32 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_33                     (0x79UL)  /* PLB trigger in33 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_34                     (0x7AUL)  /* PLB trigger in34 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_35                     (0x7BUL)  /* PLB trigger in35 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_36                     (0x7CUL)  /* PLB trigger in36 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_37                     (0x7DUL)  /* PLB trigger in37 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_38                     (0x7EUL)  /* PLB trigger in38 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_39                     (0x7FUL)  /* PLB trigger in39 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_40                     (0x80UL)  /* PLB trigger in40 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_41                     (0x81UL)  /* PLB trigger in41 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_42                     (0x82UL)  /* PLB trigger in42 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_43                     (0x83UL)  /* PLB trigger in43 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_44                     (0x84UL)  /* PLB trigger in44 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_45                     (0x85UL)  /* PLB trigger in45 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_46                     (0x86UL)  /* PLB trigger in46 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_47                     (0x87UL)  /* PLB trigger in47 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_48                     (0x88UL)  /* PLB trigger in48 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_49                     (0x89UL)  /* PLB trigger in49 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_50                     (0x8AUL)  /* PLB trigger in50 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_51                     (0x8BUL)  /* PLB trigger in51 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_52                     (0x8CUL)  /* PLB trigger in52 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_53                     (0x8DUL)  /* PLB trigger in53 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_54                     (0x8EUL)  /* PLB trigger in54 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_55                     (0x8FUL)  /* PLB trigger in55 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_56                     (0x90UL)  /* PLB trigger in56 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_57                     (0x91UL)  /* PLB trigger in57 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_58                     (0x92UL)  /* PLB trigger in58 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_59                     (0x93UL)  /* PLB trigger in59 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_60                     (0x94UL)  /* PLB trigger in60 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_61                     (0x95UL)  /* PLB trigger in61 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_62                     (0x96UL)  /* PLB trigger in62 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_63                     (0x97UL)  /* PLB trigger in63 */
#define HPM_TRGM0_OUTPUT_SRC_SYNT_SYNC                     (0x98UL)  /* SYNT sync trigger */
#define HPM_TRGM0_OUTPUT_SRC_DCCS_TRIG                     (0x99UL)  /* DCDC clock control system trigger */
#define HPM_TRGM0_OUTPUT_SRC_IRQ_TRGM_0                    (0x9AUL)  /* TRGM interrupt0 */
#define HPM_TRGM0_OUTPUT_SRC_IRQ_TRGM_1                    (0x9BUL)  /* TRGM interrupt1 */
#define HPM_TRGM0_OUTPUT_SRC_DMA_TRGM_0                    (0x9CUL)  /* TRGM dma request0 */
#define HPM_TRGM0_OUTPUT_SRC_DMA_TRGM_1                    (0x9DUL)  /* TRGM dma request1 */
#define HPM_TRGM0_OUTPUT_SRC_ESC_LATCH0                    (0x9EUL)  /* ESC latch0 */

/* trgm0_filter mux definitions */
#define HPM_TRGM0_FILTER_SRC_PWM0_P_0                      (0x0UL)   /* PWM0 capture in0 */
#define HPM_TRGM0_FILTER_SRC_PWM0_P_1                      (0x1UL)   /* PWM0 capture in1 */
#define HPM_TRGM0_FILTER_SRC_PWM0_P_2                      (0x2UL)   /* PWM0 capture in2 */
#define HPM_TRGM0_FILTER_SRC_PWM0_P_3                      (0x3UL)   /* PWM0 capture in3 */
#define HPM_TRGM0_FILTER_SRC_PWM0_P_4                      (0x4UL)   /* PWM0 capture in4 */
#define HPM_TRGM0_FILTER_SRC_PWM0_P_5                      (0x5UL)   /* PWM0 capture in5 */
#define HPM_TRGM0_FILTER_SRC_PWM0_P_6                      (0x6UL)   /* PWM0 capture in6 */
#define HPM_TRGM0_FILTER_SRC_PWM0_P_7                      (0x7UL)   /* PWM0 capture in7 */
#define HPM_TRGM0_FILTER_SRC_PWM1_P_0                      (0x8UL)   /* PWM1 capture in0 */
#define HPM_TRGM0_FILTER_SRC_PWM1_P_1                      (0x9UL)   /* PWM1 capture in1 */
#define HPM_TRGM0_FILTER_SRC_PWM1_P_2                      (0xAUL)   /* PWM1 capture in2 */
#define HPM_TRGM0_FILTER_SRC_PWM1_P_3                      (0xBUL)   /* PWM1 capture in3 */
#define HPM_TRGM0_FILTER_SRC_PWM1_P_4                      (0xCUL)   /* PWM1 capture in4 */
#define HPM_TRGM0_FILTER_SRC_PWM1_P_5                      (0xDUL)   /* PWM1 capture in5 */
#define HPM_TRGM0_FILTER_SRC_PWM1_P_6                      (0xEUL)   /* PWM1 capture in6 */
#define HPM_TRGM0_FILTER_SRC_PWM1_P_7                      (0xFUL)   /* PWM1 capture in7 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P00                     (0x10UL)  /* TRGM input0 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P01                     (0x11UL)  /* TRGM input1 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P02                     (0x12UL)  /* TRGM input2 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P03                     (0x13UL)  /* TRGM input3 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P04                     (0x14UL)  /* TRGM input4 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P05                     (0x15UL)  /* TRGM input5 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P06                     (0x16UL)  /* TRGM input6 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P07                     (0x17UL)  /* TRGM input7 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P08                     (0x18UL)  /* TRGM input8 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P09                     (0x19UL)  /* TRGM input9 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P10                     (0x1AUL)  /* TRGM input10 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P11                     (0x1BUL)  /* TRGM input11 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P12                     (0x1CUL)  /* TRGM input12 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P13                     (0x1DUL)  /* TRGM input13 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P14                     (0x1EUL)  /* TRGM input14 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P15                     (0x1FUL)  /* TRGM input15 */

/* trgm0_dma mux definitions */
#define HPM_TRGM0_DMA_SRC_PWM0_0                           (0x0UL)   /* PWM0 DMA0 */
#define HPM_TRGM0_DMA_SRC_PWM0_1                           (0x1UL)   /* PWM0 DMA1 */
#define HPM_TRGM0_DMA_SRC_PWM0_2                           (0x2UL)   /* PWM0 DMA2 */
#define HPM_TRGM0_DMA_SRC_PWM0_3                           (0x3UL)   /* PWM0 DMA3 */
#define HPM_TRGM0_DMA_SRC_PWM1_0                           (0x4UL)   /* PWM1 DMA0 */
#define HPM_TRGM0_DMA_SRC_PWM1_1                           (0x5UL)   /* PWM1 DMA1 */
#define HPM_TRGM0_DMA_SRC_PWM1_2                           (0x6UL)   /* PWM1 DMA2 */
#define HPM_TRGM0_DMA_SRC_PWM1_3                           (0x7UL)   /* PWM1 DMA3 */
#define HPM_TRGM0_DMA_SRC_QEI0                             (0x8UL)   /* QEI0 */
#define HPM_TRGM0_DMA_SRC_QEI1                             (0x9UL)   /* QEI1 */
#define HPM_TRGM0_DMA_SRC_TRGM_0                           (0xAUL)   /* TRGM DMA0 */
#define HPM_TRGM0_DMA_SRC_TRGM_1                           (0xBUL)   /* TRGM DMA1 */



#endif /* HPM_TRGMMUX_SRC_H */
