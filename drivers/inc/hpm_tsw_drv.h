/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_TSW_DRV_H
#define HPM_TSW_DRV_H

/*---------------------------------------------------------------------
 * Includes
 *--------------------------------------------------------------------*/
#include "hpm_common.h"
#include "hpm_soc_feature.h"
#include "hpm_tsw_regs.h"

/**
 * @brief TSW driver APIs
 * @defgroup tsw_interface TSW driver APIs
 * @ingroup communication_interfaces
 * @{
 */

/*---------------------------------------------------------------------
 *  Macro Constant Declarations
 *-------------------------------------------------------------------*/
#define MAC_LO(mac) (uint32_t)(mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24))
#define MAC_HI(mac) (uint32_t)(mac[4] | (mac[5] << 8))

#define MAC_MDIO_CTRL_OP_WR (0x01)
#define MAC_MDIO_CTRL_OP_RD (0x02)

#ifndef TSW_SEND_DESC_COUNT
#define TSW_SEND_DESC_COUNT (16U)
#endif

#ifndef TSW_RECV_DESC_COUNT
#define TSW_RECV_DESC_COUNT (16U)
#endif

#ifndef TSW_SEND_BUFF_LEN
#define TSW_SEND_BUFF_LEN   (1536U)
#endif

#ifndef TSW_RECV_BUFF_LEN
#define TSW_RECV_BUFF_LEN   (1536U)
#endif

#ifndef TSW_NS_IN_ONE_SEC
#define TSW_NS_IN_ONE_SEC   (1000000000UL)
#endif

#ifndef TSW_BUS_FREQ
#define TSW_BUS_FREQ        (100000000UL)
#endif
/*---------------------------------------------------------------------
 *  Typedef Struct Declarations
 *-------------------------------------------------------------------*/
typedef struct {
    union {
        uint32_t tx_hdr0;
        struct {
            uint32_t dest_port: 8; /**< dest port */
            uint32_t          : 8; /**< reserved */
            uint32_t queue    : 3; /**< the priority queue for TSW TX */
            uint32_t utag     : 3; /**< TSW-EP TX user sideband information */
            uint32_t          : 6; /**< reserved */
            uint32_t htype    : 4; /**< header type */
        } tx_hdr0_bm;
    };

    union {
        uint32_t tx_hdr1;
        struct {
            uint32_t cb: 32; /**< CB field. Optionally used for external stream identification */
        } tx_hdr1_bm;
    };

    uint32_t tx_hdr2; /**< reserved */
    uint32_t tx_hdr3; /**< reserved */
} tx_hdr_desc_t;

typedef struct {
  uint8_t   id;
  uint8_t  *buffer;
  uint16_t  length;
} tsw_frame_t;

typedef struct {
    bool    soe;
    bool    irq;
    uint8_t maxlen;
} tsw_dma_config_t;

typedef struct {
    uint16_t admin_list_length;
    uint16_t oper_list_length;
} tsw_shap_tas_listlen_t;


typedef struct  {
    uint8_t     state;      /**< gate state vector */
    uint8_t     op;         /**< gate operation code (set, set-and-hold, set-and-release) */
    uint32_t    interval;   /**< interval time in nanoseconds */
} tsw_tas_controllist_entry_t;

typedef struct {
    tsw_tas_controllist_entry_t *entry;
    uint32_t entry_count;
    uint32_t cycle_time;     /**< cycle length in nanoseconds */
    uint32_t base_time_ns;   /**< based on real time */
    uint32_t base_time_sec;  /**< based on real time */
} tsw_tas_config_t;

typedef struct {
    uint8_t integer;
    uint16_t fract;
} tsw_cbs_config_t;

typedef struct {
    uint32_t    tstamplo;
    uint32_t    tstamphi;
    uint8_t     tqueue;
    uint8_t     tuser;
} tsw_tsf_t;

/*---------------------------------------------------------------------
 *  Typedef Enum Declarations
 *-------------------------------------------------------------------*/
typedef enum {
  tsw_port_speed_10mbps   = 2,
  tsw_port_speed_100mbps  = 3,
  tsw_port_speed_1000mbps = 0
} tsw_port_speed_t;

typedef enum {
  tsw_port_phy_itf_mii   = 0,
  tsw_port_phy_itf_rmii  = 4,
  tsw_port_phy_itf_rgmii = 1
} tsw_port_phy_itf_t;

typedef enum {
  tsw_dst_port_null = 0,
  tsw_dst_port_cpu  = 1 << 0,
  tsw_dst_port_1    = 1 << 1,
  tsw_dst_port_2    = 1 << 2,
  tsw_dst_port_3    = 1 << 3,
} tsw_dst_t;

typedef enum {
  tsw_cpu_send_to_lookup    = 0x00,
  tsw_cpu_send_to_port_1    = 0x01,
  tsw_cpu_send_to_port_2    = 0x02,
  tsw_cpu_send_to_port_3    = 0x03,
  tsw_cpu_send_to_all_ports = 0x80
} tsw_cpu_send_to_port_t;

typedef enum {
  tsw_mac_mode_mii = 0,
  tsw_mac_mode_gmii
} tsw_mac_mode_t;

typedef enum {
    tsw_pps_ctrl_pps = 0,
    tsw_pps_ctrl_bin_2hz_digital_1hz,
    tsw_pps_ctrl_bin_4hz_digital_2hz,
    tsw_pps_ctrl_bin_8hz_digital_4hz,
    tsw_pps_ctrl_bin_16hz_digital_8hz,
    tsw_pps_ctrl_bin_32hz_digital_16hz,
    tsw_pps_ctrl_bin_64hz_digital_32hz,
    tsw_pps_ctrl_bin_128hz_digital_64hz,
    tsw_pps_ctrl_bin_256hz_digital_128hz,
    tsw_pps_ctrl_bin_512hz_digital_256hz,
    tsw_pps_ctrl_bin_1024hz_digital_512hz,
    tsw_pps_ctrl_bin_2048hz_digital_1024hz,
    tsw_pps_ctrl_bin_4096hz_digital_2048hz,
    tsw_pps_ctrl_bin_8192hz_digital_4096hz,
    tsw_pps_ctrl_bin_16384hz_digital_8192hz,
    tsw_pps_ctrl_bin_32768hz_digital_16384hz
} tsw_pps_ctrl_t;

typedef enum {
  tsw_shap_tas_aclist_op_set_gate_states = 0,
  tsw_shap_tas_aclist_op_set_and_hold_mac,
  tsw_shap_tas_aclist_op_set_and_release_mac
} tsw_shap_tas_alist_op_t;

typedef enum {
    tsw_shap_tas_aclist_state_open_queueu_0 = 1 << 0,
    tsw_shap_tas_aclist_state_open_queueu_1 = 1 << 1,
    tsw_shap_tas_aclist_state_open_queueu_2 = 1 << 2,
    tsw_shap_tas_aclist_state_open_queueu_3 = 1 << 3,
    tsw_shap_tas_aclist_state_open_queueu_4 = 1 << 4,
    tsw_shap_tas_aclist_state_open_queueu_5 = 1 << 5,
    tsw_shap_tas_aclist_state_open_queueu_6 = 1 << 6,
    tsw_shap_tas_aclist_state_open_queueu_7 = 1 << 7,
    tsw_shap_tas_aclist_state_open_queueu_all = 0xff,
} tsw_shap_tas_aclist_state_open_queue_t;

typedef enum {
  tsw_traffic_queue_0 = 0,
  tsw_traffic_queue_1,
  tsw_traffic_queue_2,
  tsw_traffic_queue_3,
  tsw_traffic_queue_4,
  tsw_traffic_queue_5,
  tsw_traffic_queue_6,
  tsw_traffic_queue_7
} tsw_traffic_queue_t;

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */
/*---------------------------------------------------------------------
 * Exported Functions
 *-------------------------------------------------------------------*/
/**
 * @brief Get default DMA configuration
 * @param[out] config  DMA configuration struct @ref tsw_dma_config_t
*/
void tsw_get_default_dma_config(tsw_dma_config_t *config);

/**
 * @brief   Initialize TSW send DMA
 *
 * @param ptr  TSW peripheral base address
 * @param config  DMA configuration struct @ref tsw_dma_config_t
 */
void tsw_init_send(TSW_Type *ptr, tsw_dma_config_t *config);

/**
 * @brief   Initialize TSW receive DMA
 *
 * @param ptr   TSW peripheral base address
 * @param config  DMA configuration struct @ref tsw_dma_config_t
 */
void tsw_init_recv(TSW_Type *ptr, tsw_dma_config_t *config);

/**
 * @brief Send a frame to CPU port
 *
 * @param[in] ptr    TSW peripheral base address
 * @param[in] length Frame length
 * @param[in] id     Frame index
 * @return           Result of the transmission
 */
hpm_stat_t tsw_send_frame(TSW_Type *ptr, uint8_t *buffer, uint16_t length, uint8_t id);

/**
 * @brief Commit a receive DMA descriptor
 *
 * @param[in] ptr    TSW peripheral base address
 * @param[in] buffer Pointer to a buffer of the frame to be received
 * @param[in] length Buffer length
 * @param[in] id     Frame index
 * @return    Result of the received frame
 */
hpm_stat_t tsw_commit_recv_desc(TSW_Type *ptr, uint8_t *buffer, uint16_t length, uint8_t id);

/**
 * @brief Receive a frame from CPU port
 *
 * @param[in] ptr    TSW peripheral base address
 * @param[in] frame  a struct pointer to a tsw frame struct @ref tsw_frame_t
 * @return    Result of the received frame
 */
hpm_stat_t tsw_recv_frame(TSW_Type *ptr, tsw_frame_t *frame);

/**
 * @brief Lookup Bypass Setting
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] dst_port Destination port number
 */
void tsw_mac_lookup_bypass(TSW_Type *ptr, uint8_t dst_port);

/**
 * @brief CAM VLAN Setting
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] dst_port Destination port number
 */
void tsw_set_cam_vlan_port(TSW_Type *ptr);

/**
 * @brief MDIO Interface Config
 *
 * @param[in] ptr     TSW peripheral base address
 * @param[in] port    TSW port number
 * @param[in] clk_div TSW clock division
 * @return            Result of MDIO interface config
 */
hpm_stat_t tsw_ep_set_mdio_config(TSW_Type *ptr, uint8_t port, uint8_t clk_div);

/**
 * @brief MDIO Read
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] phy_addr TSW clock division
 * @param[in] reg_addr PHY register address
 * @param[out] data    Pointer to data memory
 * @return             Result of MDIO read
 */
hpm_stat_t tsw_ep_mdio_read(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, uint32_t reg_addr, uint16_t *data);

/**
 * @brief MDIO Write
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] phy_addr TSW clock division
 * @param[in] reg_addr PHY register address
 * @param[in] data     Data value
 * @return             Result of MDIO write
 */
hpm_stat_t tsw_ep_mdio_write(TSW_Type *ptr, uint8_t port, uint32_t phy_addr, uint32_t reg_addr, uint16_t data);

/**
 * @brief Enable MAC Controller
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] mac_type MAC type 0:EMAC/1:PMAC
 * @return             Result of enabling MAC controller
 */
hpm_stat_t tsw_ep_enable_mac_ctrl(TSW_Type *ptr, uint8_t port, uint8_t mac_type);

/**
 * @brief Disable MAC Controller
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] mac_type MAC type 0:EMAC/1:PMAC
 * @return             Result of disabling MAC controller
 */
hpm_stat_t tsw_ep_disable_mac_ctrl(TSW_Type *ptr, uint8_t port, uint8_t mac_type);

/**
 * @brief Enable All MAC Controllers
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] mac_type MAC type 0:EMAC/1:PMAC
 * @return             Result of enabling all MAC controllers
 */
hpm_stat_t tsw_ep_enable_all_mac_ctrl(TSW_Type *ptr, uint8_t mac_type);

/**
 * @brief Disable All MAC Controllers
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] mac_type MAC type 0:EMAC/1:PMAC
 * @return             Result of disabling all MAC controllers
 */
hpm_stat_t tsw_ep_disable_all_mac_ctrl(TSW_Type *ptr, uint8_t mac_type);

/**
 * @brief Set MAC Address
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] mac_addr Pointer to MAC address
 * @param[in] promisc  Promiscuous Mode: Set true to enable; set false to disable
 * @return             Result of setting MAC address
 */
hpm_stat_t tsw_ep_set_mac_addr(TSW_Type *ptr, uint8_t port, uint8_t *mac_addr, bool promisc);

/**
 * @brief Set MAC Mode
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] mac_addr Pointer to MAC address
 * @param[in] promisc  Promiscuous Mode: Set true to enable; set false to disable
 * @return             Result of setting MAC address
 */
hpm_stat_t tsw_ep_set_mac_mode(TSW_Type *ptr, uint8_t port, uint8_t gmii);

/**
 * @brief Set Port GPR
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] speed    Pointer to MAC address
 * @param[in] itf      Promiscuous Mode: Set true to enable; set false to disable
 * @param[in] tx_dly   Tx delay
 * @param[in] rx_dlay  Rx delay
 */
void tsw_port_gpr(TSW_Type *ptr, uint8_t port, uint8_t speed, uint8_t itf, uint8_t tx_dly, uint8_t rx_dly);

/**
 * @brief Set TSW port speed
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port number
 * @param[in] speed     TSW port speed @ref tsw_port_speed_t
 */
void tsw_set_port_speed(TSW_Type *ptr, uint8_t port, uint8_t speed);

/**
 * @brief Set TSW port interface type
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port number
 * @param[in] itf       TSW port speed @ref tsw_port_phy_itf_t
 */
void tsw_set_port_interface(TSW_Type *ptr, uint8_t port, uint8_t itf);

/**
 * @brief Set TSW port clock delay
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port number
 * @param[in] tx_dly    TSW port TX clock delay
 * @param[in] rx_dly    TSW port RX clock delay
 */
void tsw_set_port_clock_delay(TSW_Type *ptr, uint8_t port, uint8_t tx_dly, uint8_t rx_dly);

/**
 * @brief Set Internal Frame Action
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] dest_port Destination port number
 */
void tsw_set_internal_frame_action(TSW_Type *ptr, uint8_t dest_port);

/**
 * @brief Set Broadcast Frame Action
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] dest_port Destination port number
 */
void tsw_set_broadcast_frame_action(TSW_Type *ptr, uint8_t dest_port);

/**
 * @brief Set Unknow Frame Action
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] dest_port Destination port number
 */
void tsw_set_unknown_frame_action(TSW_Type *ptr, uint8_t dest_port);

/**
 * @brief Set Lookup Table
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] entry_num Entry number
 * @param[in] dest_port Destination port number
 * @param[in] dest_mac  Destination MAC address
 */
void tsw_set_lookup_table(TSW_Type *ptr, uint16_t entry_num, uint8_t dest_port, uint64_t dest_mac);

/**
 * @brief Clear CAM
 *
 * @param[in] ptr       TSW peripheral base address
 */
void tsw_clear_cam(TSW_Type *ptr);

/**
 * @brief Enable RXFIFO to store and forward mode
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port number
 */
void tsw_enable_store_forward_mode(TSW_Type *ptr, uint8_t port);

/**
 * @brief Disable RXFIFO to store and forward mode
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port number
 */
void tsw_disable_store_forward_mode(TSW_Type *ptr, uint8_t port);

/**
 * @brief Get RTC timer increment value.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] increment local time increment
*  @return              Result of getting RTC timer increment value @ref hpm_stat_t
 */
hpm_stat_t tsw_get_rtc_time_increment(TSW_Type *ptr, uint32_t *increment);

/**
 * @brief Set RTC timer increment value.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] increment local time increment
*  @return              Result of setting RTC timer increment value @ref hpm_stat_t
 */
hpm_stat_t tsw_set_rtc_time_increment(TSW_Type *ptr, uint32_t increment);

/**
 * @brief Get RTC current time.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[out] sec      seconds
 * @param[out] nsec     nanoseconds
 * @return              Result of getting RTC current time @ref hpm_stat_t
 */
hpm_stat_t tsw_get_rtc_current_time(TSW_Type *ptr, uint32_t *sec, uint32_t *nsec);

/**
 * @brief Get RTC offset.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[out] sec      seconds
 * @param[out] nsec     nanoseconds
 * @return              Result of getting RTC offset @ref hpm_stat_t
 */
hpm_stat_t tsw_get_rtc_offset(TSW_Type *ptr, int64_t *sec, uint32_t *nsec);

/**
 * @brief Set RTC offset.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] sec       seconds
 * @param[in] nsec      nanoseconds
 * @return              Result of setting RTC offset @ref hpm_stat_t
 */
hpm_stat_t tsw_set_rtc_offset(TSW_Type *ptr, int64_t sec, uint32_t nsec);

/**
 * @brief Set RTC offset change.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] change    offset change
 * @return              Result of setting RTC offset change @ref hpm_stat_t
 */
hpm_stat_t tsw_set_rtc_offset_change(TSW_Type *ptr, uint32_t change);

/**
 * @brief Set TSYNC timer host clock divider.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] host_clkdiv    TSYNC timer host clock divider
 * @return              Result of setting TSYNC timer host clock divider @ref hpm_stat_t
 */
hpm_stat_t tsw_set_tsync_timer_hclkdiv(TSW_Type *ptr, uint8_t port, uint32_t host_clkdiv);

/**
 * @brief TSYNC timer control.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] index     TSYNC timer index
 * @param[in] period    TSYNC timer period
 * @param[in] enable    TSYNC timer enable
 * @return              Result of setting TSYNC timer control @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_timer_control(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t period, uint32_t enable);

/**
 * @brief TSYNC timer interrupt enable.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @return              Result of enabling TSYNC timer interrupt @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_timer_interrupt_enable(TSW_Type *ptr, uint8_t port);

/**
 * @brief TSYNC update data.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] bin       TSYNC bin
 * @param[in] binofs    TSYNC bin offset
 * @param[in] srcaddr   Source address of data to be updated
 * @param[in] lenbytes  Length in bytes
 * @return              Result of updating TSYNC length @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_update_data(TSW_Type *ptr, uint8_t port, uint32_t bin, uint32_t binofs, uint32_t srcaddr, uint8_t lenbytes);

/**
 * @brief TSYNC update length.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] bin       TSYNC bin
 * @param[in] lenbytes  Length in bytes
 * @return              Result of updating TSYNC length @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_update_len(TSW_Type *ptr, uint8_t port, uint32_t bin, uint8_t lenbytes, uint8_t tqueue);

/**
 * @brief TSYNC trigger TX.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] bin       TSYNC bin
 * @return              Result of triggering TSYNC TX @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_trigger_tx(TSW_Type *ptr, uint8_t port, uint32_t bin);

/**
 * @brief TSYNC get TX timestamp.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] bin       TSYNC bin
 * @param[out] timestamplo TSYNC TX timestamp low
 * @param[out] timestamphi TSYNC TX timestamp high
 * @return              Result of getting TSYNC TX timestamp @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_get_txtimestamp(TSW_Type *ptr, uint8_t port, uint32_t bin, uint32_t *timestamplo, uint32_t *timestamphi);

/**
 * @brief TSYNC get RX timestamp.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[out] timestamplo TSYNC RX timestamp low
 * @param[out] timestamphi TSYNC RX timestamp high
 * @return              Result of getting TSYNC RX timestamp @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_get_rxtimestamp(TSW_Type *ptr, uint8_t port, uint32_t *timestamplo, uint32_t *timestamphi);

/**
 * @brief TSYNC disable current events.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[out] disa     Disable current events
 * @return              Result of disabling current events @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_disable_current_events(TSW_Type *ptr, uint8_t port, uint32_t *disabled);

/**
 * @brief TSYNC get RX status.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[out] ov       Overflow status
 * @param[out] rxstatus RX status
 * @return              Result of getting RX status @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_get_rxstatus(TSW_Type *ptr, uint8_t port, uint8_t *ov, uint8_t *avnxt, uint8_t *rxsel);

/**
 * @brief TSYNC next RX buffer.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @return              Result of next RX buffer @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_next_rxbuffer(TSW_Type *ptr, uint8_t port);

/**
 * @brief TSYNC clear overflow status.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @return              Result of clearing overflow status @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_clear_overflow(TSW_Type *ptr, uint8_t port);

/**
 * @brief TSYNC get tx done status.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[out] done     Tx done status
 * @return              Result of getting tx done status @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_get_txdone(TSW_Type *ptr, uint8_t port, uint32_t *done);

/**
 * @brief TSYNC clear tx done status.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] done      Tx done status
 * @return              Result of clearing tx done status @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_clear_txdone(TSW_Type *ptr, uint8_t port, uint32_t done);

/**
 * @brief TSYNC get tmr done status.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] done      Tmr done status
 * @return              Result of getting tmr done status @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_get_tmrdone(TSW_Type *ptr, uint8_t port, uint32_t *done);

/**
 * @brief TSW get shap tas listlen.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[out] listlen  TAS listlen
 * @return              Result of getting shap tas listlen @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_get_tas_listlen(TSW_Type *ptr, uint8_t port, tsw_shap_tas_listlen_t *listlen);

/**
 * @brief SHAP set tas cycle.
 *
 * @param[in] ptr        TSW peripheral base address
 * @param[in] port       TSW port
 * @param[in] cycle_time TAS cycle time
 * @return               Result of setting tas cycle @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas_cycletime(TSW_Type *ptr, uint8_t port, uint32_t cycle_time);

/**
 * @brief SHAP set tas controllist.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] index     Index of the controllist
 * @param[in] entry     Pointer to a structure of tsw_shap_tas_listlen_t @ref tsw_shap_tas_listlen_t
 * @return              Result of setting tas controllist @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas_controllist(TSW_Type *ptr, uint8_t port, uint32_t index, tsw_tas_controllist_entry_t *entry);

/**
 * @brief SHAP get tas controllist.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] index     Index of the traffic queue
 * @param[out] entry    Pointer to a structure of tsw_shap_tas_controllist_config_t @ref tsw_shap_tas_controllist_config_t
 * @return              Result of getting tas controllist @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_get_tas_controllist(TSW_Type *ptr, uint8_t port, uint32_t index, tsw_tas_controllist_entry_t *entry);

/**
 * @brief Set tas maximum SDU ticks
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] index     Index of the traffic queue
 * @param[in] ticks     Maximum SDU ticks
 * @return              Result of setting tas maximum SDU ticks @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas_max_sdu_ticks(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t ticks);

/**
 * @brief Get tas maximum SDU ticks
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] index     Index of the traffic queue
 * @param[out] ticks    Maximum SDU ticks
 * @return              Result of getting tas maximum SDU ticks @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_get_tas_max_sdu_ticks(TSW_Type *ptr, uint8_t port, uint8_t index, uint32_t *ticks);

/**
 * @brief Set pps tod output
 *
 * @param[in] ptr       TSW peripheral base address
 * @return              Result of setting pps tod output @ref hpm_stat_t
 */
hpm_stat_t tsw_set_pps_tod_output(TSW_Type *ptr);

/**
 * @brief Enable TAS
 *
 * @param ptr[in]       TSW peripheral base address
 * @param port[in]      TSW port
 * @return hpm_stat_t   Result of enabling tas @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_enable_tas(TSW_Type *ptr, uint8_t port);

/**
 * @brief Disable TAS
 *
 * @param ptr[in]       TSW peripheral base address
 * @param port[in]      TSW port
 * @return hpm_stat_t   Result of disabling tas @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_disable_tas(TSW_Type *ptr, uint8_t port);

/**
 * @brief Set TAS list length
 *
 * @param ptr[in]       TSW peripheral base address
 * @param port[in]      TSW port
 * @param listlen[in]   list length
 * @return hpm_stat_t   Result of setting tas list length @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas_listlen(TSW_Type *ptr, uint8_t port, uint32_t listlen);

/**
 * @brief Set TAS cycle time
 *
 * @param ptr[in]        TSW peripheral base address
 * @param port[in]       TSW port
 * @param cycle_time[in] cycle time
 * @return hpm_stat_t    Result of setting tas cycle time @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas_cycletime(TSW_Type *ptr, uint8_t port, uint32_t cycle_time);

/**
 * @brief Set TAS base time
 *
 * @param ptr[in]          TSW peripheral base address
 * @param port[in]         TSW port
 * @param basetime_sec[in] second part of base time
 * @param basetime_ns[in]  nanosecond part of base time
 * @return hpm_stat_t      Result of setting tas base time @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas_basetime(TSW_Type *ptr, uint8_t port, uint32_t basetime_sec, uint32_t basetime_ns);

/**
 * @brief Trigger to change TAS config
 *
 * @param ptr[in]          TSW peripheral base address
 * @param port[in]         TSW port
 * @return hpm_stat_t      Result of triggering to change tas config @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_tas_change_config(TSW_Type *ptr, uint8_t port);

/**
 * @brief Get TAS cycle time
 *
 * @param ptr[in]          TSW peripheral base address
 * @param port[in]         TSW port
 * @param crsr[out]        TAS Status
 * @return hpm_stat_t      Result of getting tas status @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_get_tas_crsr(TSW_Type *ptr, uint8_t port, uint32_t *crsr);

/**
 * @brief  Set TAS config
 *
 * @param ptr[in]         TSW peripheral base address
 * @param port[in]        TSW port
 * @param config[in]      Pointer to TAS config @ref tsw_tas_config_t
 * @return hpm_stat_t     Result of setting tas config @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas(TSW_Type *ptr, uint8_t port, tsw_tas_config_t *config);

/**
 * @brief Set CBS config
 *
 * @param ptr[in]         TSW peripheral base address
 * @param port[in]        TSW port
 * @param index[in]       Traffic queue index
 * @param config[in]      Pointer to CBS config @ref tsw_cbs_config_t
 * @return hpm_stat_t
 */
hpm_stat_t tsw_shap_set_cbs(TSW_Type *ptr, uint8_t port, uint8_t index, tsw_cbs_config_t *config);

/**
 * @brief Get used countsed count from the TX-Timestamp FIFO
 *
 * @param ptr[in]         TSW peripheral base address
 * @param port[in]        TSW port
 * @param count[out]      Pointer to used count in TX-Timestamp FIFO
 * @return hpm_stat_t     Result of getting used count from TX-Timestamp FIFO @ref hpm_stat_t
 */
hpm_stat_t tsw_get_txtimestampfifo_used(TSW_Type *ptr, uint8_t port, uint32_t *count);

/**
 * @brief Get TX-Timestamp FIFO entry
 *
 * @param ptr[in]         TSW peripheral base address
 * @param port[in]        TSW port
 * @param entry[out]      Pointer to TX-Timestamp FIFO entry @ref tsw_tsf_t
 * @return hpm_stat_t     Result of getting TX-Timestamp FIFO entry @ref hpm_stat_t
 */
hpm_stat_t tsw_get_txtimestampfifo_entry(TSW_Type *ptr, uint8_t port, tsw_tsf_t *entry);

#if defined __cplusplus
}
#endif /* __cplusplus */
/** @} */
#endif /* HPM_TSW_DRV_H */