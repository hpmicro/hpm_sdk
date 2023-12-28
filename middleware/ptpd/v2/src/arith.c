/* arith.c */

#include "ptpd.h"

void scaledNanosecondsToInternalTime(const Integer64 * scaledNanoseconds, TimeInternal *internal)
{
    int sign;

	int64_t nanoseconds = *scaledNanoseconds;

    /*determine sign of result big integer number*/

    if (nanoseconds < 0)
    {
        nanoseconds = -nanoseconds;
        sign = -1;
    }
    else
    {
        sign = 1;
    }

    /*fractional nanoseconds are excluded (see 5.3.2)*/
    nanoseconds >>= 16;

    internal->seconds = sign * (nanoseconds / 1000000000);

    internal->nanoseconds = sign * (nanoseconds % 1000000000);
}


void fromInternalTime(const TimeInternal *internal, Timestamp *external)
{

    /*fromInternalTime is only used to convert time given by the system to a timestamp
     * As a consequence, no negative value can normally be found in (internal)

     * Note that offsets are also represented with TimeInternal structure, and can be negative,
     * but offset are never convert into Timestamp so there is no problem here.*/

    if ((internal->seconds & ~INT_MAX) || (internal->nanoseconds & ~INT_MAX))
    {
        DBG("Negative value canno't be converted into timestamp \n");
        return;
    }
    else
    {
        external->secondsField.lsb = internal->seconds;
        external->nanosecondsField = internal->nanoseconds;
        external->secondsField.msb = 0;
    }

}

void toInternalTime(TimeInternal *internal, const Timestamp *external)
{

    /*Program will not run after 2038...*/
    if (external->secondsField.lsb < INT_MAX)
    {
        internal->seconds = external->secondsField.lsb;
        internal->nanoseconds = external->nanosecondsField;
    }

    else
    {
        DBG("Clock servo canno't be executed : seconds field is higher than signed integer (32bits) \n");
        return;
    }
}

/**
 * \brief Normalize TimeInternal representation
 */
void normalizeTime(TimeInternal *r)
{
    r->seconds += r->nanoseconds / 1000000000;
    r->nanoseconds -= r->nanoseconds / 1000000000 * 1000000000;

    if (r->seconds > 0 && r->nanoseconds < 0)
    {
        r->seconds -= 1;
        r->nanoseconds += 1000000000;
    }
    else if (r->seconds < 0 && r->nanoseconds > 0)
    {
        r->seconds += 1;
        r->nanoseconds -= 1000000000;
    }
}

void addTime(TimeInternal *r, const TimeInternal *x, const TimeInternal *y)
{
    r->seconds = x->seconds + y->seconds;
    r->nanoseconds = x->nanoseconds + y->nanoseconds;

    normalizeTime(r);
}

void subTime(TimeInternal *r, const TimeInternal *x, const TimeInternal *y)
{
    r->seconds = x->seconds - y->seconds;
    r->nanoseconds = x->nanoseconds - y->nanoseconds;

    normalizeTime(r);
}

void div2Time(TimeInternal *r)
{
    r->nanoseconds += r->seconds % 2 * 1000000000;
    r->seconds /= 2;
    r->nanoseconds /= 2;
	
    normalizeTime(r);
}

Integer32 floorLog2(UInteger32 n)
{
  int pos = 0;

  if (n == 0)
    return -1;
 
  if (n >= 1<<16) { n >>= 16; pos += 16; }
  if (n >= 1<< 8) { n >>=  8; pos +=  8; }
  if (n >= 1<< 4) { n >>=  4; pos +=  4; }
  if (n >= 1<< 2) { n >>=  2; pos +=  2; }
  if (n >= 1<< 1) {           pos +=  1; }
  return pos;
}
