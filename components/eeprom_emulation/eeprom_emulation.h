/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _EEPROM_EMULATION_H
#define _EEPROM_EMULATION_H

#include <stdint.h>
#include "user_config.h"
#include "hpm_nor_flash.h"
#include "hpm_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define E2P_DEBUG_LEVEL_TRACE (0)
#define E2P_DEBUG_LEVEL_INFO  (1)
#define E2P_DEBUG_LEVEL_WARN  (2)
#define E2P_DEBUG_LEVEL_ERROR (3)

#ifndef E2P_DEBUG_TRACE
#   if E2P_DEBUG_LEVEL <= E2P_DEBUG_LEVEL_TRACE
#       define E2P_TRACE(fmt, ...) printf("%s: %d: debug: "fmt"%s\n", __func__, __LINE__, __VA_ARGS__)
#       define e2p_trace(...) E2P_TRACE(__VA_ARGS__, "")
#   else
#       define e2p_trace(...)
#   endif
#endif

#ifndef E2P_DEBUG_INFO
#   if E2P_DEBUG_LEVEL <= E2P_DEBUG_LEVEL_INFO
#       define E2P_INFO(fmt, ...) printf(""fmt"%s\n", __VA_ARGS__)
#       define e2p_info(...) E2P_INFO(__VA_ARGS__, "")
#   else
#       define e2p_info(...)
#   endif
#endif

#ifndef E2P_DEBUG_WARN
#   if E2P_DEBUG_LEVEL <= E2P_DEBUG_LEVEL_WARN
#       define E2P_WARN(fmt, ...) printf(""fmt"%s\n", __VA_ARGS__)
#       define e2p_warn(...) E2P_WARN(__VA_ARGS__, "")
#   else
#       define e2p_warn(...)
#   endif
#endif

#ifndef E2P_DEBUG_ERROR
#   if E2P_DEBUG_LEVEL <= E2P_DEBUG_LEVEL_ERROR
#       define E2P_ERR(fmt, ...) printf(""fmt"%s\n", __VA_ARGS__)
#       define e2p_err(...) E2P_ERR(__VA_ARGS__, "")
#   else
#       define e2p_err(...)
#   endif
#endif

#define E2P_MAX_VAR_CNT (100)
#ifdef EEPROM_MAX_VAR_CNT
#undef E2P_MAX_VAR_CNT
#define E2P_MAX_VAR_CNT     EEPROM_MAX_VAR_CNT
#endif

enum {
    e2p_state_valid = 0,
    e2p_state_finish = 8,
    e2p_state_write = 12,
    e2p_state_start = 14,
    e2p_state_invalid = 15
};

typedef enum {
    e2p_invalid = 0xCCCC,
    e2p_valid = 0xEEEE,
    e2p_earsed = 0xFFFF,
} e2p_valid_state;

enum {
    E2P_STATUS_OK = 0,
    E2P_ERROR,
    E2P_ERROR_NO_MEM,
    E2P_ERROR_INIT_ERR,
    E2P_ERROR_BAD_ID,
    E2P_ERROR_BAD_ADDR,
    E2P_ERROR_MUL_VAR,
};

typedef struct {
    uint32_t version;
    uint32_t magic;
    uint32_t state;
} e2p_header;

#pragma pack(push, 1)
typedef struct {
    uint32_t block_id;
    uint32_t data_addr;
    uint16_t length;
    uint16_t valid_state;
    uint32_t crc;
} e2p_block;
#pragma pack(pop)

typedef struct {
    uint32_t start_addr;
    uint32_t sector_cnt;
    uint16_t erase_size;
    uint32_t version;

    uint32_t (*flash_read)(uint8_t *buf, uint32_t addr, uint32_t size);
    uint32_t (*flash_write)(uint8_t *buf, uint32_t addr, uint32_t size);
    void (*flash_erase)(uint32_t start_addr, uint32_t size);
} e2p_config_t;

typedef struct {
    e2p_config_t config;
    nor_flash_config_t nor_config;

    uint32_t p_data;
    uint32_t p_info;
    uint32_t remain_size;
} e2p_t;

#define E2P_MAGIC_ID (0x48504D43)       /*'H' 'P' 'M' 'C'*/

#define E2P_VALID_STATE     (0xFFFFFFF0)
#define E2P_EARSED_ID       (0xFFFFFFFF)
#define E2P_EARSED_VAR      (0xFF)

#define E2P_FLUSH_TRY       (0)
#define E2P_FLUSH_BEGIN     (1)
#define E2P_CRITICAL_ENTER()   e2p_enter_critical()
#define E2P_CRITICAL_EXIT()    e2p_exit_critical()

void e2p_enter_critical(void);
void e2p_exit_critical(void);

/**
 * @brief eeprom emulation config
 * 
 * @param e2p instance context
 * @return hpm_stat_t 
 */
hpm_stat_t e2p_config(e2p_t *e2p);

/**
 * @brief eeprom emulation flush whole area, remove redundancy
 * 
 * @param flag E2P_FLUSH_TRY - conditional flush, E2P_FLUSH_BEGIN - force flush
 * @return hpm_stat_t 
 */
hpm_stat_t e2p_flush(uint8_t flag);

/**
 * @brief eeprom emulation write
 * 
 * @param block_id custom id
 * @param length data length
 * @param data 
 * @return hpm_stat_t 
 */
hpm_stat_t e2p_write(uint32_t block_id, uint16_t length, uint8_t *data);

/**
 * @brief eeprom emulation read
 * 
 * @param block_id custom id
 * @param length data length
 * @param data 
 * @return hpm_stat_t 
 */
hpm_stat_t e2p_read(uint32_t block_id, uint16_t length, uint8_t *data);

/**
 * @brief generate custom id
 * 
 * @param name 
 * @return uint32_t 
 */
uint32_t e2p_generate_id(const char *name);

/**
 * @brief format whole area, 0xFF
 */
void e2p_clear(void);

/**
 * @brief show e2p instance info include config info and store info
 */
void e2p_show_info(void);

/* 
    e2p_t demo = {
        .config.start_addr = 0x80080000,
        .config.erase_size = 4096,
        .config.sector_cnt = 128,
        .config.version = 0x4553,
        .config.flash_read = flash_read,
        .config.flash_write = flash_write,
        .config.flash_erase = flash_erase,
    };


    int main(void)
    {
        e2p_config(&demo);

        
        ...
        ...
    }

*/

#ifdef __cplusplus
}
#endif

#endif