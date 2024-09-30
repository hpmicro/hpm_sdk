/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file tsn_hal_utils.c
 * \author fra11385
 * \date 06.07.2021
 * \brief <insert file brief description>
 *
 */

#include <hal_common/tsn_hal_utils.h>

void mac_hilo2int(uint32_t machi, uint32_t maclo, uint8_t *piMac)
{
    //maclo
    piMac[0] = (maclo) & 0x000000FF;
    piMac[1] = (maclo >> 8) & 0x000000FF;
    piMac[2] = (maclo >> 16) & 0x000000FF;
    piMac[3] = (maclo >> 24) & 0x000000FF;
    //machi
    piMac[4] = (machi) & 0x000000FF;
    piMac[5] = (machi >> 8) & 0x000000FF;
}
