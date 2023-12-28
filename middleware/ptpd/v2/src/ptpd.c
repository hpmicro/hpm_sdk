/* ptpd.c */

#include "ptpd.h"

RunTimeOpts rtOpts;  /* statically allocated run-time configuration data */
PtpClock ptpClock;
ForeignMasterRecord ptpForeignRecords[DEFAULT_MAX_FOREIGN_RECORDS];

volatile uint32_t PTPTimer = 0;

int ptpd_Init(void)
{
    Integer16 ret;

    /* initialize run-time options to default values */
    rtOpts.announceInterval = DEFAULT_ANNOUNCE_INTERVAL;
    rtOpts.syncInterval = DEFAULT_SYNC_INTERVAL;
    rtOpts.clockQuality.clockAccuracy = DEFAULT_CLOCK_ACCURACY;
    rtOpts.clockQuality.clockClass = DEFAULT_CLOCK_CLASS;
    rtOpts.clockQuality.offsetScaledLogVariance = DEFAULT_CLOCK_VARIANCE; /* 7.6.3.3 */
    rtOpts.priority1 = DEFAULT_PRIORITY1;
    rtOpts.priority2 = DEFAULT_PRIORITY2;
    rtOpts.domainNumber = DEFAULT_DOMAIN_NUMBER;
    rtOpts.slaveOnly = SLAVE_ONLY;
    rtOpts.currentUtcOffset = DEFAULT_UTC_OFFSET;
    rtOpts.servo.noResetClock = DEFAULT_NO_RESET_CLOCK;
    rtOpts.servo.noAdjust = NO_ADJUST;
    rtOpts.inboundLatency.nanoseconds = DEFAULT_INBOUND_LATENCY;
    rtOpts.outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;
    rtOpts.servo.sDelay = DEFAULT_DELAY_S;
    rtOpts.servo.sOffset = DEFAULT_OFFSET_S;
    rtOpts.servo.ap = DEFAULT_AP;
    rtOpts.servo.ai = DEFAULT_AI;
    rtOpts.maxForeignRecords = sizeof(ptpForeignRecords) / sizeof(ptpForeignRecords[0]);
    rtOpts.stats = PTP_TEXT_STATS;
    rtOpts.delayMechanism = DEFAULT_DELAY_MECHANISM;

    /* Initialize run time options with command line arguments*/
    ret = ptpdStartup(&ptpClock, &rtOpts, ptpForeignRecords);

    if (ret != 0)
        return ret;

    return 0;
}

/* loop forever. doState() has a switch for the actions and events to be
   checked for 'port_state'. the actions and events may or may not change
   'port_state' by calling toState(), but once they are done we loop around
   again and perform the actions required for the new 'port_state'. */
void ptpd_periodic_handle(__IO UInteger32 localtime)
{

    static UInteger32 old_localtime = 0;

    catchAlarm(localtime - old_localtime);
    old_localtime = localtime;

    /* at least once run stack */

    do
    {
        doState(&ptpClock);
        /* if there are still some packets - run stack again */
    }
    while (netSelect(&ptpClock.netPath, 0) > 0);
}

