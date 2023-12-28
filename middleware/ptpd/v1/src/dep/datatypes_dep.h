/* datatypes_dep.h */

#ifndef DATATYPES_DEP_H
#define DATATYPES_DEP_H

typedef enum {FALSE = 0, TRUE} Boolean;
typedef signed char Integer8;
typedef unsigned char UInteger8;
typedef signed short Integer16;
typedef unsigned short UInteger16;
typedef signed int Integer32;
typedef unsigned int UInteger32;
typedef char Octet;

/**
 * \brief Struct used to average the offset from master and the one way delay
 *
 * Exponencial smoothing
 *
 * alpha = 1/2^s
 * y[1] = x[0]
 * y[n] = alpha * x[n-1] + (1-alpha) * y[n-1]
 */

typedef struct
{
    Integer32  y_prev, y_sum;
    Integer16  s;
    Integer16  s_prev;
    Integer32  n;
} Filter;

typedef struct
{
    void        *pbuf[PBUF_QUEUE_SIZE];
    Integer32   get;
    Integer32   put;
    Integer32   count;
} BufQueue;

typedef struct
{
    Integer32     multicastAddr;
    Integer32     unicastAddr;

    struct udp_pcb    *eventPcb;

    struct udp_pcb    *generalPcb;
    BufQueue      eventQ;
    BufQueue      generalQ;
} NetPath;

/* define compiler specific symbols */
#if defined   ( __CC_ARM   )
typedef long ssize_t;
#elif defined ( __ICCARM__ )
typedef long ssize_t;
#elif defined (  __GNUC__  )

#elif defined   (  __TASKING__  )
typedef long ssize_t;
#endif

#endif
