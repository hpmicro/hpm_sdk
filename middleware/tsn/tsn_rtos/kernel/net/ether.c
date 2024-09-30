/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file ether.c
 * \author fra11385
 * \date 21.09.2021
 * \brief <insert file brief description>
 *
 */

#include "kernel/net/ether.h"

const char *ETH_PRIO_NAMES[8] = { "Background", "Best Effort", "Excellent Effort",     "Critical Application",
                                  "Video",      "Voice",       "Internetwork Control", "Network Control" };
