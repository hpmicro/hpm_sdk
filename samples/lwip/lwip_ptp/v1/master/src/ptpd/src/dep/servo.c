#include "../ptpd.h"

void initClock(PtpClock *ptpClock)
{
    DBG("initClock\n");

    /* clear vars */
    ptpClock->Tms.seconds = ptpClock->Tms.nanoseconds = 0;
    ptpClock->parentDS.observed_variance = 0;
    ptpClock->parentDS.observed_drift = 0;  /* clears clock servo accumulator */

    /* one way delay */
    ptpClock->servo.owd_filt.s = ptpClock->servo.sDelay;
    ptpClock->servo.owd_filt.n = 0;

    /* offset from master */
    ptpClock->servo.ofm_filt.s = ptpClock->servo.sOffset;
    ptpClock->servo.ofm_filt.n = 0;

    ptpClock->halfEpoch = (Boolean)(ptpClock->halfEpoch || ptpClock->rtOpts->halfEpoch);
    ptpClock->rtOpts->halfEpoch = FALSE;

    /* level clock */

    if (!ptpClock->servo.noAdjust)
        adjFreq(0);

    netEmptyEventQ(&ptpClock->netPath);
}

static Integer32 order(Integer32 n)
{
    if (n < 0) {
        n = -n;
    }
    if (n == 0) {
        return 0;
    }
    return floorLog2(n);
}

/* exponencial smoothing */
static void filter(Integer32 * nsec_current, Filter * filt)
{
    Integer32 s, s2;

    /*
        using floatingpoint math
        alpha = 1/2^s
        y[1] = x[0]
        y[n] = alpha * x[n-1] + (1-alpha) * y[n-1]

        or equivalent with integer math
        y[1] = x[0]
        y_sum[1] = y[1] * 2^s
        y_sum[n] = y_sum[n-1] + x[n-1] - y[n-1]
        y[n] = y_sum[n] / 2^s
    */

    filt->n++; /* increment number of samples */

    /* if it is first time, we are running filter, initialize it*/
    if (filt->n == 1) {
        filt->y_prev = *nsec_current;
        filt->y_sum = *nsec_current;
        filt->s_prev = 0;
    }

    s = filt->s;
    /* speedup filter, if not 2^s > n */
    if ((1<<s) > filt->n) {
        /* lower the filter order */
        s = order(filt->n);
    } else {
        /* avoid overflowing of n */
        filt->n = 1<<s;
    }
    /* avoid overflowing of filter. 30 is because using signed 32bit integers */
    s2 = 30 - order(max(filt->y_prev, *nsec_current));

    /* use the lower filter order, higher will overflow */
    s = min(s, s2);

    /* if the order of the filter changed, change also y_sum value */
    if(filt->s_prev > s) {
      filt->y_sum >>= (filt->s_prev - s);
    } else if (filt->s_prev < s) {
      filt->y_sum <<= (s - filt->s_prev);
    }

    /* compute the filter itself */
    filt->y_sum += *nsec_current - filt->y_prev;
    filt->y_prev = filt->y_sum >> s;

    /* save previous order of the filter */
    filt->s_prev = s;

    DBGV("filter: %d -> %d (%d)\n", *nsec_current, filt->y_prev, s);

    /* actualize target value */
    *nsec_current = filt->y_prev;
}

void updateDelay(PtpClock *ptpClock, TimeInternal *send_time, TimeInternal *recv_time)
{
    TimeInternal Tsm;

    DBG("updateDelay\n");
    DBGVV("updateDelay: rcv: %ds, %dns; snd: %ds, %dns\n",
          recv_time->seconds, recv_time->nanoseconds,
          send_time->seconds, send_time->nanoseconds);

    /* calc 'slave_to_master_delay' */
    subTime(&Tsm, recv_time, send_time);

    /* update 'one_way_delay' */
    addTime(&ptpClock->currentDS.one_way_delay, &ptpClock->Tms, &Tsm);
    div2Time(&ptpClock->currentDS.one_way_delay);

    /* filter delay */

    if (0 != ptpClock->currentDS.one_way_delay.seconds)
    {
        DBGV("updateDelay: cannot filter with seconds\n");
    }
    else
    {
        filter(&ptpClock->currentDS.one_way_delay.nanoseconds, &ptpClock->servo.owd_filt);
    }
}

void updateOffset(PtpClock *ptpClock, TimeInternal *send_time, TimeInternal *recv_time)
{
    DBG("updateOffset\n");

    /* calc 'master_to_slave_delay' */
    subTime(&ptpClock->Tms, recv_time, send_time);

    /* update 'offset_from_master' */
    subTime(&ptpClock->currentDS.offset_from_master, &ptpClock->Tms, &ptpClock->currentDS.one_way_delay);

    /* filter offset */

    if (0 != ptpClock->currentDS.offset_from_master.seconds)
    {
        if (ptpClock->portDS.port_state == PTP_SLAVE)
        {
            setEvent(ptpClock->events, SYNCHRONIZATION_FAULT);
        }

        DBGV("updateOffset: cannot filter with seconds\n");
    }
    else
    {
        filter(&ptpClock->currentDS.offset_from_master.nanoseconds, &ptpClock->servo.ofm_filt);
    }

    /* check results */
    if (abs(ptpClock->currentDS.offset_from_master.nanoseconds) < DEFAULT_CALIBRATED_OFFSET_NS)
    {
        if (ptpClock->portDS.port_state == PTP_UNCALIBRATED)
        {
            setEvent(ptpClock->events, MASTER_CLOCK_SELECTED);
        }
    }
    else if (abs(ptpClock->currentDS.offset_from_master.nanoseconds) > DEFAULT_UNCALIBRATED_OFFSET_NS)
    {
        if (ptpClock->portDS.port_state == PTP_SLAVE)
        {
            setEvent(ptpClock->events, SYNCHRONIZATION_FAULT);
        }
    }
}

void updateClock(PtpClock *ptpClock)
{
    Integer32 adj;
    TimeInternal timeTmp;
    Integer32 offsetNorm;

    DBGV("updateClock\n");

    if (ptpClock->currentDS.offset_from_master.seconds)
    {
        DBG("updateClock seconds\n");
        /* if secs, reset clock or set freq adjustment to max */

        if (!ptpClock->servo.noAdjust)
        {
            if (!ptpClock->servo.noResetClock)
            {
                getTime(&timeTmp);
                subTime(&timeTmp, &timeTmp, &ptpClock->currentDS.offset_from_master);
                setTime(&timeTmp);
                initClock(ptpClock);
            }
            else
            {
                adj = ptpClock->currentDS.offset_from_master.nanoseconds > 0 ? ADJ_FREQ_MAX : -ADJ_FREQ_MAX;
                adjFreq(-adj);
            }
        }
    }
    else
    {
        /* the PI controller */

        /* normalize offset to 1s sync interval -> response of the servo will
         * be same for all sync interval values, but faster/slower
         * (possible lost of precision/overflow but much more stable) */
        offsetNorm = ptpClock->currentDS.offset_from_master.nanoseconds;
        if (ptpClock->defaultDS.sync_interval > 0) {
            offsetNorm >>= ptpClock->defaultDS.sync_interval;
        } else if (ptpClock->defaultDS.sync_interval < 0) {
            offsetNorm <<= -ptpClock->defaultDS.sync_interval;
        }

        /* the accumulator for the I component */
        ptpClock->parentDS.observed_drift += offsetNorm / ptpClock->servo.ai;

        /* clamp the accumulator to ADJ_FREQ_MAX for sanity */

        if (ptpClock->parentDS.observed_drift > ADJ_FREQ_MAX)
            ptpClock->parentDS.observed_drift = ADJ_FREQ_MAX;
        else if (ptpClock->parentDS.observed_drift < -ADJ_FREQ_MAX)
            ptpClock->parentDS.observed_drift = -ADJ_FREQ_MAX;

        /* apply controller output as a clock tick rate adjustment */
        if (!ptpClock->servo.noAdjust)
        {
            adj = offsetNorm / ptpClock->servo.ap + ptpClock->parentDS.observed_drift;
            adjFreq(-adj);
        }
    }

    if (ptpClock->displayStats)
        displayStats(ptpClock);

    DBGV("master-to-slave delay:  %ds %10dns\n",
         ptpClock->Tms.seconds, ptpClock->Tms.nanoseconds);

    DBG("one-way delay:           %ds %10dns\n",
        ptpClock->currentDS.one_way_delay.seconds, ptpClock->currentDS.one_way_delay.nanoseconds);

    DBG("offset from master:      %ds %10dns\n",
        ptpClock->currentDS.offset_from_master.seconds, ptpClock->currentDS.offset_from_master.nanoseconds);

    DBG("observed drift:          %d\n", ptpClock->parentDS.observed_drift);
}
