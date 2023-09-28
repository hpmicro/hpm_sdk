/*
 * Copyright (c) 2023 HPMicro
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

static e2p_block e2p_info_table[E2P_MAX_VAR_CNT];

static void e2p_print_info(e2p_t *e2p)
{
    uint32_t info_count;
    uint32_t valid_count = E2P_MAX_VAR_CNT;

    info_count = (e2p->config.start_addr + e2p->config.sector_cnt * e2p->config.erase_size - e2p->p_info) / sizeof(e2p_block) - 2;
    for (int i = 0; i < E2P_MAX_VAR_CNT; i++) {
        if (e2p_info_table[i].block_id == E2P_EARSED_ID) {
            valid_count = i;
            break;
        }
    }

    e2p_info("------------ flash->eeprom init ok -----------\n");
    e2p_info("start address: 0x%08x", e2p->config.start_addr);
    e2p_info("sector count: %u", e2p->config.sector_cnt);
    e2p_info("flash earse granularity: %u", e2p->config.erase_size);
    e2p_info("version: 0x%x", e2p->config.version);
    e2p_info("end address: 0x%08x", e2p->config.start_addr + e2p->config.sector_cnt * e2p->config.erase_size);
    e2p_info("data write addr = 0x%08x, info write addr = 0x%08x, remain flash size = 0x%x\n", \
            e2p->p_data, e2p->p_info, e2p->remain_size);
    e2p_info("valid count percent info count( %u / %u )\n", valid_count, info_count);
    e2p_info("----------------------------------------------\n");
}

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

static void e2p_config_info(e2p_t *e2p)
{
    e2p_block block;
    e2p_config_t *cfg = &e2p->config;
    uint32_t end_addr = cfg->start_addr + cfg->sector_cnt * cfg->erase_size;

    memset(&block, E2P_EARSED_VAR, sizeof(e2p_block));
    block.block_id = cfg->version;
    block.data_addr = E2P_MAGIC_ID;
    cfg->flash_write((uint8_t *)&block, end_addr - sizeof(e2p_block), sizeof(e2p_block));
    e2p_trace("e2p info has been config\n");
}

static uint32_t e2p_data_crc_calc(uint16_t length, uint8_t *data)
{
    return crc32(data, (uint32_t)length);
}

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

    if (E2P_STATUS_OK != cfg->flash_write(data, e2p->p_data, length)) {
        e2p_trace("flash write data error\n");
        return E2P_ERROR;
    }
    e2p->p_data += length;
    e2p->remain_size -= length;

    if (E2P_STATUS_OK != cfg->flash_write((uint8_t *)&block, e2p->p_info, sizeof(e2p_block))) {
        e2p_trace("flash write info error\n");
        return E2P_ERROR;
    }
    e2p->p_info -= sizeof(e2p_block);
    e2p->remain_size -= sizeof(e2p_block);

    ret = e2p_table_update(&block);
    if (E2P_STATUS_OK != ret)
        return ret;

    e2p_info("block_id[0x%08x] success write, data addr=0x%08x, remain size=0x%08x crc=0x%08x\n", block.block_id, block.data_addr, e2p->remain_size, block.crc);
    return E2P_STATUS_OK;
}

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

static void e2p_earse_info_sector(e2p_t *e2p)
{
    uint32_t data_sec_num, info_sec_num;
    uint32_t addr, size;
    e2p_config_t *cfg = &e2p->config;

    data_sec_num = (e2p->p_data - cfg->start_addr) / cfg->erase_size;
    info_sec_num = (e2p->p_info - cfg->start_addr) / cfg->erase_size;
    addr = e2p->p_info - (e2p->p_info % cfg->erase_size);
    size = (cfg->sector_cnt - info_sec_num) * cfg->erase_size;

    e2p_trace("data_sector=%u, info_sector=%u, addr=0x%08x, size=0x%08x", data_sec_num, info_sec_num, addr, size);

    if (data_sec_num < info_sec_num) {
        cfg->flash_erase(addr, size);
    } else {
        uint8_t data_buf[cfg->erase_size];
        uint32_t remain_size = e2p->p_info % cfg->erase_size;
        memset(data_buf, E2P_EARSED_VAR, sizeof(data_buf));

        e2p_trace("data tail[0x%08x] and info tail[0x%08x] in same sector\n", e2p->p_data, e2p->p_info);
        cfg->flash_read(data_buf, addr, remain_size);
        cfg->flash_erase(addr, size);
        cfg->flash_write(data_buf, addr, sizeof(data_buf));
    }
}

hpm_stat_t e2p_config(e2p_t *e2p)
{
    if (e2p->config.erase_size == 0 || e2p->config.sector_cnt == 0) {
        e2p_info("config error erase_size = %u, sector_cnt = %u\n", e2p->config.erase_size, e2p->config.sector_cnt);
        return E2P_ERROR_INIT_ERR;
    }

    if (e2p->config.flash_read == NULL || e2p->config.flash_write == NULL || e2p->config.flash_erase == NULL) {
        e2p_info("Not register operate function read = %p, write = %p, erase = %p", \
                      e2p->config.flash_read, e2p->config.flash_write, e2p->config.flash_erase);
        return E2P_ERROR_INIT_ERR;
    }

    e2p_block block;
    e2p_config_t *cfg = &e2p->config;

    e2p->p_data = cfg->start_addr;
    e2p->p_info = cfg->start_addr + cfg->sector_cnt * cfg->erase_size - 2 * sizeof(e2p_block);
    e2p->remain_size = e2p->p_info - e2p->p_data;
    memset(e2p_info_table, E2P_EARSED_VAR, sizeof(e2p_info_table));

    cfg->flash_read((uint8_t *)&block, e2p->p_info + sizeof(e2p_block), sizeof(e2p_block));
    e2p_trace("read data, block_id=%x, addr=%x, length=%x, valid_state=%x, crc=%x\n", \
              block.block_id, block.data_addr, block.length, block.valid_state, block.crc);

    if (block.block_id != cfg->version || block.data_addr != E2P_MAGIC_ID) {
        e2p_info("check version failed, begin earse all sector, it will take some time\n");
        cfg->flash_erase(e2p->p_data, cfg->sector_cnt * cfg->erase_size);
        e2p_config_info(e2p);
        e2p_print_info(e2p);
        return E2P_STATUS_OK;
    }

    while (1) {
        block.data_addr = e2p->p_info;
        block.length = sizeof(e2p_block);

        cfg->flash_read((uint8_t *)&block, block.data_addr, block.length);
        if (block.block_id == E2P_EARSED_ID)
            break;

        int ret = e2p_table_update(&block);
        if (E2P_STATUS_OK != ret)
            return ret;

        e2p->p_data += block.length;
        e2p->p_info -= sizeof(e2p_block);
        e2p->remain_size -= (block.length + sizeof(e2p_block));

        if (e2p->remain_size < 2 * sizeof(e2p_block)) {
            e2p_trace("remain flash is not enough\n");
            return E2P_ERROR_NO_MEM;
        }
    }

    e2p_print_info(e2p);
    return E2P_STATUS_OK;
}

hpm_stat_t e2p_flush(e2p_t *e2p, uint8_t flag)
{
    if (e2p->config.erase_size < e2p->remain_size && flag == 0) {
        e2p_trace("no need arrange flash\n");
        return E2P_STATUS_OK;
    }
    e2p_config_t *cfg = &e2p->config;

    e2p_earse_info_sector(e2p);

    e2p->p_data = cfg->start_addr;
    e2p->p_info = cfg->start_addr + cfg->sector_cnt * cfg->erase_size - 2 * sizeof(e2p_block);
    e2p->remain_size = e2p->p_info - e2p->p_data;

    int read_len = 0;
    int count = 0;
    uint32_t head, tail;
    uint8_t read_buf[cfg->erase_size * 2];
    int valid_num = e2p_info_table_sort();

    tail = e2p->p_data;
    for (int i = 0; i < valid_num;) {
        head = tail;

        while (1) {
            if (e2p_info_table[i].block_id == E2P_EARSED_ID || i >= valid_num) {
                e2p_trace("e2p blank[%u], need flush num[%u]\n", i, valid_num);
                break;
            }
            cfg->flash_read(read_buf + read_len, e2p_info_table[i].data_addr, e2p_info_table[i].length);
            read_len += e2p_info_table[i].length;
            tail = e2p_info_table[i].data_addr + e2p_info_table[i].length;
            i++;
            if (read_len >= cfg->erase_size)
                break;
        }

        e2p_trace("---- transfer data to buffer: len=%u\n", read_len);

        do {
            cfg->flash_erase(head, cfg->erase_size);
            head += cfg->erase_size;
        } while (head + cfg->erase_size <= tail);

        tail = head;
        uint8_t *pdata = read_buf;
        while (count < i) {
            if (e2p_info_table[count].block_id == E2P_EARSED_ID || e2p->p_data + e2p_info_table[i].length >= tail) {
                e2p_trace("write back suspend, write stop at 0x%08x/0x0%x\n", e2p->p_data, read_len);
                break;
            }

            e2p_write_private(e2p, e2p_info_table[count].block_id, e2p_info_table[count].length, pdata);
            pdata += e2p_info_table[count].length;
            count++;
        }

        e2p_trace("-----write data back: len=%u\n", pdata - read_buf);

        read_len -= (pdata - read_buf);
        if (read_len)
            memmove(read_buf, pdata, read_len);
    }

    while (tail < e2p->p_info - cfg->erase_size) {
        cfg->flash_erase(tail, cfg->erase_size);
        tail += cfg->erase_size;
    }

    uint8_t *ptr = read_buf;
    while (read_len) {
        e2p_trace("remain write back[%u], block_id[%x], data_addr[%x], length[%u], valid_state[%u], crc[%x]\n", \
            count, e2p_info_table[count].block_id, e2p_info_table[count].data_addr, e2p_info_table[count].length, e2p_info_table[count].valid_state, e2p_info_table[count].crc);
        e2p_write_private(e2p, e2p_info_table[count].block_id, e2p_info_table[count].length, ptr);
        read_len -= e2p_info_table[count].length;
        ptr += e2p_info_table[count].length;
        count++;
    }

    e2p_config_info(e2p);
    return E2P_STATUS_OK;
}

hpm_stat_t e2p_write(e2p_t *e2p, uint32_t block_id, uint16_t length, uint8_t *data)
{
    if (e2p->remain_size < length + sizeof(e2p_block)) {
        e2p_flush(e2p, E2P_FLUSH_BEGIN);
        if (e2p->remain_size < length + sizeof(e2p_block)) {
            e2p_trace("no enough flash write\n");
            return E2P_ERROR_NO_MEM;
        }
    }

    return e2p_write_private(e2p, block_id, length, data);
}

hpm_stat_t e2p_read(e2p_t *e2p, uint32_t block_id, uint16_t length, uint8_t *data)
{
    e2p_block block;
    int ret = 0;

    ret = e2p_retrieve_info(block_id, &block);
    if (ret != E2P_STATUS_OK)
        return ret;

    uint8_t tmp[block.length];
    e2p->config.flash_read(tmp, block.data_addr, block.length);

    if (block.crc != e2p_data_crc_calc(block.length, tmp)) {
        e2p_trace("crc check error, data addr = 0x%08x, crc = 0x%08x", block.data_addr, block.crc);
        return E2P_ERROR;
    }
    
    length > block.length ? (length=block.length) : length;
    memmove(data, tmp, length);
    return E2P_STATUS_OK;
}

void e2p_format(e2p_t *e2p)
{
    e2p_config_t *cfg = &e2p->config;

    cfg->flash_erase(cfg->start_addr, cfg->sector_cnt * cfg->erase_size);
}

uint32_t e2p_generate_id(const char *name)
{
    return (name[0] << 24) | (name[1] << 16) | (name[2] << 8) | (name[3]);
}


void e2p_show_info(e2p_t *e2p)
{
    e2p_print_info(e2p);
}
