/* arith.c */

#include "ptpd.h"

/* from annex C of the spec */
UInteger32 crc_algorithm(Octet *buf, Integer16 length)
{
    Integer16 i;
    UInteger8 data;
    UInteger32 polynomial = 0xedb88320, crc = 0xffffffff;

    while (length-- > 0)
    {
        data = *(UInteger8 *)(buf++);

        for (i = 0; i < 8; i++)
        {
            if ((crc ^ data)&1)
            {
                crc = (crc >> 1);
                crc ^= polynomial;
            }
            else
            {
                crc = (crc >> 1);
            }

            data >>= 1;
        }
    }

    return crc ^ 0xffffffff;
}

UInteger32 sum(Octet *buf, Integer16 length)
{
    UInteger32 sum = 0;

    while (length-- > 0)
        sum += *(UInteger8 *)(buf++);

    return sum;
}

void fromInternalTime(TimeInternal *internal, TimeRepresentation *external, Boolean halfEpoch)
{
    external->seconds = labs(internal->seconds) + halfEpoch * INT_MAX;

    if (internal->seconds < 0 || internal->nanoseconds < 0)
    {
        external->nanoseconds = labs(internal->nanoseconds) | ~INT_MAX;
    }
    else
    {
        external->nanoseconds = labs(internal->nanoseconds);
    }

    DBGVV("fromInternalTime: %10ds %11dns -> %10us %11dns\n",

          internal->seconds, internal->nanoseconds,
          external->seconds, external->nanoseconds);
}

void toInternalTime(TimeInternal *internal, TimeRepresentation *external, Boolean *halfEpoch)
{
    *halfEpoch = (Boolean)(external->seconds / INT_MAX);

    if (external->nanoseconds & ~INT_MAX)
    {
        internal->seconds = -(external->seconds % INT_MAX);
        internal->nanoseconds = -(external->nanoseconds & INT_MAX);
    }
    else
    {
        internal->seconds = external->seconds % INT_MAX;
        internal->nanoseconds = external->nanoseconds;
    }

    DBGVV("toInternalTime: %10ds %11dns <- %10us %11dns\n",

          internal->seconds, internal->nanoseconds,
          external->seconds, external->nanoseconds);
}

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
