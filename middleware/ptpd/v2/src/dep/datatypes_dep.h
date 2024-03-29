#ifndef DATATYPES_DEP_H_
#define DATATYPES_DEP_H_

/**
*\file
* \brief Implementation specific datatype

 */

/**
 * \brief 5.2 Primitive data types specification
 */
typedef enum {FALSE=0, TRUE} Boolean;
typedef unsigned char Enumeration4; /**< 4-bit enumeration */
typedef unsigned char Enumeration8; /**< 8-bit enumeration */
typedef unsigned short Enumeration16; /**< 16-bit enumeration */
typedef unsigned char UInteger4; /**< 4-bit unsigned integer */
typedef signed char Integer8; /**< 8-bit signed integer */
typedef unsigned char UInteger8; /**< 8-bit unsigned integer */
typedef signed short Integer16; /**< 16-bit signed integer */
typedef unsigned short UInteger16; /**< 16-bit unsigned integer */
typedef signed int Integer32; /**< 32-bit signed integer */
typedef unsigned int UInteger32; /**< 32-bit unsigned integer */

/**
 * \struct UInteger48
 * \brief 48-bit unsigned integer
 */
typedef struct
{
    unsigned int lsb;
    unsigned short msb;
} UInteger48;

/**
 * \brief 64-bit signed integer
 */

/*
typedef struct
{
    unsigned int lsb;
    signed int msb;
} Integer64;
*/

typedef int64_t Integer64;

typedef unsigned char Nibble; /**< 4-bit data without numerical representation */
typedef char Octet; /**< 8-bit data without numerical representation */

/**
 * \brief Struct used to average the offset from master and the one way delay
 *
 * Exponential smoothing
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

/**
 * \brief Network buffer queue
 */

typedef struct
{
    void        *pbuf[PBUF_QUEUE_SIZE];
    Integer32   get;
    Integer32   put;
    Integer32   count;
} BufQueue;

/**
 * \brief Struct used to store network datas
 */

typedef struct
{
    Integer32     multicastAddr;
    Integer32     peerMulticastAddr;
    Integer32     unicastAddr;

    struct udp_pcb    *eventPcb;

    struct udp_pcb    *generalPcb;
    BufQueue      eventQ;
    BufQueue      generalQ;
} NetPath;

#endif /*DATATYPES_DEP_H_*/
