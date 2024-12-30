/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_mbx_drv.h"
#include "hpm_sysctl_drv.h"
#if (BOARD_RUNNING_CORE == HPM_CORE0)
#include "multicore_common.h"
#endif

static volatile bool can_read = false;
static volatile bool can_send = false;

#if (BOARD_RUNNING_CORE == HPM_CORE0)
#define MBX HPM_MBX0A
#define MBX_IRQ IRQn_MBX0A

SDK_DECLARE_EXT_ISR_M(MBX_IRQ, isr_mbx)
void isr_mbx(void)
{
    volatile uint32_t sr = MBX->SR;
    volatile uint32_t cr = MBX->CR;
    if ((sr & MBX_SR_TWME_MASK) && (cr & MBX_CR_TWMEIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_TWMEIE_MASK);
        can_send = true;
    } 
    if ((sr & MBX_SR_TFMA_MASK) && (cr & MBX_CR_TFMAIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_TFMAIE_MASK);
        can_send = true;
    } 
    if ((sr & MBX_SR_RWMV_MASK) && (cr & MBX_CR_RWMVIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_RWMVIE_MASK);
        can_read = true;
    } 
    if ((sr & MBX_SR_RFMA_MASK) && (cr & MBX_CR_RFMAIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_RFMAIE_MASK);
        can_read = true;
    }
}

#else
#define MBX HPM_MBX0B
#define MBX_IRQ IRQn_MBX0B

SDK_DECLARE_EXT_ISR_M(MBX_IRQ, isr_mbx)
void isr_mbx(void)
{
    volatile uint32_t sr = MBX->SR;
    volatile uint32_t cr = MBX->CR;
    if ((sr & MBX_SR_RWMV_MASK) && (cr & MBX_CR_RWMVIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_RWMVIE_MASK);
        can_read = true;
    } 
    if ((sr & MBX_SR_RFMA_MASK) && (cr & MBX_CR_RFMAIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_RFMAIE_MASK);
        can_read = true;
    } 
    if ((sr & MBX_SR_TWME_MASK) && (cr & MBX_CR_TWMEIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_TWMEIE_MASK);
        can_send = true;
    } 
    if ((sr & MBX_SR_TFMA_MASK) && (cr & MBX_CR_TFMAIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_TFMAIE_MASK);
        can_send = true;
    } 
}
#endif


static void test_multiword_communication(uint32_t msg_count)
{
    uint32_t j = 0;
    uint32_t msg[4];
    hpm_stat_t stat;
    printf("core %d: multiword communication start\n", BOARD_RUNNING_CORE);
    mbx_init(MBX);
    can_send = false;
    can_read = false;

#if (BOARD_RUNNING_CORE == HPM_CORE0)
    /* sender */
    volatile uint32_t tx_slot_count;
    uint32_t to_be_sent = 0;
    uint32_t i = 0;
    mbx_enable_intr(MBX, MBX_CR_TFMAIE_MASK);

    i = 0;
    while(msg_count) {
        if (can_send) {
            tx_slot_count = (MBX->SR & MBX_SR_TFEC_MASK) >> MBX_SR_TFEC_SHIFT;
            printf("core %d: got %ld free space\n", BOARD_RUNNING_CORE, tx_slot_count);
            if (tx_slot_count > msg_count) {
                to_be_sent = msg_count;
            } else {
                to_be_sent = tx_slot_count;
            }
            for (j = 0; j < to_be_sent; j++) {
                msg[j] = i++;
            }
            printf("core %d: sending fifo\n", BOARD_RUNNING_CORE);
            stat = mbx_send_fifo(MBX, msg, to_be_sent);
            if (stat == status_success) {
                printf("core %d: sent fifo %ld\n", BOARD_RUNNING_CORE, tx_slot_count);
                msg_count -= to_be_sent;
            } else {
                printf("core %d: error sent fifo %ld\n", BOARD_RUNNING_CORE, tx_slot_count);

            }
            can_send = false;
            mbx_enable_intr(MBX, MBX_CR_TFMAIE_MASK);
        }
    }
#else
    /* reciever */
    volatile uint32_t rx_msg_count;
    mbx_enable_intr(MBX, MBX_CR_RFMAIE_MASK);

    while(msg_count) {
        if (can_read) {
            rx_msg_count = (MBX->SR & MBX_SR_RFVC_MASK) >> MBX_SR_RFVC_SHIFT;
            printf("core %d: getting fifo messages\n", BOARD_RUNNING_CORE);
            stat = mbx_retrieve_fifo(MBX, msg, rx_msg_count);
            if (stat == status_success) {
                printf("core %d: got fifo %ld\n", BOARD_RUNNING_CORE, rx_msg_count);
                for (j = 0; j < rx_msg_count; j++) {
                    printf("core %d: got %ld\n", BOARD_RUNNING_CORE, msg[j]);
                }
                msg_count -= rx_msg_count;
            } else {
                printf("core %d: error got fifo %ld\n", BOARD_RUNNING_CORE, rx_msg_count);
            }
            rx_msg_count = 0;
            can_read = false;
            mbx_enable_intr(MBX, MBX_CR_RFMAIE_MASK);
        }
    }

#endif
    printf("core %d: multiword communication done\n", BOARD_RUNNING_CORE);
}

static void test_singleword_communication(void)
{
    uint32_t i = 0;
    hpm_stat_t stat;
    can_send = false;
    can_read = false;
    printf("core %d: singleword communication start\n", BOARD_RUNNING_CORE);
    mbx_init(MBX);
#if (BOARD_RUNNING_CORE == HPM_CORE0)
    /* sender */
    mbx_enable_intr(MBX, MBX_CR_TWMEIE_MASK);
    
    while (1) {
        if (can_send) {
            printf("core %d: sending %ld\n", BOARD_RUNNING_CORE, i);
            stat = mbx_send_message(MBX, i);
            if (stat != status_success) {
                printf("core %d: error sending %ld\n", BOARD_RUNNING_CORE, i);
                return;
            }
            can_send = false;
            mbx_enable_intr(MBX, MBX_CR_RWMVIE_MASK);
            break;
        }
    }
    while (1) {
        if (can_read) {
            stat = mbx_retrieve_message(MBX, &i);
            if (stat == status_success) {
                printf("core %d: got %ld\n", BOARD_RUNNING_CORE, i);
            } else {
                printf("core %d: error getting message\n", BOARD_RUNNING_CORE);
                return;
            }
            break;
        }
    }
    can_send = false;
    can_read = false;
#else

    mbx_enable_intr(MBX, MBX_CR_RWMVIE_MASK);
    /* reciever */
    while (1) {
        if (can_read) {
            stat = mbx_retrieve_message(MBX, &i);
            if (stat == status_success) {
                printf("core %d: got %ld\n", BOARD_RUNNING_CORE, i);
            } else {
                printf("core %d: error getting message\n", BOARD_RUNNING_CORE);
                return;
            }
            mbx_enable_intr(MBX, MBX_CR_TWMEIE_MASK);
            break;
        }
    }

    i += 1;
    while (1) {
        if (can_send) {
            printf("core %d: sending %ld\n", BOARD_RUNNING_CORE, i);
            stat = mbx_send_message(MBX, i);
            if (stat != status_success) {
                printf("core %d: error sending %ld\n", BOARD_RUNNING_CORE, i);
                return;
            }
            can_send = false;
            break;
        }
    }
    can_send = false;
    can_read = false;
#endif
    printf("core %d: singleword communication done\n", BOARD_RUNNING_CORE);
    return;
}

int main(void)
{
#if BOARD_RUNNING_CORE == HPM_CORE0
    board_init();
    clock_add_to_group(clock_mbx0, 0);
#else
    board_init_core1();
#endif
    printf("mbx example for core %d\n", BOARD_RUNNING_CORE);
    intc_m_enable_irq_with_priority(MBX_IRQ, 1);
#if BOARD_RUNNING_CORE == HPM_CORE0
    multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);
#endif
    test_singleword_communication();
    test_multiword_communication(8);
    printf("mbx example for core %d done\n", BOARD_RUNNING_CORE);
    while(1);
}

