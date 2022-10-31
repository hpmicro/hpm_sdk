/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_INTERRUPT_H
#define HPM_INTERRUPT_H
#include "riscv/riscv_core.h"
#include "hpm_common.h"
#include "hpm_plic_drv.h"

/**
 * @brief INTERRUPT driver APIs
 * @defgroup irq_interface INTERRUPT driver APIs
 * @{
 */

#define M_MODE 0    /*!< Machine mode */
#define S_MODE 1    /*!< Supervisor mode */

#ifdef __cplusplus
extern "C" {
#endif

/* Machine mode API: these APIs are supposed to be called at machine mode */

/**
 * @brief   Enable global IRQ with mask
 *
 * @param[in] mask interrupt mask to be enabaled
 */
ATTR_ALWAYS_INLINE static inline void enable_global_irq(uint32_t mask)
{
    set_csr(CSR_MSTATUS, mask);
}

/**
 * @brief   Disable global IRQ with mask
 *
 * @param[in] mask interrupt mask to be disabled
 */
ATTR_ALWAYS_INLINE static inline void disable_global_irq(uint32_t mask)
{
    clear_csr(CSR_MSTATUS, mask);
}

/**
 * @brief   Enable IRQ from interrupt controller
 *
 */
ATTR_ALWAYS_INLINE static inline void enable_irq_from_intc(void)
{
    set_csr(CSR_MIE, CSR_MIE_MEIE_MASK);
}

/**
 * @brief   Disable IRQ from interrupt controller
 *
 */
ATTR_ALWAYS_INLINE static inline void disable_irq_from_intc(void)
{
    clear_csr(CSR_MIE, CSR_MIE_MEIE_MASK);
}

/**
 * @brief Enable machine timer IRQ
 */
ATTR_ALWAYS_INLINE static inline void enable_mchtmr_irq(void)
{
    set_csr(CSR_MIE, CSR_MIE_MTIE_MASK);
}

/**
 * @brief  Disable machine timer IRQ
 *
 */
ATTR_ALWAYS_INLINE static inline void disable_mchtmr_irq(void)
{
    clear_csr(CSR_MIE, CSR_MIE_MTIE_MASK);
}

/*
 * CPU Machine SWI control
 *
 * Machine SWI (MSIP) is connected to PLICSW irq 1.
 */
#define PLICSWI 1

/**
 * @brief   Initialize software interrupt
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_m_init_swi(void)
{
    __plic_enable_irq(HPM_PLICSW_BASE, HPM_PLIC_TARGET_M_MODE, PLICSWI);
}


/**
 * @brief   Enable software interrupt
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_m_enable_swi(void)
{
    set_csr(CSR_MIE, CSR_MIE_MSIE_MASK);
}


/**
 * @brief   Disable software interrupt
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_m_disable_swi(void)
{
    clear_csr(CSR_MIE, CSR_MIE_MSIE_MASK);
}


/**
 * @brief   Trigger software interrupt
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_m_trigger_swi(void)
{
    __plic_set_irq_pending(HPM_PLICSW_BASE, PLICSWI);
}

/**
 * @brief Claim software interrupt
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_m_claim_swi(void)
{
    __plic_claim_irq(HPM_PLICSW_BASE, 0);
}

/**
 * @brief   Complete software interrupt
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_m_complete_swi(void)
{
    __plic_complete_irq(HPM_PLICSW_BASE, HPM_PLIC_TARGET_M_MODE, PLICSWI);
}

/*
 * @brief Enable IRQ for machine mode
 *
 * @param[in] irq Interrupt number
 */
#define intc_m_enable_irq(irq) \
    intc_enable_irq(HPM_PLIC_TARGET_M_MODE, irq)

/*
 * @brief Disable IRQ for machine mode
 *
 * @param[in] irq Interrupt number
 */
#define intc_m_disable_irq(irq) \
    intc_disable_irq(HPM_PLIC_TARGET_M_MODE, irq)

#define intc_m_set_threshold(threshold) \
    intc_set_threshold(HPM_PLIC_TARGET_M_MODE, threshold)

/*
 * @brief Complete IRQ for machine mode
 *
 * @param[in] irq Interrupt number
 */
#define intc_m_complete_irq(irq) \
    intc_complete_irq(HPM_PLIC_TARGET_M_MODE, irq)

/*
 * @brief Claim IRQ for machine mode
 *
 */
#define intc_m_claim_irq() intc_claim_irq(HPM_PLIC_TARGET_M_MODE)

/*
 * @brief Enable IRQ for machine mode with priority
 *
 * @param[in] irq Interrupt number
 * @param[in] priority Priority of interrupt
 */
#define intc_m_enable_irq_with_priority(irq, priority) \
    do { \
        intc_set_irq_priority(irq, priority); \
        intc_m_enable_irq(irq); \
    } while (0)

/*
 * @brief Enable specific interrupt
 *
 * @param[in] target Target to handle specific interrupt
 * @param[in] irq Interrupt number
 */
ATTR_ALWAYS_INLINE static inline void intc_enable_irq(uint32_t target, uint32_t irq)
{
    __plic_enable_irq(HPM_PLIC_BASE, target, irq);
}

/**
 * @brief   Set interrupt priority
 *
 * @param[in] irq Interrupt number
 * @param[in] priority Priority of interrupt
 */
ATTR_ALWAYS_INLINE static inline void intc_set_irq_priority(uint32_t irq, uint32_t priority)
{
    __plic_set_irq_priority(HPM_PLIC_BASE, irq, priority);
}

/**
 * @brief   Disable specific interrupt
 *
 * @param[in] target Target to handle specific interrupt
 * @param[in] irq Interrupt number
 */
ATTR_ALWAYS_INLINE static inline void intc_disable_irq(uint32_t target, uint32_t irq)
{
    __plic_disable_irq(HPM_PLIC_BASE, target, irq);
}

/**
 * @brief   Set interrupt threshold
 *
 * @param[in] target Target to handle specific interrupt
 * @param[in] threshold Threshold of IRQ can be serviced
 */
ATTR_ALWAYS_INLINE static inline void intc_set_threshold(uint32_t target, uint32_t threshold)
{
    __plic_set_threshold(HPM_PLIC_BASE, target, threshold);
}

/**
 * @brief   Claim IRQ
 *
 * @param[in] target Target to handle specific interrupt
 *
 */
ATTR_ALWAYS_INLINE static inline uint32_t intc_claim_irq(uint32_t target)
{
    return __plic_claim_irq(HPM_PLIC_BASE, target);
}

/**
 * @brief   Complete IRQ
 *
 * @param[in] target Target to handle specific interrupt
 * @param[in] irq Specific IRQ to be completed
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_complete_irq(uint32_t target, uint32_t irq)
{
    __plic_complete_irq(HPM_PLIC_BASE, target, irq);
}

/*
 * Vectored based irq install and uninstall
 */
/* Machine mode */
extern int __vector_table[];
extern void default_irq_entry(void);

/**
 * @brief   Install ISR for certain IRQ for ram based vector table
 *
 * @param[in] irq Target interrupt number
 * @param[in] isr Interrupt service routine
 *
 */
ATTR_ALWAYS_INLINE static inline void install_isr(uint32_t irq, uint32_t isr)
{
    __vector_table[irq] = isr;
}

/**
 * @brief   Uninstall ISR for certain IRQ for ram based vector table
 *
 * @param[in] irq Target interrupt number
 *
 */
ATTR_ALWAYS_INLINE static inline void uninstall_isr(uint32_t irq)
{
    __vector_table[irq] = (int)default_irq_entry;
}

/*
 * Inline nested irq entry/exit macros
 */
/*
 * @brief Save CSR
 * @param[in] r Target CSR to be saved
 */
#define SAVE_CSR(r)                                     register long __##r = read_csr(r);

/*
 * @brief Restore macro
 *
 * @param[in] r Target CSR to be restored
 */
#define RESTORE_CSR(r)                                  write_csr(r, __##r);

#if SUPPORT_PFT_ARCH
#define SAVE_MXSTATUS()                                 SAVE_CSR(CSR_MXSTATUS)
#define RESTORE_MXSTATUS()                              RESTORE_CSR(CSR_MXSTATUS)
#else
#define SAVE_MXSTATUS()
#define RESTORE_MXSTATUS()
#endif

#ifdef __riscv_flen
#define SAVE_FCSR()                                     register int __fcsr = read_fcsr();
#define RESTORE_FCSR()                                  write_fcsr(__fcsr);
#else
#define SAVE_FCSR()
#define RESTORE_FCSR()
#endif

#ifdef __riscv_dsp
#define SAVE_UCODE()                                    SAVE_CSR(CSR_UCODE)
#define RESTORE_UCODE()                                 RESTORE_CSR(CSR_UCODE)
#else
#define SAVE_UCODE()
#define RESTORE_UCODE()
#endif

#ifdef __riscv_flen
#if __riscv_flen == 32
/* RV32I caller registers + MCAUSE + MEPC + MSTATUS +MXSTATUS + 20 FPU caller registers +FCSR + UCODE (DSP)  */
#define CONTEXT_REG_NUM (4*(16 + 4 + 20 + 1 + 1))
#else   /* __riscv_flen = 64 */
/* RV32I caller registers + MCAUSE + MEPC + MSTATUS +MXSTATUS + 20 DFPU caller + FCSR registers + UCODE (DSP)  */
#define CONTEXT_REG_NUM (4*(16 + 4 + 20*2 + 1 + 1))
#endif

#else
/* RV32I caller registers + MCAUSE + MEPC + MSTATUS +MXSTATUS + UCODE (DSP)*/
#define CONTEXT_REG_NUM (4*(16 + 4 + 1))
#endif

#ifdef __riscv_flen
/*
 * Save FPU caller registers:
 * NOTE: To simplify the logic, the FPU caller registers are always stored at word offset 20 in the stack
 */
#if __riscv_flen == 32
#define SAVE_FPU_CONTEXT()  { \
    __asm volatile("fsw ft0, 20*4(sp)\n\
             fsw ft1,  21*4(sp) \n\
             fsw ft2,  22*4(sp) \n\
             fsw ft3,  23*4(sp) \n\
             fsw ft4,  24*4(sp) \n\
             fsw ft5,  25*4(sp) \n\
             fsw ft6,  26*4(sp) \n\
             fsw ft7,  27*4(sp) \n\
             fsw fa0,  28*4(sp) \n\
             fsw fa1,  29*4(sp) \n\
             fsw fa2,  30*4(sp) \n\
             fsw fa3,  31*4(sp) \n\
             fsw fa4,  32*4(sp) \n\
             fsw fa5,  33*4(sp) \n\
             fsw fa6,  34*4(sp) \n\
             fsw fa7,  35*4(sp) \n\
             fsw ft8,  36*4(sp) \n\
             fsw ft9,  37*4(sp) \n\
             fsw ft10, 38*4(sp) \n\
             fsw ft11, 39*4(sp) \n\
             frsr t6 \n\
             sw t6,    40*4(sp) \n");\
}

/*
 * Restore FPU caller registers:
 * NOTE: To simplify the logic, the FPU caller registers are always stored at word offset 20 in the stack
 */
#define RESTORE_FPU_CONTEXT() { \
    __asm volatile("flw ft0, 20*4(sp)\n\
             flw ft1,  21*4(sp) \n\
             flw ft2,  22*4(sp) \n\
             flw ft3,  23*4(sp) \n\
             flw ft4,  24*4(sp) \n\
             flw ft5,  25*4(sp) \n\
             flw ft6,  26*4(sp) \n\
             flw ft7,  27*4(sp) \n\
             flw fa0,  28*4(sp) \n\
             flw fa1,  29*4(sp) \n\
             flw fa2,  30*4(sp) \n\
             flw fa3,  31*4(sp) \n\
             flw fa4,  32*4(sp) \n\
             flw fa5,  33*4(sp) \n\
             flw fa6,  34*4(sp) \n\
             flw fa7,  35*4(sp) \n\
             flw ft8,  36*4(sp) \n\
             flw ft9,  37*4(sp) \n\
             flw ft10, 38*4(sp) \n\
             flw ft11, 39*4(sp) \n\
             lw t6,    40*4(sp) \n\
             fssr t6, t6 \n");\
}
#else /*__riscv_flen == 64*/
#define SAVE_FPU_CONTEXT()  { \
    __asm volatile("fsd ft0, 20*4(sp)\n\
             fsd ft1,  22*4(sp) \n\
             fsd ft2,  24*4(sp) \n\
             fsd ft3,  26*4(sp) \n\
             fsd ft4,  28*4(sp) \n\
             fsd ft5,  30*4(sp) \n\
             fsd ft6,  32*4(sp) \n\
             fsd ft7,  34*4(sp) \n\
             fsd fa0,  36*4(sp) \n\
             fsd fa1,  38*4(sp) \n\
             fsd fa2,  40*4(sp) \n\
             fsd fa3,  42*4(sp) \n\
             fsd fa4,  44*4(sp) \n\
             fsd fa5,  46*4(sp) \n\
             fsd fa6,  48*4(sp) \n\
             fsd fa7,  50*4(sp) \n\
             fsd ft8,  52*4(sp) \n\
             fsd ft9,  54*4(sp) \n\
             fsd ft10, 56*4(sp) \n\
             fsd ft11, 58*4(sp) \n\
             frsr t6 \n\
             sw t6,    60*4(sp) \n");\
}

/*
 * Restore FPU caller registers:
 * NOTE: To simplify the logic, the FPU caller registers are always stored at word offset 20 in the stack
 */
#define RESTORE_FPU_CONTEXT() { \
    __asm volatile("fld ft0, 20*4(sp)\n\
             fld ft1,  22*4(sp) \n\
             fld ft2,  24*4(sp) \n\
             fld ft3,  26*4(sp) \n\
             fld ft4,  28*4(sp) \n\
             fld ft5,  30*4(sp) \n\
             fld ft6,  32*4(sp) \n\
             fld ft7,  34*4(sp) \n\
             fld fa0,  36*4(sp) \n\
             fld fa1,  38*4(sp) \n\
             fld fa2,  40*4(sp) \n\
             fld fa3,  42*4(sp) \n\
             fld fa4,  44*4(sp) \n\
             fld fa5,  46*4(sp) \n\
             fld fa6,  48*4(sp) \n\
             fld fa7,  50*4(sp) \n\
             fld ft8,  52*4(sp) \n\
             fld ft9,  54*4(sp) \n\
             fld ft10, 56*4(sp) \n\
             fld ft11, 58*4(sp) \n\
             lw t6,    60*4(sp) \n\
             fssr t6, t6 \n");\
}
#endif
#else
#define SAVE_FPU_CONTEXT()
#define RESTORE_FPU_CONTEXT()
#endif

/**
 * @brief Save the caller registers based on the RISC-V ABI specification
 */
#define SAVE_CALLER_CONTEXT()   { \
    __asm volatile("addi sp, sp, %0" : : "i"(-CONTEXT_REG_NUM) :);\
    __asm volatile("sw ra,  0*4(sp)\n\
            sw t0,  1*4(sp) \n\
            sw t1,  2*4(sp) \n\
            sw t2,  3*4(sp) \n\
            sw a0,  4*4(sp) \n\
            sw a1,  5*4(sp) \n\
            sw a2,  6*4(sp) \n\
            sw a3,  7*4(sp) \n\
            sw a4,  8*4(sp) \n\
            sw a5,  9*4(sp) \n\
            sw a6, 10*4(sp) \n\
            sw a7, 11*4(sp) \n\
            sw t3, 12*4(sp) \n\
            sw t4, 13*4(sp) \n\
            sw t5, 14*4(sp) \n\
            sw t6, 15*4(sp)"); \
            SAVE_FPU_CONTEXT(); \
}

/**
 * @brief Restore the caller registers based on the RISC-V ABI specification
 */
#define RESTORE_CALLER_CONTEXT() { \
        __asm volatile("lw ra,  0*4(sp)\n\
            lw t0,  1*4(sp) \n\
            lw t1,  2*4(sp) \n\
            lw t2,  3*4(sp) \n\
            lw a0,  4*4(sp) \n\
            lw a1,  5*4(sp) \n\
            lw a2,  6*4(sp) \n\
            lw a3,  7*4(sp) \n\
            lw a4,  8*4(sp) \n\
            lw a5,  9*4(sp) \n\
            lw a6, 10*4(sp) \n\
            lw a7, 11*4(sp) \n\
            lw t3, 12*4(sp) \n\
            lw t4, 13*4(sp) \n\
            lw t5, 14*4(sp) \n\
            lw t6, 15*4(sp) \n");\
            RESTORE_FPU_CONTEXT(); \
        __asm volatile("addi sp, sp, %0" : : "i"(CONTEXT_REG_NUM) :);\
}

#ifdef __riscv_dsp

#if defined(__riscv_flen)
#if __riscv_flen == 32
/*
 * Save DSP context
 * NOTE: DSP context registers are stored at word offset 41 in the stack
 */
#define SAVE_DSP_CONTEXT() { \
    __asm volatile("csrr t6, ucode\n\
            sw t6, 41*4(sp)\n");  \
}
/*
 * @brief Restore DSP context
 * @note DSP context registers are stored at word offset 41 in the stack
 */
#define RESTORE_DSP_CONTEXT() {\
    __asm volatile("lw t6, 41*4(sp)\n\
            csrw ucode, t6 \n"); \
}
#else
/*
 * Save DSP context
 * NOTE: DSP context registers are stored at word offset 61 in the stack
 */
#define SAVE_DSP_CONTEXT() { \
    __asm volatile("csrr t6, ucode\n\
            sw t6, 61*4(sp)\n");  \
}
/*
 * @brief Restore DSP context
 * @note  DSP context registers are  stored at word offset 61 in the stack
 */
#define RESTORE_DSP_CONTEXT() {\
    __asm volatile("lw t6, 61*4(sp)\n\
            csrw ucode, t6 \n"); \
}
#endif
#else /* FPU is not enabled */
/*
 * Save DSP context
 * NOTE:  DSP context registers are stored at word offset 20 in the stack
 */
#define SAVE_DSP_CONTEXT() { \
    __asm volatile("csrr t6, ucode\n\
            sw t6, 20*4(sp)\n");  \
}
/*
 * @brief Restore DSP context
 * @note DSP context registers are stored at word offset 20 in the stack
 */
#define RESTORE_DSP_CONTEXT() {\
    __asm volatile("lw t6, 20*4(sp)\n\
            csrw ucode, t6 \n"); \
}
#endif
#else
#define SAVE_DSP_CONTEXT()
#define RESTORE_DSP_CONTEXT()
#endif

/*
 * @brief Enter Nested IRQ Handling
 * @note To simplify the logic, Nested IRQ related registers are stored in the stack as below:
 *       MCAUSE - word offset 16 (not used in the vectored mode)
 *       EPC - word offset 17
 *       MSTATUS = word offset 18
 *       MXSTATUS = word offset 19
 */
#define ENTER_NESTED_IRQ_HANDLING_M() {\
    __asm volatile("csrr t6, mepc\n\
            sw t6, 17*4(sp) \n\
            csrr t6, mstatus \n\
            sw t6, 18*4(sp) \n\
            csrr t6, %0\n\
            sw t6, 19*4(sp) \n\
    " : : "i" CSR_MXSTATUS);\
    SAVE_DSP_CONTEXT(); \
    __asm volatile("csrrsi t6, mstatus, 8\n"); \
}

/*
 * @brief Complete IRQ Handling
 */
#define COMPLETE_IRQ_HANDLING_M(irq_num) {\
    clear_csr(CSR_MSTATUS, CSR_MSTATUS_MIE_MASK);\
    __asm volatile("lui t0, 0xe4200\n\
            addi t0, t0, 0x4 \n");\
    __asm volatile("li t1, %0" : : "i" (irq_num) :); \
    __asm volatile("sw t1, 0(t0)\n\
            fence io, io\n"); \
}

/*
 * @brief Exit Nested IRQ Handling
 * @note To simplify the logic, Nested IRQ related registers are stored in the stack as below:
 *       MCAUSE - word offset 16 (not used in the vectored mode)
 *       EPC - word offset 17
 *       MSTATUS = word offset 18
 *       MXSTATUS = word offset 19
 */
#define EXIT_NESTED_IRQ_HANDLING_M() { \
    __asm volatile("lw t6, 18*4(sp) \n\
            csrw mstatus, t6 \n\
            lw t6, 17*4(sp) \n\
            csrw mepc, t6 \n\
            lw t6, 19*4(sp) \n\
            csrw %0, t6 \n" : : "i" CSR_MXSTATUS);\
            RESTORE_DSP_CONTEXT(); \
}


/* @brief Nested IRQ entry macro : Save CSRs and enable global irq. */
#define NESTED_IRQ_ENTER()                              \
        SAVE_CSR(CSR_MEPC)                              \
        SAVE_CSR(CSR_MSTATUS)                           \
        SAVE_MXSTATUS()                                 \
        SAVE_FCSR()                                     \
        SAVE_UCODE()                                    \
        set_csr(CSR_MSTATUS, CSR_MSTATUS_MIE_MASK);

/* @brief Nested IRQ exit macro : Restore CSRs */
#define NESTED_IRQ_EXIT()                               \
        RESTORE_CSR(CSR_MSTATUS)                        \
        RESTORE_CSR(CSR_MEPC)                           \
        RESTORE_MXSTATUS()                              \
        RESTORE_FCSR()                                  \
        RESTORE_UCODE()

/*
 * @brief Nested IRQ exit macro : Restore CSRs
 * @param[in] irq Target interrupt number
 */
#define NESTED_VPLIC_COMPLETE_INTERRUPT(irq)            \
do {                                                    \
    clear_csr(CSR_MIE, CSR_MIP_MEIP_MASK);                       \
    __plic_complete_irq(HPM_PLIC_BASE, HPM_PLIC_TARGET_M_MODE, irq);  \
    __asm volatile("fence io, io");                     \
    set_csr(CSR_MIE, CSR_MIP_MEIP_MASK);                         \
} while (0)

#ifdef __cplusplus
#define HPM_EXTERN_C extern "C"
#else
#define HPM_EXTERN_C
#endif

#define ISR_NAME_M(irq_num) default_isr_##irq_num
/**
 * @brief Declare an external interrupt handler for machine mode
 *
 * @param[in] irq_num - IRQ number index
 * @param[in] isr - Application IRQ handler function pointer
 */
#ifndef USE_NONVECTOR_MODE
#define SDK_DECLARE_EXT_ISR_M(irq_num, isr) \
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C void ISR_NAME_M(irq_num)(void) __attribute__((section(".isr_vector")));\
void ISR_NAME_M(irq_num)(void) \
{ \
    SAVE_CALLER_CONTEXT(); \
    ENTER_NESTED_IRQ_HANDLING_M();\
    __asm volatile("la t1, %0\n\t" : : "i" (isr) : );\
    __asm volatile("jalr t1\n");\
    COMPLETE_IRQ_HANDLING_M(irq_num);\
    EXIT_NESTED_IRQ_HANDLING_M();\
    RESTORE_CALLER_CONTEXT();\
    __asm volatile("mret\n");\
}
#else
#define SDK_DECLARE_EXT_ISR_M(irq_num, isr) \
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C void ISR_NAME_M(irq_num)(void) __attribute__((section(".isr_vector")));\
void ISR_NAME_M(irq_num)(void) \
{ \
    isr(); \
}
#endif


/**
 * @brief Declare machine timer interrupt handler
 *
 * @param[in] isr - MCHTMR IRQ handler function pointer
 */
#define SDK_DECLARE_MCHTMR_ISR(isr) \
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C void mchtmr_isr(void) __attribute__((section(".isr_vector"))); \
void mchtmr_isr(void) \
{ \
    isr();\
}

/**
 * @brief Declare machine software interrupt handler
 *
 * @param[in] isr - SWI IRQ handler function pointer
 */
#define SDK_DECLARE_SWI_ISR(isr)\
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C void swi_isr(void) __attribute__((section(".isr_vector"))); \
void swi_isr(void)  \
{ \
    isr();\
}


#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* HPM_INTERRUPT_H */
