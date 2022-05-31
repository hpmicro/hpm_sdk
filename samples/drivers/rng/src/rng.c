/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_rng_drv.h"
#include "hpm_sysctl_drv.h"

int main(void)
{
    hpm_stat_t stat;
    uint32_t i;
    uint32_t rand[8];

    board_init();
    printf("rng example\n");
    printf("rng init\n");
    stat = rng_init(HPM_RNG);
    if (stat) {
        printf("failed to rng_init: 0x%x\n", stat);
        return 1;
    }
    printf("rng get rand wait\n");
    stat = rng_rand_wait(HPM_RNG, rand, sizeof(rand));
    if (stat) {
        printf("failed to rng_rand_wait: 0x%x\n", stat);
        return 1;
    }
    for (i = 0; i < 8; i++) {
        printf("rand[%d] = 0x%x\n", i, rand[i]);
    }
    printf("rng selftest\n");
    stat = rng_run_selftest(HPM_RNG);
    if (stat) {
        printf("failed to rng_run_selftest: 0x%x\n", stat);
        return 1;
    }

    return 0;
}
