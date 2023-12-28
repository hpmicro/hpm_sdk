/* ptpd_dep.h */

#ifndef PTPD_DEP_H
#define PTPD_DEP_H

/* system messages */
#ifdef PTPD_ERR
#define ERROR(...)  { printf("(E) "); printf(__VA_ARGS__); }
#else
#define ERROR(...)
#endif

/* debug messages */
#ifdef PTPD_DBGVV
#define PTPD_DBGV
#define PTPD_DBG
#define PTPD_ERR
#define DBGVV(...) printf("(V) " __VA_ARGS__)
#else
#define DBGVV(...)
#endif


#ifdef PTPD_DBGV
#define PTPD_DBG
#define PTPD_ERR
#define DBGV(...)  { TimeInternal tmpTime; getTime(&tmpTime); printf("(d %d.%09d) ", tmpTime.seconds, tmpTime.nanoseconds); printf(__VA_ARGS__); }
#else
#define DBGV(...)
#endif

#ifdef PTPD_DBG
#define PTPD_ERR
#define DBG(...)  { TimeInternal tmpTime; getTime(&tmpTime); printf("(D %d.%09d) ", tmpTime.seconds, tmpTime.nanoseconds); printf(__VA_ARGS__); }
#else
#define DBG(...)
#endif


/* endian corrections */
#if defined(PTPD_MSBF)
#define shift8(x,y)   ( (x) << ((3-y)<<3) )
#define shift16(x,y)  ( (x) << ((1-y)<<4) )
#elif defined(PTPD_LSBF)
#define shift8(x,y)   ( (x) << ((y)<<3) )
#define shift16(x,y)  ( (x) << ((y)<<4) )
#endif

#define flip16(x) htons(x)
#define flip32(x) htonl(x)

/*
#if defined(PTPD_MSBF)
#define flip16(x) (x)
#define flip32(x) (x)
#elif defined(PTPD_LSBF)
static inline Integer16 flip16(Integer16 x)
{
   return (((x) >> 8) & 0x00ff) | (((x) << 8) & 0xff00);
}

static inline Integer32 flip32(x)
{
  return (((x) >> 24) & 0x000000ff) | (((x) >> 8 ) & 0x0000ff00) |
         (((x) << 8 ) & 0x00ff0000) | (((x) << 24) & 0xff000000);
}
#endif
*/

/* bit array manipulation */
#define getFlag(x,y)  (Boolean)!!( *(UInteger8*)((x)+((y)<8?1:0)) &   (1<<((y)<8?(y):(y)-8)) )
#define setFlag(x,y)    ( *(UInteger8*)((x)+((y)<8?1:0)) |=   1<<((y)<8?(y):(y)-8)  )
#define clearFlag(x,y)  ( *(UInteger8*)((x)+((y)<8?1:0)) &= ~(1<<((y)<8?(y):(y)-8)) )


/* msg.c */
Boolean msgPeek(Octet*, ssize_t);
void msgUnpackHeader(Octet*, MsgHeader*);
void msgUnpackSync(Octet*, MsgSync*);
void msgUnpackDelayReq(Octet*, MsgDelayReq*);
void msgUnpackFollowUp(Octet*, MsgFollowUp*);
void msgUnpackDelayResp(Octet*, MsgDelayResp*);
void msgUnpackManagement(Octet*, MsgManagement*);
UInteger8 msgUnloadManagement(PtpClock*, Octet*, MsgManagement*);
void msgUnpackManagementPayload(Octet *buf, MsgManagement *manage);
void msgPackHeader(PtpClock*, Octet*);
void msgPackSync(PtpClock*, Octet*, Boolean, TimeRepresentation*);
void msgPackDelayReq(PtpClock*, Octet*, Boolean, TimeRepresentation*);
void msgPackFollowUp(PtpClock*, Octet*, UInteger16, TimeRepresentation*);
void msgPackDelayResp(PtpClock*, Octet*, MsgHeader*, TimeRepresentation*);
UInteger16 msgPackManagement(PtpClock*, Octet*, MsgManagement*);
UInteger16 msgPackManagementResponse(PtpClock*, Octet*, MsgHeader*, MsgManagement*);

/* net.c */
/* linux API dependent */
Boolean netInit(NetPath*, PtpClock*);
Boolean netShutdown(NetPath*);
int netSelect(NetPath*, const TimeInternal*);
ssize_t netRecvEvent(NetPath*, Octet*, TimeInternal*);
ssize_t netRecvGeneral(NetPath*, Octet*);
ssize_t netSendEvent(NetPath*, const Octet*, UInteger16, TimeInternal*);
ssize_t netSendGeneral(NetPath*, const Octet*, UInteger16);
void netEmptyEventQ(NetPath *netPath);

/* servo.c */
void initClock(PtpClock*);
void updateDelay(PtpClock*, TimeInternal*, TimeInternal*);
void updateOffset(PtpClock*, TimeInternal*, TimeInternal*);
void updateClock(PtpClock*);

/* startup.c */
/* unix API dependent */
Integer16 ptpdStartup(PtpClock*, RunTimeOpts*, ForeignMasterRecord*);
void ptpdShutdown(PtpClock*);

/* sys.c */
/* unix API dependent */
void displayStats(const PtpClock*);
void getTime(TimeInternal*);
void setTime(const TimeInternal*);
void updateTime(const TimeInternal*);
UInteger16 getRand(UInteger32*);
Boolean adjFreq(Integer32);

/* timer.c */
void initTimer(void);
void timerUpdate(IntervalTimer*);
void timerStop(UInteger16, IntervalTimer*);
void timerStart(UInteger16, UInteger32, IntervalTimer*);
Boolean timerExpired(UInteger16, IntervalTimer*);
void catchAlarm(UInteger32 time_ms);

/* protocol.c */
void doState(PtpClock*);
void toState(PtpClock*, UInteger8);


#endif /*PTPD_DEP_H*/
