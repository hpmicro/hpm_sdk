/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "hpm_ppor_drv.h"

SYSCTL_Type *ptr = HPM_SYSCTL;
PPOR_Type *ppor_ptr = HPM_PPOR;

typedef struct {
    uint8_t name[16];
    clock_node_t node;
    monitor_target_t monitor_target;
} node_info_t;

typedef struct {
    uint8_t name[16];
    clock_source_t source;
    monitor_target_t monitor_target;
} source_info_t;

const node_info_t node_name[] = {
    {"cpu0", clock_node_cpu0, monitor_target_clk_top_cpu0},
    {"mchtmr0", clock_node_mchtmr0, monitor_target_clk_top_mchtmr0},
    {"cpu1", clock_node_cpu1, monitor_target_clk_top_cpu1},
    {"mchtmr1", clock_node_mchtmr1, monitor_target_clk_top_mchtmr1},
    {"axi0", clock_node_axi0, monitor_target_clk_top_axi0},
    {"axi1", clock_node_axi1, monitor_target_clk_top_axi1},
    {"axi2", clock_node_axi2, monitor_target_clk_top_axi2},
    {"ahb0", clock_node_ahb0, monitor_target_clk_top_ahb0},
    {"dram", clock_node_dram, monitor_target_clk_top_dram},
    {"xpi0", clock_node_xpi0, monitor_target_clk_top_xpi0},
    {"xpi1", clock_node_xpi1, monitor_target_clk_top_xpi1},
    {"gptmr0", clock_node_gptmr0, monitor_target_clk_top_gptmr0},
    {"gptmr1", clock_node_gptmr1, monitor_target_clk_top_gptmr1},
    {"gptmr2", clock_node_gptmr2, monitor_target_clk_top_gptmr2},
    {"gptmr3", clock_node_gptmr3, monitor_target_clk_top_gptmr3},
    {"gptmr4", clock_node_gptmr4, monitor_target_clk_top_gptmr4},
    {"gptmr5", clock_node_gptmr5, monitor_target_clk_top_gptmr5},
    {"gptmr6", clock_node_gptmr6, monitor_target_clk_top_gptmr6},
    {"gptmr7", clock_node_gptmr7, monitor_target_clk_top_gptmr7},
    {"uart0", clock_node_uart0, monitor_target_clk_top_uart0},
    {"uart1", clock_node_uart1, monitor_target_clk_top_uart1},
    {"uart2", clock_node_uart2, monitor_target_clk_top_uart2},
    {"uart3", clock_node_uart3, monitor_target_clk_top_uart3},
    {"uart4", clock_node_uart4, monitor_target_clk_top_uart4},
    {"uart5", clock_node_uart5, monitor_target_clk_top_uart5},
    {"uart6", clock_node_uart6, monitor_target_clk_top_uart6},
    {"uart7", clock_node_uart7, monitor_target_clk_top_uart7},
    {"uart8", clock_node_uart8, monitor_target_clk_top_uart8},
    {"uart9", clock_node_uart9, monitor_target_clk_top_uart9},
    {"uarta", clock_node_uarta, monitor_target_clk_top_uarta},
    {"uartb", clock_node_uartb, monitor_target_clk_top_uartb},
    {"uartc", clock_node_uartc, monitor_target_clk_top_uartc},
    {"uartd", clock_node_uartd, monitor_target_clk_top_uartd},
    {"uarte", clock_node_uarte, monitor_target_clk_top_uarte},
    {"uartf", clock_node_uartf, monitor_target_clk_top_uartf},
    {"i2c0", clock_node_i2c0, monitor_target_clk_top_i2c0},
    {"i2c1", clock_node_i2c1, monitor_target_clk_top_i2c1},
    {"i2c2", clock_node_i2c2, monitor_target_clk_top_i2c2},
    {"i2c3", clock_node_i2c3, monitor_target_clk_top_i2c3},
    {"spi0", clock_node_spi0, monitor_target_clk_top_spi0},
    {"spi1", clock_node_spi1, monitor_target_clk_top_spi1},
    {"spi2", clock_node_spi2, monitor_target_clk_top_spi2},
    {"spi3", clock_node_spi3, monitor_target_clk_top_spi3},
    {"can0", clock_node_can0, monitor_target_clk_top_can0},
    {"can1", clock_node_can1, monitor_target_clk_top_can1},
    {"can2", clock_node_can2, monitor_target_clk_top_can2},
    {"can3", clock_node_can3, monitor_target_clk_top_can3},
    {"ptpc", clock_node_ptpc, monitor_target_clk_top_ptpc},
    {"ana0", clock_node_ana0, monitor_target_clk_top_ana0},
    {"ana1", clock_node_ana1, monitor_target_clk_top_ana1},
    {"ana2", clock_node_ana2, monitor_target_clk_top_ana2},
    {"aud0", clock_node_aud0, monitor_target_clk_top_aud0},
    {"aud1", clock_node_aud1, monitor_target_clk_top_aud1},
    {"aud2", clock_node_aud2, monitor_target_clk_top_aud2},
    {"dis0", clock_node_dis0, monitor_target_clk_top_dis0},
    {"cam0", clock_node_cam0, monitor_target_clk_top_cam0},
    {"cam1", clock_node_cam1, monitor_target_clk_top_cam1},
    {"eth0", clock_node_eth0, monitor_target_clk_top_eth0},
    {"eth1", clock_node_eth1, monitor_target_clk_top_eth1},
    {"ptp0", clock_node_ptp0, monitor_target_clk_top_ptp0},
    {"ptp1", clock_node_ptp1, monitor_target_clk_top_ptp1},
    {"ref0", clock_node_ref0, monitor_target_clk_top_ref0},
    {"ref1", clock_node_ref1, monitor_target_clk_top_ref1},
    {"ntmr0", clock_node_ntmr0, monitor_target_clk_top_ntmr0},
    {"ntmr1", clock_node_ntmr1, monitor_target_clk_top_ntmr1},
    {"sdxc0", clock_node_sdxc0, monitor_target_clk_top_sdxc0},
    {"sdxc1", clock_node_sdxc1, monitor_target_clk_top_sdxc1},
};

const source_info_t clock_source_name[] = {
    {"osc0_clk0", clock_source_osc0_clk0, monitor_target_osc0_clk0},
    {"pll0_clk0", clock_source_pll0_clk0, monitor_target_pll0_clk0},
    {"pll1_clk0", clock_source_pll1_clk0, monitor_target_pll1_clk0},
    {"pll1_clk1", clock_source_pll1_clk1, monitor_target_pll1_clk1},
    {"pll2_clk0", clock_source_pll2_clk0, monitor_target_pll2_clk0},
    {"pll2_clk1", clock_source_pll2_clk1, monitor_target_pll2_clk1},
    {"pll3_clk0", clock_source_pll3_clk0, monitor_target_pll3_clk0},
    {"pll4_clk0", clock_source_pll4_clk0, monitor_target_pll4_clk0},
};

const char *i2s_node_name[] = {
    "i2s0", "i2s1", "i2s2", "i2s3"
};

const char *adc_node_name[] = {
    "adc0", "adc1", "adc2", "adc3"
};

const source_info_t i2s_clock_source_name[] = {
    {"ahb_clk", clock_source_adc_i2s_ahb_clk, monitor_target_clk_top_ahb0},
    {"aud0_clk", clock_source_i2s_aud0_clk, monitor_target_clk_top_aud0},
    {"aud1_clk", clock_source_i2s_aud1_clk, monitor_target_clk_top_aud1},
    {"aud2_clk", clock_source_i2s_aud2_clk, monitor_target_clk_top_aud2},
};

const source_info_t adc_clock_source_name[] = {
    {"ahb_clk", clock_source_adc_i2s_ahb_clk, monitor_target_clk_top_ahb0},
    {"ana0_clk", clock_source_adc_ana0_clk, monitor_target_clk_top_ana0},
    {"ana1_clk", clock_source_adc_ana1_clk, monitor_target_clk_top_ana1},
    {"ana2_clk", clock_source_adc_ana2_clk, monitor_target_clk_top_ana2},
};

const char *linkable_resource_name[] = {
    "ahbp", "axis", "axic", "axiv", "dramc", "rom0", "lm0", "lm1", "mchtmr0",
    "mchtmr1", "ram0", "ram1", "xpi0", "xpi1", "sdp0", "rng0", "keym", "dma0",
    "dma1", "gpio", "mbx0", "mbx1", "wdog0", "wdog1", "wdog2", "wdog3",
    "gptmr0", "gptmr1", "gptmr2", "gptmr3", "gptmr4", "gptmr5", "gptmr6", "gptmr7",
    "uart0", "uart1", "uart2", "uart3", "uart4", "uart5", "uart6", "uart7",
    "uart8", "uart9", "uarta", "uartb", "uartc", "uartd", "uarte", "uartf",
    "i2c0", "i2c1", "i2c2", "i2c3", "spi0", "spi1", "spi2", "spi3",
    "can0", "can1", "can2", "can3", "ptpc", "adc0", "adc1", "adc2",
    "acmp", "i2s0", "i2s1", "i2s2", "i2s3", "pdm0", "dao", "synt",
    "mot0", "mot1", "mot2", "mot3", "mcss", "dis0", "cam0", "cam1",
    "jpeg", "pdma", "eth0", "eth1", "ntm0", "ntm1", "sdc0", "sdc1",
    "usb0", "usb1", "ref0", "ref1",
};

#define CLOCK_DIV_MAX ((SYSCTL_CLOCK_DIV_MASK >> SYSCTL_CLOCK_DIV_SHIFT) + 1)

bool test_config_adc_i2s_clock(void)
{
    uint32_t node, src_node;
    uint32_t source, source2;
    uint32_t div;
    hpm_stat_t stat;
    printf("test clock adc/i2s config\n");

    source2 = rand() % ARRAY_SIZE(adc_clock_source_name);
    switch (source2) {
        case clock_source_adc_i2s_ahb_clk:
            src_node = clock_node_ahb0;
            break;
        case clock_source_adc_ana0_clk:
            src_node = clock_node_ana0;
            break;
        case clock_source_adc_ana1_clk:
            src_node = clock_node_ana1;
            break;
        case clock_source_adc_ana2_clk:
            src_node = clock_node_ana2;
            break;
        default:
            printf("invalid clock source for adc/i2s: %d\n", source2);
            break;
    }
    source = rand() % ARRAY_SIZE(clock_source_name);
    div = rand() % CLOCK_DIV_MAX + 1;
    printf("config %s clock source to %s\n", node_name[src_node].name, clock_source_name[source].name);
    stat = sysctl_config_clock(ptr, src_node, source, div);
    if (stat != status_success) {
        return false;
    }
    node = rand() % ARRAY_SIZE(adc_node_name);
    printf("config %s to %s\n", adc_node_name[node], adc_clock_source_name[source2].name);
    stat = sysctl_set_adc_i2s_clock_mux(ptr, node + clock_node_adc_i2s_start, source2);
    if (stat != status_success) {
        return false;
    }
    stat = sysctl_add_resource_to_cpu0(ptr, node + sysctl_resource_adc0);
    if (stat != status_success) {
        printf("failed to enable resource %s to cpu0\n", linkable_resource_name[node + sysctl_resource_adc0 - sysctl_resource_linkable_start]);
        return false;
    }
    return true;
}

bool test_config_clock(void)
{
    uint32_t node;
    uint32_t source;
    uint32_t div;
    uint32_t src_freq, tgt_freq;
    hpm_stat_t stat;

    printf("test clock config\n");
    node = rand() % ARRAY_SIZE(node_name);
    source = rand() % ARRAY_SIZE(clock_source_name);
    div = rand() % CLOCK_DIV_MAX + 1;
    stat = sysctl_config_clock(ptr, node_name[node].node, source, div);
    src_freq = sysctl_monitor_measure_frequency(ptr, 0, clock_source_name[source].monitor_target, true);
    tgt_freq = sysctl_monitor_measure_frequency(ptr, 0, node_name[node].monitor_target, true);
    printf("config %s to %s (@%d Hz) divided by %d: %d Hz\n",
            node_name[node].name, clock_source_name[source].name, src_freq, div, tgt_freq);
    return stat == status_success;
}

void test_enable_resource(void)
{
    uint32_t i;
    hpm_stat_t stat;
    printf("test enable resource\n");

    for (i = sysctl_resource_linkable_start; i < sysctl_resource_linkable_end; i++) {
        stat = sysctl_add_resource_to_cpu0(ptr, i);
        if (stat != status_success) {
            printf("[error]: failed to enable resource %s to cpu0\n", linkable_resource_name[i - sysctl_resource_linkable_start]);
        }
    }
}

void test_resource_related(void)
{
    test_enable_resource();
    printf("\n");
}

void test_clock_related(void)
{
    if (!test_config_clock()) {
        printf("[error]: test_config_clock() failed\n");
    }
    if (!test_config_adc_i2s_clock()) {
        printf("[error]: test_config_adc_i2s_clock() failed\n");
    }
    printf("\n");
}

void reset_soc(void)
{
    printf("%s\n", __func__);
    ppor_sw_reset(ppor_ptr, 10);
}

bool access_cpu_gpr(uint8_t start, uint8_t count, bool lock)
{
    uint32_t data[SYSCTL_SOC_CPU_GPR_COUNT], result[SYSCTL_SOC_CPU_GPR_COUNT];
    bool ret = true;
    printf("%s: GPR lock %s\n", __func__, lock ? "enabled" : "disabled");
    for (uint32_t i = 0; i < count; i++) {
        data[i] = rand();
    }

    if (status_success != sysctl_cpu0_set_gpr(ptr, start, count, data, lock)) {
        printf("failed to set gpr @%d, %d\n", start, count);
        while(1);
    }
    printf("write data to gpr @%d, %d\n", start, count);
    if (status_success != sysctl_cpu0_get_gpr(ptr, start, count, result)) {
        printf("failed to read data from gpr @%d, %d\n", start, count);
        while(1);
    }
    if (memcmp(data, result, count * sizeof(uint32_t))) {
        ret = false;
        printf("result: ");
        for (uint32_t i = 0; i < count; i++) {
            printf("0x%x ", result[i]);
        }
    }
    printf("\n");
    return ret;
}

void test_cpu_gpr_related(void)
{
    if (!access_cpu_gpr(0, 3, false)) {
        printf("[error]: Failed to write CPU GPR without lock\n");
    } else {
        printf("[expected]: Write CPU GPR without lock done\n");
    }
    if (!access_cpu_gpr(0, 3, true)) {
        printf("[error]: Failed to write CPU GPR with lock\n");
    } else {
        printf("[expected]: Write CPU GPR with lock done\n");
    }
    if (!access_cpu_gpr(0, 3, true)) {
        printf("[expected]: Failed to write CPU GPR with lock\n");
    } else {
        printf("[error]: Write GPR with lock done\n");
    }
    if (!access_cpu_gpr(3, 4, false)) {
        printf("[error]: Failed to write CPU GPR without lock\n");
    } else {
        printf("[expected]: Write CPU GPR without lock done\n");
    }
    printf("\n");
}

int main(void)
{
    board_init();
    printf("sysctl example\n");

    /*Enable software reset*/
    ppor_reset_set_source_enable(ppor_ptr, ppor_reset_software);
    /*If a software reset occurs, clear the reset flag and print the message*/
    if (ppor_reset_get_flags(ppor_ptr) & ppor_reset_software) {
        ppor_reset_clear_flags(ppor_ptr, ppor_reset_software);
        printf("Software reset has occurred\n");
        while (1) {
        };
    }

    /*
     * test resource related APIs
     */
    test_resource_related();

    /*
     * test clock config related APIs
     */
    test_clock_related();

    /*
     * test CPU GPR related APIs
     */
    test_cpu_gpr_related();

    /*
     * reset soc
     */
    reset_soc();
    while(1);
    return 0;
}
