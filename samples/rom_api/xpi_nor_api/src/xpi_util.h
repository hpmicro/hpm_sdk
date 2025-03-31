/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef XPI_UTIL_H
#define XPI_UTIL_H

#include "hpm_romapi.h"
#include "hpm_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Instruction Sequence context
 */
typedef struct {
    uint32_t phase_idx; /*!< Phase index  */
    xpi_instr_seq_t seq; /*!< Instruction sequence entry */
} xpi_instr_seq_ctx_t;

/**
 * @brief Initialize the Instruction Sequence Context
 * @param [in, out] ctx Command Sequence Context
 */
ATTR_RAMFUNC void xpi_instr_seq_init(xpi_instr_seq_ctx_t *ctx);

/**
 * @brief Add the Command Phase to Instruction Sequence
 * @param [in,out] ctx Command Sequence Context
 * @param [in] cmd Command supported by the device (for example: Serial NOR FLASH, SPI ADC, etc)
 * @param [in] pads Pads used to send the CMD to device (valid value: 1/2/4/8)
 * @param [in] is_ddr Determine whether the command should be sent out in DDR mode
 */
ATTR_RAMFUNC void xpi_instr_seq_add_cmd_phase(xpi_instr_seq_ctx_t *ctx, uint8_t cmd, uint8_t pads, bool is_ddr);

/**
 * @brief Add the Address Phase to Instruction Sequence
 * @param [in,out] ctx Command Sequence Context
 * @param [in] addr_bits Address bits (for example, 24-bit / 32-bit)
 * @param [in] pads Pads used to send the address to device (valid value: 1/2/4/8)
 * @param [in] is_ddr Determine whether the address should be sent out in DDR mode
 */
ATTR_RAMFUNC void xpi_instr_seq_add_addr_phase(xpi_instr_seq_ctx_t *ctx, uint8_t addr_bits, uint8_t pads, bool is_ddr);


/**
 * @brief Add the Row Address Phase to Instruction Sequence
 * @param [in,out] ctx Command Sequence Context
 * @param [in] addr_bits Row Address bits (for example, 24-bit / 32-bit)
 * @param [in] pads Pads used to send the address to device (valid value: 1/2/4/8)
 * @param [in] is_ddr Determine whether the address should be sent out in DDR mode
 */
ATTR_RAMFUNC void xpi_instr_seq_add_row_addr_phase(xpi_instr_seq_ctx_t *ctx, uint8_t addr_bits, uint8_t pads, bool is_ddr);

/**
 * @brief Add the Dummy Phase to Instruction Sequence
 * @param [in,out] ctx Command Sequence Context
 * @param [in] dummy_cycles Dummy cycles (for example, 4/6/8)
 * @param [in] pads Pads used to send the dummy to device (valid value: 1/2/4/8)
 * @param [in] is_ddr Determine whether the dummy should be sent out in DDR mode
 */
ATTR_RAMFUNC void xpi_instr_seq_add_dummy_phase(xpi_instr_seq_ctx_t *ctx, uint8_t dummy_cycles, uint8_t pads, bool is_ddr);


/**
 * @brief Add the Mode Phase to Instruction Sequence
 * @param [in,out] ctx Command Sequence Context
 * @param [in] mode_opt mode option
 * @param [in] mode_bits bits of mode option
 * @param [in] pads Pads used to send the dummy to device (valid value: 1/2/4/8)
 * @param [in] is_ddr Determine whether the dummy should be sent out in DDR mode
 */
ATTR_RAMFUNC void xpi_instr_seq_add_mode_phase(xpi_instr_seq_ctx_t *ctx, uint8_t mode_opt, uint32_t mode_bits, uint8_t pads, bool is_ddr);

/**
 * @brief Add the DUMMY RWDS Phase to Instruction Sequence
 * @param [in,out] ctx Command Sequence Context
 * @param [in] dummy_rwds_cycles DUMMY RWDS cycles (for example, 4/6/8)
 * @param [in] pads Pads used to send the dummy to device (valid value: 1/2/4/8)
 * @param [in] is_ddr Determine whether the dummy should be sent out in DDR mode
 */
ATTR_RAMFUNC void xpi_instr_seq_add_dummy_rwds_phase(xpi_instr_seq_ctx_t *ctx, uint8_t dummy_rwds_cycles, uint8_t pads, bool is_ddr);

/**
 * @brief Add the Write Phase to Instruction Sequence
 * @param [in,out] ctx Command Sequence Context
 * @param [in] pads Pads used to send the data to device (valid value: 1/2/4/8)
 * @param [in] is_ddr Determine whether the data should be sent out in DDR mode
 */
ATTR_RAMFUNC void xpi_instr_seq_add_write_phase(xpi_instr_seq_ctx_t *ctx, uint8_t pads, bool is_ddr);

/**
 * @brief Add the Read Phase to Instruction Sequence
 * @param [in,out] ctx Command Sequence Context
 * @param [in] pads Pads used to read the data from device (valid value: 1/2/4/8)
 * @param [in] is_ddr Determine whether the data should be read out in DDR mode
 */
ATTR_RAMFUNC void xpi_instr_seq_add_read_phase(xpi_instr_seq_ctx_t *ctx, uint8_t pads, bool is_ddr);

/**
 * @brief Send command to Device
 * @param [in] base XPI base address
 * @param [in] chn XPI channel
 * @param [in] cmd_ctx XPI NOR command context
 * @param [in] addr Flash address
 *
 * @return status after issuing command to device
 */
ATTR_RAMFUNC hpm_stat_t xpi_send_cmd(XPI_Type *base,
                                     xpi_xfer_channel_t chn,
                                     const xpi_instr_seq_ctx_t *cmd_ctx,
                                     uint32_t addr);


/**
 * @brief Write data to Device
 * @param [in] base XPI base address
 * @param [in] chn XPI channel
 * @param [in] cmd_ctx XPI NOR command context
 * @param [in] offset The device offset that the data will be written to
 * @param [in] buf buffer that hold the data to be written
 * @param [in] bytes_to_write size for the data to be written
 *
 * @return Command execution status
 */
ATTR_RAMFUNC hpm_stat_t xpi_write_data(XPI_Type *base,
                                       xpi_xfer_channel_t chn,
                                       const xpi_instr_seq_ctx_t *cmd_ctx,
                                       uint32_t offset,
                                       const void *buf,
                                       uint16_t bytes_to_write);

/**
 * @brief Read out data from Device
 * @param [in] base XPI base address
 * @param [in] chn XPI channel
 * @param [in] cmd_ctx XPI NOR command context
 * @param [in] offset The device offset that the data will be written to
 * @param [in] buf buffer that hold the data read out from device
 * @param [in] bytes_to_write size for the data to be read out
 *
 * @return Command execution status
 */
ATTR_RAMFUNC hpm_stat_t xpi_read_data(XPI_Type *base,
                                      xpi_xfer_channel_t chn,
                                      const xpi_instr_seq_ctx_t *cmd_ctx,
                                      uint32_t offset,
                                      void *buf,
                                      uint16_t bytes_to_read);

#ifdef __cplusplus
}
#endif


#endif /* XPI_UTIL_H */