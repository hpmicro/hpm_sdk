/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file tsn_hal_reg.h
 * \author fra11385
 * \date 06.07.2021
 * \brief <insert file brief description>
 *
 */

#ifndef TSN_HAL_REG_H_
#define TSN_HAL_REG_H_

#include <stdint.h>

#include <tsn_hal_port.h>

/**
 * Write value to register.
 * @param addr  	address
 * @param var		pointer from value source
 */
static inline void wreg(uint32_t addr, void *var)
{
    MEM32(addr) = *((uint32_t *)var);
}

/**
 * Read value from register.
 * @param addr  	address
 * @param var		pointer to value destination
 */
static inline void rreg(uint32_t addr, void *var)
{
    *((uint32_t *)(var)) = MEM32(addr);
}

/**
 * Write value to register with logging.
 * @param addr  	address
 * @param var		pointer from value source
 * @param ctx		context information for logging
 * @param regname	register name to print for logging
 */
static inline void wregl(uint32_t addr, void *var, const char *ctx, char *regname)
{
    logk(LOG_REGISTER, ctx, "write to   reg %-15s @ 0x%04X%04X = 0x%04X%04X\n", regname, ((addr) >> 16), ((addr) & 0xFFFF), ((*((uint32_t *)var)) >> 16),
         ((*((uint32_t *)var)) & 0xFFFF));
    MEM32(addr) = *((uint32_t *)var);
}

/**
 * Read value from register with logging.
 * @param addr  	address
 * @param var		pointer to value destination
 * @param ctx		context information for logging
 * @param regname	register name to print for logging
 */
static inline void rregl(uint32_t addr, void *var, const char *ctx, char *regname)
{
    *((uint32_t *)(var)) = MEM32(addr);
    logk(LOG_REGISTER, ctx, "read  from reg %-15s @ 0x%04X%04X = 0x%04X%04X\n", regname, ((addr) >> 16), ((addr) & 0xFFFF), ((*((uint32_t *)var)) >> 16),
         ((*((uint32_t *)var)) & 0xFFFF));
}

#endif /* TSN_HAL_REG_H_ */
