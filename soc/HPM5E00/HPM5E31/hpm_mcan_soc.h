/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_MCAN_SOC_H
#define HPM_MCAN_SOC_H

#include <stdint.h>
#include "hpm_mcan_regs.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"

/**
 * @brief The Attribute of Message Attribute
 */
typedef struct {
    uint32_t ram_base;  /*!< The base address of Message Buffer */
    uint32_t ram_size;  /*!< Total size of the Message buffer */
} mcan_msg_buf_attr_t;

#define MCAN_SOC_TSU_SRC_TWO_STAGES     (1U)

#define HPM_MCAN_EXT_TBSEL_NUM  (4U)
#define HPM_MCAN_TBSEL_BASE (0xF02FF000UL)
#define HPM_MCAN_TBSEL (*(volatile uint32_t *)HPM_MCAN_TBSEL_BASE)
#define HPM_MCAN_TBSEL_BITWIDTH (6U)
#define HPM_MCAN_TBSEL_MASK ((1UL << HPM_MCAN_TBSEL_BITWIDTH) - 1UL)
#define HPM_MCAN_TBSEL0_SHIFT (8U)

#define MCAN_MSG_BUF_BASE_VALID_START (0xF0200000UL)    /* This is the start address of AHB_RAM */
#define MCAN_MSG_BUG_SIZE_MAX         (32UL * 1024UL)   /* This is the total size of AHB_RAM */
#define MCAN_MSG_BUF_BASE_VALID_END   (MCAN_MSG_BUF_BASE_VALID_START + MCAN_MSG_BUG_SIZE_MAX)

#define MCAN_MSG_BUF_ALIGNMENT_SIZE   (4U)

/**
 * @brief MCAN MSG BUF base address (AHB_RAM)
 */
#define MCAN_MSG_BUF_SIZE_IN_WORDS (640U)   /*!< Default Message Buffer Size in this driver */
#define MCAN_IP_SLOT_SIZE (0x4000U)

/**
 * @brief TSU External Timebase Sources
 */
#define MCAN_TSU_EXT_TIMEBASE_SRC_MIN     (0U)
#define MCAN_TSU_EXT_TIMEBASE_SRC_TBSEL_0 (MCAN_TSU_EXT_TIMEBASE_SRC_MIN)
#define MCAN_TSU_EXT_TIMEBASE_SRC_TBSEL_1 (1U)
#define MCAN_TSU_EXT_TIMEBASE_SRC_TBSEL_2 (2U)
#define MCAN_TSU_EXT_TIMEBASE_SRC_TBSEL_3 (3U)
#define MCAN_TSU_EXT_TIMEBASE_SRC_MAX     (MCAN_TSU_EXT_TIMEBASE_SRC_TBSEL_3)

/**
 * @brief MCAN TSU timebase option for each External Timebase
 */
#define MCAN_TSU_TBSEL_PTPC0 (0x20)
#define MCAN_TSU_TBSEL_MCAN0 (0x00)
#define MCAN_TSU_TBSEL_MCAN1 (0x01)
#define MCAN_TSU_TBSEL_MCAN2 (0x02)
#define MCAN_TSU_TBSEL_MCAN3 (0x03)


extern mcan_msg_buf_attr_t mcan_soc_msg_buf_attr[MCAN_SOC_MAX_COUNT];

#ifdef __cpluspus
extern "C" {
#endif

/**
 * @brief Get the MCAN instance index from base address
 * @param [in] base MCAN base
 * @return MCAN instance index
 */
static inline uint32_t mcan_get_instance_from_base(MCAN_Type *base)
{
    uint32_t index = ((uint32_t) base - HPM_MCAN0_BASE) / MCAN_IP_SLOT_SIZE;
    return index;
}

/**
 * @brief Set External Timebase Source for MCAN TSU
 * @param [in] ptr MCAN base
 * @param [in] src External Timebase source
 */
static inline void mcan_set_tsu_ext_timebase_src(MCAN_Type *ptr, uint8_t src)
{
    if (src < HPM_MCAN_EXT_TBSEL_NUM) {
        ptr->GLB_CTL = (ptr->GLB_CTL & ~MCAN_GLB_CTL_TSU_TBIN_SEL_MASK) | MCAN_GLB_CTL_TSU_TBIN_SEL_SET(src);
    }
}

/**
 * @brief Set the Source for specified external timebase
 *
 * @param [in] ptr MCAN base
 * @param [in] ext_tbsel External TBSEL index
 * @param [in] tbsel_option Timebase source selection
 */
static inline void mcan_set_tsu_tbsel_option(MCAN_Type *ptr, uint8_t ext_tbsel, uint8_t tbsel_option)
{
    (void) ptr;
    if (ext_tbsel < HPM_MCAN_EXT_TBSEL_NUM) {
        uint32_t tbsel_shift = (ext_tbsel * HPM_MCAN_TBSEL_BITWIDTH) + HPM_MCAN_TBSEL0_SHIFT;
        uint32_t tbsel_mask = HPM_MCAN_TBSEL_MASK << tbsel_shift;
        HPM_MCAN_TBSEL = (HPM_MCAN_TBSEL & ~tbsel_mask) | (((uint32_t) tbsel_option << tbsel_shift) & tbsel_mask);
    }
}

/**
 * @brief Enable Standby Pin for MCAN
 * @param [in] ptr MCAN base
 */
static inline void mcan_enable_standby_pin(MCAN_Type *ptr)
{
    ptr->GLB_CTL |= MCAN_GLB_CTL_M_CAN_STBY_MASK;
}

/**
 * @brief Disable Standby pin for MCAN
 * @param [in] ptr MCAN base
 */
static inline void mcan_disable_standby_pin(MCAN_Type *ptr)
{
    ptr->GLB_CTL &= ~MCAN_GLB_CTL_M_CAN_STBY_MASK;
}

/**
 * @brief Set the attribute of the Message Buffer for specified MCAN
 * @param [in] ptr MCAN base
 * @param [in] attr The attribute of message buffer
 * @retval status_success No error occurred
 * @retval status_invalid_argument Invalid argument was detected
 */
static inline hpm_stat_t mcan_set_msg_buf_attr(MCAN_Type *ptr, const mcan_msg_buf_attr_t *attr)
{
    if ((attr == NULL) || \
        (attr->ram_base < MCAN_MSG_BUF_BASE_VALID_START) || \
        (attr->ram_base >= MCAN_MSG_BUF_BASE_VALID_END) || \
        (attr->ram_size > MCAN_MSG_BUG_SIZE_MAX) || \
        (attr->ram_base + attr->ram_size > MCAN_MSG_BUF_BASE_VALID_END) || \
        (attr->ram_base % MCAN_MSG_BUF_ALIGNMENT_SIZE != 0U) || \
        (attr->ram_size % MCAN_MSG_BUF_ALIGNMENT_SIZE != 0)) {
        return status_invalid_argument;
    }
    uint32_t instance = mcan_get_instance_from_base(ptr);
    if (instance >= MCAN_SOC_MAX_COUNT) {
        return status_invalid_argument;
    }

    mcan_soc_msg_buf_attr[instance].ram_base = attr->ram_base;
    mcan_soc_msg_buf_attr[instance].ram_size = attr->ram_size;

    return status_success;
}
/**
 * @brief Get RAM base for MCAN
 * @param [in] ptr MCAN base
 * @return RAM base for MCAN
 */
static inline uint32_t mcan_get_ram_base(MCAN_Type *ptr)
{
    (void) ptr;
    /* NOTE: As the `mcan_get_ram_offset` returns the actual offset in AHB RAM, this function always returns
     *       the base address of the AHB_RAM
     */
    return MCAN_MSG_BUF_BASE_VALID_START;
}

/**
 * @brief Get the MCAN RAM offset in the dedicated/shared RAM for
 * @param [in] ptr MCAN base
 * @return RAM offset for MCAN
 */
static inline uint32_t mcan_get_ram_offset(MCAN_Type *ptr)
{
    uint32_t instance = mcan_get_instance_from_base(ptr);
    return (mcan_soc_msg_buf_attr[instance].ram_base - MCAN_MSG_BUF_BASE_VALID_START);
}

/**
 * @brief Get MCAN RAM size
 * @param [in] ptr MCAN base
 * @return RAM size in bytes
 */
static inline uint32_t mcan_get_ram_size(MCAN_Type *ptr)
{
    uint32_t instance = mcan_get_instance_from_base(ptr);
    return mcan_soc_msg_buf_attr[instance].ram_size;
}

#ifdef __cpluspus
}
#endif

#endif /* HPM_MCAN_SOC_H */