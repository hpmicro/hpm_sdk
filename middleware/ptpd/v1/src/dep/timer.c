/* timer.c */

#include "../ptpd.h"

unsigned int elapsed_ms;

void catchAlarm(UInteger32 time_ms)
{
    elapsed_ms += time_ms;
    DBGVV("catch_alarm: elapsed %d\n", elapsed_ms);
}

void initTimer(void)
{
    DBG("initTimer\n");
    elapsed_ms = 0;
}

void timerUpdate(IntervalTimer *itimer)
{

    int i, delta;

    delta = elapsed_ms;

    if (delta <= 0)
        return;

    for (i = 0; i < TIMER_ARRAY_SIZE; ++i)
    {
        if (i == 1) {
            DBGV("%d\n", itimer[i].left);
            i = 1;
        }
        if ((itimer[i].interval) > 0 && ((itimer[i].left) -= delta) <= 0)
        {
            itimer[i].left = itimer[i].interval;
            itimer[i].expire = TRUE;
            DBGV("timerUpdate: timer %u expired\n", i);
        }
    }

    elapsed_ms = 0;
}

void timerStop(UInteger16 index, IntervalTimer *itimer)
{
    if (index >= TIMER_ARRAY_SIZE)
        return;

    itimer[index].interval = 0;
}

void timerStart(UInteger16 index, UInteger32 interval_ms, IntervalTimer *itimer)
{
    if (index >= TIMER_ARRAY_SIZE)
        return;

    itimer[index].expire = FALSE;

    itimer[index].left = interval_ms;

    itimer[index].interval = itimer[index].left;

    DBGV("timerStart: set timer %d to %d\n", index, interval_ms);
}

Boolean timerExpired(UInteger16 index, IntervalTimer *itimer)
{
    timerUpdate(itimer);

    if (index >= TIMER_ARRAY_SIZE)
        return FALSE;

    if (!itimer[index].expire)
        return FALSE;

    itimer[index].expire = FALSE;

    return TRUE;
}
