/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_SDIO_H
#define HPM_SDMMC_SDIO_H

/**
 *
 * @brief HPM SDIO driver APIs
 * @defgroup hpm_sdmmc HPM SDMMC stack
 * @ingroup hpm_sdmmc_interfaces
 * @{
 *
 */

#include <stdint.h>
#include "hpm_sdmmc_common.h"


#define SDIO_CMD53_OP_MODE_FIXED_ADDR   0                           /*!< Fixed Address in CMD53 */
#define SDIO_CMD53_OP_MODE_INCR_ADDR    1                           /*!< Incremental Address in CMD53 */

#define SDIO_CMD_TIMEOUT_MS (1000UL)                                /*!< SDIO Command Timeout in milliseconds */

#define SDIO_FBR_REG_BASE(fbr) ((fbr) * 0x100)                      /*!< FBR base address for specified function */

#define SDIO_MAX_FUNC_NUM        7                                  /*!< Maximum supported functions */

/**
 * @brief SDIO Speed definitions
 */
#define SDIO_SPEED_DEFAULT_SPEED 0UL
#define SDIO_SPEED_HIGH_SPEED    1UL
#define SDIO_SPEED_SDR12         SDIO_SPEED_DEFAULT_SPEED
#define SDIO_SPEED_SDR25         SDIO_SPEED_HIGH_SPEED
#define SDIO_SPEED_SDR50         2UL
#define SDIO_SPEED_SDR104        3UL
#define SDIO_SPEED_DDR50         4UL

/**
 * @brief  SDIO Bus width definitions
 */
#define SDIO_CCCR_BUS_WIDTH_1BIT 0UL
#define SDIO_CCCR_BUS_WIDTH_4BIT 2UL

/**
 * @brief SDIO BUS speed definitions in CCCR register
 */
#define SDIO_CCCR_BUS_SPEED_MASK     (0x7UL << 1)
#define SDIO_CCCR_BUS_SPEED_SUPPORT_HIGHSPEED (1)
#define SDIO_CCCR_BUS_SPEED_DEFAULT (SDIO_SPEED_DEFAULT_SPEED << 1)
#define SDIO_CCCR_BUS_SPEED_HIGH    (SDIO_SPEED_HIGH_SPEED << 1)
#define SDIO_CCCR_BUS_SPEED_SDR12   (SDIO_CCCR_BUS_SPEED_DEFAULT)
#define SDIO_CCCR_BUS_SPEED_SDR25   (SDIO_CCCR_BUS_SPEED_HIGH)
#define SDIO_CCCR_BUS_SPEED_SDR50   (SDIO_SPEED_SDR50 << 1)
#define SDIO_CCCR_BUS_SPEED_SDR104  (SDIO_SPEED_SDR104 << 1)
#define SDIO_CCCR_BUS_SPEED_DDR50   (SDIO_SPEED_DDR50 << 1)

/**
 * @brief CCCR register address
 */
#define SDIO_REG_CCCR_SDIO_REV      0
#define SDIO_REG_CCCR_SD_REV        1
#define SDIO_REG_CCCR_IO_ENABLE     2
#define SDIO_REG_CCCR_IO_READY      3
#define SDIO_REG_CCCR_INT_ENABLE    4
#define SDIO_REG_CCCR_INT_PENDING   5
#define SDIO_REG_CCCR_IO_ABORT      6
#define SDIO_REG_CCCR_BUS_IF_CTRL   7
#define SDIO_REG_CCCR_CARD_CAP      8
#define SDIO_REG_CCCR_COM_CIS_PTR   9
#define SDIO_REG_CCCR_BUS_SUSPEND   0xC
#define SDIO_REG_CCCR_FUNC_SEL      0xD
#define SDIO_REG_CCCR_EXEC_FLAGS    0xE
#define SDIO_REG_CCCR_READY_FLAGS   0xF
#define SDIO_REG_CCCR_FN0_BLK_SIZE  0x10
#define SDIO_REG_CCCR_POWER_CTRL    0x12
#define SDIO_REG_CCCR_BUS_SPEED_SEL 0x13
#define SDIO_REG_CCCR_UHS_SUPPORT   0x14
#define SDIO_REG_CCCR_DRV_STRENGTH  0x15
#define SDIO_REG_CCCR_INT_EXT       0x16

/**
 * @brief CCCR register range definitions
 */
#define SDIO_REG_CCCR_START         SDIO_REG_CCCR_SDIO_REV
#define SDIO_REG_CCCR_END           SDIO_REG_CCCR_INT_EXT

#define SDIO_REG_FBR_CIS_PTR_OFFSET         0x9
#define SDIO_REG_FBR_CSR_PTR_OFFSET         0xC
#define SDIO_REG_FBR_MAX_BLK_SIZE_OFFSET    0x10

/**
 * @brief CISTPL type definitions
 */
#define CISTPL_NULL                 (0)
#define CISTPL_CHECKSUM             (0x10)
#define CISTPL_VERS_1               (0x15)
#define CISTPL_ALTSTR               (0x16)
#define CISTPL_MANFID               (0x20)
#define CISTPL_FUNCID               (0x21)
#define CISTPL_FUNCE                (0x22)
#define CISTPL_SDIO_STD             (0x91)
#define CISTPL_SDIO_EXT             (0x92)
#define CISTPL_END                  (0xFF)

/**
 * @brief CMD5 argument definition
 */
typedef union {
    uint32_t value;

    struct {
        uint32_t : 8;
        uint32_t volt_2p0_2p1: 1;
        uint32_t volt_2p1_2p2: 1;
        uint32_t volt_2p2_2p3: 1;
        uint32_t volt_2p3_2p4: 1;
        uint32_t volt_2p4_2p5: 1;
        uint32_t volt_2p5_2p6: 1;
        uint32_t volt_2p6_2p7: 1;
        uint32_t volt_2p7_2p8: 1;
        uint32_t volt_2p8_2p9: 1;
        uint32_t volt_2p9_3p0: 1;
        uint32_t volt_3p0_3p1: 1;
        uint32_t volt_3p1_3p2: 1;
        uint32_t volt_3p2_3p3: 1;
        uint32_t volt_3p3_3p4: 1;
        uint32_t volt_3p4_3p5: 1;
        uint32_t volt_3p5_3p6: 1;
        uint32_t s18r: 1;
        uint32_t : 7;
    };
} sdio_cmd5_arg_t;

/**
 * @brief CMD5 response definition
 */
typedef union {
    uint32_t value;

    struct {
        uint32_t : 8;
        uint32_t volt_2p0_2p1: 1;
        uint32_t volt_2p1_2p2: 1;
        uint32_t volt_2p2_2p3: 1;
        uint32_t volt_2p3_2p4: 1;
        uint32_t volt_2p4_2p5: 1;
        uint32_t volt_2p5_2p6: 1;
        uint32_t volt_2p6_2p7: 1;
        uint32_t volt_2p7_2p8: 1;
        uint32_t volt_2p8_2p9: 1;
        uint32_t volt_2p9_3p0: 1;
        uint32_t volt_3p0_3p1: 1;
        uint32_t volt_3p1_3p2: 1;
        uint32_t volt_3p2_3p3: 1;
        uint32_t volt_3p3_3p4: 1;
        uint32_t volt_3p4_3p5: 1;
        uint32_t volt_3p5_3p6: 1;
        uint32_t s18r: 1;
        uint32_t : 2;
        uint32_t memory_present: 1;
        uint32_t num_func: 3;
        uint32_t ready: 1;
    };
} sdio_cmd5_resp_t;

/**
 * @brief  Response R5 definition
 */
typedef union {
    uint32_t value;

    struct {
        uint8_t rw_data;
        uint8_t err_out_of_range: 1;
        uint8_t err_num_func: 1;
        uint8_t : 1;
        uint8_t err_general: 1;
        uint8_t io_current_state: 2;
        uint8_t err_illegal_cmd: 1;
        uint8_t err_com_crc: 1;
        uint8_t stuff[2];
    };
} sdio_resp_r5_t;

/**
 * @brief  CMD5 argument definition
 */
typedef union {
    uint32_t value;

    struct {
        uint32_t wr_data: 8;
        uint32_t : 1;
        uint32_t reg_addr: 17;
        uint32_t : 1;
        uint32_t read_afer_write: 1;
        uint32_t func_num: 3;
        uint32_t write: 1;
    };
} sdio_cmd52_arg_t;

/**
 * @brief CMD53 argument definition
 */
typedef union {
    uint32_t value;

    struct {
        uint32_t count: 9;
        uint32_t reg_addr: 17;
        uint32_t op_code: 1;
        uint32_t block_mode: 1;
        uint32_t func_num: 3;
        uint32_t rw_flag: 1;
    };
} sdio_cmd53_arg_t;

/**
 * @brief CCCR register map
 */
typedef struct {
    union {
        uint8_t sdio_version; /* offset: 0x00 */
        struct {
            uint8_t cccr_rev: 4;
            uint8_t sdio_rev: 4;
        };
    };

    uint8_t sd_version; /* offset: 0x01 */
    uint8_t io_enable; /* offset: 0x02 */
    uint8_t io_ready; /* offset: 0x03 */
    uint8_t int_enable; /* offset: 0x04 */
    uint8_t int_pending; /* offset: 0x05 */
    union {
        uint8_t io_abort; /* offset: 0x06 */
        struct {
            uint8_t abort_select: 3;
            uint8_t reset: 1;
        };
    };

    union {
        uint8_t bus_if_ctrl; /* offset: 0x07 */
        struct {
            uint8_t bus_width: 2;
            uint8_t support_8bit: 1;
            uint8_t : 4;
            uint8_t cd_disable: 1;
        };
    };

    union {
        uint8_t card_capability; /* offset: 0x08 */
        struct {
            uint8_t support_cmd52: 1;
            uint8_t support_multi_block_transfer: 1;
            uint8_t support_read_wait: 1;
            uint8_t support_bus_control: 1;
            uint8_t support_block_gap_interrupt: 1;
            uint8_t enable_block_gap_interrupt: 1;
            uint8_t low_speed_card: 1;
            uint8_t low_speed_card_4bit_mode: 1;
        };
    };

    uint8_t common_cis_pointer[3]; /* offset: 0x08 */
    union {
        uint8_t bus_suspend; /* offset: 0x0c */
        struct {
            uint8_t bus_status: 1;
            uint8_t bus_release_req: 1;
        };
    };

    uint8_t function_select; /* offset: 0x0d */
    uint8_t exec_flags; /* offset: 0x0e */
    uint8_t ready_flags; /* offset: 0x0f */
    uint8_t fn0_block_size[2]; /* offset: 0x10 */
    union {
        uint8_t power_control; /* offset: 0x12 */
        struct {
            uint8_t support_master_pwr_ctrl: 1;
            uint8_t enable_master_pwr_ctrl: 1;
        };
    };

    union {
        uint8_t bus_speed_select; /* offset: 0x13 */
        struct {
            uint8_t support_highspeed: 1;
            uint8_t bus_speed_sel: 3;
            uint8_t : 4;
        };
    };

    union {
        uint8_t uhs1_support; /* offset: 0x14 */
        struct {
            uint8_t support_sdr50: 1;
            uint8_t support_sdr104: 1;
            uint8_t support_ddr50: 1;
        };
    };

    union {
        uint8_t driver_strength; /* offset: 0x15 */
    };

    union {
        uint8_t interrupt_extension; /* offset: 0x16 */
    };
} sdio_cccr_t;

/**
 * @brief Standard SDIO Function Interface Code Definitions
 */
#define STD_SDIO_FUNC_IF_CODE_NON_STD       0
#define STD_SDIO_FUNC_IF_CODE_UART          1
#define STD_SDIO_FUNC_IF_CODE_BT_TYPEA      2
#define STD_SDIO_FUNC_IF_CODE_BT_TYPEB      3
#define STD_SDIO_FUNC_IF_CODE_GPS           4
#define STD_SDIO_FUNC_IF_CODE_CAMERA        5
#define STD_SDIO_FUNC_IF_CODE_PHS           6
#define STD_SDIO_FUNC_IF_CODE_WLAN          7
#define STD_SDIO_FUNC_IF_CODE_ATA           8
#define STD_SDIO_FUNC_IF_CODE_BT_TYPEA_AMP  9
#define STD_SDIO_FUNC_IF_CODE_ISDIO         0xE
#define STD_SDIO_FUNC_IF_CODE_EXT_STD       0xF

/**
 * @brief FBR register map
 */
typedef struct {
    uint8_t std_sdio_func_if_code: 4;
    uint8_t : 2;
    uint8_t support_csa: 1;
    uint8_t csa_enable: 1;

    uint8_t ext_std_sdio_func_if_code;

    uint8_t support_pwr_sel: 1;
    uint8_t enable_pwr_sel: 1;
    uint8_t : 2;
    uint8_t pwr_sel: 4;

    uint8_t std_isdio_func_if_code;
    uint16_t sda_mid_manf;
    uint16_t mid_card;
    uint8_t isdio_type_support_code;

    uint8_t cis_ptr[3];
    uint8_t csa_ptr[3];
    uint8_t data_acc_win_to_csa;
    uint16_t io_blk_size;
} sdio_fbr_t;

/**
 * @brief TPL block structure
 */
typedef struct sdio_tpl_block {
    struct sdio_tpl_block *next;
    uint8_t tpl_code;
    uint8_t tpl_size;
    uint8_t *tpl_data;
} *sdio_tpl_blk_t;

/**
 * @brief SDIO Common In
 */
typedef struct {
    sdio_tpl_blk_t tpl_link;
} sdio_common_cis_t;

/**
 * @brief SDIO Function extension structure
 */
typedef struct {
    union {
        uint8_t func_info;

        struct {
            uint8_t support_wakeup: 1;
            uint8_t : 7;
        };
    };

    uint8_t std_io_rev;
    uint32_t psn;
    uint32_t csa_size;

    union {
        uint8_t csa_property;

        struct {
            uint8_t csa_wp: 1;
            uint8_t csa_nf: 1;
            uint8_t : 6;
        };
    };

    uint16_t max_blk_size;
    uint32_t ocr;
    uint8_t op_min_pwr;
    uint8_t op_avg_pwr;
    uint8_t op_max_pwr;
    uint8_t sb_min_pwr;
    uint8_t sb_avg_pwr;
    uint8_t sb_max_pwr;
    uint16_t min_bw;
    uint16_t opt_bw;
    uint16_t enable_timeout_val;
    uint16_t sp_avg_pwr_3v3;
    uint16_t sp_max_pwr_3v3;
    uint16_t hp_avg_pwr_3v3;
    uint16_t hp_max_pwr_3v3;
    uint16_t lp_avg_pwr_3v3;
    uint16_t lp_max_pwr_3v3;
}
ATTR_PACKED sdio_func_funce_t;

/**
 * @brief SDIO CIS information
 */
typedef struct {
    union {
        struct {
            uint16_t manufacturer;
            uint16_t product;
            uint16_t func0_blk_size;
            uint8_t max_tran_speed;
        };

        struct {
            uint8_t std_id;
            uint8_t std_type;
            bool support_wakeup;
            uint8_t std_io_rev;
            uint16_t max_blk_size;
            uint16_t timeout_val_in_10ms;
            bool csa_wp;
            bool csa_nf;
            uint32_t serial_num;
            uint32_t csa_size;
            uint32_t ocr;
        };
    };

    sdio_tpl_blk_t tpl_link;
} sdio_cis_t;

typedef struct _sdmmc_sdio sdio_card_t;

/**
 * @brief  SDIO Function information
 */
typedef struct {
    sdio_card_t *card;
    uint8_t func_idx;
    uint32_t max_block_size;
    uint32_t current_block_size;
    uint8_t std_sdio_func_if_code;
    sdio_cis_t cis;
    sdio_fbr_t fbr;
} sdio_func_t;


/**
 * @brief SDIO Card context
 */
struct _sdmmc_sdio {
    sdmmc_host_t *host;
    bool is_host_ready;
    sdmmc_operation_voltage_t operation_voltage;
    sdmmc_speed_mode_t current_timing;
    uint8_t num_of_functions;
    uint16_t relative_addr;
    uint32_t max_tran_speed_in_kbps;
    sdio_cccr_t cccr;
    sdio_cis_t common_cis;
    sdio_func_t *sdio_func[SDIO_MAX_FUNC_NUM + 1];
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the CIS register address from raw register value
 * @param [in] reg_ptr  Pointer to the register value
 * @return Converted CIS CSR address
 */
static inline uint32_t sdio_cis_csr_addr(const uint8_t *reg_ptr)
{
    uint32_t reg_addr = 0;
    for (uint32_t i = 0; i < 3; i++) {
        reg_addr |= ((uint32_t) reg_ptr[i]) << (i * 8);
    }
    return reg_addr;
}

/**
 * @brief Initialize SDIO device
 * @param [in,out] card SDIO card context
 * @return SDIO device initialization status
 */
hpm_stat_t sdio_init(sdio_card_t *card);

/**
 * @brief De-initialize SDIO device
 * @param [in,out] card SDIO card context
 */
void sdio_deinit(sdio_card_t *card);

/**
 * @brief Initialize SDIO card
 * @param [in,out] card SDIO card context
 * @return SDIO card initialization status
 */
hpm_stat_t sdio_card_init(sdio_card_t *card);

/**
 * @brief De-Initialize SDIO card
 * @param [in,out] card SDIO card context
 * @return SDIO card initialization status
 */
hpm_stat_t sdio_card_deinit(sdio_card_t *card);

/**
 * @brief Initialize SDIO host
 * @param [in,out] card SDIO card context
 */
hpm_stat_t sdio_host_init(sdio_card_t *card);

/**
 * @brief De-initialize SDIO host
 * @param [in,out] card SDIO card context
 *
 * @return De-initialization status
 */
hpm_stat_t sdio_host_deinit(sdio_card_t *card);

/**
 * @brief Execute SDIO_IO_RW_DIRECT command (CMD52)
 * @param [in,out] card SDIO card context
 * @param [in] write  true: write, false: read
 * @param [in] func_idx  SDIO function index
 * @param [in] reg_addr Register address
 * @param [in,out] pdata pass in data if write is true or read_after_write is true, output data if write is false
 * @param [in] read_after_write  true: read data after write
 *
 * @return SDIO_IO_RW_DIRECT command status
 */
hpm_stat_t sdio_io_rw_direct(sdio_card_t *card,
                             bool write,
                             uint32_t func_idx,
                             uint32_t reg_addr,
                             uint8_t *pdata,
                             bool read_after_write);

/**
 * @brief Set the Block size for specified Function
 * @param [in,out] card SDIO card context
 * @param [in] func_idx  SDIO function index
 * @param [in] block_size Block size
 *
 * @return Command execution status
 */
hpm_stat_t sdio_set_block_size(sdio_card_t *card, uint32_t func_idx, uint32_t block_size);

/**
 * @brief Execute SDIO_IO_RW_EXTEND command (CMD53)
 * @param [in,out] card SDIO card context
 * @param [in] write  true: write, false: read
 * @param [in] func_idx SDIO function index
 * @param [in] reg_addr Register address
 * @param [in] op_code Operation code, SDIO_CMD53_OP_MODE_FIXED_ADDR or SDIO_CMD53_OP_MODE_INCR_ADDR
 * @param [in,out] pbuf Pointer to data buffer
 * @param [in] blocks Number of blocks to be transferred
 * @param [in] block_size Block size in bytes
 *
 * @return Command execution status
 */
hpm_stat_t sdio_io_rw_extend(sdio_card_t *card,
                             bool write,
                             uint32_t func_idx,
                             uint32_t reg_addr,
                             uint32_t op_code,
                             uint8_t *pbuf,
                             uint32_t blocks,
                             uint32_t block_size);

/**
 * @brief Write multiple bytes to the FIFO address for specified SDIO Function
 * @param [in,out] card SDIO card context
 * @param [in] func_idx SDIO function index
 * @param [in] reg_addr Register address
 * @param [in,out] pbuf Pointer to data buffer
 * @param [in] length bytes to be written
 *
 * @return Command execution status
 */
hpm_stat_t sdio_io_write_multi_bytes_to_fifo(sdio_card_t *card,
                                             uint32_t func_idx,
                                             uint32_t reg_addr,
                                             uint8_t *pbuf,
                                             uint32_t length);

/**
 * @brief Read multiple bytes from the FIFO address for specified SDIO Function
 * @param [in,out] card SDIO card context
 * @param [in] func_idx SDIO function index
 * @param [in] reg_addr Register address
 * @param [out] pbuf Pointer to data buffer
 * @param [in] length bytes to be read
 *
 * @return Command execution status
 */
hpm_stat_t sdio_io_read_multi_bytes_from_fifo(sdio_card_t *card,
                                              uint32_t func_idx,
                                              uint32_t reg_addr,
                                              uint8_t *pbuf,
                                              uint32_t length);

/**
 * @brief Write multiple bytes to the address for specified SDIO Function
 * @param [in,out] card SDIO card context
 * @param [in] func_idx SDIO function index
 * @param [in] reg_addr Register address
 * @param [in] pbuf Pointer to data buffer
 * @param [in] length   bytes to be written
 *
 * @return Command execution status
 */
hpm_stat_t sdio_io_write_incr_multi_bytes(sdio_card_t *card,
                                          uint32_t func_idx,
                                          uint32_t reg_addr,
                                          uint8_t *pbuf,
                                          uint32_t length);

/**
 * @brief Read multiple bytes from the address for specified SDIO Function
 * @param [in,out] card SDIO card context
 * @param [in] func_idx SDIO function index
 * @param [in] reg_addr Register address
 * @param [in,out] pbuf Pointer to data buffer
 * @param [in] length bytes to be read
 *
 * @return Command execution status
 */
hpm_stat_t sdio_io_read_incr_multi_bytes(sdio_card_t *card,
                                         uint32_t func_idx,
                                         uint32_t reg_addr,
                                         uint8_t *pbuf,
                                         uint32_t length);

/**
 * @brief Read 1 byte from the address for specified SDIO Function
 * @param [in,out] card SDIO card context
 * @param [in] func_idx SDIO function index
 * @param [in] reg_addr Register address
 * @param [out] pdata Pointer to data buffer
 *
 * @return Command execution status
 */
hpm_stat_t sdio_io_read_byte(sdio_card_t *card, uint32_t func_idx, uint32_t reg_addr, uint8_t *pdata);

/**
 * @brief Write 1 byte to the address for specified SDIO Function
 * @param [in,out] card SDIO card context
 * @param [in] func_idx SDIO function index
 * @param [in] reg_addr Register address
 * @param [in] byte_data data to be written
 *
 * @return Command execution status
 */
hpm_stat_t sdio_io_write_byte(sdio_card_t *card, uint32_t func_idx, uint32_t reg_addr, uint8_t byte_data);

/**
 * @brief Get the string for specified function interface code
 * @param [in] std_func_if_code Standard function interface code
 *
 * @return string for the specified function interface code
 */
const char *sdio_get_func_if_code_str(uint8_t std_func_if_code);

/**
 * @brief Read the specified Function Basic Register
 * @param [in,out] card SDIO card context
 * @param [in] func_idx SDIO function index
 * @param [out] fbr Pointer to FBR buffer
 *
 * @return Command execution status
 */
hpm_stat_t sdio_read_fbr(sdio_card_t *card, uint32_t func_idx, sdio_fbr_t *fbr);

/**
 * @brief Read Card Information Structure (CIS)
 * @param [in,out] card SDIO card context
 * @param [in] cis_ptr CIS pointer
 * @param [out] cis CIS buffer
 * @param [in] keep_tpl_raw_data true: keep the raw data of TPL, false: discard the raw data of TPL
 *
 * @return Command execution status
 */
hpm_stat_t sdio_read_cis(sdio_card_t *card, uint32_t cis_ptr, sdio_cis_t *cis, bool keep_tpl_raw_data);

/**
 * @brief Get the maximum transfer speed
 *
 * @param [in] card SDIO card context
 *
 * @return Command execution status
 */
hpm_stat_t sdio_get_max_transfer_speed(sdio_card_t *card);

/**
 * @brief Initialize SDIO functions
 * @param [in,out] card SDIO card context
 *
 * @return Command execution status
 */
hpm_stat_t sdio_init_funcs(sdio_card_t *card);

/**
 * @brief De-Initialize SDIO functions
 * @param [in,out] card SDIO card context
 *
 * @return Command execution status
 */
hpm_stat_t sdio_deinit_funcs(sdio_card_t *card);

/**
 * @brief Enables SDIO interrupts for the specified card.
 *
 * This function activates the SDIO interrupts by utilizing the host controller.
 * It is essential to ensure that the card and its associated host controller
 * are properly initialized before calling this function.
 *
 * @param [in,out] card A pointer to the sdio_card_t structure representing the SDIO card.
 *             Must not be NULL and should have a valid host controller pointer.
 *
 * @return hpm_stat_t: Status of the operation.
 *         - status_success: Interrupts were successfully enabled.
 *         - status_invalid_argument: If the card pointer or its host controller is NULL.
 */
hpm_stat_t sdio_enable_interrupt(sdio_card_t *card);


/**
 * @brief Disables SDIO interrupts for the specified card.
 *
 * This function disables the SDIO interrupts associated with the given SDIO card.
 * It ensures that the card and its host are valid before proceeding to disable the interrupts.
 *
 * @param [in,out] card A pointer to the sdio_card_t structure representing the SDIO card.
 * @return Returns status_success if the interrupts were successfully disabled, or
 *         status_invalid_argument if the card or its host is not properly initialized.
 */
hpm_stat_t sdio_disable_interrupt(sdio_card_t *card);

/**
 * @brief Registers an interrupt callback function for an SDIO card.
 *
 * This function allows the user to register a callback that will be invoked when an SDIO interrupt occurs.
 * The provided callback function should be designed to handle SDIO-specific events or data associated with the interrupt.
 *
 * @param [in,out] card A pointer to the SDIO card context structure. Must not be NULL.
 * @param [in] sdio_irq_callback A pointer to the user-defined interrupt callback function. This function will be called when an SDIO interrupt is detected.
 *                          The function signature should match `void (*)(void *)`, where the parameter is a generic pointer provided by the user.
 * @param [in] sdio_irq_param A pointer to any user-defined data that needs to be passed to the interrupt callback function. Can be NULL if no data needs to be passed.

 * @return hpm_stat_t: Status of the registration operation.
 *         - status_success: Callback successfully registered.
 *         - status_invalid_argument: If the card pointer is NULL or the card host is not initialized.
 *
 * @note Ensure the SDIO card and host controller are properly initialized before calling this function.
 */
hpm_stat_t sdio_register_irq_callback(sdio_card_t *card,
                                      void (*sdio_irq_callback)(void *param),
                                      void *sdio_irq_param);
/**
 * @}
 */


#ifdef __cplusplus
}
#endif


#endif /* HPM_SDMMC_SDIO_H */
