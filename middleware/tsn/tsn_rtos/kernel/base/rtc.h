/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   rtc.h
 * \author zimmerli
 * \date   2020-01-13
 * \brief  Real-Time-Clock generic driver
 *
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>

#include <kernel/base/time64.h>

/**
 * \brief RTC device driver
 *
 * RTC has local time.
 * Real-Time is local time plus real-time-offset.
 */
struct rtc_s {
    uint32_t id;                                                             //!< RTC device driver id
    void *port;                                                              //!< pointer to tsn ep device driver
    void *prv;                                                               //!< pointer to private data
    void (*getLocalTime64)(struct rtc_s *rtc, struct timespec64 *localtime); //!< function pointer, get local time
    void (*getRtcOffset)(struct rtc_s *rtc, struct timespec64 *offset);      //!< function pointer, get rtc offset
    void (*setRtcOffset)(struct rtc_s *rtc, struct timespec64 *offset);      //!< function pointer, set rtc offset
    void (*adjFine)(struct rtc_s *rtc, int64_t ppb);                         //!< function pointer, fine adjust nominal frequency
};

struct rtc_s *rtc_register(void);
struct rtc_s *rtc_default(void);
void rtc_get_local_time64(struct rtc_s *rtc, struct timespec64 *localtime);
void rtc_get_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset);
void rtc_set_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset);
void rtc_adjfine(struct rtc_s *rtc, int64_t ppb);

#endif /* RTC_H_ */
