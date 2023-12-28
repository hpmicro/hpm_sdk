/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ramstress_lib.h"
#include <stdio.h>

#define RAMSTRESS_LOG(...) printf(__VA_ARGS__)


struct ramfunc_list {
    int is_enable;
    char *info;
    int (*func)(uint32_t *buf1, uint32_t *buf2, uint32_t count);
};

#include <stdlib.h>
static uint32_t interface_random32(void)
{
    return (uint32_t)rand();
}

int ramstress_algo_fill_selfaddress(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1;
    volatile uint32_t *p2;
    uint32_t i, j;
    for (j = 0; j < 16; j++) {
        RAMSTRESS_LOG("fill_selfaddress: %u/%u\n", j, 16);
        p1 = buf1;
        p2 = buf2;
        for (i = 0; i < size; i++) {
            *p1 = ((j + i) & 0x01ul) ? (uint32_t)p1 : ~((uint32_t)p1);
            *p2 = ((j + i) & 0x01ul) ? (uint32_t)p2 : ~((uint32_t)p2);
            p1++;
            p2++;
        }

        p1 = buf1;
        p2 = buf2;
        volatile uint32_t *error_addr = 0;
        for (uint32_t i = 0; i < size; i++) {
            if (*p1 != (((j + i) & 0x01ul) ? (uint32_t)p1 : ~((uint32_t)p1))) {
                error_addr = p1;
                break;
            } else if (*p2 != (((j + i) & 0x01ul) ? (unsigned long)p2 : ~((unsigned long)p2))) {
                error_addr = p2;
                break;
            } else {
                p1++;
                p2++;
            }
        }

        if (error_addr) {
            RAMSTRESS_LOG("FAILURE: possible bad address at 0x%p.\n", error_addr);
            return -1;
        }
    }

    return 0;
}


static int ramstress_compare_buffer_result(volatile uint32_t *buf1, volatile uint32_t *buf2, uint32_t size)
{
    while (size--) {
        if (*buf1 != *buf2) {
            RAMSTRESS_LOG("FAILURE: %X[at: %p] ! %X[at: %p]\n", *buf1, buf1, *buf2, buf2);
            return -1;
        }
    }

    return 0;
}

int ramstress_algo_random(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1 = buf1;
    volatile uint32_t *p2 = buf2;
    uint32_t random;

    for (uint32_t i = 0; i < size; i++) {
        random = interface_random32();
        *p1++ = random;
        *p2++ = random;
    }

    return ramstress_compare_buffer_result(buf1, buf2, size);
}

int ramstress_algo_or(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1 = buf1;
    volatile uint32_t *p2 = buf2;
    uint32_t random = interface_random32();

    for (uint32_t i = 0; i < size; i++) {
        *p1++ |= random;
        *p2++ |= random;
    }

    return ramstress_compare_buffer_result(buf1, buf2, size);
}

int ramstress_algo_xor(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1 = buf1;
    volatile uint32_t *p2 = buf2;
    uint32_t random = interface_random32();

    for (uint32_t i = 0; i < size; i++) {
        *p1++ ^= random;
        *p2++ ^= random;
    }

    return ramstress_compare_buffer_result(buf1, buf2, size);
}

int ramstress_algo_and(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1 = buf1;
    volatile uint32_t *p2 = buf2;
    uint32_t random = interface_random32();

    for (uint32_t i = 0; i < size; i++) {
        *p1++ &= random;
        *p2++ &= random;
    }

    return ramstress_compare_buffer_result(buf1, buf2, size);
}

int ramstress_algo_sub(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1 = buf1;
    volatile uint32_t *p2 = buf2;
    uint32_t random = interface_random32();

    for (uint32_t i = 0; i < size; i++) {
        *p1++ -= random;
        *p2++ -= random;
    }

    return ramstress_compare_buffer_result(buf1, buf2, size);
}

int ramstress_algo_mul(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1 = buf1;
    volatile uint32_t *p2 = buf2;
    uint32_t random = interface_random32();

    for (uint32_t i = 0; i < size; i++) {
        *p1++ *= random;
        *p2++ *= random;
    }

    return ramstress_compare_buffer_result(buf1, buf2, size);
}

int ramstress_algo_div(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1 = buf1;
    volatile uint32_t *p2 = buf2;
    uint32_t random = interface_random32();

    random = (random == 0) ? (random + 1) : random;

    for (uint32_t i = 0; i < size; i++) {
        *p1++ /= random;
        *p2++ /= random;
    }

    return ramstress_compare_buffer_result(buf1, buf2, size);
}

int ramstress_algo_seqinc(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1 = buf1;
    volatile uint32_t *p2 = buf2;
    uint32_t random = interface_random32();

    for (uint32_t i = 0; i < size; i++) {
        *p1++ = (i + random);
        *p2++ = (i + random);
    }

    return ramstress_compare_buffer_result(buf1, buf2, size);
}

int ramstress_algo_solidbits(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1;
    volatile uint32_t *p2;
    uint32_t i, j;
    uint32_t random;
    uint32_t all_one_bits = 0xfffffffful;

    for (j = 0; j < 64; j++) {
        p1 = buf1;
        p2 = buf2;
        random = (j & 0x01) == 0 ? all_one_bits : 0;

        RAMSTRESS_LOG("solidbits: random(%u/64) = 0x%X running...\n", j, random);
        for (i = 0; i < size; i++) {
            *p1++ = (i & 0x01) == 0 ? random : ~random;
            *p2++ = (i & 0x01) == 0 ? random : ~random;
        }

        if (ramstress_compare_buffer_result(buf1, buf2, size)) {
            return -1;
        }
    }

    return 0;
}

int ramstress_algo_checkerboard(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1;
    volatile uint32_t *p2;
    uint32_t i, j;
    uint32_t random;
    uint32_t checkerboard1 = 0x55555555ul;
    uint32_t checkerboard2 = 0xaaaaaaaaul;

    for (j = 0; j < 64; j++) {
        p1 = buf1;
        p2 = buf2;
        random = (j & 0x01) == 0 ? checkerboard1 : checkerboard2;

        RAMSTRESS_LOG("checkerboard: random(%u/64) = 0x%X running...\n", j, random);
        for (i = 0; i < size; i++) {
            *p1++ = (i & 0x01) == 0 ? random : ~random;
            *p2++ = (i & 0x01) == 0 ? random : ~random;
        }

        if (ramstress_compare_buffer_result(buf1, buf2, size)) {
            return -1;
        }
    }

    return 0;
}

int ramstress_algo_blockseq(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1;
    volatile uint32_t *p2;
    uint32_t i, j;
    uint32_t random;

    for (j = 0; j < 0xff; j++) {
        p1 = buf1;
        p2 = buf2;
        random = j<<24 | j<<16 | j<<8 | j;

        RAMSTRESS_LOG("blockseq: random(%u/256) = 0x%X running...\n", j, random);
        for (i = 0; i < size; i++) {
            *p1++ = random;
            *p2++ = random;
        }

        if (ramstress_compare_buffer_result(buf1, buf2, size)) {
            return -1;
        }
    }

    return 0;
}

int ramstress_algo_walkbits0(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1;
    volatile uint32_t *p2;
    uint32_t i, j;
    uint32_t random;
    uint32_t bit_len = 32;

    for (j = 0; j < bit_len * 2; j++) {
        p1 = buf1;
        p2 = buf2;

        if (j < bit_len) {
            /* DIR: up */
            random = 0x01ul<<j;
        } else {
            /* DIR: down */
            random = 0x01ul<<(bit_len - (j - bit_len) - 1);
        }

        RAMSTRESS_LOG("walkbits0: random(%u/%u) = 0x%X running...\n", j, bit_len * 2, random);
        for (i = 0; i < size; i++) {
            *p1++ = random;
            *p2++ = random;
        }

        if (ramstress_compare_buffer_result(buf1, buf2, size)) {
            return -1;
        }
    }

    return 0;
}

int ramstress_algo_walkbits1(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1;
    volatile uint32_t *p2;
    uint32_t i, j;
    uint32_t random;
    uint32_t bit_len = 32;

    for (j = 0; j < bit_len * 2; j++) {
        p1 = buf1;
        p2 = buf2;

        if (j < bit_len) {
            /* DIR: up */
            random = 0x01ul<<j;
        } else {
            /* DIR: down */
            random = 0x01ul<<(bit_len - (j - bit_len) - 1);
        }

        random = ~random;

        RAMSTRESS_LOG("walkbits1: random(%u/%u) = 0x%X running...\n", j, bit_len * 2, random);
        for (i = 0; i < size; i++) {
            *p1++ = random;
            *p2++ = random;
        }

        if (ramstress_compare_buffer_result(buf1, buf2, size)) {
            return -1;
        }
    }

    return 0;
}

int ramstress_algo_bitspread(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1;
    volatile uint32_t *p2;
    uint32_t i, j;
    uint32_t random;
    uint32_t bit_len = 32;

    for (j = 0; j < bit_len * 2; j++) {
        p1 = buf1;
        p2 = buf2;

        if (j < bit_len) {
            /* DIR: up */
            random = 0x01ul<<j | 0x01ul<<(j + 2);
        } else {
            /* DIR: down */
            random = 0x01ul<<(bit_len - (j - bit_len) - 1) |
                    0x01ul<<(bit_len - (j - bit_len) + 1);
        }

        RAMSTRESS_LOG("bitspread: random(%u/%u) = 0x%X running...\n", j, bit_len * 2, random);
        for (i = 0; i < size; i++) {
            *p1++ = (i & 0x01) == 0 ? random : ~random;
            *p2++ = (i & 0x01) == 0 ? random : ~random;
        }

        if (ramstress_compare_buffer_result(buf1, buf2, size)) {
            return -1;
        }
    }

    return 0;
}

int ramstress_algo_bitflip(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint32_t *p1;
    volatile uint32_t *p2;
    uint32_t i, j, k;
    uint32_t random;
    uint32_t bit_len = 32;

    for (k = 0; k < bit_len; k++) {
        random = 0x01ul<<k;
        for (j = 0; j < 8; j++) {
            p1 = buf1;
            p2 = buf2;
            random = ~random;
            RAMSTRESS_LOG("bitflip: random(%u/%u) = 0x%X running...\n", (k * 8 + j), bit_len * 8, random);
            for (i = 0; i < size; i++) {
                *p1++ = (i & 0x01) == 0 ? random : ~random;
                *p2++ = (i & 0x01) == 0 ? random : ~random;
            }

            if (ramstress_compare_buffer_result(buf1, buf2, size)) {
                return -1;
            }
        }
    }

    return 0;
}

int ramstress_algo_8bits(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint8_t *p8;
    volatile uint32_t *p32;
    uint32_t i, j;
    uint32_t random;

    for (j = 0; j < 2; j++) {
        if (j & 0x01) {
            p8 = (volatile uint8_t *)buf1;
            p32 = buf2;
        } else {
            p8 = (volatile uint8_t *)buf2;
            p32 = buf1;
        }

        volatile uint8_t *t8;
        for (i = 0; i < size; i++) {
            random = interface_random32();
            t8 = (volatile uint8_t *)&random;

            for (int n = 0; n < 4; n++) {
                *p8++ = *t8++;
            }
            *p32++ = random;
        }

        if (ramstress_compare_buffer_result(buf1, buf2, size)) {
            return -1;
        }
    }

    return 0;
}

int ramstress_algo_16bits(uint32_t *buf1, uint32_t *buf2, uint32_t size)
{
    volatile uint16_t *p16;
    volatile uint32_t *p32;
    uint32_t i, j;
    uint32_t random;

    for (j = 0; j < 2; j++) {
        if (j & 0x01) {
            p16 = (volatile uint16_t *)buf1;
            p32 = buf2;
        } else {
            p16 = (volatile uint16_t *)buf2;
            p32 = buf1;
        }

        volatile uint16_t *t16;
        for (i = 0; i < size; i++) {
            random = interface_random32();
            t16 = (volatile uint16_t *)&random;

            for (int n = 0; n < 2; n++) {
                *p16++ = *t16++;
            }
            *p32++ = random;
        }

        if (ramstress_compare_buffer_result(buf1, buf2, size)) {
            return -1;
        }
    }

    return 0;
}

const struct ramfunc_list func_lists[] = {
    {1, "algo_fill_selfaddress", ramstress_algo_fill_selfaddress},
    {1, "algo_random", ramstress_algo_random},
    {1, "algo_or", ramstress_algo_or},
    {1, "algo_xor", ramstress_algo_xor},
    {1, "algo_and", ramstress_algo_and},
    {1, "algo_sub", ramstress_algo_sub},
    {1, "algo_mul", ramstress_algo_mul},
    {1, "algo_div", ramstress_algo_div},
    {1, "algo_seqinc", ramstress_algo_seqinc},
    {1, "algo_solidbits", ramstress_algo_solidbits},
    {1, "algo_checkerboard", ramstress_algo_checkerboard},
    {1, "algo_blockseq", ramstress_algo_blockseq},
    {1, "algo_walkbits0", ramstress_algo_walkbits0},
    {1, "algo_walkbits1", ramstress_algo_walkbits1},
    {1, "algo_bitspread", ramstress_algo_bitspread},
    {1, "algo_bitflip", ramstress_algo_bitflip},
    {1, "algo_8bits", ramstress_algo_8bits},
    {1, "algo_16bits", ramstress_algo_16bits},
};

int ramstress_run(uint32_t ram_base, uint32_t ram_size_bytes)
{
    int func_num = sizeof(func_lists) / sizeof(func_lists[0]);
    uint32_t word_num = ram_size_bytes / 4;
    uint32_t *buf1 = (uint32_t *)ram_base;
    uint32_t *buf2 = buf1 + (word_num / 2);
    int ret;

    for (int i = 0; i < func_num; i++) {
        if (func_lists[i].is_enable &&
            func_lists[i].info && func_lists[i].func) {
                ret = func_lists[i].func(buf1, buf2, word_num / 2);
                if (ret == 0) {
                    RAMSTRESS_LOG("[%s]: PASSED\n", func_lists[i].info);
                } else {
                    RAMSTRESS_LOG("[%s]: FAILED\n", func_lists[i].info);
                    return -1;
                }
            }
    }

    RAMSTRESS_LOG("\n=============================================\n");
    RAMSTRESS_LOG("All cases are PASSED\n");
    RAMSTRESS_LOG("=============================================\n");
    return 0;
}


