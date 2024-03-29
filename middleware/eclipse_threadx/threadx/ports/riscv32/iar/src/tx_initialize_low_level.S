/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** ThreadX Component                                                     */ 
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*  #include "tx_api.h"
    #include "tx_initialize.h"
    #include "tx_thread.h"
    #include "tx_timer.h"  */
    
    EXTERN      _tx_thread_system_stack_ptr
    EXTERN      _tx_initialize_unused_memory
    EXTERN      _tx_thread_context_save
    EXTERN      _tx_thread_context_restore
    EXTERN      _tx_timer_interrupt
    EXTERN      __disable_interrupt
    EXTERN      _tx_initialize_low_level2
    EXTERN      tx_cpu_ts_setup
    RSEG        FREE_MEM:DATA
    PUBLIC      __tx_free_memory_start
__tx_free_memory_start:
    DS32        4    


    SECTION `.isr_vector`:CODE:REORDER:NOROOT(2)
    CODE
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_initialize_low_level                           RISC-V32/IAR     */
/*                                                           6.1          */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Microsoft Corporation                             */ 
/*    Tom van Leeuwen, Technolution B.V.                                  */
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function is responsible for any low-level processor            */ 
/*    initialization, including setting up interrupt vectors, setting     */ 
/*    up a periodic timer interrupt source, saving the system stack       */ 
/*    pointer for use in ISR processing later, and finding the first      */ 
/*    available RAM memory address for tx_application_define.             */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_initialize_kernel_enter           ThreadX entry function        */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  01-24-2024     Zihan Xu                 Add system tick init function */
/*                                                                        */ 
/**************************************************************************/ 
/* VOID   _tx_initialize_low_level(VOID)
{  */
    PUBLIC  _tx_initialize_low_level
_tx_initialize_low_level:
    add     sp, sp, -4
    sw      ra, 0(sp)
    call    __disable_interrupt
    
    sw      sp, _tx_thread_system_stack_ptr, t0     ; Save system stack pointer

    la      t0, __tx_free_memory_start              ; Pickup first free address
    sw      t0, _tx_initialize_unused_memory, t1    ; Save unused memory address

    call    tx_cpu_ts_setup
    lw      ra, 0(sp)
    add     sp, sp, 4
    ret
    END