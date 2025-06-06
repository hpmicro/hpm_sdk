/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_INTERRUPT_H
#define HPM_INTERRUPT_H
#include "hpm_common.h"
#include "hpm_csr_drv.h"
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
 * @brief   Disable global IRQ with mask and return mstatus
 *
 * @param[in] mask interrupt mask to be disabled
 * @retval current mstatus value before irq mask is disabled
 */
ATTR_ALWAYS_INLINE static inline uint32_t disable_global_irq(uint32_t mask)
{
    return read_clear_csr(CSR_MSTATUS, mask);
}

/**
 * @brief   Restore global IRQ with mask
 *
 * @param[in] mask interrupt mask to be restored
 */
ATTR_ALWAYS_INLINE static inline void restore_global_irq(uint32_t mask)
{
    set_csr(CSR_MSTATUS, mask);
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

/**
 * @brief   Delegate IRQ handling
 *
 * @param[in] mask interrupt mask to be delegated
 */
ATTR_ALWAYS_INLINE static inline void delegate_irq(uint32_t mask)
{
    set_csr(CSR_MIDELEG, mask);
}

/**
 * @brief   Undelegate IRQ handling
 *
 * @param[in] mask interrupt mask to be undelegated
 */
ATTR_ALWAYS_INLINE static inline void undelegate_irq(uint32_t mask)
{
    clear_csr(CSR_MIDELEG, mask);
}


/* Supervisor mode API: these APIs are supposed to be called at supervisor mode */

/**
 * @brief   Enable global IRQ with mask for supervisor mode
 *
 * @param[in] mask interrupt mask to be enabaled
 */
ATTR_ALWAYS_INLINE static inline void enable_s_global_irq(uint32_t mask)
{
    set_csr(CSR_SSTATUS, mask);
}

/**
 * @brief   Disable global IRQ with mask and return sstatus for supervisor mode
 *
 * @param[in] mask interrupt mask to be disabled
 * @retval current sstatus value before irq mask is disabled
 */
ATTR_ALWAYS_INLINE static inline uint32_t disable_s_global_irq(uint32_t mask)
{
    return read_clear_csr(CSR_SSTATUS, mask);
}

/**
 * @brief   Restore global IRQ with mask for supervisor mode
 *
 * @param[in] mask interrupt mask to be restored
 */
ATTR_ALWAYS_INLINE static inline void restore_s_global_irq(uint32_t mask)
{
    set_csr(CSR_SSTATUS, mask);
}

/**
 * @brief   Disable IRQ from interrupt controller for supervisor mode
 *
 */
ATTR_ALWAYS_INLINE static inline void disable_s_irq_from_intc(void)
{
    clear_csr(CSR_SIE, CSR_SIE_SEIE_MASK);
}

/**
 * @brief   Enable IRQ from interrupt controller for supervisor mode
 *
 */
ATTR_ALWAYS_INLINE static inline void enable_s_irq_from_intc(void)
{
    set_csr(CSR_SIE, CSR_SIE_SEIE_MASK);
}

/**
 * @brief Enable machine timer IRQ for supervisor mode
 */
ATTR_ALWAYS_INLINE static inline void enable_s_mchtmr_irq(void)
{
    set_csr(CSR_SIE, CSR_SIE_STIE_MASK);
}

/**
 * @brief  Disable machine timer IRQ
 *
 */
ATTR_ALWAYS_INLINE static inline void disable_s_mchtmr_irq(void)
{
    clear_csr(CSR_SIE, CSR_SIE_STIE_MASK);
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

#define intc_m_get_threshold() \
    intc_get_threshold(HPM_PLIC_TARGET_M_MODE)

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



/* Supervisor mode */

/**
 * @brief   Enable software interrupt for supervisor mode
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_s_enable_swi(void)
{
    set_csr(CSR_SIE, CSR_SIE_SSIE_MASK);
}


/**
 * @brief   Disable software interrupt for supervisor mode
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_s_disable_swi(void)
{
    clear_csr(CSR_SIE, CSR_SIE_SSIE_MASK);
}


/**
 * @brief   Trigger software interrupt for supervisor mode
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_s_trigger_swi(void)
{
    set_csr(CSR_SIP, CSR_SIP_SSIP_MASK);
}


/**
 * @brief   Complete software interrupt for supervisor mode
 *
 */
ATTR_ALWAYS_INLINE static inline void intc_s_complete_swi(void)
{
    clear_csr(CSR_SIP, CSR_SIP_SSIP_MASK);
}

/*
 * @brief Enable IRQ for supervisor mode
 *
 * @param[in] irq Interrupt number
 */
#define intc_s_enable_irq(irq) \
    intc_enable_irq(HPM_PLIC_TARGET_S_MODE, irq)

/*
 * @brief Disable IRQ for supervisor mode
 *
 * @param[in] irq Interrupt number
 */
#define intc_s_disable_irq(irq) \
    intc_disable_irq(HPM_PLIC_TARGET_S_MODE, irq)

#define intc_set_s_threshold(threshold) \
    intc_set_threshold(HPM_PLIC_TARGET_S_MODE, threshold)

/*
 * @brief Complete IRQ for supervisor mode
 *
 * @param[in] irq Interrupt number
 */
#define intc_s_complete_irq(irq) \
    intc_complete_irq(HPM_PLIC_TARGET_S_MODE, irq)

/*
 * @brief Claim IRQ for supervisor mode
 *
 */
#define intc_s_claim_irq() intc_claim_irq(HPM_PLIC_TARGET_S_MODE)

/*
 * @brief Enable IRQ for supervisor mode with priority
 *
 * @param[in] irq Interrupt number
 * @param[in] priority Priority of interrupt
 */
#define intc_s_enable_irq_with_priority(irq, priority) \
    do { \
        intc_set_irq_priority(irq, priority); \
        intc_s_enable_irq(irq); \
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
 * @brief   Get interrupt threshold
 *
 * @param[in] target Target to handle specific interrupt
 */
ATTR_ALWAYS_INLINE static inline uint32_t intc_get_threshold(uint32_t target)
{
    return __plic_get_threshold(HPM_PLIC_BASE, target);
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
    __vector_table[irq] = (int) default_irq_entry;
}

/* Supervisor mode */
extern int __vector_s_table[];
extern void default_s_irq_entry(void);
/**
 * @brief   Install ISR for certain IRQ for ram based vector table for supervisor mode
 *
 * @param[in] irq Target interrupt number
 * @param[in] isr Interrupt service routine
 *
 */
ATTR_ALWAYS_INLINE static inline void install_s_isr(uint32_t irq, uint32_t isr)
{
    __vector_s_table[irq] = isr;
}

/**
 * @brief   Uninstall ISR for certain IRQ for ram based vector table for supervisor mode
 *
 * @param[in] irq Target interrupt number
 *
 */
ATTR_ALWAYS_INLINE static inline void uninstall_s_isr(uint32_t irq)
{
    __vector_s_table[irq] = (int) default_s_irq_entry;
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

#if defined(SUPPORT_PFT_ARCH) && SUPPORT_PFT_ARCH
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
/* RV32I caller registers + MCAUSE + MEPC + MSTATUS + FCSR + UCODE (DSP) + MCCTL + 20 FPU caller registers  */
#define CONTEXT_REG_NUM HPM_ALIGN_UP((4 * (22 + 20)), 16)
#else   /* __riscv_flen = 64 */
/* RV32I caller registers + MCAUSE + MEPC + MSTATUS + FCSR + UCODE (DSP) + MCCTL + 20 DFPU caller */
#define CONTEXT_REG_NUM HPM_ALIGN_UP((4 * (22 + 20 * 2)), 16)
#endif

#else
/* RV32I caller registers + MCAUSE + MEPC + MSTATUS + FCSR + UCODE (DSP) + MCCTL */
#define CONTEXT_REG_NUM HPM_ALIGN_UP((4 * 22), 16)
#endif

#ifdef __riscv_flen
/*
 * Save FPU caller registers:
 * NOTE: To simplify the logic, the FPU caller registers are always stored at word offset 22 in the stack
 */
#if __riscv_flen == 32
#ifdef __ICCRISCV__
#define SAVE_FPU_CONTEXT()  { \
    __asm volatile("\n\
             c.fswsp ft0,  22*4\n\
             c.fswsp ft1,  23*4 \n\
             c.fswsp ft2,  24*4 \n\
             c.fswsp ft3,  25*4 \n\
             c.fswsp ft4,  26*4 \n\
             c.fswsp ft5,  27*4 \n\
             c.fswsp ft6,  28*4 \n\
             c.fswsp ft7,  29*4 \n\
             c.fswsp fa0,  30*4 \n\
             c.fswsp fa1,  31*4 \n\
             c.fswsp fa2,  32*4 \n\
             c.fswsp fa3,  33*4 \n\
             c.fswsp fa4,  34*4 \n\
             c.fswsp fa5,  35*4 \n\
             c.fswsp fa6,  36*4 \n\
             c.fswsp fa7,  37*4 \n\
             c.fswsp ft8,  38*4 \n\
             c.fswsp ft9,  39*4 \n\
             c.fswsp ft10, 40*4 \n\
             c.fswsp ft11, 41*4 \n");\
}

/*
 * Restore FPU caller registers:
 * NOTE: To simplify the logic, the FPU caller registers are always stored at word offset 22 in the stack
 */
#define RESTORE_FPU_CONTEXT() { \
    __asm volatile("\n\
             c.flwsp ft0,  22*4\n\
             c.flwsp ft1,  23*4 \n\
             c.flwsp ft2,  24*4 \n\
             c.flwsp ft3,  25*4 \n\
             c.flwsp ft4,  26*4 \n\
             c.flwsp ft5,  27*4 \n\
             c.flwsp ft6,  28*4 \n\
             c.flwsp ft7,  29*4 \n\
             c.flwsp fa0,  30*4 \n\
             c.flwsp fa1,  31*4 \n\
             c.flwsp fa2,  32*4 \n\
             c.flwsp fa3,  33*4 \n\
             c.flwsp fa4,  34*4 \n\
             c.flwsp fa5,  35*4 \n\
             c.flwsp fa6,  36*4 \n\
             c.flwsp fa7,  37*4 \n\
             c.flwsp ft8,  38*4 \n\
             c.flwsp ft9,  39*4 \n\
             c.flwsp ft10, 40*4 \n\
             c.flwsp ft11, 41*4 \n");\
}
#else /* __ICCRISCV__ not defined */
#define SAVE_FPU_CONTEXT()  { \
    __asm volatile("\n\
             c.fswsp ft0,  22*4(sp)\n\
             c.fswsp ft1,  23*4(sp) \n\
             c.fswsp ft2,  24*4(sp) \n\
             c.fswsp ft3,  25*4(sp) \n\
             c.fswsp ft4,  26*4(sp) \n\
             c.fswsp ft5,  27*4(sp) \n\
             c.fswsp ft6,  28*4(sp) \n\
             c.fswsp ft7,  29*4(sp) \n\
             c.fswsp fa0,  30*4(sp) \n\
             c.fswsp fa1,  31*4(sp) \n\
             c.fswsp fa2,  32*4(sp) \n\
             c.fswsp fa3,  33*4(sp) \n\
             c.fswsp fa4,  34*4(sp) \n\
             c.fswsp fa5,  35*4(sp) \n\
             c.fswsp fa6,  36*4(sp) \n\
             c.fswsp fa7,  37*4(sp) \n\
             c.fswsp ft8,  38*4(sp) \n\
             c.fswsp ft9,  39*4(sp) \n\
             c.fswsp ft10, 40*4(sp) \n\
             c.fswsp ft11, 41*4(sp) \n");\
}

/*
 * Restore FPU caller registers:
 * NOTE: To simplify the logic, the FPU caller registers are always stored at word offset 22 in the stack
 */
#define RESTORE_FPU_CONTEXT() { \
    __asm volatile("\n\
             c.flwsp ft0,  22*4(sp)\n\
             c.flwsp ft1,  23*4(sp) \n\
             c.flwsp ft2,  24*4(sp) \n\
             c.flwsp ft3,  25*4(sp) \n\
             c.flwsp ft4,  26*4(sp) \n\
             c.flwsp ft5,  27*4(sp) \n\
             c.flwsp ft6,  28*4(sp) \n\
             c.flwsp ft7,  29*4(sp) \n\
             c.flwsp fa0,  30*4(sp) \n\
             c.flwsp fa1,  31*4(sp) \n\
             c.flwsp fa2,  32*4(sp) \n\
             c.flwsp fa3,  33*4(sp) \n\
             c.flwsp fa4,  34*4(sp) \n\
             c.flwsp fa5,  35*4(sp) \n\
             c.flwsp fa6,  36*4(sp) \n\
             c.flwsp fa7,  37*4(sp) \n\
             c.flwsp ft8,  38*4(sp) \n\
             c.flwsp ft9,  39*4(sp) \n\
             c.flwsp ft10, 40*4(sp) \n\
             c.flwsp ft11, 41*4(sp) \n");\
}
#endif
#else /*__riscv_flen == 64*/
#ifdef __ICCRISCV__
#define SAVE_FPU_CONTEXT()  { \
    __asm volatile("\n\
             c.fsdsp ft0,  22*4\n\
             c.fsdsp ft1,  24*4 \n\
             c.fsdsp ft2,  26*4 \n\
             c.fsdsp ft3,  28*4 \n\
             c.fsdsp ft4,  30*4 \n\
             c.fsdsp ft5,  32*4 \n\
             c.fsdsp ft6,  34*4 \n\
             c.fsdsp ft7,  36*4 \n\
             c.fsdsp fa0,  38*4 \n\
             c.fsdsp fa1,  40*4 \n\
             c.fsdsp fa2,  42*4 \n\
             c.fsdsp fa3,  44*4 \n\
             c.fsdsp fa4,  46*4 \n\
             c.fsdsp fa5,  48*4 \n\
             c.fsdsp fa6,  50*4 \n\
             c.fsdsp fa7,  52*4 \n\
             c.fsdsp ft8,  54*4 \n\
             c.fsdsp ft9,  56*4 \n\
             c.fsdsp ft10, 58*4 \n\
             c.fsdsp ft11, 60*4 \n");\
}

/*
 * Restore FPU caller registers:
 * NOTE: To simplify the logic, the FPU caller registers are always stored at word offset 22 in the stack
 */
#define RESTORE_FPU_CONTEXT() { \
    __asm volatile("\n\
             c.fldsp ft0,  22*4\n\
             c.fldsp ft1,  24*4 \n\
             c.fldsp ft2,  26*4 \n\
             c.fldsp ft3,  28*4 \n\
             c.fldsp ft4,  30*4 \n\
             c.fldsp ft5,  32*4 \n\
             c.fldsp ft6,  34*4 \n\
             c.fldsp ft7,  36*4 \n\
             c.fldsp fa0,  38*4 \n\
             c.fldsp fa1,  40*4 \n\
             c.fldsp fa2,  42*4 \n\
             c.fldsp fa3,  44*4 \n\
             c.fldsp fa4,  46*4 \n\
             c.fldsp fa5,  48*4 \n\
             c.fldsp fa6,  50*4 \n\
             c.fldsp fa7,  52*4 \n\
             c.fldsp ft8,  54*4 \n\
             c.fldsp ft9,  56*4 \n\
             c.fldsp ft10, 58*4 \n\
             c.fldsp ft11, 60*4 \n");\
}
#else /*__riscv_flen == 64*/
#define SAVE_FPU_CONTEXT()  { \
    __asm volatile("\n\
             c.fsdsp ft0,  22*4(sp)\n\
             c.fsdsp ft1,  24*4(sp) \n\
             c.fsdsp ft2,  26*4(sp) \n\
             c.fsdsp ft3,  28*4(sp) \n\
             c.fsdsp ft4,  30*4(sp) \n\
             c.fsdsp ft5,  32*4(sp) \n\
             c.fsdsp ft6,  34*4(sp) \n\
             c.fsdsp ft7,  36*4(sp) \n\
             c.fsdsp fa0,  38*4(sp) \n\
             c.fsdsp fa1,  40*4(sp) \n\
             c.fsdsp fa2,  42*4(sp) \n\
             c.fsdsp fa3,  44*4(sp) \n\
             c.fsdsp fa4,  46*4(sp) \n\
             c.fsdsp fa5,  48*4(sp) \n\
             c.fsdsp fa6,  50*4(sp) \n\
             c.fsdsp fa7,  52*4(sp) \n\
             c.fsdsp ft8,  54*4(sp) \n\
             c.fsdsp ft9,  56*4(sp) \n\
             c.fsdsp ft10, 58*4(sp) \n\
             c.fsdsp ft11, 60*4(sp) \n");\
}

/*
 * Restore FPU caller registers:
 * NOTE: To simplify the logic, the FPU caller registers are always stored at word offset 22 in the stack
 */
#define RESTORE_FPU_CONTEXT() { \
    __asm volatile("\n\
             c.fldsp ft0,  22*4(sp)\n\
             c.fldsp ft1,  24*4(sp) \n\
             c.fldsp ft2,  26*4(sp) \n\
             c.fldsp ft3,  28*4(sp) \n\
             c.fldsp ft4,  30*4(sp) \n\
             c.fldsp ft5,  32*4(sp) \n\
             c.fldsp ft6,  34*4(sp) \n\
             c.fldsp ft7,  36*4(sp) \n\
             c.fldsp fa0,  38*4(sp) \n\
             c.fldsp fa1,  40*4(sp) \n\
             c.fldsp fa2,  42*4(sp) \n\
             c.fldsp fa3,  44*4(sp) \n\
             c.fldsp fa4,  46*4(sp) \n\
             c.fldsp fa5,  48*4(sp) \n\
             c.fldsp fa6,  50*4(sp) \n\
             c.fldsp fa7,  52*4(sp) \n\
             c.fldsp ft8,  54*4(sp) \n\
             c.fldsp ft9,  56*4(sp) \n\
             c.fldsp ft10, 58*4(sp) \n\
             c.fldsp ft11, 60*4(sp) \n");\
}
#endif
#endif
#else
#define SAVE_FPU_CONTEXT()
#define RESTORE_FPU_CONTEXT()
#endif

#ifdef __ICCRISCV__
/**
 * @brief Save the caller registers based on the RISC-V ABI specification
 */
#define SAVE_CALLER_CONTEXT()   { \
    __asm volatile("addi sp, sp, %0" : : "i"(-CONTEXT_REG_NUM) :);\
    __asm volatile("\n\
            c.swsp ra,  0*4 \n\
            c.swsp t0,  1*4 \n\
            c.swsp t1,  2*4 \n\
            c.swsp t2,  3*4 \n\
            c.swsp s1,  4*4 \n\
            c.swsp a0,  5*4 \n\
            c.swsp a1,  6*4 \n\
            c.swsp a2,  7*4 \n\
            c.swsp a3,  8*4 \n\
            c.swsp a4,  9*4 \n\
            c.swsp a5, 10*4 \n\
            c.swsp a6, 11*4 \n\
            c.swsp a7, 12*4 \n\
            c.swsp s2, 13*4 \n\
            c.swsp s3, 14*4 \n\
            c.swsp s4, 15*4 \n\
            c.swsp s5, 16*4 \n\
            c.swsp s6, 17*4 \n\
            c.swsp t3, 18*4 \n\
            c.swsp t4, 19*4 \n\
            c.swsp t5, 20*4 \n\
            c.swsp t6, 21*4"); \
    SAVE_FPU_CONTEXT(); \
}

/**
 * @brief Restore the caller registers based on the RISC-V ABI specification
 */
#define RESTORE_CALLER_CONTEXT() { \
        __asm volatile("\n\
            c.lwsp ra,  0*4 \n\
            c.lwsp t0,  1*4 \n\
            c.lwsp t1,  2*4 \n\
            c.lwsp t2,  3*4 \n\
            c.lwsp s1,  4*4 \n\
            c.lwsp a0,  5*4 \n\
            c.lwsp a1,  6*4 \n\
            c.lwsp a2,  7*4 \n\
            c.lwsp a3,  8*4 \n\
            c.lwsp a4,  9*4 \n\
            c.lwsp a5, 10*4 \n\
            c.lwsp a6, 11*4 \n\
            c.lwsp a7, 12*4 \n\
            c.lwsp s2, 13*4 \n\
            c.lwsp s3, 14*4 \n\
            c.lwsp s4, 15*4 \n\
            c.lwsp s5, 16*4 \n\
            c.lwsp s6, 17*4 \n\
            c.lwsp t3, 18*4 \n\
            c.lwsp t4, 19*4 \n\
            c.lwsp t5, 20*4 \n\
            c.lwsp t6, 21*4 \n");\
        RESTORE_FPU_CONTEXT(); \
        __asm volatile("addi sp, sp, %0" : : "i"(CONTEXT_REG_NUM) :);\
}
#else
/**
 * @brief Save the caller registers based on the RISC-V ABI specification
 */
#define SAVE_CALLER_CONTEXT()   { \
    __asm volatile("addi sp, sp, %0" : : "i"(-CONTEXT_REG_NUM) :);\
    __asm volatile("\n\
            c.swsp ra,  0*4(sp) \n\
            c.swsp t0,  1*4(sp) \n\
            c.swsp t1,  2*4(sp) \n\
            c.swsp t2,  3*4(sp) \n\
            c.swsp s1,  4*4(sp) \n\
            c.swsp a0,  5*4(sp) \n\
            c.swsp a1,  6*4(sp) \n\
            c.swsp a2,  7*4(sp) \n\
            c.swsp a3,  8*4(sp) \n\
            c.swsp a4,  9*4(sp) \n\
            c.swsp a5, 10*4(sp) \n\
            c.swsp a6, 11*4(sp) \n\
            c.swsp a7, 12*4(sp) \n\
            c.swsp s2, 13*4(sp) \n\
            c.swsp s3, 14*4(sp) \n\
            c.swsp s4, 15*4(sp) \n\
            c.swsp s5, 16*4(sp) \n\
            c.swsp s6, 17*4(sp) \n\
            c.swsp t3, 18*4(sp) \n\
            c.swsp t4, 19*4(sp) \n\
            c.swsp t5, 20*4(sp) \n\
            c.swsp t6, 21*4(sp)"); \
    SAVE_FPU_CONTEXT(); \
}

/**
 * @brief Restore the caller registers based on the RISC-V ABI specification
 */
#define RESTORE_CALLER_CONTEXT() { \
        __asm volatile("\n\
            c.lwsp ra,  0*4(sp) \n\
            c.lwsp t0,  1*4(sp) \n\
            c.lwsp t1,  2*4(sp) \n\
            c.lwsp t2,  3*4(sp) \n\
            c.lwsp s1,  4*4(sp) \n\
            c.lwsp a0,  5*4(sp) \n\
            c.lwsp a1,  6*4(sp) \n\
            c.lwsp a2,  7*4(sp) \n\
            c.lwsp a3,  8*4(sp) \n\
            c.lwsp a4,  9*4(sp) \n\
            c.lwsp a5, 10*4(sp) \n\
            c.lwsp a6, 11*4(sp) \n\
            c.lwsp a7, 12*4(sp) \n\
            c.lwsp s2, 13*4(sp) \n\
            c.lwsp s3, 14*4(sp) \n\
            c.lwsp s4, 15*4(sp) \n\
            c.lwsp s5, 16*4(sp) \n\
            c.lwsp s6, 17*4(sp) \n\
            c.lwsp t3, 18*4(sp) \n\
            c.lwsp t4, 19*4(sp) \n\
            c.lwsp t5, 20*4(sp) \n\
            c.lwsp t6, 21*4(sp) \n");\
        RESTORE_FPU_CONTEXT(); \
        __asm volatile("addi sp, sp, %0" : : "i"(CONTEXT_REG_NUM) :);\
}
#endif

#ifdef __riscv_flen
#define SAVE_FPU_STATE() { \
        __asm volatile("frcsr s1\n"); \
}

#define RESTORE_FPU_STATE() { \
        __asm volatile("fscsr s1\n"); \
}
#else
#define SAVE_FPU_STATE()
#define RESTORE_FPU_STATE()
#endif

#ifdef __riscv_dsp
/*
 * @brief Save DSP context
 */
#define SAVE_DSP_CONTEXT() { \
        __asm volatile("csrrs s4, %0, x0\n" ::"i"(CSR_UCODE):);  \
}
/*
 * @brief Restore DSP context
 */
#define RESTORE_DSP_CONTEXT() {\
       __asm volatile("csrw %0, s4\n" ::"i"(CSR_UCODE):); \
}

#else
#define SAVE_DSP_CONTEXT()
#define RESTORE_DSP_CONTEXT()
#endif

/*
 * @brief Save MCCTL context
 */
#define SAVE_MCCTL_CONTEXT() { \
        __asm volatile("csrrs s5, %0, x0\n" ::"i"(CSR_MCCTLBEGINADDR):);  \
        __asm volatile("csrrs s6, %0, x0\n" ::"i"(CSR_MCCTLDATA):);  \
}
/*
 * @brief Restore MCCTL context
 */
#define RESTORE_MCCTL_CONTEXT() {\
        __asm volatile("csrw %0, s6\n" ::"i"(CSR_MCCTLDATA):); \
        __asm volatile("csrw %0, s5\n" ::"i"(CSR_MCCTLBEGINADDR):); \
}

/*
 * @brief Enter Nested IRQ Handling
 * @note To simplify the logic, Nested IRQ related registers are stored in the stack as below:
 *       MCAUSE - word offset 16 (not used in the vectored mode)
 *       EPC - word offset 17
 *       MSTATUS = word offset 18
 *       MXSTATUS = word offset 19
 */
#define ENTER_NESTED_IRQ_HANDLING_M() { \
    __asm volatile("\n\
            csrr s2, mepc    \n\
            csrr s3, mstatus \n");\
    SAVE_FPU_STATE(); \
    SAVE_DSP_CONTEXT(); \
    SAVE_MCCTL_CONTEXT(); \
    __asm volatile("csrsi mstatus, 8"); \
}

/*
 * @brief Complete IRQ Handling
 */
#define COMPLETE_IRQ_HANDLING_M(irq_num) { \
    __asm volatile("csrci mstatus, 8"); \
    __asm volatile("lui a4, 0xe4200"); \
    __asm volatile("li a3, %0" : : "i" (irq_num) :); \
    __asm volatile("sw a3, 4(a4)"); \
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
    __asm volatile("\n\
            csrw mstatus, s3 \n\
            csrw mepc, s2 \n");\
            RESTORE_FPU_STATE(); \
            RESTORE_DSP_CONTEXT(); \
            RESTORE_MCCTL_CONTEXT(); \
}


#define ENTER_NESTED_IRQ_HANDLING_S() {\
     __asm volatile("\n\
            csrr s2, sepc    \n\
            csrr s3, sstatus \n");\
    SAVE_FPU_STATE(); \
    SAVE_DSP_CONTEXT(); \
    __asm volatile("csrsi sstatus, 2"); \
}
#define COMPLETE_IRQ_HANDLING_S(irq_num) {\
    __asm volatile("csrci sstatus, 2"); \
    __asm volatile("lui a4, 0xe4201"); \
    __asm volatile("li a3, %0" : : "i" (irq_num) :); \
    __asm volatile("sw a3, 4(a4)"); \
}

/*
 * @brief Exit Nested IRQ Handling at supervisor mode
 * @note To simplify the logic, Nested IRQ related registers are stored in the stack as below:
 *       SCAUSE - word offset 16 (not used in the vectored mode)
 *       EPC - word offset 17
 *       SSTATUS = word offset 18
 */
#define EXIT_NESTED_IRQ_HANDLING_S() { \
    __asm volatile("\n\
            csrw sstatus, s3 \n\
            csrw sepc, s2 \n");\
            RESTORE_FPU_STATE(); \
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

#ifdef __cplusplus
#define HPM_EXTERN_C extern "C"
#else
#define HPM_EXTERN_C
#endif

#define ISR_NAME_M(irq_num) default_isr_##irq_num
#define ISR_NAME_S(irq_num) default_isr_s_##irq_num
/**
 * @brief Declare an external interrupt handler for machine mode
 *
 * @param[in] irq_num - IRQ number index
 * @param[in] isr - Application IRQ handler function pointer
 */
#if !defined(USE_NONVECTOR_MODE) || (USE_NONVECTOR_MODE == 0)
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#define FREERTOS_VECTOR_ISR_WRAPPER_NAME(irq_num)   irq_handler_wrapper_##irq_num
#define SDK_DECLARE_EXT_ISR_M(irq_num, isr) \
void isr(void) __attribute__((section(".isr_vector"))); \
HPM_EXTERN_C void FREERTOS_VECTOR_ISR_WRAPPER_NAME(irq_num)(void) __attribute__((section(".isr_vector"))); \
void FREERTOS_VECTOR_ISR_WRAPPER_NAME(irq_num)(void) \
{ \
    isr();\
}

/**
 * @brief Declare an external interrupt handler for supervisor mode
 *
 * @param[in] irq_num - IRQ number index
 * @param[in] isr - Application IRQ handler function pointer
 */
#define SDK_DECLARE_EXT_ISR_S(irq_num, isr) \
void isr(void) __attribute__((section(".isr_s_vector")));\
HPM_EXTERN_C void FREERTOS_VECTOR_ISR_WRAPPER_NAME(irq_num)(void) __attribute__((section(".isr_s_vector")));\
void FREERTOS_VECTOR_ISR_WRAPPER_NAME(irq_num)(void) \
{ \
    isr();\
}
#else

#define SDK_DECLARE_EXT_ISR_M(irq_num, isr) \
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C HPM_ATTR_MACHINE_INTERRUPT void ISR_NAME_M(irq_num)(void);\
HPM_ATTR_MACHINE_INTERRUPT void ISR_NAME_M(irq_num)(void) \
{ \
    SAVE_CALLER_CONTEXT(); \
    ENTER_NESTED_IRQ_HANDLING_M();\
    __asm volatile("la t1, %0\n\t" : : "i" (isr) : );\
    __asm volatile("jalr t1\n");\
    COMPLETE_IRQ_HANDLING_M(irq_num);\
    EXIT_NESTED_IRQ_HANDLING_M();\
    RESTORE_CALLER_CONTEXT();\
    __asm volatile("fence io, io");\
}

/**
 * @brief Declare an external interrupt handler for supervisor mode
 *
 * @param[in] irq_num - IRQ number index
 * @param[in] isr - Application IRQ handler function pointer
 */
#define SDK_DECLARE_EXT_ISR_S(irq_num, isr) \
void isr(void) __attribute__((section(".isr_s_vector")));\
HPM_EXTERN_C HPM_ATTR_SUPERVISOR_INTERRUPT void ISR_NAME_S(irq_num)(void);\
HPM_ATTR_SUPERVISOR_INTERRUPT void ISR_NAME_S(irq_num)(void) {\
    SAVE_CALLER_CONTEXT(); \
    ENTER_NESTED_IRQ_HANDLING_S();\
    __asm volatile("la t1, %0\n\t" : : "i" (isr) : );\
    __asm volatile("jalr t1\n");\
    COMPLETE_IRQ_HANDLING_S(irq_num);\
    EXIT_NESTED_IRQ_HANDLING_S();\
    RESTORE_CALLER_CONTEXT();\
    __asm volatile("fence io, io");\
}

#endif
#else

#define SDK_DECLARE_EXT_ISR_M(irq_num, isr) \
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C void ISR_NAME_M(irq_num)(void) __attribute__((section(".isr_vector")));\
void ISR_NAME_M(irq_num)(void) {           \
    isr();                                            \
}

#define SDK_DECLARE_EXT_ISR_S(irq_num, isr) \
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C void ISR_NAME_S(irq_num)(void) __attribute__((section(".isr_vector")));\
void ISR_NAME_S(irq_num)(void) {           \
    isr();                                            \
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
void mchtmr_isr(void) {\
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
void swi_isr(void) {\
    isr();\
}

/* Supervisor mode */

/**
 * @brief Declare machine timer interrupt handler
 *
 * @param[in] isr - MCHTMR IRQ handler function pointer
 */
#define SDK_DECLARE_MCHTMR_ISR_S(isr) \
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C void mchtmr_s_isr(void) __attribute__((section(".isr_vector"))); \
void mchtmr_s_isr(void) {\
    isr();\
}

/**
 * @brief Declare machine software interrupt handler
 *
 * @param[in] isr - SWI IRQ handler function pointer
 */
#define SDK_DECLARE_SWI_ISR_S(isr)\
void isr(void) __attribute__((section(".isr_vector")));\
HPM_EXTERN_C void swi_s_isr(void) __attribute__((section(".isr_vector"))); \
void swi_s_isr(void) {\
    isr();\
}

#define CSR_MSTATUS_MPP_S_MODE (0x1)
#define MODE_SWITCH_FROM_M(mstatus, mepc, label, mode) \
do { \
    if (label) { \
        write_csr(mepc, label); \
    } \
    clear_csr(mstatus, CSR_MSTATUS_MPP_MASK); \
    set_csr(mstatus, CSR_MSTATUS_MPP_SET(mode)); \
} while(0)

typedef void (*s_mode_entry)(void);

/**
 * @brief Switch mode to supervisor from machine
 *
 * @param[in] entry - entry point after mode is switched
 */
static inline void switch_to_s_mode(s_mode_entry entry)
{
    write_csr(CSR_SEPC, entry);
    MODE_SWITCH_FROM_M(CSR_MSTATUS, CSR_MEPC, entry, CSR_MSTATUS_MPP_S_MODE);
    if (entry) {
        __asm("mret");
    }
}
#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* HPM_INTERRUPT_H */
