/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * @file hpm_tsw_drv.h
 * @brief TSW (Time-Sensitive Switch) driver API definitions
 *
 * This file provides the API definitions for the TSW peripheral driver.
 * The TSW is a time-sensitive network switch that supports various
 * time-sensitive networking features including TAS (Time-Aware Shaper),
 * CBS (Credit-Based Shaper), FPE (Frame Preemption), FRER (Frame Replication
 * and Elimination for Reliability), and PSFP (Per-Stream Filtering and Policing).
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
/**
 * @brief Extract lower 32 bits of MAC address
 * @param mac MAC address array (6 bytes)
 * @return Lower 32 bits of MAC address
 */
#define MAC_LO(mac) (uint32_t)(mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24))

/**
 * @brief Extract upper 16 bits of MAC address
 * @param mac MAC address array (6 bytes)
 * @return Upper 16 bits of MAC address
 */
#define MAC_HI(mac) (uint32_t)(mac[4] | (mac[5] << 8))

/**
 * @brief MDIO control operation: Write
 */
#define MAC_MDIO_CTRL_OP_WR (0x01)

/**
 * @brief MDIO control operation: Read
 */
#define MAC_MDIO_CTRL_OP_RD (0x02)

/**
 * @brief Default send descriptor count
 * @note This can be overridden by defining TSW_SEND_DESC_COUNT before including this header
 */
#ifndef TSW_SEND_DESC_COUNT
#define TSW_SEND_DESC_COUNT (16U)
#endif

/**
 * @brief Default receive descriptor count
 * @note This can be overridden by defining TSW_RECV_DESC_COUNT before including this header
 */
#ifndef TSW_RECV_DESC_COUNT
#define TSW_RECV_DESC_COUNT (16U)
#endif

/**
 * @brief Default send buffer length in bytes
 * @note This can be overridden by defining TSW_SEND_BUFF_LEN before including this header
 */
#ifndef TSW_SEND_BUFF_LEN
#define TSW_SEND_BUFF_LEN   (1536U)
#endif

/**
 * @brief Default receive buffer length in bytes
 * @note This can be overridden by defining TSW_RECV_BUFF_LEN before including this header
 */
#ifndef TSW_RECV_BUFF_LEN
#define TSW_RECV_BUFF_LEN   (1536U)
#endif

/**
 * @brief Nanoseconds in one second
 * @note This can be overridden by defining TSW_NS_IN_ONE_SEC before including this header
 */
#ifndef TSW_NS_IN_ONE_SEC
#define TSW_NS_IN_ONE_SEC   (1000000000UL)
#endif

/**
 * @brief TSW bus frequency in Hz
 * @note This can be overridden by defining TSW_BUS_FREQ before including this header
 */
#ifndef TSW_BUS_FREQ
#define TSW_BUS_FREQ        (100000000UL)
#endif

/**
 * @brief MM2S DMA wait circular buffer timeout value
 * @note This can be overridden by defining TSW_MM2S_DMA_WAIT_CBUFF_TIMEOUT before including this header
 */
#ifndef TSW_MM2S_DMA_WAIT_CBUFF_TIMEOUT
#define TSW_MM2S_DMA_WAIT_CBUFF_TIMEOUT (1000U)
#endif

/**
 * @brief MM2S DMA check receive buffer empty timeout value
 * @note This can be overridden by defining TSW_MM2S_DMA_CHECK_RBUFE_TIMEOUT before including this header
 */
#ifndef TSW_MM2S_DMA_CHECK_RBUFE_TIMEOUT
#define TSW_MM2S_DMA_CHECK_RBUFE_TIMEOUT (1000U)
#endif

/**
 * @brief Ethernet MAC address size in bytes
 */
#define TSW_ENET_MAC              (6U)

/**
 * @brief FPE MMS minimum verify time minimum value (1ms)
 */
#define TSW_FPE_MMS_MIN_VTIME_MIN (1U)

/**
 * @brief FPE MMS maximum verify time maximum value (128ms)
 */
#define TSW_FPE_MMS_MAX_VTIME_MAX (128U)
/*---------------------------------------------------------------------
 *  Typedef Struct Declarations
 *-------------------------------------------------------------------*/
/**
 * @brief TSW TX header descriptor structure
 */
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

/**
 * @brief TSW RX header descriptor structure
 */
typedef struct {
    union {
        uint32_t rx_hdr0;
        struct {
            uint32_t src_port : 8; /**< source port */
            uint32_t          : 8; /**< reserved */
            uint32_t queue    : 3; /**< no effect on the CPU RX port*/
            uint32_t utag     : 3; /**< TSW-EP RX user sideband information */
            uint32_t          : 2; /**< reserved */
            uint32_t fpe      : 1; /**< indicates that a frame comes from preemptible stream */
            uint32_t          : 3; /**< reserved */
            uint32_t htype    : 4; /**< header type */
        } rx_hdr0_bm;
    };

    union {
        uint32_t rx_hdr1;
        struct {
            uint32_t cb: 32; /**< CB field. */
        } rx_hdr1_bm;
    };

    uint32_t timestamp_lo;  /**< RX Timestamp nanosecond part */
    uint32_t timestamp_hi;  /**< RX Timestamp second part */
} rx_hdr_desc_t;

/**
 * @brief TSW frame action configuration structure
 */
typedef struct {
    uint16_t dest;  /**< Destination port */
    uint8_t  queue; /**< Queue number */
    uint8_t  drop;  /**< Drop flag */
    uint8_t  qsel;  /**< Queue select */
    uint8_t  utag;  /**< User tag */
} tsw_frame_action_config_t;

/**
 * @brief TSW frame structure
 */
typedef struct {
  uint8_t   id;     /**< Frame ID */
  uint8_t  *buffer; /**< Frame buffer pointer */
  uint16_t  length; /**< Frame length */
} tsw_frame_t;

/**
 * @brief TSW DMA configuration structure
 */
typedef struct {
    bool    soe;    /**< Stop on error flag */
    bool    irq;    /**< Interrupt enable */
    uint8_t maxlen; /**< Maximum length */
} tsw_dma_config_t;

/**
 * @brief TSW SHAP TAS list length structure
 */
typedef struct {
    uint16_t admin_list_length; /**< Administrative list length */
    uint16_t oper_list_length;  /**< Operational list length */
} tsw_shap_tas_listlen_t;

/**
 * @brief TSW TAS control list entry structure
 */
typedef struct  {
    uint8_t     state;      /**< gate state vector */
    uint8_t     op;         /**< gate operation code (set, set-and-hold, set-and-release) */
    uint32_t    interval;   /**< interval time in nanoseconds */
} tsw_tas_controllist_entry_t;

typedef struct {
    tsw_tas_controllist_entry_t *entry; /**< pointer to the TAS control list entry */
    uint32_t entry_count;    /**< entry count */
    uint32_t cycle_time;     /**< cycle length in nanoseconds */
    uint32_t base_time_ns;   /**< based on real time */
    uint32_t base_time_sec;  /**< based on real time */
} tsw_tas_config_t;

/**
 * @brief TSW CBS (Credit-Based Shaper) configuration structure
 */
typedef struct {
    uint8_t integer; /**< Integer part */
    uint16_t fract;  /**< Fractional part */
} tsw_cbs_config_t;

/**
 * @brief TSW timestamp FIFO entry structure
 */
typedef struct {
    uint32_t    tstamplo; /**< Timestamp low */
    uint32_t    tstamphi; /**< Timestamp high */
    uint8_t     tqueue;   /**< Traffic queue */
    uint8_t     tuser;    /**< Traffic user */
} tsw_tsf_t;

/**
 * @brief TSW FPE MMS status structure
 */
typedef struct {
    bool vfail; /**< Verification fail */
    bool vok;   /**< Verification OK */
    bool hld;   /**< Hold */
} tsw_fpe_mms_status_t;

/**
 * @brief TSW FPE configuration structure
 */
typedef struct {
    uint8_t tqueue;          /**< Traffic queue */
    uint32_t vtime;          /**< Verify time */
    uint32_t frag_size;      /**< Fragment size */
    bool dis_verification;   /**< Disable verification */
    bool link_error;         /**< Link error */
} tsw_fpe_config_t;

/**
 * @brief TSW CB STMID lookup MAC structure
 */
typedef struct {
    uint32_t mach; /**< MAC address high */
    uint32_t macl; /**< MAC address low */
    uint32_t vid;  /**< VLAN ID */
} tsw_cb_stmid_lookup_mac_t;

/**
 * @brief TSW CB STMID active MAC structure
 */
typedef struct {
    uint32_t mach; /**< MAC address high */
    uint32_t macl; /**< MAC address low */
    uint8_t pcp;   /**< Priority code point */
    uint8_t vid;   /**< VLAN ID */
} tsw_cb_stmid_active_mac_t;

/**
 * @brief TSW CB STMID entry structure
 */
typedef struct {
    uint8_t idx;                                     /**< Entry index */
    bool enable;                                     /**< Enable flag */
    bool seqgen;                                     /**< Sequence generation enable */
    uint8_t smac;                                    /**< Source MAC index */
    uint8_t mode;                                    /**< Lookup mode */
    uint8_t actctl;                                  /**< Active destination MAC control */
    uint8_t sid;                                     /**< Stream ID */
    int32_t seqnum;                                  /**< Sequence number */
    uint32_t match;                                  /**< Match value */
    tsw_cb_stmid_lookup_mac_t lookup_mac;            /**< Lookup MAC address configuration */
    tsw_cb_stmid_active_mac_t active_mac;            /**< Active MAC address configuration */
} tsw_cb_stmid_entry_t;

/**
 * @brief TSW CB FRER XR function configuration structure
 */
typedef struct {
    bool fen;    /**< Function enable */
    uint8_t fidx; /**< Function index */
} tsw_cb_frer_xrfunc_config_t;

/**
 * @brief TSW CB FRER SID function configuration structure
 */
typedef struct {
    uint8_t sid;                                    /**< Stream ID */
    tsw_cb_frer_xrfunc_config_t irfunc;             /**< Individual recovery function */
    tsw_cb_frer_xrfunc_config_t srfunc;              /**< Sequence recovery function */
} tsw_cb_frer_sid_func_config_t;

/**
 * @brief TSW CB FRER latent error detection configuration structure
 */
typedef struct {
    bool  enable_detection; /**< Enable detection */
    uint32_t reset_period;  /**< Reset period */
    uint32_t test_period;   /**< Test period */
    uint32_t threshold;     /**< Threshold */
    uint32_t err_count;     /**< Error count */
} tsw_cb_frer_latent_error_dectecton_config_t;

/**
 * @brief TSW CB FRER recovery function configuration structure
 */
typedef struct {
    uint8_t fidx;                                           /**< Function index */
    bool freset;                                            /**< Function reset */
    bool take_no_sequence;                                  /**< Take no sequence */
    uint8_t paths;                                          /**< Number of paths */
    uint8_t history_len;                                    /**< History length */
    uint8_t algo;                                           /**< Algorithm */
    uint8_t xrfunc;                                         /**< XR function */
    uint32_t timeout_in_ms;                                 /**< Timeout in milliseconds */
    tsw_cb_frer_latent_error_dectecton_config_t latent_error_dectection_config; /**< Latent error detection config */
} tsw_cb_frer_recovery_func_config_t;

/**
 * @brief TSW PSFER gate control list entry structure
 */
typedef struct {
    uint8_t state;      /**< Gate state */
    uint8_t ipv;        /**< IP version */
    uint32_t max_octets; /**< Maximum octets */
    uint32_t interval;   /**< Interval */
} tsw_psfer_gate_control_list_entry_t;

/**
 * @brief TSW PSFER gate control list configuration structure
 */
typedef struct {
    tsw_psfer_gate_control_list_entry_t *entry; /**< Gate control list entries */
    uint8_t list_len;                           /**< List length */
    uint32_t cycle_time;                         /**< Cycle time */
    uint32_t base_time_ns;                       /**< Base time nanoseconds */
    uint32_t base_time_sec;                      /**< Base time seconds */
} tsw_psfer_gate_control_list_config_t;

/**
 * @brief TSW CB FRER frame count egress structure
 */
typedef struct {
    uint32_t egess_frame_count[8]; /**< Egress frame count array */
} tsw_cb_frer_frame_count_egress_t;

/**
 * @brief TSW PSFP gate static mode configuration structure
 */
typedef struct {
    uint8_t idx;                           /**< Gate index */
    bool closed_due_to_octets_exceeded;    /**< Closed due to octets exceeded */
    bool closed_due_to_invalid_rx;        /**< Closed due to invalid RX */
    uint8_t state;                         /**< Gate state */
    uint8_t ipv;                           /**< IP version */
} tsw_psfp_gate_static_mode_config_t;

/**
 * @brief TSW PSFP gate dynamic mode configuration structure
 */
typedef struct {
    uint8_t idx;                                        /**< Gate index */
    bool closed_due_to_octets_exceeded;                 /**< Closed due to octets exceeded */
    bool closed_due_to_invalid_rx;                      /**< Closed due to invalid RX */
    tsw_psfer_gate_control_list_config_t gate_control_list_config; /**< Gate control list configuration */
} tsw_psfp_gate_dynamic_mode_config_t;

/**
 * @brief TSW PSFP flow meter XIR configuration structure
 */
typedef struct {
    uint8_t integer; /**< Integer part */
    uint16_t fract;  /**< Fractional part */
} tsw_psfp_flow_meter_xir_config_t;

/**
 * @brief TSW PSFP flow meter configuration structure
 */
typedef struct {
    uint8_t idx;                              /**< Flow meter index */
    bool reset;                                /**< Reset flag */
    bool mark_all_frames_red;                  /**< Mark all frames red */
    bool drop_on_yellow;                       /**< Drop on yellow */
    bool color_mode;                           /**< Color mode */
    bool coupling_flag;                        /**< Coupling flag */
    tsw_psfp_flow_meter_xir_config_t cir;      /**< Committed information rate */
    tsw_psfp_flow_meter_xir_config_t eir;      /**< Excess information rate */
    uint32_t cbs_in_bits;                      /**< Committed burst size in bits */
    uint32_t ebs_in_bits;                      /**< Excess burst size in bits */
} tsw_psfp_flow_meter_config_t;

/**
 * @brief TSW PSFP filter configuration structure
 */
typedef struct {
    uint8_t idx;                      /**< Filter index */
    bool enable_blocking;              /**< Enable blocking */
    bool enable_size_checking;         /**< Enable size checking */
    bool enable_flow_meter;            /**< Enable flow meter */
    bool filter_match_sid;             /**< Filter match stream ID */
    bool filter_match_pcp;             /**< Filter match priority code point */
    uint8_t pcp;                       /**< Priority code point */
    uint8_t flow_meter_id;            /**< Flow meter ID */
    uint8_t gate_id;                   /**< Gate ID */
    uint8_t stream_id;                 /**< Stream ID */
    uint32_t max_frame_size_in_octects; /**< Maximum frame size in octets */
} tsw_psfp_filter_config_t;

/*---------------------------------------------------------------------
 *  Typedef Enum Declarations
 *-------------------------------------------------------------------*/
/**
 * @brief TSW port speed enumeration
 */
typedef enum {
  tsw_port_speed_10mbps   = 2,  /**< 10 Mbps */
  tsw_port_speed_100mbps  = 3,  /**< 100 Mbps */
  tsw_port_speed_1000mbps = 0   /**< 1000 Mbps */
} tsw_port_speed_t;

/**
 * @brief TSW port PHY interface type enumeration
 */
typedef enum {
  tsw_port_phy_itf_mii   = 0,  /**< MII interface */
  tsw_port_phy_itf_rmii  = 4,  /**< RMII interface */
  tsw_port_phy_itf_rgmii = 1   /**< RGMII interface */
} tsw_port_phy_itf_t;

/**
 * @brief TSW destination port enumeration
 */
typedef enum {
  tsw_dst_port_null = 0,      /**< Null destination port */
  tsw_dst_port_cpu  = 1 << 0, /**< CPU port */
  tsw_dst_port_1    = 1 << 1, /**< Port 1 */
  tsw_dst_port_2    = 1 << 2, /**< Port 2 */
  tsw_dst_port_3    = 1 << 3, /**< Port 3 */
} tsw_dst_t;

/**
 * @brief TSW frame action type enumeration
 */
typedef enum {
    tsw_frame_action_type_internal  = 0, /**< Internal frame action */
    tsw_frame_action_type_broadcast = 1, /**< Broadcast frame action */
    tsw_frame_action_type_unknown   = 2, /**< Unknown frame action */
} tsw_frame_action_type_t;

/**
 * @brief TSW frame action queue select type enumeration
 */
typedef enum {
    tsw_frame_action_qsel_type_use_pcp_vlan_pvid = 0, /**< Use PCP VLAN PVID */
    tsw_frame_action_qsel_type_use_pcp_remapping = 1, /**< Use PCP remapping */
    tsw_frame_action_qsel_type_reserved          = 2, /**< Reserved */
    tsw_frame_action_qsel_type_use_queue         = 3, /**< Use queue */
} tsw_frame_action_qsel_type_t;

/**
 * @brief TSW CPU send to port enumeration
 */
typedef enum {
  tsw_cpu_send_to_lookup    = 0x00, /**< Send to lookup */
  tsw_cpu_send_to_port_1    = 0x01, /**< Send to port 1 */
  tsw_cpu_send_to_port_2    = 0x02, /**< Send to port 2 */
  tsw_cpu_send_to_port_3    = 0x03, /**< Send to port 3 */
  tsw_cpu_send_to_all_ports = 0x80  /**< Send to all ports */
} tsw_cpu_send_to_port_t;

/**
 * @brief TSW MAC mode enumeration
 */
typedef enum {
  tsw_mac_mode_mii = 0,  /**< MII mode */
  tsw_mac_mode_gmii      /**< GMII mode */
} tsw_mac_mode_t;

/**
 * @brief TSW MAC type enumeration
 */
typedef enum {
    tsw_mac_type_emac = 0, /**< EMAC type */
    tsw_mac_type_pmac      /**< PMAC type */
} tsw_mac_type_t;

/**
 * @brief TSW PPS control enumeration
 */
typedef enum {
    tsw_pps_ctrl_pps = 0,                    /**< PPS control */
    tsw_pps_ctrl_bin_2hz_digital_1hz,         /**< Binary 2Hz, digital 1Hz */
    tsw_pps_ctrl_bin_4hz_digital_2hz,        /**< Binary 4Hz, digital 2Hz */
    tsw_pps_ctrl_bin_8hz_digital_4hz,        /**< Binary 8Hz, digital 4Hz */
    tsw_pps_ctrl_bin_16hz_digital_8hz,       /**< Binary 16Hz, digital 8Hz */
    tsw_pps_ctrl_bin_32hz_digital_16hz,      /**< Binary 32Hz, digital 16Hz */
    tsw_pps_ctrl_bin_64hz_digital_32hz,       /**< Binary 64Hz, digital 32Hz */
    tsw_pps_ctrl_bin_128hz_digital_64hz,     /**< Binary 128Hz, digital 64Hz */
    tsw_pps_ctrl_bin_256hz_digital_128hz,    /**< Binary 256Hz, digital 128Hz */
    tsw_pps_ctrl_bin_512hz_digital_256hz,    /**< Binary 512Hz, digital 256Hz */
    tsw_pps_ctrl_bin_1024hz_digital_512hz,   /**< Binary 1024Hz, digital 512Hz */
    tsw_pps_ctrl_bin_2048hz_digital_1024hz,  /**< Binary 2048Hz, digital 1024Hz */
    tsw_pps_ctrl_bin_4096hz_digital_2048hz,  /**< Binary 4096Hz, digital 2048Hz */
    tsw_pps_ctrl_bin_8192hz_digital_4096hz,  /**< Binary 8192Hz, digital 4096Hz */
    tsw_pps_ctrl_bin_16384hz_digital_8192hz, /**< Binary 16384Hz, digital 8192Hz */
    tsw_pps_ctrl_bin_32768hz_digital_16384hz /**< Binary 32768Hz, digital 16384Hz */
} tsw_pps_ctrl_t;

/**
 * @brief TSW SHAP TAS access control list operation enumeration
 */
typedef enum {
  tsw_shap_tas_aclist_op_set_gate_states = 0,        /**< Set gate states */
  tsw_shap_tas_aclist_op_set_and_hold_mac,           /**< Set and hold MAC */
  tsw_shap_tas_aclist_op_set_and_release_mac         /**< Set and release MAC */
} tsw_shap_tas_alist_op_t;

/**
 * @brief TSW SHAP TAS access control list state open queue enumeration
 */
typedef enum {
    tsw_shap_tas_aclist_state_open_queueu_0 = 1 << 0,    /**< Open queue 0 */
    tsw_shap_tas_aclist_state_open_queueu_1 = 1 << 1,    /**< Open queue 1 */
    tsw_shap_tas_aclist_state_open_queueu_2 = 1 << 2,    /**< Open queue 2 */
    tsw_shap_tas_aclist_state_open_queueu_3 = 1 << 3,    /**< Open queue 3 */
    tsw_shap_tas_aclist_state_open_queueu_4 = 1 << 4,    /**< Open queue 4 */
    tsw_shap_tas_aclist_state_open_queueu_5 = 1 << 5,    /**< Open queue 5 */
    tsw_shap_tas_aclist_state_open_queueu_6 = 1 << 6,    /**< Open queue 6 */
    tsw_shap_tas_aclist_state_open_queueu_7 = 1 << 7,    /**< Open queue 7 */
    tsw_shap_tas_aclist_state_open_queueu_all = 0xff,    /**< Open all queues */
} tsw_shap_tas_aclist_state_open_queue_t;

/**
 * @brief TSW traffic queue enumeration
 */
typedef enum {
  tsw_traffic_queue_0 = 0, /**< Traffic queue 0 */
  tsw_traffic_queue_1,     /**< Traffic queue 1 */
  tsw_traffic_queue_2,     /**< Traffic queue 2 */
  tsw_traffic_queue_3,     /**< Traffic queue 3 */
  tsw_traffic_queue_4,     /**< Traffic queue 4 */
  tsw_traffic_queue_5,     /**< Traffic queue 5 */
  tsw_traffic_queue_6,     /**< Traffic queue 6 */
  tsw_traffic_queue_7      /**< Traffic queue 7 */
} tsw_traffic_queue_t;

/**
 * @brief TSW FPE MMS statistics counter enumeration
 */
typedef enum {
    tsw_fpe_mms_frame_reassembly_error_counter = 0,        /**< Frame reassembly error counter */
    tsw_fpe_mms_frame_rejected_due_to_wrong_smd,          /**< Frame rejected due to wrong SMD */
    tsw_fpe_mms_frame_assembly_ok_counter,                 /**< Frame assembly OK counter */
    tsw_fpe_mms_fragment_rx_counter,                       /**< Fragment RX counter */
    tsw_fpe_mms_fragment_tx_counter,                      /**< Fragment TX counter */
    tsw_fpe_mms_hold_request_counter,                     /**< Hold request counter */
} tsw_fpe_mms_statistics_counter_t;

/**
 * @brief TSW FPE MMS fragment size enumeration
 */
typedef enum {
    tsw_fpe_mms_fragment_size_60_octets = 0,   /**< Fragment size 60 octets */
    tsw_fpe_mms_fragment_size_124_octets,      /**< Fragment size 124 octets */
    tsw_fpe_mms_fragment_size_188_octets,      /**< Fragment size 188 octets */
    tsw_fpe_mms_fragment_size_252_octets       /**< Fragment size 252 octets */
} tsw_fpe_mms_fragment_size_t;

/**
 * @brief TSW STMID lookup mode enumeration
 */
typedef enum {
    tsw_stmid_lookup_mode_priority = 1, /**< Priority lookup mode */
    tsw_stmid_lookup_mode_tagged = 2,   /**< Tagged lookup mode */
    tsw_stmid_lookup_mode_all            /**< All lookup mode */
} tsw_stmid_lookup_mode_t;

/**
 * @brief TSW STMID active destination MAC control enumeration
 */
typedef enum {
    tsw_stmid_actctl_disabled = 0,                                      /**< Disabled */
    tsw_stmid_actctl_use_amac_with_removed_vlan_tag = 1,                /**< Use AMAC with removed VLAN tag */
    tsw_stmid_actctl_use_amac_with_replaced_or_inserted_vlan_tag = 3   /**< Use AMAC with replaced or inserted VLAN tag */
} tsw_stmid_active_dest_mac_control_t;

/**
 * @brief TSW STMID control lookup mode enumeration
 */
typedef enum {
    tsw_stmid_control_lookup_by_dest_mac = 0, /**< Lookup by destination MAC */
    tsw_stmid_control_lookup_by_src_mac       /**< Lookup by source MAC */
} tsw_stmid_control_lookup_mode_t;

/**
 * @brief TSW CB FRER algorithm enumeration
 */
typedef enum {
    tsw_cb_frer_algo_vector_recovery = 0, /**< Vector recovery algorithm */
    tsw_cb_frer_algo_match_recovery       /**< Match recovery algorithm */
} tsw_cb_frer_algo_t;

/**
 * @brief TSW CB FRER X function recovery enumeration
 */
typedef enum {
    tsw_cb_frer_xfunc_recovery_sequence = 0,    /**< Sequence recovery */
    tsw_cb_frer_xfunc_recovery_individual       /**< Individual recovery */
} tsw_cb_frer_xfunc_recovery_t;

/**
 * @brief TSW CB FRER frame count egress enumeration
 */
typedef enum {
    presented_frames = 0,              /**< Presented frames */
    discarded_frames,                  /**< Discarded frames */
    tagless_frames,                    /**< Tagless frames */
    rougue_frames,                     /**< Rogue frames */
    out_of_oder_frames,                 /**< Out of order frames */
    lost_frames,                       /**< Lost frames */
    recover_func_resets,               /**< Recovery function resets */
    latent_err_dectection_resets       /**< Latent error detection resets */
} tsw_cb_frer_frame_ount_egress_t;

/**
 * @brief TSW PSFP gate mode enumeration
 */
typedef enum {
    tsw_psfp_gate_mode_static = 0,  /**< Static gate mode */
    tsw_psfp_gate_mode_dynamic,     /**< Dynamic gate mode */
    tsw_psfp_gate_mode_unknown      /**< Unknown gate mode */
} tsw_psfp_gate_mode_t;

/**
 * @brief TSW PSFP gate state enumeration
 */
typedef enum {
    tsw_psfp_gate_closed = 0, /**< Gate closed */
    tsw_psfp_gate_open        /**< Gate open */
} tsw_psfp_gate_t;

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
 * @param[in] buffer Frame buffer pointer
 * @param[in] length Frame length
 * @param[in] id     Frame index
 * @return           Result of the transmission
 */
hpm_stat_t tsw_send_frame(TSW_Type *ptr, uint8_t *buffer, uint16_t length, uint8_t id);

/**
 * @brief   Send a frame to CPU port and check response
 *
 * @param[in] ptr    TSW peripheral base address
 * @param[in] buffer Frame buffer pointer
 * @param[in] length Frame length
 * @param[in] id     Frame index
 * @return           Result of the transmission and checking response
 */
hpm_stat_t tsw_send_frame_check_response(TSW_Type *ptr, uint8_t *buffer, uint16_t length, uint8_t id);

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
 * @brief CAM VLAN Port Setting
 *
 * @param[in] ptr      TSW peripheral base address
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
 * @param[in] mac_type MAC type @ref tsw_mac_type_t
 * @return             Result of enabling MAC controller
 */
hpm_stat_t tsw_ep_enable_mac_ctrl(TSW_Type *ptr, uint8_t port, tsw_mac_type_t mac_type);

/**
 * @brief Disable MAC Controller
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] mac_type MAC type @ref tsw_mac_type_t
 * @return             Result of disabling MAC controller
 */
hpm_stat_t tsw_ep_disable_mac_ctrl(TSW_Type *ptr, uint8_t port, tsw_mac_type_t mac_type);

/**
 * @brief Enable All MAC Controllers
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] mac_type MAC type @ref tsw_mac_type_t
 * @return             Result of enabling all MAC controllers
 */
hpm_stat_t tsw_ep_enable_all_mac_ctrl(TSW_Type *ptr, tsw_mac_type_t mac_type);

/**
 * @brief Disable All MAC Controllers
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] mac_type MAC type @ref tsw_mac_type_t
 * @return             Result of disabling all MAC controllers
 */
hpm_stat_t tsw_ep_disable_all_mac_ctrl(TSW_Type *ptr, tsw_mac_type_t mac_type);

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
 * @note This API will be deprecated from V2.0.0
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] gmii     GMII Mode: Set 1 for 1000Mbps or 0 for 10Mbps/100Mbps
 * @return             Result of setting MAC mode
 */
hpm_stat_t tsw_ep_set_mac_mode(TSW_Type *ptr, uint8_t port, uint8_t gmii);

/**
 * @brief   Set MAC Mode For Specified MAC
 *
 * @param[in] ptr   TSW peripheral base address
 * @param[in] port  TSW port number
 * @param[in] gmii  GMII Mode:  Set 1 for 1000Mbps or 0 for 10Mbps/100Mbps
 * @param[in] mac_type MAC type @ref tsw_mac_type_t
 * @return hpm_stat_t
 */
hpm_stat_t tsw_ep_set_xmac_mode(TSW_Type *ptr, uint8_t port, uint8_t gmii, tsw_mac_type_t mac_type);

/**
 * @brief Set Port GPR
 *
 * @param[in] ptr      TSW peripheral base address
 * @param[in] port     TSW port number
 * @param[in] speed    TSW port speed
 * @param[in] itf      TSW port interface type
 * @param[in] tx_dly   Tx clock delay
 * @param[in] rx_dly   Rx clock delay
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
 * @brief Get Default Frame Action Config
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[out] config   Pointer to frame action config @ref tsw_frame_action_config_t
 */
 void tsw_get_default_frame_action_config(TSW_Type *ptr, tsw_frame_action_config_t *config);

/**
 * @brief Set Frame Action
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] config    Pointer to frame action config @ref tsw_frame_action_config_t
 * @param[in] type      Frame action type @ref tsw_frame_action_type_t
 * @return              Result of setting frame action @ref hpm_stat_t
 */
hpm_stat_t tsw_set_frame_action(TSW_Type *ptr, tsw_frame_action_config_t *config, uint8_t type);

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
 * @brief Clear Internal Frame Action
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] dest_port Destination port number
 */
void tsw_clear_internal_frame_action(TSW_Type *ptr, uint8_t dest_port);

/**
 * @brief Clear Broadcast Frame Action
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] dest_port Destination port number
 */
void tsw_clear_broadcast_frame_action(TSW_Type *ptr, uint8_t dest_port);

/**
 * @brief Clear Unknown Frame Action
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] dest_port Destination port number
 */
void tsw_clear_unknown_frame_action(TSW_Type *ptr, uint8_t dest_port);

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
 * @param[in] tqueue    Traffic queue
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
 * @param[out] disabled Disable current events
 * @return              Result of disabling current events @ref hpm_stat_t
 */
hpm_stat_t tsw_tsync_disable_current_events(TSW_Type *ptr, uint8_t port, uint32_t *disabled);

/**
 * @brief TSYNC get RX status.
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[out] ov       Overflow status
 * @param[out] avnxt    Available next buffer
 * @param[out] rxsel    RX buffer select
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
 * @param[out] entry    Pointer to a structure of tsw_tas_controllist_entry_t @ref tsw_tas_controllist_entry_t
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
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @return hpm_stat_t   Result of enabling tas @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_enable_tas(TSW_Type *ptr, uint8_t port);

/**
 * @brief Disable TAS
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @return hpm_stat_t   Result of disabling tas @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_disable_tas(TSW_Type *ptr, uint8_t port);

/**
 * @brief Set TAS list length
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] port      TSW port
 * @param[in] listlen   list length
 * @return hpm_stat_t   Result of setting tas list length @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas_listlen(TSW_Type *ptr, uint8_t port, uint32_t listlen);

/**
 * @brief Set TAS base time
 *
 * @param[in] ptr          TSW peripheral base address
 * @param[in] port         TSW port
 * @param[in] basetime_sec second part of base time
 * @param[in] basetime_ns  nanosecond part of base time
 * @return hpm_stat_t      Result of setting tas base time @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas_basetime(TSW_Type *ptr, uint8_t port, uint32_t basetime_sec, uint32_t basetime_ns);

/**
 * @brief Trigger to change TAS config
 *
 * @param[in] ptr          TSW peripheral base address
 * @param[in] port         TSW port
 * @return hpm_stat_t      Result of triggering to change tas config @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_tas_change_config(TSW_Type *ptr, uint8_t port);

/**
 * @brief Get TAS cycle time
 *
 * @param[in] ptr          TSW peripheral base address
 * @param[in] port         TSW port
 * @param[out] crsr        TAS Status
 * @return hpm_stat_t      Result of getting tas status @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_get_tas_crsr(TSW_Type *ptr, uint8_t port, uint32_t *crsr);

/**
 * @brief  Set TAS config
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] port        TSW port
 * @param[in] config      Pointer to TAS config @ref tsw_tas_config_t
 * @return hpm_stat_t     Result of setting TAS config @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_tas(TSW_Type *ptr, uint8_t port, tsw_tas_config_t *config);

/**
 * @brief Set CBS config
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] port        TSW port
 * @param[in] index       Traffic queue index
 * @param[in] config      Pointer to CBS config @ref tsw_cbs_config_t
 * @return hpm_stat_t     Result of setting CBS config @ref hpm_stat_t
 */
hpm_stat_t tsw_shap_set_cbs(TSW_Type *ptr, uint8_t port, uint8_t index, tsw_cbs_config_t *config);

/**
 * @brief Get used count from the TX-Timestamp FIFO
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] port        TSW port
 * @param[out] count      Pointer to used count in TX-Timestamp FIFO
 * @return hpm_stat_t     Result of getting used count from TX-Timestamp FIFO @ref hpm_stat_t
 */
hpm_stat_t tsw_get_txtimestampfifo_used(TSW_Type *ptr, uint8_t port, uint32_t *count);

/**
 * @brief Get TX-Timestamp FIFO entry
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] port        TSW port
 * @param[out] entry      Pointer to TX-Timestamp FIFO entry @ref tsw_tsf_t
 * @return hpm_stat_t     Result of getting TX-Timestamp FIFO entry @ref hpm_stat_t
 */
hpm_stat_t tsw_get_txtimestampfifo_entry(TSW_Type *ptr, uint8_t port, tsw_tsf_t *entry);

/**
 * @brief   Get default MMS config
 *
 * @param[in] ptr              TSW peripheral base address
 * @param[in] port             TSW port
 * @param[out] config          Pointer to MMS config @ref tsw_fpe_config_t
 * @return hpm_stat_t          Result of getting default MMS config @ref hpm_stat_t
 */
hpm_stat_t tsw_fpe_get_default_mms_ctrl_config(TSW_Type *ptr, uint8_t port, tsw_fpe_config_t *config);

/**
 * @brief  Enable MMS
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] port        TSW port
 * @return hpm_stat_t     Result of enabling MMS @ref hpm_stat_t
 */
hpm_stat_t tsw_fpe_enable_mms(TSW_Type *ptr, uint8_t port);

/**
 * @brief  Disable MMS
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] port        TSW port
 * @return hpm_stat_t     Result of disabling MMS @ref hpm_stat_t
 */
hpm_stat_t tsw_fpe_disable_mms(TSW_Type *ptr, uint8_t port);

/**
 * @brief   Set MMS config
 *
 * @param[in] ptr              TSW peripheral base address
 * @param[in] port             TSW port
 * @param[in] config           Pointer to MMS config @ref tsw_fpe_config_t
 * @return hpm_stat_t          Result of setting MMS config @ref hpm_stat_t
 */
hpm_stat_t tsw_fpe_set_mms_ctrl(TSW_Type *ptr, uint8_t port, tsw_fpe_config_t *config);

/**
 * @brief   Reset MMS statistics counter
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] port        TSW port
 * @param[in] counter     MMS statistics counter
 * @return hpm_stat_t     Result of resetting MMS statistics counter @ref hpm_stat_t
 */
hpm_stat_t tsw_fpe_reset_mms_statistics_counter(TSW_Type *ptr, uint8_t port, tsw_fpe_mms_statistics_counter_t counter);

/**
 * @brief   Get MMS statistics counter
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] port        TSW port
 * @param[in] counter     MMS statistics counter
 * @param[out] value      Pointer to MMS statistics counter value
 * @return hpm_stat_t     Result of getting MMS statistics counter @ref hpm_stat_t
 */
hpm_stat_t tsw_fpe_get_mms_statistics_counter(TSW_Type *ptr, uint8_t port, tsw_fpe_mms_statistics_counter_t counter, uint32_t *value);

/**
 * @brief   Set STMID entry for ingress
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] entry     Pointer to STMID entry @ref tsw_cb_stmid_entry_t
 * @return hpm_stat_t   Result of setting STMID entry for ingress @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_stmid_ingress_set_entry(TSW_Type *ptr, tsw_cb_stmid_entry_t *entry);

/**
 * @brief   Get STMID entry for ingress
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[out] entry    Pointer to STMID entry @ref tsw_cb_stmid_entry_t
 * @return hpm_stat_t   Result of getting STMID entry for ingress @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_stmid_ingress_get_entry(TSW_Type *ptr, tsw_cb_stmid_entry_t *entry);

/**
 * @brief   Set STMID entry for egress
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] entry     Pointer to STMID entry @ref tsw_cb_stmid_entry_t
 * @return hpm_stat_t   Result of setting STMID entry for egress @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_stmid_egress_set_entry(TSW_Type *ptr, tsw_cb_stmid_entry_t *entry);

/**
 * @brief    Enable RTAG with CB frer for ingress
 *
 * @param[in] ptr       TSW peripheral base address
 * @return hpm_stat_t   Result of enabling RTAG with frer for ingress @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_frer_ingress_enable_rtag(TSW_Type *ptr);

/**
 * @brief    specify recovery functions for stream for egress
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] config    Pointer to recovery functions config @ref tsw_cb_frer_sid_func_config_t
 * @return hpm_stat_t   Result of setting up recovery functions for stream with CB frer for egress @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_frer_egress_set_sid_func(TSW_Type *ptr, tsw_cb_frer_sid_func_config_t *config);

/**
 * @brief    Set up recovery functions for stream with CB frer for egress
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] config    Pointer to recovery functions config @ref tsw_cb_frer_recovery_func_config_t
 * @return hpm_stat_t   Result of setting up recovery functions for stream with CB frer for egress @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_frer_egress_set_recovery_func(TSW_Type *ptr, tsw_cb_frer_recovery_func_config_t *config);

/**
 * @brief    Clear latten error flag
 *
 * @param[in] ptr       TSW peripheral base address
 * @return hpm_stat_t   Result of clearing latten error flag @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_frer_egress_clear_latten_error_flag(TSW_Type *ptr);

/**
 * @brief    Get count of latten error
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[out] count    Pointer to value of frer egress counters
 * @return hpm_stat_t   Result of getting count of latten error @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_frer_egress_get_count(TSW_Type *ptr, tsw_cb_frer_frame_count_egress_t *count);

/**
 * @brief    Set system cycle numbers for one millisecond
 *
 * @param[in] ptr         TSW peripheral base address
 * @param[in] msec_cycles Numbers of SYS_CLK cycles during 1 ms
 * @return hpm_stat_t     Result of getting count of latten error @ref hpm_stat_t
 */
hpm_stat_t tsw_cb_frer_set_msec_cycles(TSW_Type *ptr, uint32_t msec_cycles);

/**
 * @brief   Set PSFP filter
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] config    Pointer to PSFP filter config @ref tsw_psfp_filter_config_t
 * @return hpm_stat_t   Result of setting PSFP filter @ref hpm_stat_t
 */
hpm_stat_t tsw_psfp_set_filter(TSW_Type *ptr, tsw_psfp_filter_config_t *config);

/**
 * @brief   Set PSFP gate with static mode
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] config    Pointer to PSFP gate config @ref tsw_psfp_gate_static_mode_config_t
 * @return hpm_stat_t   Result of setting PSFP gate with static mode @ref hpm_stat_t
 */
hpm_stat_t tsw_psfp_set_gate_static_mode(TSW_Type *ptr, tsw_psfp_gate_static_mode_config_t *config);

/**
 * @brief   Set PSFP gate with dynamic mode
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] config    Pointer to PSFP gate config @ref tsw_psfp_gate_dynamic_mode_config_t
 * @return hpm_stat_t   Result of setting PSFP gate with dynamic mode @ref hpm_stat_t
 */
hpm_stat_t tsw_psfp_set_gate_dynamic_mode(TSW_Type *ptr, tsw_psfp_gate_dynamic_mode_config_t *config);

/**
 * @brief   Set PSFP flow meter
 *
 * @param[in] ptr       TSW peripheral base address
 * @param[in] config    Pointer to PSFP flow meter config @ref tsw_psfp_flow_meter_config_t
 * @return hpm_stat_t   Result of setting PSFP flow meter @ref hpm_stat_t
 */
hpm_stat_t tsw_psfp_set_flow_meter(TSW_Type *ptr, tsw_psfp_flow_meter_config_t *config);


#if defined __cplusplus
}
#endif /* __cplusplus */
/** @} */
#endif /* HPM_TSW_DRV_H */
