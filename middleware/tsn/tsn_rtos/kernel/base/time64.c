/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   time64.c
 * \author zimmerli
 * \date   2020-01-15
 * \brief  Time64
 *
 */

#include "time64.h"

/**
 * \brief normalize time, resulting nanoseconds in range 0 .. 10^9-1
 *
 * @param ts    pointer to timespec64, destination of normalized time
 * @param sec   input seconds
 * @param nsec  input nanoseconds, can be out of range 0 .. 10^9-1
 */
void set_normalized_timespec64(struct timespec64 *ts, int64_t sec, int64_t nsec)
{
    while (nsec >= NSEC_PER_SEC) {
        nsec -= NSEC_PER_SEC;
        ++sec;
    }
    while (nsec < 0) {
        nsec += NSEC_PER_SEC;
        --sec;
    }

    ts->tv_sec = sec;
    ts->tv_nsec = nsec;
}
