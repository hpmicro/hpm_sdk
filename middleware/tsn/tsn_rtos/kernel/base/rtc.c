/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   rtc.c
 * \author zimmerli
 * \date   2020-01-15
 * \brief  Real-Time-Clock generic driver
 *
 */

#include "rtc.h"
#include <kernel/base/kernel.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"

// ----------------------------------------------------------------
static uint32_t _rtc_dev_cnt = 0;
struct rtc_s *default_rtc = NULL;

// ----------------------------------------------------------------
/**
 * \brief Allocate and register RTC device driver
 *
 * @return  pointer to RTC device driver structure, NULL on error
 */
struct rtc_s *rtc_register(void)
{
    struct rtc_s *p;

    // alloc struct
    p = (struct rtc_s *)kallocz(sizeof(struct rtc_s));
    if (!p)
        return NULL;

    //set first rtc as default
    if (!default_rtc && _rtc_dev_cnt == 0)
        default_rtc = p;

    // fill fields
    p->id = _rtc_dev_cnt;
    _rtc_dev_cnt++;

    return p;
}

struct rtc_s *rtc_default(void)
{
    return default_rtc;
}

/**
 * \brief Get time of local clock
 *
 * @param rtc           pointer to device driver
 * @param localtime     pointer to timespec, result
 */
void rtc_get_local_time64(struct rtc_s *rtc, struct timespec64 *localtime)
{
    if (!rtc || !rtc->getLocalTime64)
        return;

    rtc->getLocalTime64(rtc, localtime);
}

/**
 * \brief Get RTC offset
 *
 * @param rtc       pointer to device driver
 * @param offset    pointer to timespec, resulting offset
 */
void rtc_get_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset)
{
    if (!rtc || !rtc->getRtcOffset)
        return;

    rtc->getRtcOffset(rtc, offset);
}

/**
 * \brief Set RTC offset
 *
 * @param rtc       pointer to device driver
 * @param offset    pointer to timespec, offset to be set
 */
void rtc_set_rtc_offset(struct rtc_s *rtc, struct timespec64 *offset)
{
    if (!rtc || !rtc->setRtcOffset)
        return;

    rtc->setRtcOffset(rtc, offset);
}

/**
 * \brief Fine adjust nominal frequency of RTC.
 *
 * @param rtc       pointer to device driver
 * @param ppb       frequency adjustment in parts-per-billion
 */
void rtc_adjfine(struct rtc_s *rtc, int64_t ppb)
{
    if (!rtc || !rtc->adjFine)
        return;

    rtc->adjFine(rtc, ppb);
}
