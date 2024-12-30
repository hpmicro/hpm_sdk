/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_mbx_drv.h"
#include "hpm_sysctl_drv.h"

static volatile bool mbxa_can_read;
static volatile bool mbxa_can_send;
static volatile bool mbxb_can_read;
static volatile bool mbxb_can_send;

#define MBXA     HPM_MBX0A
#define MBXA_IRQ IRQn_MBX0A
#define MBXB     HPM_MBX0B
#define MBXB_IRQ IRQn_MBX0B

SDK_DECLARE_EXT_ISR_M(MBXA_IRQ, isr_mbxa)
void isr_mbxa(void)
{
    volatile uint32_t sr = MBXA->SR;
    volatile uint32_t cr = MBXA->CR;
    if ((sr & MBX_SR_TWME_MASK) && (cr & MBX_CR_TWMEIE_MASK)) {
        mbx_disable_intr(MBXA, MBX_CR_TWMEIE_MASK);
        mbxa_can_send = true;
    }
    if ((sr & MBX_SR_TFMA_MASK) && (cr & MBX_CR_TFMAIE_MASK)) {
        mbx_disable_intr(MBXA, MBX_CR_TFMAIE_MASK);
        mbxa_can_send = true;
    }
    if ((sr & MBX_SR_RWMV_MASK) && (cr & MBX_CR_RWMVIE_MASK)) {
        mbx_disable_intr(MBXA, MBX_CR_RWMVIE_MASK);
        mbxa_can_read = true;
    }
    if ((sr & MBX_SR_RFMA_MASK) && (cr & MBX_CR_RFMAIE_MASK)) {
        mbx_disable_intr(MBXA, MBX_CR_RFMAIE_MASK);
        mbxa_can_read = true;
    }
}

SDK_DECLARE_EXT_ISR_M(MBXB_IRQ, isr_mbxb)
void isr_mbxb(void)
{
    volatile uint32_t sr = MBXB->SR;
    volatile uint32_t cr = MBXB->CR;
    if ((sr & MBX_SR_RWMV_MASK) && (cr & MBX_CR_RWMVIE_MASK)) {
        mbx_disable_intr(MBXB, MBX_CR_RWMVIE_MASK);
        mbxb_can_read = true;
    }
    if ((sr & MBX_SR_RFMA_MASK) && (cr & MBX_CR_RFMAIE_MASK)) {
        mbx_disable_intr(MBXB, MBX_CR_RFMAIE_MASK);
        mbxb_can_read = true;
    }
    if ((sr & MBX_SR_TWME_MASK) && (cr & MBX_CR_TWMEIE_MASK)) {
        mbx_disable_intr(MBXB, MBX_CR_TWMEIE_MASK);
        mbxb_can_send = true;
    }
    if ((sr & MBX_SR_TFMA_MASK) && (cr & MBX_CR_TFMAIE_MASK)) {
        mbx_disable_intr(MBXB, MBX_CR_TFMAIE_MASK);
        mbxb_can_send = true;
    }
}


static void test_singleword_communication(void)
{
    uint32_t i = 0;
    hpm_stat_t stat;
    mbxa_can_send = false;
    mbxa_can_read = false;
    mbxb_can_send = false;
    mbxb_can_read = false;
    printf("mbx singleword communication start\n");
    mbx_init(MBXA);
    mbx_init(MBXB);

    /* mbxa as sender, mbxb as receiver */
    mbx_enable_intr(MBXA, MBX_CR_TWMEIE_MASK);
    mbx_enable_intr(MBXB, MBX_CR_RWMVIE_MASK);

    while (1) {
        if (mbxa_can_send) {
            printf("mbxa sending %ld\n", i);
            stat = mbx_send_message(MBXA, i);
            if (stat != status_success) {
                printf("mbxa error sending %ld\n", i);
                return;
            }
            break;
        }
    }
    while (1) {
        if (mbxb_can_read) {
            stat = mbx_retrieve_message(MBXB, &i);
            if (stat == status_success) {
                printf("mbxb got %ld\n", i);
            } else {
                printf("mbxb error getting message\n");
                return;
            }
            break;
        }
    }

    i += 1;
    /* mbxb as sender, mbxb as receiver */
    mbx_enable_intr(MBXB, MBX_CR_TWMEIE_MASK);
    mbx_enable_intr(MBXA, MBX_CR_RWMVIE_MASK);

    while (1) {
        if (mbxb_can_send) {
            printf("mbxb sending %ld\n", i);
            stat = mbx_send_message(MBXB, i);
            if (stat != status_success) {
                printf("mbxb error sending %ld\n", i);
                return;
            }
            break;
        }
    }
    while (1) {
        if (mbxa_can_read) {
            stat = mbx_retrieve_message(MBXA, &i);
            if (stat == status_success) {
                printf("mbxa: got %ld\n", i);
            } else {
                printf("mbxa: error getting message\n");
                return;
            }
            mbx_enable_intr(MBXA, MBX_CR_TWMEIE_MASK);
            break;
        }
    }

    printf("mbx singleword communication done\n");
    return;
}

static void test_multiword_communication(uint32_t msg_count)
{
    uint32_t j = 0;
    uint32_t msg[4];
    hpm_stat_t stat;
    printf("mbx multiword communication start\n");
    mbx_init(MBXA);
    mbx_init(MBXB);
    mbxa_can_send = false;
    mbxa_can_read = false;
    mbxb_can_send = false;
    mbxb_can_read = false;

    volatile uint32_t tx_slot_count;
    volatile uint32_t rx_msg_count;
    uint32_t to_be_sent = 0;
    uint32_t i = 0;
    /* mbxa as sender, mbxb as receiver */
    mbx_enable_intr(MBXA, MBX_CR_TFMAIE_MASK);
    mbx_enable_intr(MBXB, MBX_CR_RFMAIE_MASK);

    while (msg_count != 0) {
        if (mbxa_can_send) {
            tx_slot_count = (MBXA->SR & MBX_SR_TFEC_MASK) >> MBX_SR_TFEC_SHIFT;
            printf("mbxa got %ld free space\n", tx_slot_count);
            if (tx_slot_count > msg_count) {
                to_be_sent = msg_count;
            } else {
                to_be_sent = tx_slot_count;
            }
            for (j = 0; j < to_be_sent; j++) {
                msg[j] = i++;
            }
            printf("mbxa sending fifo\n");
            stat = mbx_send_fifo(MBXA, msg, to_be_sent);
            if (stat == status_success) {
                printf("mbxa sent fifo %ld\n",  tx_slot_count);
                for (j = 0; j < tx_slot_count; j++) {
                    printf("mbxb sent %ld\n", msg[j]);
                }
                msg_count -= to_be_sent;
            } else {
                printf("mbxa error sent fifo %ld\n", tx_slot_count);

            }
        }

        if (mbxb_can_read) {
            rx_msg_count = (MBXB->SR & MBX_SR_RFVC_MASK) >> MBX_SR_RFVC_SHIFT;
            printf("mbxb getting fifo messages\n");
            stat = mbx_retrieve_fifo(MBXB, msg, rx_msg_count);
            if (stat == status_success) {
                printf("mbxb got fifo %ld\n", rx_msg_count);
                for (j = 0; j < rx_msg_count; j++) {
                    printf("mbxb got %ld\n", msg[j]);
                }
                msg_count -= rx_msg_count;
            } else {
                printf("mbxb error got fifo %ld\n", rx_msg_count);
            }
        }
    }

    printf("mbx multiword communication done\n");
}

int main(void)
{
    board_init();
    clock_add_to_group(clock_mbx0, 0);
    printf("mbx single core example\n");
    intc_m_enable_irq_with_priority(MBXA_IRQ, 1);
    intc_m_enable_irq_with_priority(MBXB_IRQ, 1);
    test_singleword_communication();
    test_multiword_communication(8);

    while (1) {
    }
    return 0;
}

