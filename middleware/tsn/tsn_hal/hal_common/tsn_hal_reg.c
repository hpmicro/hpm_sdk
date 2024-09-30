/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file tsn_hal_reg.c
 * \author fra11385
 * \date 06.07.2021
 * \brief <insert file brief description>
 *
 */

#include <hal_common/tsn_hal_reg.h>

extern inline void rreg(uint32_t addr, void *variable);
extern inline void wreg(uint32_t addr, void *variable);
extern inline void rregl(uint32_t addr, void *variable, const char *ctx, char *regname);
extern inline void wregl(uint32_t addr, void *variable, const char *ctx, char *regname);
