/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   kernel.h
 * \author zimmerli
 * \date   2020-01-10
 * \brief  Kernel functions
 *
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
//#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"
#include <tsn_rtos_config.h>

/*
 * Definitions
 */
#define KERNEL_PRIO_BG     0
#define KERNEL_PRIO_NORMAL 1
#define KERNEL_PRIO_HIGH   2

#define KERNEL_PRINTK_BLOCKTIME 1000

#define KERNEL_TASK_FPU() portTASK_USES_FLOATING_POINT()

#define ENDLESS_LOOP      \
    while (1) {           \
        portNOP();        \
        vTaskDelay(1000); \
    }

typedef uintptr_t addr_t;

////////////////////////////////////////////////////////////////////////////////////////
//TODO deprecated
/*
 * Memory/Register access macro
 *
 * Access is expected to be aligned!
 */
#define MEM8(addr)  (*(volatile uint8_t *)(addr_t)(addr))
#define MEM16(addr) (*(volatile uint16_t *)(addr_t)(addr))
#define MEM32(addr) (*(volatile uint32_t *)(addr_t)(addr))

#define MAC_LO(mac) (uint32_t)(mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24))
#define MAC_HI(mac) (uint32_t)(mac[4] | (mac[5] << 8))
////////////////////////////////////////////////////////////////////////////////////////

typedef void (*irq_callback_t)(void *context);

/*
 * Platform specific
 *
 * functions must be defined in arch dependent code under arch/
 *
 */
/**
 * \brief Initialize platform
 */
void platform_init(void);
/**
 * \brief Reboot platform
 */
void platform_reset(void);
/**
 * \brief wrapper to variadic function of platform printf
 *
 * @param format    format string
 * @param arg       argument list
 * @return          number of printed bytes
 */
int platform_vprintk(const char *format, va_list arg);
/**
 * \brief Register interrupt handler
 *
 * @param id        interrupt number
 * @param callback  interrupt handler function
 * @param context   pointer to object used by interrupt handler
 */
void platform_register_irq(uint32_t id, irq_callback_t callback, void *context);
/**
 * \brief Enable interupt
 *
 * @param id        interrupt number
 */
void platform_enable_irq(uint32_t id);

///////////////////////////////////
//TODO old, deprecated
/*
 * Kernel printk and logging
 */
int printk(const char *format, ...);
int printk_unsafe(const char *format, ...);
///////////////////////////////////

char *_itoa(int value, char *str, int base);
char *_ltoa(long value, char *str, int base);

//use logs, logc, loge (see defines below); don't use internal_logk directly
typedef enum {
    PRINT,
    ONE_LINE,
    START,
    CONT,
    END
} log_mode;

void internal_logk(log_mode mode, int level, const char *ctx, const char *file, int line, const char *func, const char *format, ...);
void internal_vlogk(log_mode mode, int level, const char *ctx, const char *file, int line, const char *func, const char *format, va_list args);

//log control
//#define MAX_LOG_FORMAT_BUFFER_SIZE	(128)

//#define LOG_LIKEPRINT

//TODO evtl Namen und Werte der Loglevel anpassen an "Standard" (see log4j)
//log level
#define LOG_LOWEST_LEVEL  LOG_ALWAYS
#define LOG_ALWAYS        1
#define LOG_ERROR         2
#define LOG_WARNING       3
#define LOG_INFO          4
#define LOG_DEBUG         5
#define LOG_REGISTER      6
#define LOG_HIGHEST_LEVEL LOG_REGISTER

#ifndef MAX_LOG_LEVEL
#define MAX_LOG_LEVEL LOG_REGISTER
#endif

#ifndef DEFAULT_LOG_LEVEL
#define DEFAULT_LOG_LEVEL LOG_DEBUG
#endif

#ifndef LOG_FILEINFO
#define LOG_FILEINFO 1
#endif

//TODO logs, logc, loge
#ifdef LOG_LIKEPRINT
#pragma GCC diagnostic ignored "-Wunused-variable"
#define logk(level, ctx, format, ...)                                               \
    do {                                                                            \
        if (MAX_LOG_LEVEL >= level)                                                 \
            internal_logk(PRINT, level, ctx, NULL, 0, NULL, format, ##__VA_ARGS__); \
    } while (0)
#define logs(level, ctx, format, ...)                                               \
    do {                                                                            \
        if (MAX_LOG_LEVEL >= level)                                                 \
            internal_logk(PRINT, level, ctx, NULL, 0, NULL, format, ##__VA_ARGS__); \
    } while (0)
#define logc(level, format, ...)                                                     \
    do {                                                                             \
        if (MAX_LOG_LEVEL >= level)                                                  \
            internal_logk(PRINT, level, NULL, NULL, 0, NULL, format, ##__VA_ARGS__); \
    } while (0)
#define loge(level, format, ...)                                                     \
    do {                                                                             \
        if (MAX_LOG_LEVEL >= level)                                                  \
            internal_logk(PRINT, level, NULL, NULL, 0, NULL, format, ##__VA_ARGS__); \
    } while (0)
#else
#if LOG_FILEINFO == 1
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define logk(level, ctx, format, ...)                                                                         \
    do {                                                                                                      \
        if (MAX_LOG_LEVEL >= level)                                                                           \
            internal_logk(ONE_LINE, level, ctx, __FILENAME__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__); \
    } while (0)
#define logs(level, ctx, format, ...)                                                                      \
    do {                                                                                                   \
        if (MAX_LOG_LEVEL >= level)                                                                        \
            internal_logk(START, level, ctx, __FILENAME__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__); \
    } while (0)
#define logc(level, format, ...)                                                                           \
    do {                                                                                                   \
        if (MAX_LOG_LEVEL >= level)                                                                        \
            internal_logk(CONT, level, NULL, __FILENAME__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__); \
    } while (0)
#define loge(level, format, ...)                                                                          \
    do {                                                                                                  \
        if (MAX_LOG_LEVEL >= level)                                                                       \
            internal_logk(END, level, NULL, __FILENAME__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__); \
    } while (0)
#else
#define logk(level, ctx, format, ...)                                                  \
    do {                                                                               \
        if (MAX_LOG_LEVEL >= level)                                                    \
            internal_logk(ONE_LINE, level, ctx, NULL, 0, NULL, format, ##__VA_ARGS__); \
    } while (0)
#define logs(level, ctx, format, ...)                                               \
    do {                                                                            \
        if (MAX_LOG_LEVEL >= level)                                                 \
            internal_logk(START, level, ctx, NULL, 0, NULL, format, ##__VA_ARGS__); \
    } while (0)
#define logc(level, format, ...)                                                    \
    do {                                                                            \
        if (MAX_LOG_LEVEL >= level)                                                 \
            internal_logk(CONT, level, NULL, NULL, 0, NULL, format, ##__VA_ARGS__); \
    } while (0)
#define loge(level, format, ...)                                                   \
    do {                                                                           \
        if (MAX_LOG_LEVEL >= level)                                                \
            internal_logk(END, level, NULL, NULL, 0, NULL, format, ##__VA_ARGS__); \
    } while (0)
#endif
#endif

void set_runtime_max_loglevel(uint8_t level);
void switch_loglevel_printmode(bool printmode);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WREG32(addr, value, ctx, regname)                                                                                \
    {                                                                                                                    \
        logk(LOG_REGISTER, ctx, "write to reg %s @ %#X%04X = %#X\n", regname, ((addr) >> 16), ((addr) & 0xFFFF), value); \
        MEM32(addr) = *((uint32_t *)&value);                                                                             \
    }

#define RREG32(addr, variable, ctx, regname)                                                                             \
    {                                                                                                                    \
        uint32_t _reg = MEM32(addr);                                                                                     \
        logk(LOG_REGISTER, ctx, "read from reg %s @ %#X%04X = %#X\n", regname, ((addr) >> 16), ((addr) & 0xFFFF), _reg); \
        *((uint32_t *)(variable)) = _reg;                                                                                \
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////

/*
 * Kernel init
 */
void kernel_init(addr_t addr, uint32_t size);
void kernel_start(TaskFunction_t taskInitCode, uint16_t addStackSz);

/*
 * create Task and assert if failed
 */
//void kTaskCreatea(TaskFunction_t pxTaskCode, const char * const pcName, const configSTACK_DEPTH_TYPE usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask);
#define kTaskCreate_a(context, pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask) \
    {                                                                                                     \
        uint32_t tmp;                                                                                     \
        tmp = xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);     \
        kassert(tmp == pdPASS);                                                                           \
    }

/*
 * create Task and return if failed
 */
//void kTaskCreatea(TaskFunction_t pxTaskCode, const char * const pcName, const configSTACK_DEPTH_TYPE usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask);
#define kTaskCreate_r(context, pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask) \
    {                                                                                                     \
        uint32_t tmp;                                                                                     \
        tmp = xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);     \
        if (tmp != pdPASS)                                                                                \
            return;                                                                                       \
    }

/*
 * create Task and print error if failed
 */
//void kTaskCreatea(TaskFunction_t pxTaskCode, const char * const pcName, const configSTACK_DEPTH_TYPE usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask);
#define kTaskCreate_e(context, pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask) \
    {                                                                                                     \
        uint32_t tmp;                                                                                     \
        tmp = xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);     \
        printf("starting task '%s' ... %s\n", pcName, (tmp == pdPASS ? "done" : "failed"));               \
    }

/*
 * Memory mgmt
 */
void *kallocz(size_t size);
void *kalloczl(size_t size, const char *ctx);
char *kstrdup(char *str);
void kfree(void *p);
void kfreeN(void **pv);
#define KfreeN(pointer) kfreeN((void **)&(pointer))

struct netb_s *kalloc_netb(uint32_t timeout);
void kfree_netb(struct netb_s *netb);
UBaseType_t kavail_netb(void);

/*
 * Assertion
 */
#define kassert(condition) configASSERT(condition)
//void kassert(int condition);

uint32_t _ns_to_systicks(uint32_t time_ns, uint32_t busfreq);
uint32_t _systicks_to_ns(uint32_t time_ticks, uint32_t busfreq);

#define UNKNOWN_PORT (int32_t)(-1)

//eth1 -> 0; eth2 -> 1
int32_t interface_string2index(const char *pstring);

//0 -> eth1; 1 -> eth2
char *interface_index2string(int32_t index, char *str);

//cpu -> 0; eth1 -> 1; eth2 -> 2
int32_t interface_string2port(const char *pstring);

//0 -> cpu; 1 -> eth1; 2 -> eth2
char *interface_port2string(int32_t port, char *str);
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//TODO verschieben nach klib.c/klib.h
uint8_t *mac_string2int(const char *psMac, uint8_t *piMac);
char *mac_int2string(char *psMac, uint8_t *piMac);

//void mac_hilo2int(uint32_t machi, uint32_t maclo, uint8_t *piMac);

int str2long(const char *context, long long int *value, const char *svalue, int base);

const char *system_name_get(void);
const char *system_desc_get(void);

#endif /* KERNEL_H_ */
