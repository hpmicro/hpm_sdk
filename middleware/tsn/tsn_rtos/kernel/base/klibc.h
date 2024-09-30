/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   klibc.h
 * \author zimmerli
 * \date   2020-01-20
 * \brief  Kernel Libc-like helper functions
 *
 */

#ifndef KLIBC_H_
#define KLIBC_H_

#include <stdint.h>
#include <stdbool.h>

#include <kernel/base/kernel.h>

// ----------------------------------------------------------------
#define INCL_KLIBC_DEBUGSUPPORT

// ----------------------------------------------------------------
// network byte order <-> host byte order

static inline uint16_t klibc_htons(uint16_t value)
{
    return (((value & 0xFF) << 8 | (value >> 8)) & 0xFFFF);
}

static inline uint32_t klibc_htonl(uint32_t value)
{
    return (((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24));
}

#define klibc_ntohs(value) klibc_htons(value)
#define klibc_ntohl(value) klibc_htonl(value)

// ----------------------------------------------------------------
// debug support
#ifdef INCL_KLIBC_DEBUGSUPPORT

/**
 * \brief Minimum-Maximum tupel
 */
typedef struct {
    int32_t min;
    int32_t max;
} klibc_minmax_t;

static inline void klibc_minmax_clear(klibc_minmax_t *minmax)
{
    minmax->min = INT32_MAX;
    minmax->max = INT32_MIN;
}

static inline void klibc_minmax_update(klibc_minmax_t *minmax, int32_t v)
{
    if (v < minmax->min)
        minmax->min = v;
    if (v > minmax->max)
        minmax->max = v;
}

/**
 * \brief ring buffer of int32_t
 */
typedef struct {
    int sz;
    int wp;
    int32_t *buf;
} klibc_ringlog_s32;

static inline void klibc_ringlog_add(klibc_ringlog_s32 *log, int32_t v)
{
    log->wp++;
    if (log->wp >= log->sz)
        log->wp = 0;
    log->buf[log->wp] = v;
}

#define KLIBC_RINGLOGS32(name, size) \
    int32_t name##_buf[size];        \
    klibc_ringlog_s32 name = { .sz = size, .wp = 0, .buf = name##_buf };

#endif // INCL_KLIBC_DEBUGSUPPORT

// ----------------------------------------------------------------
// light weight List
typedef struct list_data {
    void *data;
    uint8_t idx; //1...
    struct list_data *prev;
    struct list_data *next;
} list_data_t;

list_data_t *list_create(void);
void list_free(list_data_t *top, bool free_data);
uint8_t list_size(list_data_t *top);
list_data_t *list_next(list_data_t *top, list_data_t *data);
list_data_t *list_prev(list_data_t *top, list_data_t *data);
bool list_end(list_data_t *top, list_data_t *data);

list_data_t *list_add(list_data_t *top, void *data);
list_data_t *list_get(list_data_t *top, uint8_t idx);
void list_del(list_data_t *top, void *data);
void list_del_entry(list_data_t *top, list_data_t *entry);
//void list_del_idx(list_data_t *top, uint8_t idx);
// ----------------------------------------------------------------

// ----------------------------------------------------------------
/*
 * unaligned access support:
 *
 * reduced performance compared to aligned access
 */

static inline uint16_t _get_unaligned_le16(uint8_t *p)
{
    return p[0] | p[1] << 8;
}

static inline uint32_t _get_unaligned_le32(uint8_t *p)
{
    return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

static inline void _put_unaligned_le16(uint16_t val, uint8_t *p)
{
    *p++ = val;
    *p++ = val >> 8;
}

static inline void _put_unaligned_le32(uint32_t val, uint8_t *p)
{
    _put_unaligned_le16(val >> 16, p + 2);
    _put_unaligned_le16(val, p);
}

static inline uint16_t __get_unaligned_le16(addr_t addr)
{
    return _get_unaligned_le16((uint8_t *)addr);
}

static inline uint32_t __get_unaligned_le32(addr_t addr)
{
    return _get_unaligned_le32((uint8_t *)addr);
}

static inline void __put_unaligned_le16(uint16_t val, addr_t addr)
{
    _put_unaligned_le16(val, (uint8_t *)addr);
}

static inline void __put_unaligned_le32(uint32_t val, addr_t addr)
{
    _put_unaligned_le16(val >> 16, (uint8_t *)addr + 2);
    _put_unaligned_le16(val, (uint8_t *)addr);
}

static inline void __put_unaligned_le64(uint64_t val, addr_t addr)
{
    _put_unaligned_le32(val >> 32, (uint8_t *)addr + 4);
    _put_unaligned_le32(val, (uint8_t *)addr);
}

static inline uint16_t _get_unaligned_be16(uint8_t *p)
{
    return p[1] | p[0] << 8;
}

static inline uint32_t _get_unaligned_be32(uint8_t *p)
{
    return p[3] | p[2] << 8 | p[1] << 16 | p[0] << 24;
}

static inline void _put_unaligned_be16(uint16_t val, uint8_t *p)
{
    *p++ = val >> 8;
    *p++ = val;
}

static inline uint16_t __get_unaligned_be16(addr_t addr)
{
    return _get_unaligned_be16((uint8_t *)addr);
}

static inline uint32_t __get_unaligned_be32(addr_t addr)
{
    return _get_unaligned_be32((uint8_t *)addr);
}

static inline void __put_unaligned_be16(uint16_t val, addr_t addr)
{
    _put_unaligned_be16(val, (uint8_t *)addr);
}

static inline void __put_unaligned_be32(uint32_t val, addr_t addr)
{
    _put_unaligned_be16(val >> 16, (uint8_t *)addr);
    _put_unaligned_be16(val, (uint8_t *)addr + 2);
}

// math
#define kmin(a, b) (a < b ? a : b)
#define kmax(a, b) (a > b ? a : b)

// misc
#define gen_enum_val(val)  val,
#define gen_enum_str(str)  #str
#define gen_enum_astr(str) #str,
#define gen_enum_hstr(str) #str "|"

#define def_enum(storagecls, name)                 \
    typedef enum {                                 \
        name(gen_enum_val, gen_enum_val) no_##name \
    } name##_t;                                    \
                                                   \
    storagecls char *name##_to_str(uint32_t e);    \
    storagecls uint32_t name##_from_str(char *str);

#define impl_enum(storagecls, name)                                                                                        \
    storagecls const uint32_t max_##name = no_##name;                                                                      \
                                                                                                                           \
    storagecls char *name##_str[no_##name + 1] = { name(gen_enum_astr, gen_enum_astr) name(gen_enum_hstr, gen_enum_str) }; \
                                                                                                                           \
    storagecls char *name##_to_str(uint32_t e)                                                                             \
    {                                                                                                                      \
        if (e < 0 || e > no_##name)                                                                                        \
            return name##_str[no_##name];                                                                                  \
        return name##_str[e];                                                                                              \
    }                                                                                                                      \
                                                                                                                           \
    storagecls uint32_t name##_from_str(char *str)                                                                         \
    {                                                                                                                      \
        for (int i = 0; i < no_##name; i++)                                                                                \
            if (strcmp(str, name##_str[i]) == 0)                                                                           \
                return i;                                                                                                  \
        return no_##name;                                                                                                  \
    }

#define gen_enum(storagecls, name) def_enum(storagecls, name) impl_enum(storagecls, name)

char *ltrim(const char *str);
list_data_t *split(const char *str, const char *delim);

#endif /* KLIBC_H_ */
