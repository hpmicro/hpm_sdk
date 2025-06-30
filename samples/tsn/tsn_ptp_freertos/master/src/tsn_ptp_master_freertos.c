/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/*  HPM example includes. */
#include <stdio.h>
#include "board.h"
#include "hpm_gpio_drv.h"
#include "ptp4tsync.h"
#include "kernel/base/rtc.h"
#include "hpm_tsw_drv.h"
#include "hpm_tsw_phy_common.h"
#include "tsn_ptp_master_freertos.h"

#define APP_TSW_SZ_MEMPOOL           (100 * 1024)
#define APP_TSW_OFFS_TSN_PORT        (0x020000)
#define APP_TSW_OFFS_TSN_PORTN(port) ((port) * APP_TSW_OFFS_TSN_PORT)
#define APP_TSW_PORT_NUM             BOARD_TSW_PORT_NUM
#define APP_TSW_IRQ                  BOARD_TSW_IRQ
#define APP_TSW_RTC_PORT_NUM         TSW_SOC_RTC_PORT + 1

static uint8_t mac[MAC_ADDR_LEN] = { MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5 };
static uint8_t _init_ptp4tsync = 1;
struct tsn_ctrl_dev_s *p;

extern void _rtc_init(uint32_t baseaddr, uint32_t busfreq);
extern void _rtc_get_local_time64(struct rtc_s *rtc, struct timespec64 *localtime);
extern void _rtc_get_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset);
extern void _rtc_set_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset);
extern void _rtc_adjfine(struct rtc_s *rtc, int64_t ppb);
extern void task_tsn_tsync(void *pvParameters);
extern void _tsync_init(uint32_t baseaddr, uint32_t busfreq);
extern void _tsn_irq_handler(struct tsn_ctrl_dev_s *tsn_dev);

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(4) uint8_t mempool[APP_TSW_SZ_MEMPOOL];

static tsw_phy_status_t last_status = {.tsw_phy_link = tsw_phy_link_unknown};

static void preset_system_rtc(struct rtc_s *rtc, uint32_t presetvalue)
{
    struct timespec64 rtc_offset;

    rtc_offset.tv_sec = presetvalue;
    rtc_offset.tv_nsec = 0;
    rtc_set_rtc_offset(rtc, &rtc_offset);
}

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t tsw_init(TSW_Type *ptr)
{
    rtl8211_config_t phy_config;

    /* Disable all MACs(TX/RX) */
    tsw_ep_disable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    /* Set Mac Address */
    tsw_ep_set_mac_addr(ptr, BOARD_TSW_PORT, mac, true);

    /* Set MAC Mode: GMII, CLKSEL: refclk */
    tsw_ep_set_mac_mode(ptr, BOARD_TSW_PORT, BOARD_TSW_PORT_ITF == tsw_port_phy_itf_rgmii ? tsw_mac_mode_gmii : tsw_mac_mode_mii);

    /* Set port PHY interface */
    tsw_set_port_interface(ptr, BOARD_TSW_PORT, BOARD_TSW_PORT_ITF);

    /* Enable all MACs(TX/RX) */
    tsw_ep_enable_all_mac_ctrl(ptr, tsw_mac_type_emac);

    /* Clear CAM */
    tsw_clear_cam(ptr);

    /* Wait for cam clearing completion */
    board_delay_ms(10);

    /* Enable VLAN-ID 1 at all ports */
    tsw_set_cam_vlan_port(ptr);

    /* Set MDC clock frequency to 2.5MHz */
    tsw_ep_set_mdio_config(BOARD_TSW, BOARD_TSW_PORT, 19);

    /* Initialize PHY */
    rtl8211_reset(ptr, BOARD_TSW_PORT);
    rtl8211_basic_mode_default_config(ptr, &phy_config);
    if (rtl8211_basic_mode_init(ptr, BOARD_TSW_PORT, &phy_config) == true) {
        printf("TSW phy init passed !\n");
        return status_success;
    } else {
        printf("TSW phy init failed !\n");
        return status_fail;
    }

    return status_success;
}

SDK_DECLARE_EXT_ISR_M(APP_TSW_IRQ, isr_tsw)
void isr_tsw(void)
{
    _tsn_irq_handler(p);
}

void tsw_self_adaptive_port_speed(void)
{
    tsw_phy_status_t status = { 0 };

    tsw_port_speed_t port_speed[] = { tsw_port_speed_10mbps, tsw_port_speed_100mbps, tsw_port_speed_1000mbps };
    char *speed_str[] = { "10Mbps", "100Mbps", "1000Mbps" };
    char *duplex_str[] = { "Half duplex", "Full duplex" };

    rtl8211_get_phy_status(BOARD_TSW, BOARD_TSW_PORT, &status);

    if (status.tsw_phy_link || (status.tsw_phy_link != last_status.tsw_phy_link)) {
        if (memcmp(&last_status, &status, sizeof(tsw_phy_status_t)) != 0) {
            memcpy(&last_status, &status, sizeof(tsw_phy_status_t));
            if (status.tsw_phy_link) {
                printf("Link Status: Up\n");
                printf("Link Speed:  %s\n", speed_str[status.tsw_phy_speed]);
                printf("Link Duplex: %s\n", duplex_str[status.tsw_phy_duplex]);

                tsw_set_port_speed(BOARD_TSW, BOARD_TSW_PORT, port_speed[status.tsw_phy_speed]);

                if (!status.tsw_phy_duplex) {
                    printf("Error: PHY is in half duplex now, but TSW MAC supports only full duplex mode!\n");
                    return;
                }
            } else {
                printf("Link Status: Down\n");
            }
        }
    }
}

void timer_callback(TimerHandle_t xTimer)
{
    (void)xTimer;

    tsw_self_adaptive_port_speed();
}

static void task_tsw_init(void *pvParameters)
{
    (void)pvParameters;

    TimerHandle_t timer_handle;
    char *tsync_name = kallocz(4 + 5 + 1);
    tsn_ep_prv_t *ep_prv;

    /* allocate private struct */
    ep_prv = kallocz(sizeof(tsn_ep_prv_t));
    configASSERT(ep_prv != NULL);
    ep_prv->baseaddr = HPM_TSW_BASE + APP_TSW_OFFS_TSN_PORTN(APP_TSW_PORT_NUM);
    ep_prv->busfreq = TSW_BUS_FREQ;

    /* allocate port devices */
    p = (struct tsn_ctrl_dev_s *)kallocz(sizeof(struct tsn_ctrl_dev_s) * 1);
    configASSERT(p != NULL);
    p->prv = ep_prv;
    p->id = 0;

    /* allocate tsn device (netdev) */
    p->netdev = netdev_register();
    configASSERT(p->netdev != NULL);

    /* tsn port (netdev initialization) */
    netdev_set_name(p->netdev, TSN_PORT_INTERFACE_NAME_PREFIX, 0);
    p->netdev->tsn_port = 0;
    p->netdev->prv = ep_prv;
    p->netdev->needed_head = TX_HDR_SIZE;

    tsn_ctrl_set_prio2q_map(p->netdev);

    /* create rtc */
    p->rtc = rtc_register();
    configASSERT(p->rtc != NULL);

    if (p->rtc) {
        _rtc_init(HPM_TSW_BASE + APP_TSW_OFFS_TSN_PORTN(APP_TSW_RTC_PORT_NUM), TSW_BUS_FREQ);
        p->rtc->port = p;
        p->rtc->prv = (void *)(HPM_TSW_BASE + APP_TSW_OFFS_TSN_PORTN(APP_TSW_RTC_PORT_NUM));
        p->rtc->getLocalTime64 = _rtc_get_local_time64;
        p->rtc->getRtcOffset = _rtc_get_rtc_offset;
        p->rtc->setRtcOffset = _rtc_set_rtc_offset;
        p->rtc->adjFine = _rtc_adjfine;
    }

    tsw_set_pps_tod_output(HPM_TSW);

    _tsync_init(HPM_TSW_BASE + APP_TSW_OFFS_TSN_PORTN(APP_TSW_PORT_NUM), TSW_BUS_FREQ);

    intc_m_enable_irq_with_priority(APP_TSW_IRQ, 1);

    ep_prv->qTxPtp = xQueueCreate(PTP_BUFFER_NUM, sizeof(uint8_t));
    configASSERT(ep_prv->qTxPtp != NULL);

    for (uint8_t i = 0; i < PTP_BUFFER_NUM; i++) {
        xQueueSend(ep_prv->qTxPtp, &i, 0);
    }

    strcpy(tsync_name, (const char *)"syn ");
    strcat(tsync_name, p->netdev->name);
    kTaskCreate_e(DRIVER_CONTEXT, task_tsn_tsync, tsync_name, (configMINIMAL_STACK_SIZE + 1024), (void *)p, KERNEL_PRIO_HIGH, &ep_prv->hTaskTsync);

    tsn_ctrl_set_delay(p, 1000, 250, 250);
    tsn_ctrl_set_delay(p, 100, 500, 500);

    netdev_set_addr(p->netdev, mac, 1);

    preset_system_rtc(p->rtc, RTC_PRESET);

    if (_init_ptp4tsync) {
        ptp4sync_start(p->rtc, mac);
        ptp4sync_add_domain(0, 253, 1, PTP4SYNC_PRIMARY);
        ptp4sync_set_traffic_prio(0, 0);
        ptp4sync_add_port(0, 0, p->netdev);
    }

    timer_handle = xTimerCreate((const char *)NULL, (TickType_t)1000, (UBaseType_t)pdTRUE, (void *const)1, (TimerCallbackFunction_t)timer_callback);

    if (NULL != timer_handle) {
        xTimerStart(timer_handle, 0);
    }

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    /* Initialize GPIOs */
    board_init_tsw_pins(BOARD_TSW);

    /* Reset an TSW PHY */
    board_reset_tsw_phy(BOARD_TSW, BOARD_TSW_PORT);

    printf("This is a TSW demo: PTP Master\n");

#if defined(RGMII) && RGMII
    board_init_tsw_rgmii_clock_delay(BOARD_TSW, BOARD_TSW_PORT);
#endif

    tsw_init(BOARD_TSW);

    kernel_init((addr_t)mempool, APP_TSW_SZ_MEMPOOL);
    kernel_start(task_tsw_init, 0);

    for (;;) {
    }

    return 0;
}
