/* sys.c */

#include "../ptpd.h"

void displayStats(const PtpClock *ptpClock)
{
}

void getTime(TimeInternal *time)
{
    enet_ptp_ts_system_t timestamp;

    enet_get_ptp_timestamp(ENET, &timestamp);
    time->seconds = timestamp.sec;
    time->nanoseconds = timestamp.nsec;
}

void setTime(const TimeInternal *time)
{
    enet_ptp_ts_update_t timestamp;

    timestamp.sec = time->seconds;
    timestamp.nsec = time->nanoseconds;

    enet_set_ptp_timestamp(ENET, &timestamp);
    DBG("setTime: resetting system clock to %ds %dns\n", time->seconds, time->nanoseconds);
}

void updateTime(const TimeInternal *time)
{
    enet_ptp_ts_update_t timeoffset;

    DBGV("updateTime: %ds %dns\n", time->seconds, time->nanoseconds);

    timeoffset.sec = -time->seconds;
    timeoffset.nsec = -time->nanoseconds;

    if (-time->seconds < 0 || (-time->seconds == 0 && -time->nanoseconds < 0)) {
        timeoffset.sign = 1;
        timeoffset.sec = time->seconds;
        timeoffset.nsec = time->nanoseconds;
    } else {
        timeoffset.sign = 0;
        timeoffset.sec = -time->seconds;
        timeoffset.nsec = -time->nanoseconds;
    }

    /* Coarse update method */
    enet_update_ptp_timeoffset(ENET, &timeoffset);
    DBGV("%s: updated\n", __FUNCTION__);
}

UInteger32 getRand(UInteger32 randMax)
{
    return rand() % randMax;
}

Boolean adjFreq(Integer32 adj)
{
    DBGV("adjFreq %d\n", adj);

    if (adj > ADJ_FREQ_MAX)
        adj = ADJ_FREQ_MAX;
    else if (adj < -ADJ_FREQ_MAX)
        adj = -ADJ_FREQ_MAX;

    /* Fine update method */
    enet_adjust_ptp_time_freq(ENET, adj);
    DBGV("%s: updated\n", __FUNCTION__);
    return TRUE;
}
