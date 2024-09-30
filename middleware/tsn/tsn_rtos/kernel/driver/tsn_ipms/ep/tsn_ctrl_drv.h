/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   tsn_ctrl_drv.h
 * \author zimmerli
 * \date   2020-01-15
 * \brief  TSN-EP (TSN_CTRL) driver
 *
 */

#ifndef TSN_CTRL_DRV_H_
#define TSN_CTRL_DRV_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/net/netdev.h>
#include <kernel/base/rtc.h>

#define TSN_PORT_INTERFACE_NAME_PREFIX "eth"

// ----------------------------------------------------------------
// maximum packet size: 1600 Byte
#define MAX_SDU_SZ 1600
#define MAX_TUSER  15 /*7*/

#define TX_HDR_SIZE 16 //TSN_HDR_SIZE
#define RX_HDR_SIZE 16 //TSN_HDR_SIZE

#define HDR_TQ_MASK  0x07
#define HDR_TQ_SHIFT 8
#define HDR_TU_MASK  0x07
#define HDR_TU_SHIFT 11

#define PTP_BUFFER_NUM 8

// ----------------------------------------------------------------

struct tsn_ctrl_dev_s;

typedef struct {
    int32_t tx;
    int32_t rx;
} delay_t;

//(16) modules 0x0001 - 0x8000
#define INCL_RTC    0x0001
#define INCL_SHAPER 0x0002
#define INCL_FPE    0x0004
#define INCL_CB0    0x0008
#define INCL_QCI    0x0010

typedef struct {
    uint16_t modules;
    uint16_t lwidth; //TODO tas_max --> lwidth
    uint8_t tqc;     //TODO hw_queues --> tqc
    uint8_t tqd;
    uint8_t dw;
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_revision;
    uint8_t fd;
    uint8_t sd;
} tsn_ep_core_info_t;

typedef struct {
    uint32_t baseaddr;
    uint32_t busfreq;
    tsn_ep_core_info_t *coreInfo;
    //TODO deprecated see coreInfo
    uint16_t tas_max;
    TaskHandle_t hTaskTsync;
    TaskHandle_t hTaskRx;
    TaskHandle_t hTaskTsf;
    struct dma_device_s *dmatx;
    struct dma_device_s *dmarx;
    SemaphoreHandle_t semDma;
    void *ptpArg[PTP_BUFFER_NUM];
    QueueHandle_t qTxPtp;
    QueueHandle_t qTsf[MAX_TUSER + 1];
    //uint8_t prio2tq[8]; //see netdev_s
    delay_t delay100;
    delay_t delay1000;
} tsn_ep_prv_t;

/**
 * \brief TSN-EP device driver
 */
typedef struct tsn_ctrl_dev_s {
    uint32_t id;             //!< device driver id			//REMARK do not use it as port id, tsn port id etc. !!!
    void *prv;               //!< pointer to private data
    struct netdev_s *netdev; //!< pointer to network device
    struct mdiobus_s *mdio;  //!< pointer to mdio device
    struct phydev_s *phydev; //!< pointer to PHY device
    struct rtc_s *rtc;       //!< pointer to rtc, can be NULL
} tsn_ctrl_dev_t;

struct tsn_ctrl_dev_s *tsn_ctrl_drv_register(addr_t baseaddr, uint32_t busfreq, uint8_t irqnum, struct dma_device_s *dmatx, struct dma_device_s *dmarx);
void tsn_ep_core_info(addr_t baseaddr, tsn_ep_core_info_t *tsn_core_info);
void tsn_ctrl_tsync_set_timer(struct netdev_s *netdev, uint32_t id, uint32_t period, uint32_t enable);
int32_t tsn_ctrl_tsync_tx(struct netdev_s *netdev, struct netb_s *netb, uint8_t tq, void *arg);
void tsn_ctrl_set_delay(struct tsn_ctrl_dev_s *tsn_dev, int speed, int32_t delay_tx, int32_t delay_rx);
void tsn_ctrl_set_prio2q_map(struct netdev_s *netdev);

#endif /* TSN_CTRL_DRV_H_ */
