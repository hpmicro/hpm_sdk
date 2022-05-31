/* ptpd.c */

#include "ptpd.h"

RunTimeOpts rtOpts;  /* statically allocated run-time configuration data */
PtpClock ptpClock;
ForeignMasterRecord ptpForeignRecords[DEFUALT_MAX_FOREIGN_RECORDS];

volatile uint32_t PTPTimer = 0;

int ptpd_Init(void)
{
    Integer16 ret;

    /* initialize run-time options to default values */
    rtOpts.syncInterval = DEFAULT_SYNC_INTERVAL;
    memcpy(rtOpts.subdomainName, DEFAULT_PTP_DOMAIN_NAME, PTP_SUBDOMAIN_NAME_LENGTH);
    memcpy(rtOpts.clockIdentifier, IDENTIFIER_DFLT, PTP_CODE_STRING_LENGTH);
    rtOpts.clockVariance = DEFAULT_CLOCK_VARIANCE;
    rtOpts.clockStratum = DEFAULT_CLOCK_STRATUM;
    rtOpts.unicastAddress[0] = 0;
    rtOpts.inboundLatency.nanoseconds = DEFAULT_INBOUND_LATENCY;
    rtOpts.outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;
    rtOpts.noResetClock = DEFAULT_NO_RESET_CLOCK;
    rtOpts.sDelay = DEFAULT_DELAY_S;
    rtOpts.sOffset = DEFAULT_OFFSET_S;
    rtOpts.ap = DEFAULT_AP;
    rtOpts.ai = DEFAULT_AI;
    rtOpts.max_foreign_records = sizeof(ptpForeignRecords) / sizeof(ForeignMasterRecord);
    rtOpts.currentUtcOffset = DEFAULT_UTC_OFFSET;
    rtOpts.clockPreferred = FALSE;
    rtOpts.slaveOnly = TRUE;
    rtOpts.displayStats = TRUE;

    /* Initialize run time options with command line arguments*/
    ret = ptpdStartup(&ptpClock, &rtOpts, ptpForeignRecords);

    if (ret != 0)
        return ret;

    return 0;
}


/* doState() has a switch for the actions and events to be
   checked for 'port_state'. the actions and events may or may not change
   'port_state' by calling toState(), but once they are done we loop around
   again and perform the actions required for the new 'port_state'. */
void ptpd_periodic_handle(__IO UInteger32 localtime)
{
    static UInteger32 old_localtime = 0;

    catchAlarm(localtime - old_localtime);
    old_localtime = localtime;

    do
    {
        doState(&ptpClock);
    }
    while (netSelect(&ptpClock.netPath, 0) > 0);
}
