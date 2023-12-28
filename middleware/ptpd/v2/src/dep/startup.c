/* startup.c */

#include "../ptpd.h"

void ptpdShutdown(PtpClock *ptpClock)
{
    netShutdown(&ptpClock->netPath);
}

Integer16 ptpdStartup(PtpClock * ptpClock, RunTimeOpts *rtOpts, ForeignMasterRecord* foreign)
{
    ptpClock->rtOpts = rtOpts;

    ptpClock->foreignMasterDS.records = foreign;

    /* 9.2.2 */

    if (rtOpts->slaveOnly)
        rtOpts->clockQuality.clockClass = DEFAULT_CLOCK_CLASS_SLAVE_ONLY;

    /* no negative or zero attenuation */
    if (rtOpts->servo.ap < 1)
        rtOpts->servo.ap = 1;

    if (rtOpts->servo.ai < 1)
        rtOpts->servo.ai = 1;

    DBG("event POWER UP\n");

    toState(ptpClock, PTP_INITIALIZING);
    doState(ptpClock);
    toState(ptpClock, rtOpts->slaveOnly ? PTP_SLAVE: PTP_MASTER);
    doState(ptpClock);

    return 0;
}