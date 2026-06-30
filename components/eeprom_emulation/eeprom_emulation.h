/*
 * Copyright (c) 2023,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _EEPROM_EMULATION_H
#define _EEPROM_EMULATION_H

#include <stdint.h>
#include <stdio.h>
#include "hpm_common.h"
#include "user_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Debug level definitions
 */
#define E2P_DEBUG_LEVEL_TRACE (0)
#define E2P_DEBUG_LEVEL_INFO  (1)
#define E2P_DEBUG_LEVEL_WARN  (2)
#define E2P_DEBUG_LEVEL_ERROR (3)
#define E2P_DEBUG_LEVEL_NONE  (4)

#ifndef E2P_DEBUG_LEVEL
#define E2P_DEBUG_LEVEL E2P_DEBUG_LEVEL_INFO
#endif

#ifndef E2P_MAX_VAR_CNT
#define E2P_MAX_VAR_CNT (100)
#endif

#ifndef E2P_FLUSH_BUF_SIZE
#define E2P_FLUSH_BUF_SIZE (512)
#endif

#ifndef E2P_ATTR
#ifndef E2P_PLACE_FLASH
#define E2P_ATTR ATTR_RAMFUNC
#else
#define E2P_ATTR
#endif
#endif

#if E2P_DEBUG_LEVEL <= E2P_DEBUG_LEVEL_TRACE
#define e2p_trace(fmt, ...) \
    printf("[E2P TRACE] %s:%d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#else
#define e2p_trace(...)
#endif

#if E2P_DEBUG_LEVEL <= E2P_DEBUG_LEVEL_INFO
#define e2p_info(fmt, ...) printf("[E2P INFO] " fmt "\n", ##__VA_ARGS__)
#else
#define e2p_info(...)
#endif

#if E2P_DEBUG_LEVEL <= E2P_DEBUG_LEVEL_WARN
#define e2p_warn(fmt, ...) printf("[E2P WARN] " fmt "\n", ##__VA_ARGS__)
#else
#define e2p_warn(...)
#endif

#if E2P_DEBUG_LEVEL <= E2P_DEBUG_LEVEL_ERROR
#define e2p_err(fmt, ...) printf("[E2P ERROR] " fmt "\n", ##__VA_ARGS__)
#else
#define e2p_err(...)
#endif

/**
 * @brief Critical section support
 *
 * E2P_CRITICAL_ENTER() must return a uint32_t level for nested safety.
 * E2P_CRITICAL_EXIT(level) takes the level returned by E2P_CRITICAL_ENTER().
 *
 * Provide default implementations in user_config.h or port layer (hpm_nor_flash.c).
 * If not defined, the defaults from eeprom_emulation.c will be used.
 */
extern uint32_t e2p_enter_critical(void);
extern void e2p_exit_critical(uint32_t level);

#ifndef E2P_CRITICAL_ENTER
#define E2P_CRITICAL_ENTER() e2p_enter_critical()
#endif

#ifndef E2P_CRITICAL_EXIT
#define E2P_CRITICAL_EXIT(level) e2p_exit_critical(level)
#endif

/**
 * @brief Area state machine values
 *
 * Values decrease from 15 to 0, using only 1->0 bit transitions.
 * This is compatible with NOR flash in-place writes without erasing.
 * Transition: invalid(15) -> start(14) -> write(12) -> finish(8) -> valid(0)
 */
enum {
    e2p_state_valid   = 0,
    e2p_state_finish  = 8,
    e2p_state_write   = 12,
    e2p_state_start   = 14,
    e2p_state_invalid = 15
};

/**
 * @brief Block validity state written alongside each info descriptor
 */
typedef enum {
    e2p_block_invalid = 0xCCCC,
    e2p_block_valid   = 0xEEEE,
    e2p_block_erased  = 0xFFFF,
} e2p_block_state_t;

/**
 * @brief Error codes
 */
enum {
    E2P_STATUS_OK = 0,
    E2P_ERROR,
    E2P_ERROR_NO_MEM,
    E2P_ERROR_INIT_ERR,
    E2P_ERROR_BAD_ID,
    E2P_ERROR_BAD_ADDR,
    E2P_ERROR_OVERFLOW,
    E2P_ERROR_CRC,
    E2P_ERROR_NOT_FOUND,
};

/**
 * @brief Area header (stored at the tail of each half-area)
 */
typedef struct {
    uint32_t version;
    uint32_t magic;
    uint32_t state;
} e2p_header_t;

/**
 * @brief Block info descriptor
 */
#pragma pack(push, 1)
typedef struct {
    uint32_t block_id;
    uint32_t data_addr;
    uint16_t length;
    uint16_t valid_state;
    uint32_t crc;
} e2p_block_t;
#pragma pack(pop)

/**
 * @brief EEPROM emulation configuration
 *
 * Flash port function pointers provide the abstraction layer.
 * Implement read/write/erase for your flash medium and assign here.
 */
typedef struct {
    uint32_t start_addr;
    uint32_t sector_cnt;
    uint32_t erase_size;
    uint32_t version;

    hpm_stat_t (*flash_read)(uint8_t *buf, uint32_t addr, uint32_t size);
    hpm_stat_t (*flash_write)(const uint8_t *buf, uint32_t addr, uint32_t size);
    hpm_stat_t (*flash_erase)(uint32_t start_addr, uint32_t size);
} e2p_config_t;

/**
 * @brief EEPROM emulation runtime context
 */
typedef struct {
    e2p_config_t config;
    uint32_t p_data;
    uint32_t p_info;
    uint32_t remain_size;
} e2p_t;

#define E2P_MAGIC_ID       (0x48504D43)  /* 'H' 'P' 'M' 'C' */
#define E2P_ERASED_ID      (0xFFFFFFFF)
#define E2P_ERASED_BYTE    (0xFF)

#define E2P_FLUSH_TRY      (0)
#define E2P_FLUSH_FORCE    (1)

/**
 * @brief Initialize eeprom emulation
 *
 * Validates flash headers, recovers from incomplete flush on
 * power-loss, and rebuilds the in-memory index table.
 *
 * @param[in] e2p pointer to a pre-configured instance context
 * @return E2P_STATUS_OK on success, error code otherwise
 */
hpm_stat_t e2p_config(e2p_t *e2p);

/**
 * @brief Flush / defragment the storage area
 *
 * Compacts all valid entries into the backup half-area, erases the
 * current half, and switches over.  A state machine guarantees
 * power-off safety at every step.
 *
 * @param[in] flag E2P_FLUSH_TRY  - flush only when remain < erase_size
 *                 E2P_FLUSH_FORCE - always flush
 * @return E2P_STATUS_OK on success, error code otherwise
 */
hpm_stat_t e2p_flush(uint8_t flag);

/**
 * @brief Write data to eeprom emulation
 *
 * Appends a new data entry.  If the same block_id already exists the
 * old copy becomes redundant and is removed at the next flush.
 * Triggers an automatic flush when space is insufficient.
 *
 * @param[in] block_id user-defined identifier
 * @param[in] length   data length in bytes
 * @param[in] data     pointer to data buffer
 * @return E2P_STATUS_OK on success, error code otherwise
 */
hpm_stat_t e2p_write(uint32_t block_id, uint16_t length,
                     const uint8_t *data);

/**
 * @brief Read data from eeprom emulation
 *
 * Returns the latest valid data for the given block_id.
 * CRC32 is verified before the data is copied to the caller buffer.
 *
 * @param[in]  block_id user-defined identifier
 * @param[in]  length   buffer length (may be < stored length)
 * @param[out] data     pointer to output buffer
 * @return E2P_STATUS_OK on success, E2P_ERROR_NOT_FOUND / E2P_ERROR_CRC
 */
hpm_stat_t e2p_read(uint32_t block_id, uint16_t length, uint8_t *data);

/**
 * @brief Delete a data entry
 *
 * Marks the entry as invalid in the in-memory table.  The flash
 * space is reclaimed at the next flush.
 *
 * @param[in] block_id user-defined identifier
 * @return E2P_STATUS_OK on success, E2P_ERROR_NOT_FOUND if absent
 */
hpm_stat_t e2p_delete(uint32_t block_id);

/**
 * @brief Generate a 32-bit block_id from a name string
 *
 * Packs up to the first 4 characters into a uint32_t.
 * Shorter strings are zero-padded in the lower bytes.
 *
 * @param[in] name  null-terminated string (at least 1 character)
 * @return 32-bit block_id
 */
uint32_t e2p_generate_id(const char *name);

/**
 * @brief Format / erase the entire storage area (both halves)
 */
void e2p_clear(void);

/**
 * @brief Print current eeprom emulation status information
 */
void e2p_show_info(void);

/*
 * Usage example (NOR flash port):
 *
 *     #include "hpm_nor_flash.h"
 *
 *     static nor_flash_config_t g_nor_cfg = { ... };
 *
 *     static hpm_stat_t my_read(uint8_t *buf, uint32_t addr, uint32_t sz) {
 *         return nor_flash_read(&g_nor_cfg, buf, addr, sz);
 *     }
 *     static hpm_stat_t my_write(const uint8_t *buf, uint32_t addr, uint32_t sz) {
 *         return nor_flash_write(&g_nor_cfg, buf, addr, sz);
 *     }
 *     static hpm_stat_t my_erase(uint32_t addr, uint32_t sz) {
 *         return nor_flash_erase(&g_nor_cfg, addr, sz);
 *     }
 *
 *     e2p_t demo = {
 *         .config.start_addr  = 0x80080000,
 *         .config.erase_size  = 4096,
 *         .config.sector_cnt  = 128,
 *         .config.version     = 0x4553,
 *         .config.flash_read  = my_read,
 *         .config.flash_write = my_write,
 *         .config.flash_erase = my_erase,
 *     };
 *
 *     int main(void) {
 *         nor_flash_init(&g_nor_cfg);
 *         e2p_config(&demo);
 *         ...
 *     }
 */

#ifdef __cplusplus
}
#endif

#endif /* _EEPROM_EMULATION_H */
