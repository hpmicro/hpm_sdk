/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file tsn_hal_utils.h
 * \author fra11385
 * \date 06.07.2021
 * \brief <insert file brief description>
 *
 */

#ifndef TSN_HAL_UTILS_H_
#define TSN_HAL_UTILS_H_

#include <stdint.h>

#define TSN_LL_RET_OK          0 //!< low-level function return value: ok
#define TSN_LL_RET_FAIL        1 //!< low-level function return value: fail (general error)
#define TSN_LL_RET_UNSUPPORTED 2 //!< low-level function return value: funktion unsupported
#define TSN_LL_RET_INVALIDDATA 3 //!< low-level function return value: invalid data

#define MAC_LO(mac) (uint32_t)(mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24))
#define MAC_HI(mac) (uint32_t)(mac[4] | (mac[5] << 8))

void mac_hilo2int(uint32_t machi, uint32_t maclo, uint8_t *piMac);

#define TO_UINT32(struct_var) (*((uint32_t *)(&struct_var)))

#endif /* TSN_HAL_UTILS_H_ */
