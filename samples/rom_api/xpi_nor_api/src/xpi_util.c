/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "xpi_util.h"


ATTR_RAMFUNC void xpi_instr_seq_init(xpi_instr_seq_ctx_t *ctx)
{
    (void) memset(ctx, 0, sizeof((*ctx)));
}

ATTR_RAMFUNC void xpi_instr_seq_add_cmd_phase(xpi_instr_seq_ctx_t *ctx, uint8_t cmd, uint8_t pads, bool is_ddr)
{
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t cmd_phase = is_ddr ? XPI_PHASE_CMD_DDR : XPI_PHASE_CMD_SDR;
    uint16_t cmd_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(cmd_phase, cmd_pads, cmd);
}

ATTR_RAMFUNC void xpi_instr_seq_add_addr_phase(xpi_instr_seq_ctx_t *ctx, uint8_t addr_bits, uint8_t pads, bool is_ddr)
{
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t addr_phase = is_ddr ? XPI_PHASE_RADDR_DDR : XPI_PHASE_RADDR_SDR;
    uint16_t addr_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(addr_phase, addr_pads, addr_bits);
}

ATTR_RAMFUNC void xpi_instr_seq_add_row_addr_phase(xpi_instr_seq_ctx_t *ctx, uint8_t addr_bits, uint8_t pads, bool is_ddr)
{
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t addr_phase = is_ddr ? XPI_PHASE_RADDR_DDR : XPI_PHASE_RADDR_SDR;
    uint16_t addr_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(addr_phase, addr_pads, addr_bits);
}

ATTR_RAMFUNC void xpi_instr_seq_add_column_addr_phase(xpi_instr_seq_ctx_t *ctx, uint8_t addr_bits, uint8_t pads, bool is_ddr)
{
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t addr_phase = is_ddr ? XPI_PHASE_CADDR_DDR : XPI_PHASE_CADDR_SDR;
    uint16_t addr_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(addr_phase, addr_pads, addr_bits);
}

ATTR_RAMFUNC void xpi_instr_seq_add_dummy_phase(xpi_instr_seq_ctx_t *ctx, uint8_t dummy_cycles, uint8_t pads, bool is_ddr)
{
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t dummy_phase = is_ddr ? XPI_PHASE_DUMMY_DDR : XPI_PHASE_DUMMY_SDR;
    uint16_t dummy_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(dummy_phase, dummy_pads, dummy_cycles);
}

ATTR_RAMFUNC void xpi_instr_seq_add_mode_phase(xpi_instr_seq_ctx_t *ctx, uint8_t mode_opt, uint32_t mode_bits, uint8_t pads, bool is_ddr)
{
    uint32_t mode_instr = (mode_bits == 8) ? (is_ddr ? XPI_PHASE_MODE8_DDR : XPI_PHASE_MODE8_SDR) : (is_ddr ? XPI_PHASE_MODE4_DDR : XPI_PHASE_MODE4_SDR);
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t mode_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(mode_instr, mode_pads, mode_opt);
}

ATTR_RAMFUNC void xpi_instr_seq_add_dummy_rwds_phase(xpi_instr_seq_ctx_t *ctx, uint8_t dummy_rwds_cycles, uint8_t pads, bool is_ddr)
{
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t dummy_phase = is_ddr ? XPI_PHASE_DUMMY_RWDS_DDR : XPI_PHASE_DUMMY_RWDS_SDR;
    uint16_t dummy_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(dummy_phase, dummy_pads, dummy_rwds_cycles);
}

ATTR_RAMFUNC void xpi_instr_seq_add_write_phase(xpi_instr_seq_ctx_t *ctx, uint8_t pads, bool is_ddr)
{
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t write_phase = is_ddr ? XPI_PHASE_WRITE_DDR : XPI_PHASE_WRITE_SDR;
    uint16_t write_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(write_phase, write_pads, 4);
}

ATTR_RAMFUNC void xpi_instr_seq_add_read_phase(xpi_instr_seq_ctx_t *ctx, uint8_t pads, bool is_ddr)
{
    uint16_t *sub_instr = (uint16_t *) &ctx->seq.entry[0];
    uint16_t read_phase = is_ddr ? XPI_PHASE_READ_DDR : XPI_PHASE_READ_SDR;
    uint16_t read_pads = (pads == 8) ? XPI_8PADS : ((pads == 4) ? XPI_4PADS : ((pads == 2) ? XPI_2PADS : XPI_1PAD));
    sub_instr[ctx->phase_idx++] = SUB_INSTR(read_phase, read_pads, 4);
}

ATTR_RAMFUNC hpm_stat_t xpi_send_cmd(XPI_Type *base,
                                     xpi_xfer_channel_t chn,
                                     const xpi_instr_seq_ctx_t *cmd_ctx,
                                     uint32_t addr)
{
    xpi_xfer_ctx_t xfer_ctx = { 0 };
    xfer_ctx.cmd_type = xpi_apb_xfer_type_cmd;
    xfer_ctx.addr = addr;
    xfer_ctx.channel = chn;
    xfer_ctx.seq_idx = 1;
    xfer_ctx.seq_num = 1;
    (void) ROM_API_TABLE_ROOT->xpi_driver_if->update_instr_table(base, cmd_ctx->seq.entry, 1, 1);
    return ROM_API_TABLE_ROOT->xpi_driver_if->transfer_blocking(base, &xfer_ctx);
}

ATTR_RAMFUNC hpm_stat_t xpi_write_data(XPI_Type *base,
                                       xpi_xfer_channel_t chn,
                                       const xpi_instr_seq_ctx_t *cmd_ctx,
                                       uint32_t offset,
                                       const void *buf,
                                       uint16_t bytes_to_write)
{
    xpi_xfer_ctx_t xfer_ctx = { 0 };
    xfer_ctx.cmd_type = xpi_apb_xfer_type_write;
    xfer_ctx.addr = offset;
    xfer_ctx.channel = chn;
    xfer_ctx.seq_idx = 1;
    xfer_ctx.seq_num = 1;
    xfer_ctx.buf = (uint32_t *) buf;
    xfer_ctx.xfer_size = bytes_to_write;
    (void) ROM_API_TABLE_ROOT->xpi_driver_if->update_instr_table(base, cmd_ctx->seq.entry, 1, 1);
    return ROM_API_TABLE_ROOT->xpi_driver_if->transfer_blocking(base, &xfer_ctx);
}

ATTR_RAMFUNC hpm_stat_t xpi_read_data(XPI_Type *base,
                                      xpi_xfer_channel_t chn,
                                      const xpi_instr_seq_ctx_t *cmd_ctx,
                                      uint32_t offset,
                                      void *buf,
                                      uint16_t bytes_to_read)
{
    xpi_xfer_ctx_t xfer_ctx = { 0 };
    xfer_ctx.cmd_type = xpi_apb_xfer_type_read;
    xfer_ctx.addr = offset;
    xfer_ctx.channel = chn;
    xfer_ctx.seq_idx = 1;
    xfer_ctx.seq_num = 1;
    xfer_ctx.buf = (uint32_t *) buf;
    xfer_ctx.xfer_size = bytes_to_read;
    (void) ROM_API_TABLE_ROOT->xpi_driver_if->update_instr_table(base, cmd_ctx->seq.entry, 1, 1);
    return ROM_API_TABLE_ROOT->xpi_driver_if->transfer_blocking(base, &xfer_ctx);
}