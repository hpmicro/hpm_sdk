/* startup.c */

#include "../ptpd.h"

void ptpdShutdown(PtpClock *ptpClock)
{
    netShutdown(&ptpClock->netPath);
}

Integer16 ptpdStartup(PtpClock *ptpClock, RunTimeOpts *rtOpts, ForeignMasterRecord* foreign)
{
    ptpClock->rtOpts = rtOpts;

    ptpClock->foreignMasterDS.records = foreign;

    if (rtOpts->slaveOnly)
        rtOpts->clockStratum = 255;

    /* no negative or zero attenuation */
    if (rtOpts->ap < 1)
        rtOpts->ap = 1;

    if (rtOpts->ai < 1)
        rtOpts->ai = 1;

    DBG("event POWER UP\n");

    toState(ptpClock, PTP_INITIALIZING);
    doState(ptpClock);
	toState(ptpClock, rtOpts->slaveOnly ? PTP_SLAVE: PTP_MASTER);
    doState(ptpClock);

    return 0;
}