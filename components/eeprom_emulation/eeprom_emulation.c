/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eeprom_emulation.h"
#include "hpm_crc32.h"

#define E2P_OFFSET(TYPE, MEMBER) ((uint32_t)(&(((TYPE *)0)->MEMBER)))
#define E2P_SWAP(a, b)\
do {\
    uint32_t tmp = (a);\
    (a) = (b);\
    (b) = tmp;\
} while (0)

static e2p_t e2p_dummy;
static e2p_t *e2p_raw, *e2p_valid_ctx;
static e2p_block e2p_info_table[E2P_MAX_VAR_CNT];

E2P_ATTR
static void e2p_print_info(e2p_t *e2p)
{
    uint32_t info_count;
    uint32_t valid_count = E2P_MAX_VAR_CNT;

    info_count = (e2p->config.start_addr + e2p->config.sector_cnt * e2p->config.erase_size - e2p->p_info - sizeof(e2p_header)) / sizeof(e2p_block) - 1;
    for (int i = 0; i < E2P_MAX_VAR_CNT; i++) {
        if (e2p_info_table[i].block_id == E2P_EARSED_ID) {
            valid_count = i;
            break;
        }
    }

    e2p_info("------------ flash->eeprom init ok -----------");
    e2p_info("start address: 0x%08x", e2p->config.start_addr);
    e2p_info("sector count: %u", e2p->config.sector_cnt);
    e2p_info("flash earse granularity: %u", e2p->config.erase_size);
    e2p_info("version: 0x%x", e2p->config.version);
    e2p_info("end address: 0x%08x", e2p->config.start_addr + e2p->config.sector_cnt * e2p->config.erase_size);
    e2p_info("data write addr = 0x%08x, info write addr = 0x%08x, remain flash size = 0x%x", \
            e2p->p_data, e2p->p_info, e2p->remain_size);
    e2p_info("valid count percent info count( %u / %u )", valid_count, info_count);
    e2p_info("----------------------------------------------\n");
}

E2P_ATTR
static hpm_stat_t e2p_table_update(e2p_block *block)
{
    uint32_t i = 0;

    if (block->valid_state == e2p_invalid)
        return E2P_STATUS_OK;

    for ( ; i < E2P_MAX_VAR_CNT; i++) {
        if (e2p_info_table[i].block_id == E2P_EARSED_ID)
            break;
        if (e2p_info_table[i].block_id == block->block_id) {
            e2p_trace("block_id[0x%08x] multiple write, flush api solve repeat\n", block->block_id);
            break;
        }
    }

    if (i == E2P_MAX_VAR_CNT)
        return E2P_ERROR_MUL_VAR;

    memcpy(&e2p_info_table[i], block, sizeof(e2p_block));
    return E2P_STATUS_OK;
}

E2P_ATTR
static void e2p_format(e2p_t *e2p)
{
    e2p_config_t *cfg = &e2p->config;

    E2P_CRITICAL_ENTER();
    cfg->flash_erase(cfg->start_addr, cfg->sector_cnt * cfg->erase_size);
    E2P_CRITICAL_EXIT();
}

E2P_ATTR
static void e2p_config_info(e2p_t *e2p)
{
    e2p_header header;
    e2p_config_t *cfg = &e2p->config;
    uint32_t end_addr = cfg->start_addr + cfg->sector_cnt * cfg->erase_size;

    memset(&header, E2P_EARSED_VAR, sizeof(e2p_header));
    header.version = cfg->version;
    header.magic = E2P_MAGIC_ID;
    E2P_CRITICAL_ENTER();
    cfg->flash_write((uint8_t *)&header, end_addr - sizeof(e2p_header), sizeof(e2p_header));
    E2P_CRITICAL_EXIT();
}

E2P_ATTR
static uint32_t e2p_data_crc_calc(uint16_t length, uint8_t *data)
{
    return crc32(data, (uint32_t)length);
}

E2P_ATTR
static hpm_stat_t e2p_retrieve_info(uint32_t block_id, e2p_block *block)
{
    int i = 0;

    if (block_id == E2P_EARSED_ID)
        return E2P_ERROR_BAD_ID;

    for ( ; i < E2P_MAX_VAR_CNT; i++) {
        if (e2p_info_table[i].block_id == block_id) {
            e2p_trace("find read block, pos at table[%u]\n", i);
            memcpy(block, &e2p_info_table[i], sizeof(e2p_block));
            break;
        }
    }

    return (i == E2P_MAX_VAR_CNT ? E2P_ERROR_BAD_ID : E2P_STATUS_OK);
}

E2P_ATTR
static hpm_stat_t e2p_write_private(e2p_t *e2p, uint32_t block_id, uint16_t length, uint8_t *data)
{
    int ret = 0;
    e2p_block block;
    e2p_config_t *cfg = &e2p->config;

    if (e2p->remain_size < length + sizeof(e2p_block)) {
        e2p_trace("no enough flash\n");
        return E2P_ERROR_NO_MEM;
    }

    block.block_id = block_id;
    block.data_addr = e2p->p_data;
    block.length = length;
    block.valid_state = e2p_valid;
    block.crc = e2p_data_crc_calc(length, data);

    E2P_CRITICAL_ENTER();
    ret = cfg->flash_write(data, e2p->p_data, length);
    E2P_CRITICAL_EXIT();
    if (E2P_STATUS_OK != ret) {
        e2p_trace("flash write data error\n");
        return E2P_ERROR;
    }
    e2p->p_data += length;
    e2p->remain_size -= length;

    E2P_CRITICAL_ENTER();
    ret = cfg->flash_write((uint8_t *)&block, e2p->p_info, sizeof(e2p_block));
    E2P_CRITICAL_EXIT();
    if (E2P_STATUS_OK != ret) {
        e2p_trace("flash write info error\n");
        return E2P_ERROR;
    }
    e2p->p_info -= sizeof(e2p_block);
    e2p->remain_size -= sizeof(e2p_block);

    ret = e2p_table_update(&block);
    if (E2P_STATUS_OK != ret)
        return ret;

    e2p_trace("block_id[0x%08x] success write, data addr=0x%08x, remain size=0x%08x crc=0x%08x\n", \
            block.block_id, block.data_addr, e2p->remain_size, block.crc);
    return E2P_STATUS_OK;
}

E2P_ATTR
static int e2p_info_table_sort(void)
{
    int count = 0;
    e2p_block block;

    do {
        if (e2p_info_table[count].block_id == E2P_EARSED_ID) {
            break;
        }
        count++;
    } while (count < E2P_MAX_VAR_CNT);

    if (count == 0)
        return count;

    for (int i = 0; i < count; i++) {
        memcpy(&block, &e2p_info_table[i], sizeof(e2p_block));
        for (int j = i + 1; j < count; j++) {
            if (e2p_info_table[j].data_addr < block.data_addr) {
                E2P_SWAP(e2p_info_table[j].block_id, block.block_id);
                E2P_SWAP(e2p_info_table[j].data_addr, block.data_addr);
                E2P_SWAP(e2p_info_table[j].length, block.length);
                E2P_SWAP(e2p_info_table[j].valid_state, block.valid_state);
                E2P_SWAP(e2p_info_table[j].crc, block.crc);
            }
        }
        memcpy(&e2p_info_table[i], &block, sizeof(e2p_block));
    }

    return count;
}

E2P_ATTR
static void e2p_dummy_config(e2p_t *e2p)
{
    e2p_config_t *cfg = &e2p->config;
    e2p_t *dummy = &e2p_dummy;
    uint32_t cnt_bisect, addr_bisect, addr_end;

    if (dummy->config.sector_cnt == 0) {
        cnt_bisect = cfg->sector_cnt / 2;
        addr_end = cfg->start_addr + cfg->sector_cnt * cfg->erase_size;
    } else {
        cnt_bisect = cfg->sector_cnt;
        addr_end = cfg->start_addr + cfg->sector_cnt * cfg->erase_size * 2;
    }

    addr_bisect = cfg->start_addr + cnt_bisect * cfg->erase_size;
    memset(e2p_info_table, E2P_EARSED_VAR, sizeof(e2p_info_table));
    e2p->p_data = cfg->start_addr;
    e2p->p_info = addr_bisect - sizeof(e2p_block) - sizeof(e2p_header);
    e2p->remain_size = e2p->p_info - e2p->p_data;
    e2p->config.sector_cnt = cnt_bisect;
    e2p_raw = e2p;
    memcpy(dummy, e2p, sizeof(e2p_t));
    dummy->p_data = addr_bisect;
    dummy->p_info = addr_end - sizeof(e2p_block) - sizeof(e2p_header);
    dummy->remain_size = dummy->p_info - dummy->p_data;
    dummy->config.start_addr = addr_bisect;
}

E2P_ATTR
static void e2p_check_version(e2p_t *e2p)
{
    e2p_config_t *cfg = &e2p->config;
    e2p_header header;
    uint32_t addr_header = cfg->start_addr + cfg->sector_cnt * cfg->erase_size - sizeof(e2p_header);
    cfg->flash_read((uint8_t *)&header, addr_header, sizeof(e2p_header));
    e2p_trace("read data, version=%x, magic=%x, state=%x\n", header.version, header.magic, header.state);
    if (header.version != cfg->version || header.magic != E2P_MAGIC_ID) {
        e2p_info("check version failed, begin earse sector, it will take some time\n");
        e2p_format(e2p);
        e2p_config_info(e2p);
    }
}

E2P_ATTR
static e2p_t *e2p_get_valid_context(e2p_t *e2p, uint8_t *flush)
{
    e2p_t *valid = e2p;
    e2p_t *invalid = &e2p_dummy;
    e2p_config_t *cfg = &e2p->config;
    e2p_header header, dummy_header;
    uint32_t other_state;
    uint32_t raw_addr_header = cfg->start_addr + cfg->sector_cnt * cfg->erase_size - sizeof(e2p_header);
    uint32_t dummy_addr_header = invalid->config.start_addr + invalid->config.sector_cnt * invalid->config.erase_size - sizeof(e2p_header);

    cfg->flash_read((uint8_t *)&header, raw_addr_header, sizeof(e2p_header));
    cfg->flash_read((uint8_t *)&dummy_header, dummy_addr_header, sizeof(e2p_header));

    if ((header.state != e2p_state_valid) && (dummy_header.state != e2p_state_valid)) {
        if ((header.state == e2p_state_finish) && (dummy_header.state == e2p_state_invalid)) {
            valid = e2p;
        } else if ((header.state == e2p_state_invalid) && (dummy_header.state == e2p_state_finish)) {
            valid = invalid;
        } else if (((header.state & 0x0f) == e2p_state_invalid) && ((dummy_header.state & 0x0f) == e2p_state_invalid)) {
            valid = e2p;
        } else {
            valid = NULL;
        }

        header.state = e2p_state_valid;
        E2P_CRITICAL_ENTER();
        cfg->flash_write((uint8_t *)&header, valid->p_info + sizeof(e2p_block), sizeof(e2p_header));
        E2P_CRITICAL_EXIT();
        return valid;
    }

    if (header.state == e2p_state_valid) {
        valid = e2p;
        other_state = dummy_header.state;
    } else if (dummy_header.state == e2p_state_valid) {
        valid = invalid;
        invalid = e2p;
        other_state = header.state;
    }
    /* power-off between new data write over and earse old data */
    if (other_state == e2p_state_finish) {
        e2p_format(valid);
        e2p_config_info(valid);
        valid = invalid;
        header.state = e2p_state_valid;
        E2P_CRITICAL_ENTER();
        cfg->flash_write((uint8_t *)&header, valid->p_info + sizeof(e2p_block), sizeof(e2p_header));
        E2P_CRITICAL_EXIT();
    /* power off between start flush and write new data, reflush */
    } else if (other_state == e2p_state_start) {
        *flush = 1;
    /* power off in writing process, clean buffer, reflush */
    } else if (other_state == e2p_state_write) {
        e2p_format(invalid);
        e2p_config_info(invalid);
        *flush = 1;
    }

    return valid;
}

E2P_ATTR
hpm_stat_t e2p_config(e2p_t *e2p)
{
    if (e2p->config.erase_size == 0 || e2p->config.sector_cnt == 0) {
        e2p_err("config error erase_size = %u, sector_cnt = %u\n", e2p->config.erase_size, e2p->config.sector_cnt);
        return E2P_ERROR_INIT_ERR;
    }

    if (e2p->config.flash_read == NULL || e2p->config.flash_write == NULL || e2p->config.flash_erase == NULL) {
        e2p_err("Not register operate function read = %p, write = %p, erase = %p", \
                      e2p->config.flash_read, e2p->config.flash_write, e2p->config.flash_erase);
        return E2P_ERROR_INIT_ERR;
    }

    if ((e2p->config.sector_cnt % 2) == 1) {
        e2p_err("Sector count must be even, %u sector now, maybe %u sector?", \
                    e2p->config.sector_cnt, e2p->config.sector_cnt + 1);
        return E2P_ERROR_INIT_ERR;
    }

    e2p_block block;
    e2p_t *dummy = &e2p_dummy;
    e2p_config_t *cfg = &e2p->config;
    uint8_t need_flush = 0;

    e2p_dummy_config(e2p);
    e2p_check_version(e2p);
    e2p_check_version(dummy);
    e2p_valid_ctx = e2p_get_valid_context(e2p, &need_flush);

    if (e2p_valid_ctx == NULL)
        return E2P_ERROR_INIT_ERR;

    while (1) {
        block.data_addr = e2p_valid_ctx->p_info;
        block.length = sizeof(e2p_block);
        cfg->flash_read((uint8_t *)&block, block.data_addr, block.length);
        if (block.block_id == E2P_EARSED_ID)
            break;

        int ret = e2p_table_update(&block);
        if (E2P_STATUS_OK != ret)
            return ret;

        e2p_valid_ctx->p_data += block.length;
        e2p_valid_ctx->p_info -= sizeof(e2p_block);
        e2p_valid_ctx->remain_size -= (block.length + sizeof(e2p_block));

        if (e2p_valid_ctx->remain_size < 2 * sizeof(e2p_block)) {
            e2p_info("remain flash is not enough\n");
            return E2P_ERROR_NO_MEM;
        }
    }

    if (need_flush)
        e2p_flush(E2P_FLUSH_BEGIN);

    e2p_print_info(e2p_valid_ctx);
    return E2P_STATUS_OK;
}

E2P_ATTR
hpm_stat_t e2p_flush(uint8_t flag)
{
    if (e2p_valid_ctx->config.erase_size < e2p_valid_ctx->remain_size && flag == E2P_FLUSH_TRY)
        return E2P_STATUS_OK;

    e2p_t *new;
    e2p_t *e2p = e2p_valid_ctx;

    /* switch to new instance */
    new = (e2p == e2p_raw) ? &e2p_dummy : e2p_raw;
    e2p_config_t *cfg = &new->config;
    uint32_t state = e2p_state_start;
    uint32_t addr_state = cfg->start_addr + cfg->sector_cnt * cfg->erase_size - sizeof(uint32_t);
    /* indicate start switch */
    E2P_CRITICAL_ENTER();
    cfg->flash_write((uint8_t *)&state, addr_state, sizeof(uint32_t));
    E2P_CRITICAL_EXIT();

    int valid_num = e2p_info_table_sort();
    uint8_t read_buf[cfg->erase_size];
    int count = 0;
    uint32_t read_len = 0;

    e2p->p_data = e2p->config.start_addr;
    e2p->p_info = e2p->config.start_addr + e2p->config.sector_cnt * e2p->config.erase_size - sizeof(e2p_block) - sizeof(e2p_header);
    e2p->remain_size = e2p->p_info - e2p->p_data;
    /* indicate begin write to new */
    state = e2p_state_write;
    E2P_CRITICAL_ENTER();
    cfg->flash_write((uint8_t *)&state, addr_state, sizeof(uint32_t));
    E2P_CRITICAL_EXIT();

    for (int i = 0; i < valid_num;) {
        while (1) {
            if (e2p_info_table[i].block_id == E2P_EARSED_ID || i >= valid_num)
                break;
            if (cfg->erase_size - read_len < e2p_info_table[i].length)
                break;
            cfg->flash_read(read_buf + read_len, e2p_info_table[i].data_addr, e2p_info_table[i].length);
            read_len += e2p_info_table[i].length;
            i++;
        }
        e2p_trace("---- transfer data to buffer: len=%u\n", read_len);
        uint8_t *pdata = read_buf;

        while (count < i) {
            if (e2p_info_table[count].block_id == E2P_EARSED_ID)
                break;

            e2p_write_private(new, e2p_info_table[count].block_id, e2p_info_table[count].length, pdata);
            pdata += e2p_info_table[count].length;
            count++;
        }
        e2p_trace("-----write data back: len=%u\n", pdata - read_buf);
        read_len = 0;
        pdata = read_buf;
    }
    /* indicate write finish, begin erase old */
    state = e2p_state_finish;
    E2P_CRITICAL_ENTER();
    cfg->flash_write((uint8_t *)&state, addr_state, sizeof(uint32_t));
    E2P_CRITICAL_EXIT();

    e2p_format(e2p);
    e2p_config_info(e2p);
    /* indicate all work done, use new area */
    state = e2p_state_valid;
    E2P_CRITICAL_ENTER();
    cfg->flash_write((uint8_t *)&state, addr_state, sizeof(uint32_t));
    E2P_CRITICAL_EXIT();
    e2p_valid_ctx = new;

    return E2P_STATUS_OK;
}

E2P_ATTR
hpm_stat_t e2p_write(uint32_t block_id, uint16_t length, uint8_t *data)
{
    if (e2p_valid_ctx->remain_size < length + sizeof(e2p_block)) {
        e2p_flush(E2P_FLUSH_BEGIN);
        if (e2p_valid_ctx->remain_size < length + sizeof(e2p_block)) {
            e2p_trace("no enough flash write\n");
            return E2P_ERROR_NO_MEM;
        }
    }

    return e2p_write_private(e2p_valid_ctx, block_id, length, data);
}

E2P_ATTR
hpm_stat_t e2p_read(uint32_t block_id, uint16_t length, uint8_t *data)
{
    e2p_block block;
    int ret = 0;

    ret = e2p_retrieve_info(block_id, &block);
    if (ret != E2P_STATUS_OK)
        return ret;

    uint8_t tmp[block.length];
    e2p_valid_ctx->config.flash_read(tmp, block.data_addr, block.length);

    if (block.crc != e2p_data_crc_calc(block.length, tmp)) {
        e2p_trace("crc check error, data addr = 0x%08x, crc = 0x%08x", block.data_addr, block.crc);
        return E2P_ERROR;
    }
    
    length > block.length ? (length=block.length) : length;
    memmove(data, tmp, length);
    return E2P_STATUS_OK;
}

E2P_ATTR
void e2p_clear(void)
{
    e2p_config_t *cfg = &e2p_raw->config;

    E2P_CRITICAL_ENTER();
    cfg->flash_erase(cfg->start_addr, cfg->sector_cnt * cfg->erase_size * 2);
    E2P_CRITICAL_EXIT();
}

E2P_ATTR
uint32_t e2p_generate_id(const char *name)
{
    return (name[0] << 24) | (name[1] << 16) | (name[2] << 8) | (name[3]);
}

E2P_ATTR
void e2p_show_info(void)
{
    e2p_print_info(e2p_valid_ctx);
}
