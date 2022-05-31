/* ptpd.h */

#ifndef PTPD_H
#define PTPD_H

#include "constants.h"
#include "dep/constants_dep.h"
#include "dep/datatypes_dep.h"
#include "datatypes.h"
#include "dep/ptpd_dep.h"


/* arith.c */
UInteger32 crc_algorithm(Octet*, Integer16);
UInteger32 sum(Octet*, Integer16);
void fromInternalTime(TimeInternal*, TimeRepresentation*, Boolean);
void toInternalTime(TimeInternal*, TimeRepresentation*, Boolean*);
void normalizeTime(TimeInternal*);
void addTime(TimeInternal*, const TimeInternal*, const TimeInternal*);
void subTime(TimeInternal*, const TimeInternal*, const TimeInternal*);

/**
 * \brief Divide the TimeInternal by 2 and normalize
 */
void div2Time(TimeInternal*);

/**
 * \brief Returns the floor form of binary logarithm for a 32 bit integer.
 * -1 is returned if ''n'' is 0.
 */
Integer32 floorLog2(UInteger32);

/**
 * \brief return maximum of two numbers
 */
static __inline Integer32 max(Integer32 a, Integer32 b)
{
   return a > b ? a : b;
}

/**
 * \brief return minimum of two numbers
 */
static __inline Integer32 min(Integer32 a, Integer32 b)
{
   return a > b ? b : a;
}


/* bmc.c */
UInteger8 bmc(PtpClock*);
void m1(PtpClock*);
void s1(PtpClock*, MsgHeader*, MsgSync*);
void initData(PtpClock*);
void addForeign(PtpClock*, MsgHeader*, MsgSync*);
Integer32 comparePortIdentity(const PortIdentity*, const PortIdentity*);

/* protocol.c */
int ptpd_Init(void);

void ptpd_periodic_handle(__IO UInteger32 localtime);

#define getEvent(eventField, mask) (Boolean)(((eventField) & (mask)) == (mask))
#define setEvent(eventField, mask) (eventField) |= (mask)
#define clearEvent(eventField, mask) (eventField) &= ~(mask)

#endif
