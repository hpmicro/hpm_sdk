/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   time64.h
 * \author zimmerli
 * \date   2020-01-15
 * \brief  Time64
 *
 */

#ifndef TIME64_H_
#define TIME64_H_

#include <stdint.h>

/**
 * \brief Time format
 */
struct timespec64 {
    int64_t tv_sec;   //!< seconds, signed
    uint32_t tv_nsec; //!< nanoseconds, 0 .. 10^9-1
};

#define MSEC_PER_SEC 1000L
#define USEC_PER_SEC 1000000L
#define NSEC_PER_SEC 1000000000LL

extern void set_normalized_timespec64(struct timespec64 *ts, int64_t sec, int64_t nsec);

static inline struct timespec64 timespec64_add(struct timespec64 lhs, struct timespec64 rhs)
{
    struct timespec64 ts_delta;
    set_normalized_timespec64(&ts_delta, lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
    return ts_delta;
}

/**
 * \brief subtract timespec, normalize result
 *
 * @param lhs   left hand side of operation
 * @param rhs   right hand side of operation
 * @return      normalized timespec
 */
static inline struct timespec64 timespec64_sub(struct timespec64 lhs, struct timespec64 rhs)
{
    struct timespec64 ts_delta;
    set_normalized_timespec64(&ts_delta, lhs.tv_sec - rhs.tv_sec, (int64_t)lhs.tv_nsec - (int64_t)rhs.tv_nsec);
    return ts_delta;
}

/**
 * \brief: Convert timespec to nanoseconds
 *
 * @param ts    pointer to timespec
 * @return      scalar nanoseconds
 */
static inline int64_t timespec64_to_ns(const struct timespec64 *ts)
{
    return ((int64_t)ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

#endif /* TIME64_H_ */
