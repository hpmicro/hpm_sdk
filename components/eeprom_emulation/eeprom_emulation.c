/*
 * Copyright (c) 2023,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eeprom_emulation.h"
#include "hpm_crc32.h"

static e2p_t e2p_backup;
static e2p_t *e2p_primary;
static e2p_t *e2p_active;
static e2p_block_t e2p_info_table[E2P_MAX_VAR_CNT];
static uint8_t e2p_flush_buf[E2P_FLUSH_BUF_SIZE];

E2P_ATTR
static void e2p_print_info(e2p_t *e2p)
{
    uint32_t end_addr = e2p->config.start_addr
                        + e2p->config.sector_cnt * e2p->config.erase_size;
    uint32_t info_count = (end_addr - e2p->p_info - sizeof(e2p_header_t))
                          / sizeof(e2p_block_t) - 1;
    uint32_t valid_count = 0;

    for (uint32_t i = 0; i < E2P_MAX_VAR_CNT; i++) {
        if (e2p_info_table[i].block_id == E2P_ERASED_ID) {
            valid_count = i;
            break;
        }
    }

    e2p_info("------------ e2p init ok -----------");
    e2p_info("start address : 0x%08x", e2p->config.start_addr);
    e2p_info("sector count  : %u", e2p->config.sector_cnt);
    e2p_info("erase size    : %u", e2p->config.erase_size);
    e2p_info("version       : 0x%x", e2p->config.version);
    e2p_info("end address   : 0x%08x", end_addr);
    e2p_info("data_ptr=0x%08x  info_ptr=0x%08x  remain=0x%x",
             e2p->p_data, e2p->p_info, e2p->remain_size);
    e2p_info("entries valid / total : %u / %u", valid_count, info_count);
    e2p_info("------------------------------------");

    (void)info_count;
    (void)valid_count;
}

E2P_ATTR
static hpm_stat_t e2p_table_update(e2p_block_t *block)
{
    uint32_t i;

    for (i = 0; i < E2P_MAX_VAR_CNT; i++) {
        if (e2p_info_table[i].block_id == E2P_ERASED_ID) {
            break;
        }
        if (e2p_info_table[i].block_id == block->block_id) {
            memcpy(&e2p_info_table[i], block, sizeof(e2p_block_t));
            return E2P_STATUS_OK;
        }
    }

    if (block->valid_state != e2p_block_valid) {
        return E2P_STATUS_OK;
    }

    if (i >= E2P_MAX_VAR_CNT) {
        e2p_err("info table overflow, max=%u", E2P_MAX_VAR_CNT);
        return E2P_ERROR_OVERFLOW;
    }

    memcpy(&e2p_info_table[i], block, sizeof(e2p_block_t));
    return E2P_STATUS_OK;
}

E2P_ATTR
static hpm_stat_t e2p_format(e2p_t *e2p)
{
    e2p_config_t *cfg = &e2p->config;

    uint32_t level = E2P_CRITICAL_ENTER();
    hpm_stat_t ret = cfg->flash_erase(cfg->start_addr,
                                      cfg->sector_cnt * cfg->erase_size);
    E2P_CRITICAL_EXIT(level);
    return ret;
}

E2P_ATTR
static hpm_stat_t e2p_write_header(e2p_t *e2p)
{
    e2p_header_t header;
    e2p_config_t *cfg = &e2p->config;
    uint32_t addr = cfg->start_addr
                    + cfg->sector_cnt * cfg->erase_size
                    - sizeof(e2p_header_t);

    memset(&header, E2P_ERASED_BYTE, sizeof(e2p_header_t));
    header.version = cfg->version;
    header.magic = E2P_MAGIC_ID;

    uint32_t level = E2P_CRITICAL_ENTER();
    hpm_stat_t ret = cfg->flash_write((const uint8_t *)&header,
                                      addr, sizeof(e2p_header_t));
    E2P_CRITICAL_EXIT(level);
    return ret;
}

E2P_ATTR
static uint32_t e2p_data_crc_calc(uint16_t length, const uint8_t *data)
{
    return crc32(data, (uint32_t)length);
}

E2P_ATTR
static hpm_stat_t e2p_retrieve_info(uint32_t block_id, e2p_block_t *block)
{
    if (block_id == E2P_ERASED_ID) {
        return E2P_ERROR_BAD_ID;
    }

    for (uint32_t i = 0; i < E2P_MAX_VAR_CNT; i++) {
        if (e2p_info_table[i].block_id == block_id) {
            if (e2p_info_table[i].valid_state != e2p_block_valid) {
                return E2P_ERROR_NOT_FOUND;
            }
            memcpy(block, &e2p_info_table[i], sizeof(e2p_block_t));
            return E2P_STATUS_OK;
        }
    }

    return E2P_ERROR_NOT_FOUND;
}

E2P_ATTR
static hpm_stat_t e2p_write_internal(e2p_t *e2p, uint32_t block_id,
                                     uint16_t length, const uint8_t *data)
{
    e2p_block_t block;
    e2p_config_t *cfg = &e2p->config;
    hpm_stat_t ret;

    if (e2p->remain_size < length + sizeof(e2p_block_t)) {
        e2p_trace("insufficient space: need=%u remain=%u",
                  (uint32_t)(length + sizeof(e2p_block_t)),
                  e2p->remain_size);
        return E2P_ERROR_NO_MEM;
    }

    block.block_id = block_id;
    block.data_addr = e2p->p_data;
    block.length = length;
    block.valid_state = e2p_block_valid;
    block.crc = e2p_data_crc_calc(length, data);

    uint32_t level = E2P_CRITICAL_ENTER();
    ret = cfg->flash_write(data, e2p->p_data, length);
    E2P_CRITICAL_EXIT(level);
    if (E2P_STATUS_OK != ret) {
        e2p_err("flash write data failed at 0x%08x", e2p->p_data);
        return E2P_ERROR;
    }
    e2p->p_data += length;
    e2p->remain_size -= length;

    level = E2P_CRITICAL_ENTER();
    ret = cfg->flash_write((const uint8_t *)&block,
                           e2p->p_info, sizeof(e2p_block_t));
    E2P_CRITICAL_EXIT(level);
    if (E2P_STATUS_OK != ret) {
        e2p_err("flash write info failed at 0x%08x", e2p->p_info);
        return E2P_ERROR;
    }
    e2p->p_info -= sizeof(e2p_block_t);
    e2p->remain_size -= sizeof(e2p_block_t);

    ret = e2p_table_update(&block);
    if (E2P_STATUS_OK != ret) {
        return ret;
    }

    e2p_trace("written id=0x%08x addr=0x%08x remain=0x%x crc=0x%08x",
              block.block_id, block.data_addr,
              e2p->remain_size, block.crc);
    return E2P_STATUS_OK;
}

E2P_ATTR
static int e2p_info_table_sort(void)
{
    int count = 0;

    while (count < E2P_MAX_VAR_CNT) {
        if (e2p_info_table[count].block_id == E2P_ERASED_ID) {
            break;
        }
        count++;
    }

    if (count <= 1) {
        return count;
    }

    for (int i = 0; i < count - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < count; j++) {
            if (e2p_info_table[j].data_addr
                < e2p_info_table[min_idx].data_addr) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            e2p_block_t tmp;
            memcpy(&tmp, &e2p_info_table[i], sizeof(e2p_block_t));
            memcpy(&e2p_info_table[i],
                   &e2p_info_table[min_idx], sizeof(e2p_block_t));
            memcpy(&e2p_info_table[min_idx], &tmp, sizeof(e2p_block_t));
        }
    }

    return count;
}

E2P_ATTR
static void e2p_init_area_ptrs(e2p_t *e2p)
{
    uint32_t end_addr = e2p->config.start_addr
                        + e2p->config.sector_cnt * e2p->config.erase_size;

    e2p->p_data = e2p->config.start_addr;
    e2p->p_info = end_addr - sizeof(e2p_block_t) - sizeof(e2p_header_t);
    e2p->remain_size = e2p->p_info - e2p->p_data;
}

E2P_ATTR
static void e2p_setup_areas(e2p_t *e2p)
{
    e2p_t *backup = &e2p_backup;
    uint32_t half_cnt;
    uint32_t half_addr;

    half_cnt = e2p->config.sector_cnt / 2;
    half_addr = e2p->config.start_addr + half_cnt * e2p->config.erase_size;

    memset(e2p_info_table, E2P_ERASED_BYTE, sizeof(e2p_info_table));

    e2p->config.sector_cnt = half_cnt;
    e2p_init_area_ptrs(e2p);
    e2p_primary = e2p;

    memcpy(backup, e2p, sizeof(e2p_t));
    backup->config.start_addr = half_addr;
    e2p_init_area_ptrs(backup);
}

E2P_ATTR
static void e2p_check_version(e2p_t *e2p, bool update_invalid)
{
    e2p_config_t *cfg = &e2p->config;
    e2p_header_t header;
    uint32_t addr = cfg->start_addr
                    + cfg->sector_cnt * cfg->erase_size
                    - sizeof(e2p_header_t);

    cfg->flash_read((uint8_t *)&header, addr, sizeof(e2p_header_t));
    e2p_trace("header: ver=0x%x magic=0x%x state=0x%x",
              header.version, header.magic, header.state);

    if (header.version != cfg->version || header.magic != E2P_MAGIC_ID) {
        e2p_info("version mismatch, erasing area at 0x%08x",
                 cfg->start_addr);
        if (update_invalid) {
            e2p_format(e2p);
            e2p_write_header(e2p);
        }
    }
}

E2P_ATTR
static hpm_stat_t e2p_write_state(e2p_t *e2p, uint32_t state)
{
    e2p_config_t *cfg = &e2p->config;
    uint32_t addr = cfg->start_addr
                    + cfg->sector_cnt * cfg->erase_size
                    - sizeof(uint32_t);

    uint32_t level = E2P_CRITICAL_ENTER();
    hpm_stat_t ret = cfg->flash_write((const uint8_t *)&state,
                                      addr, sizeof(uint32_t));
    E2P_CRITICAL_EXIT(level);
    return ret;
}

E2P_ATTR
static e2p_t *e2p_recover_context(e2p_t *e2p, uint8_t *need_flush)
{
    e2p_t *valid = e2p;
    e2p_t *other = &e2p_backup;
    e2p_config_t *cfg = &e2p->config;
    e2p_header_t hdr_a, hdr_b;
    uint32_t addr_a, addr_b;

    addr_a = cfg->start_addr
             + cfg->sector_cnt * cfg->erase_size
             - sizeof(e2p_header_t);
    addr_b = other->config.start_addr
             + other->config.sector_cnt * other->config.erase_size
             - sizeof(e2p_header_t);

    cfg->flash_read((uint8_t *)&hdr_a, addr_a, sizeof(e2p_header_t));
    cfg->flash_read((uint8_t *)&hdr_b, addr_b, sizeof(e2p_header_t));

    /*
     * Mask to 4 bits: erased flash (0xFFFFFFFF & 0x0F = 15 = invalid).
     * This avoids the raw 0xFFFFFFFF != 15 comparison bug.
     */
    uint32_t state_a = hdr_a.state & 0x0F;
    uint32_t state_b = hdr_b.state & 0x0F;

    /* Case 1: neither area marked valid — first boot or both recovered */
    if (state_a != e2p_state_valid && state_b != e2p_state_valid) {
        if (state_a == e2p_state_finish && state_b == e2p_state_invalid) {
            valid = e2p;
        } else if (state_a == e2p_state_invalid && state_b == e2p_state_finish) {
            valid = other;
        } else {
            valid = e2p;
        }
        e2p_write_state(valid, e2p_state_valid);
        return valid;
    }

    /* Case 2: exactly one area is valid — check the other for recovery */
    uint32_t other_state;
    if (state_a == e2p_state_valid) {
        valid = e2p;
        other = &e2p_backup;
        other_state = state_b;
    } else {
        valid = &e2p_backup;
        other = e2p;
        other_state = state_a;
    }

    if (other_state == e2p_state_finish) {
        /* Power-off after data copied but before old area erased */
        e2p_format(valid);
        e2p_write_header(valid);
        valid = other;
        e2p_write_state(valid, e2p_state_valid);
    } else if (other_state == e2p_state_start
               || other_state == e2p_state_write) {
        /* Power-off during flush — clean the incomplete area, re-flush */
        e2p_format(other);
        e2p_write_header(other);
        *need_flush = 1;
    }

    return valid;
}

/* ------------------------------------------------------------------ */
/*  public API                                                          */
/* ------------------------------------------------------------------ */

E2P_ATTR
hpm_stat_t e2p_config(e2p_t *e2p)
{
    if (e2p->config.erase_size == 0 || e2p->config.sector_cnt == 0) {
        e2p_err("invalid config: erase_size=%u sector_cnt=%u",
                e2p->config.erase_size, e2p->config.sector_cnt);
        return E2P_ERROR_INIT_ERR;
    }

    if (e2p->config.flash_read == NULL
        || e2p->config.flash_write == NULL
        || e2p->config.flash_erase == NULL) {
        e2p_err("flash ops not registered");
        return E2P_ERROR_INIT_ERR;
    }

    if ((e2p->config.sector_cnt % 2) != 0) {
        e2p_err("sector_cnt must be even: %u (try %u)",
                e2p->config.sector_cnt, e2p->config.sector_cnt + 1);
        return E2P_ERROR_INIT_ERR;
    }

    e2p_block_t block;
    e2p_config_t *cfg;
    hpm_stat_t ret = E2P_STATUS_OK;
    uint8_t need_flush = 0;

    e2p_setup_areas(e2p);
    e2p_check_version(e2p, true);
    e2p_check_version(&e2p_backup, true);

    e2p_active = e2p_recover_context(e2p, &need_flush);
    if (e2p_active == NULL) {
        return E2P_ERROR_INIT_ERR;
    }

    cfg = &e2p_active->config;

    /* Rebuild in-memory index from flash */
    while (1) {
        cfg->flash_read((uint8_t *)&block,
                        e2p_active->p_info, sizeof(e2p_block_t));
        if (block.block_id == E2P_ERASED_ID) {
            break;
        }

        ret = e2p_table_update(&block);
        if (E2P_STATUS_OK != ret) {
            return ret;
        }

        uint32_t used = (uint32_t)block.length + sizeof(e2p_block_t);
        if (used > e2p_active->remain_size) {
            e2p_warn("area overflow detected during init");
            need_flush = 1;
            break;
        }

        e2p_active->p_data += block.length;
        e2p_active->p_info -= sizeof(e2p_block_t);
        e2p_active->remain_size -= used;
    }

    if (need_flush) {
        ret = e2p_flush(E2P_FLUSH_FORCE);
        if (E2P_STATUS_OK != ret) {
            return ret;
        }
    }

    e2p_print_info(e2p_active);
    return E2P_STATUS_OK;
}

E2P_ATTR
hpm_stat_t e2p_flush(uint8_t flag)
{
    if (flag == E2P_FLUSH_TRY
        && e2p_active->config.erase_size < e2p_active->remain_size) {
        return E2P_STATUS_OK;
    }

    e2p_t *target;
    e2p_t *old = e2p_active;
    hpm_stat_t ret;

    target = (old == e2p_primary) ? &e2p_backup : e2p_primary;

    /* Phase 1: mark target as "flush starting" */
    e2p_write_state(target, e2p_state_start);

    int valid_num = e2p_info_table_sort();
    e2p_init_area_ptrs(target);

    /* Phase 2: mark target as "writing data" */
    e2p_write_state(target, e2p_state_write);

    /* Transfer valid entries one by one via static buffer */
    for (int i = 0; i < valid_num; i++) {
        if (e2p_info_table[i].block_id == E2P_ERASED_ID) {
            break;
        }
        if (e2p_info_table[i].valid_state != e2p_block_valid) {
            continue;
        }

        uint16_t len = e2p_info_table[i].length;
        if (len > E2P_FLUSH_BUF_SIZE) {
            e2p_err("entry %u bytes exceeds flush buffer %u",
                    len, E2P_FLUSH_BUF_SIZE);
            return E2P_ERROR;
        }

        old->config.flash_read(e2p_flush_buf,
                               e2p_info_table[i].data_addr, len);
        ret = e2p_write_internal(target,
                                 e2p_info_table[i].block_id,
                                 len, e2p_flush_buf);
        if (ret != E2P_STATUS_OK) {
            e2p_err("flush write failed for id 0x%08x",
                    e2p_info_table[i].block_id);
            return ret;
        }
    }

    /* Compact table: remove invalid / deleted entries */
    int wr = 0;
    for (int rd = 0; rd < E2P_MAX_VAR_CNT; rd++) {
        if (e2p_info_table[rd].block_id == E2P_ERASED_ID) {
            break;
        }
        if (e2p_info_table[rd].valid_state == e2p_block_valid) {
            if (wr != rd) {
                memcpy(&e2p_info_table[wr],
                       &e2p_info_table[rd], sizeof(e2p_block_t));
            }
            wr++;
        }
    }
    memset(&e2p_info_table[wr], E2P_ERASED_BYTE,
           (E2P_MAX_VAR_CNT - wr) * sizeof(e2p_block_t));

    /* Phase 3: data transfer complete */
    e2p_write_state(target, e2p_state_finish);

    /* Phase 4: erase old area */
    e2p_format(old);
    e2p_write_header(old);

    /* Phase 5: target is now the active area */
    e2p_write_state(target, e2p_state_valid);
    e2p_active = target;

    return E2P_STATUS_OK;
}

E2P_ATTR
hpm_stat_t e2p_write(uint32_t block_id, uint16_t length,
                     const uint8_t *data)
{
    if (e2p_active->remain_size < length + sizeof(e2p_block_t)) {
        hpm_stat_t ret = e2p_flush(E2P_FLUSH_FORCE);
        if (ret != E2P_STATUS_OK) {
            return ret;
        }
        if (e2p_active->remain_size < length + sizeof(e2p_block_t)) {
            e2p_err("no space after flush: need=%u remain=%u",
                    (uint32_t)(length + sizeof(e2p_block_t)),
                    e2p_active->remain_size);
            return E2P_ERROR_NO_MEM;
        }
    }

    return e2p_write_internal(e2p_active, block_id, length, data);
}

E2P_ATTR
hpm_stat_t e2p_read(uint32_t block_id, uint16_t length, uint8_t *data)
{
    e2p_block_t block;
    hpm_stat_t ret;

    ret = e2p_retrieve_info(block_id, &block);
    if (ret != E2P_STATUS_OK) {
        return ret;
    }

    if (block.length > E2P_FLUSH_BUF_SIZE) {
        e2p_err("stored length %u exceeds buffer %u",
                block.length, E2P_FLUSH_BUF_SIZE);
        return E2P_ERROR;
    }

    e2p_active->config.flash_read(e2p_flush_buf,
                                  block.data_addr, block.length);

    if (block.crc != e2p_data_crc_calc(block.length, e2p_flush_buf)) {
        e2p_err("crc mismatch at 0x%08x expected=0x%08x",
                block.data_addr, block.crc);
        return E2P_ERROR_CRC;
    }

    uint16_t copy_len = (length < block.length) ? length : block.length;
    memcpy(data, e2p_flush_buf, copy_len);
    return E2P_STATUS_OK;
}

E2P_ATTR
hpm_stat_t e2p_delete(uint32_t block_id)
{
    for (uint32_t i = 0; i < E2P_MAX_VAR_CNT; i++) {
        if (e2p_info_table[i].block_id == block_id) {
            if (e2p_info_table[i].valid_state != e2p_block_valid) {
                return E2P_ERROR_NOT_FOUND;
            }
            e2p_info_table[i].valid_state = e2p_block_invalid;
            e2p_trace("deleted id=0x%08x", block_id);
            return E2P_STATUS_OK;
        }
    }
    return E2P_ERROR_NOT_FOUND;
}

E2P_ATTR
void e2p_clear(void)
{
    e2p_config_t *cfg = &e2p_primary->config;

    uint32_t level = E2P_CRITICAL_ENTER();
    cfg->flash_erase(cfg->start_addr,
                     cfg->sector_cnt * cfg->erase_size * 2);
    E2P_CRITICAL_EXIT(level);
    memset(e2p_info_table, E2P_ERASED_BYTE, sizeof(e2p_info_table));
}

E2P_ATTR
uint32_t e2p_generate_id(const char *name)
{
    uint32_t id = 0;

    if (name == NULL || strlen(name) == 0) {
        e2p_info("name is NULL or empty");
        return 0;
    }

    if (strlen(name) > 4) {
        e2p_info("length exceeds 4, risk of duplicate names: %s", name);
        return 0;
    }

    for (int i = 0; i < 4 && name[i] != '\0'; i++) {
        id |= ((uint32_t)(uint8_t)name[i]) << ((3 - i) * 8);
    }
    return id;
}

E2P_ATTR
void e2p_show_info(void)
{
    e2p_print_info(e2p_active);
}
