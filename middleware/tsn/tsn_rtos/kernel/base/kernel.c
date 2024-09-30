/*!
 * Copyright (C) Fraunhofer-Institut for Photonic Microsystems (IPMS)
 * Maria-Reiche-Str. 2
 * 01109 Dresden
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * \file   kernel.c
 * \author zimmerli
 * \date   2020-01-10
 * \brief  Kernel functions
 *
 */

#include "kernel.h"
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include <kernel/net/phydev.h>
#include <kernel/net/netb.h>

//check TSN RTOS configuration
#include "config.h"

// ----------------------------------------------------------------
#define NETBUF_SZ       2048
#define NETBUF_OFS_HEAD 64

typedef struct netb_s *netb_ptr;

/**
 * \brief Kernel management structure
 */
struct kernel_s {
    TaskHandle_t task_init;      //!< handle of init task
    SemaphoreHandle_t sem_print; //!< printk lock
    QueueHandle_t qeueue_netb;   //!< queue of empty struct netb_s
};

/**
 * \brief Kernel management
 */
struct kernel_s _kernel_data;

// ----------------------------------------------------------------

/**
 * \brief Initialize Kernel
 *
 * @param addr      address of free memory to be used by network buffer
 * @param size      size of memory to be used network buffer
 */
void kernel_init(addr_t addr, uint32_t size)
{
    addr_t endaddr;
    uint32_t minqsize;
    uint32_t tmp;
    netb_ptr netb;

    /* init printk semaphore */
    _kernel_data.sem_print = xSemaphoreCreateMutex();
    configASSERT(_kernel_data.sem_print != NULL);

    /* init netb */
    // create queue
    minqsize = size / NETBUF_SZ;
    _kernel_data.qeueue_netb = xQueueCreate(minqsize, sizeof(netb_ptr));
    configASSERT(_kernel_data.qeueue_netb != NULL);

    // check assumption sizeof(struct netb_s) <= NETBUF_OFS_HEAD
    configASSERT(sizeof(struct netb_s) <= NETBUF_OFS_HEAD);

    // set empty netb
    endaddr = addr + size - NETBUF_SZ;
    while (addr <= endaddr) {
        // set pointer
        netb = (netb_ptr)(addr);

        // fill data
        netb->head = addr + NETBUF_OFS_HEAD;
        netb->end = addr + NETBUF_SZ;
        netb->data = netb->head;
        netb->tail = netb->head;

        // send to queue
        tmp = xQueueSend(_kernel_data.qeueue_netb, &netb, 0);
        configASSERT(tmp == pdPASS);

        // inc addr
        addr += NETBUF_SZ;
    }
}

/**
 * \brief       Start Kernel, start init task
 *
 * @param taskInitCode  init function pointer
 * @param addStackSz    stack size needed by init task, additional to configMINIMAL_STACK_SIZE
 */
void kernel_start(TaskFunction_t taskInitCode, uint16_t addStackSz)
{
    /* create sys task */
    kTaskCreate_a("KERNEL", taskInitCode, "init", configMINIMAL_STACK_SIZE + addStackSz, NULL, KERNEL_PRIO_HIGH, &_kernel_data.task_init);

    /* Start scheduler */
    vTaskStartScheduler();
}

// ----------------------------------------------------------------

void swap(char *x, char *y)
{
    char tmp = *x;
    *x = *y;
    *y = tmp;
}

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        swap((str + start), (str + end));
        start++;
        end--;
    }
}

// Implementation of itoa()
char *_itoa(int num, char *str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

#define BUFSIZE (sizeof(long) * 8 + 1)
char *_ltoa(long N, char *str, int base)
{
    register int i = 2;
    long uarg;
    char *tail, *head = str, buf[BUFSIZE];

    if (36 < base || 2 > base)
        base = 10; /* can only use 0-9, A-Z        */

    tail = &buf[BUFSIZE - 1]; /* last character position      */
    *tail-- = '\0';

    if (10 == base && N < 0L) {
        *head++ = '-';
        uarg = -N;
    } else
        uarg = N;

    if (uarg) {
        for (i = 1; uarg; ++i) {
            register ldiv_t r;

            r = ldiv(uarg, base);
            *tail-- = (char)(r.rem + ((9L < r.rem) ? ('A' - 10L) : '0'));
            uarg = r.quot;
        }
    } else
        *tail-- = '0';

    memcpy(head, ++tail, i);

    return str;
}
// ----------------------------------------------------------------
static bool indent_enabled = true;

void __internal_set_indent(void)
{
    indent_enabled = true;
}
void __internal_unset_indent(void)
{
    indent_enabled = false;
}

static uint8_t rt_max_log_level = DEFAULT_LOG_LEVEL;
static bool rt_log_print_mode = false;

void set_runtime_max_loglevel(uint8_t level)
{
    rt_max_log_level = level;
}

void switch_loglevel_printmode(bool printmode)
{
    rt_log_print_mode = printmode;
}

/**
 * \brief logging function
 *
 * @param format    format string
 */

void internal_logk(log_mode mode, int level, const char *ctx, const char *file, int line, const char *func, const char *format, ...)
{
    (void)mode;
    (void)level;
    (void)ctx;
    (void)file;
    (void)line;
    (void)func;
    (void)format;
}

void internal_vlogk(log_mode mode, int level, const char *ctx, const char *file, int line, const char *func, const char *format, va_list args)
{
    (void)mode;
    (void)level;
    (void)ctx;
    (void)file;
    (void)line;
    (void)func;
    (void)format;
    (void)args;
}

// ----------------------------------------------------------------

void *kalloczl(size_t size, const char *ctx)
{
    void *p = kallocz(size);

    logk(LOG_ALWAYS, ctx, "allocate %d bytes %s\n", size, p == NULL ? "failed" : "ok");

    return p;
}

/**
 * \brief Allocate and zero memory
 *
 * @param size  needed size in bytes
 * @return      pointer to memory, NULL on error
 */
void *kallocz(size_t size)
{
    void *p;

    // allocate and zero memory
    p = pvPortMalloc(size);
    if (p) {
        memset(p, 0, size);
    }

    return p;
}

/**
 * \brief Duplicate string to new memory location
 * @param str string to duplicate
 * @return pointer to the newly allocated string, or a null pointer if an error occurred.
 */
char *kstrdup(char *str)
{
    char *dup;
    size_t size = strlen(str) + 1;

    dup = pvPortMalloc(size);
    if (dup) {
        memcpy(dup, str, size);
    }

    return dup;
}

/**
 * \brief free memory
 *
 * @param pv  pointer to memory
 */
void kfree(void *pv)
{
    vPortFree(pv);
}

void kfreeN(void **pv)
{
    vPortFree(*pv);
    *pv = NULL;
}

/**
 * \brief Allocate free network buffer
 *
 * @param timeout   timeout for allocation
 * @return          pointer to network buffer, NULL on error
 */
struct netb_s *kalloc_netb(uint32_t timeout)
{
    uint32_t tmp;
    netb_ptr netb;

    tmp = xQueueReceive(_kernel_data.qeueue_netb, &netb, timeout);
    if (tmp != pdPASS)
        netb = NULL;
    if (netb)
        netb_set_empty(netb);

    return netb;
}

/**
 * \brief Free network buffer
 *
 * @param netb      pointer to network buffer
 */
void kfree_netb(struct netb_s *netb)
{
    uint32_t tmp;

    /* simple check for consistency */
    configASSERT((addr_t)(netb) + NETBUF_OFS_HEAD == netb->head);

    /* enqueue to list */
    tmp = xQueueSend(_kernel_data.qeueue_netb, &netb, 0);
    configASSERT(tmp == pdPASS);
}

UBaseType_t kavail_netb()
{
    return uxQueueMessagesWaiting(_kernel_data.qeueue_netb);
}

// ----------------------------------------------------------------
/**
 * Kernel assertion, stop system on error
 *
 * @param condition     assertion condition
 */
//void kassert(int condition) {
//    configASSERT(condition);
//}

uint32_t _ns_to_systicks(uint32_t time_ns, uint32_t busfreq)
{
    uint64_t tmp;
    tmp = busfreq;
    tmp *= time_ns;
    tmp /= 1000000000LL;
    return tmp;
}

uint32_t _systicks_to_ns(uint32_t time_ticks, uint32_t busfreq)
{
    uint64_t tmp;
    tmp = time_ticks;
    tmp = tmp * 1000000000LL;
    tmp /= busfreq;
    return tmp;
}

const char interface_string[] = "eth";
const char cpu_string[] = "cpu";

//TODO avoid sscanf
uint8_t *mac_string2int(const char *psMac, uint8_t *piMac)
{
    if (sscanf(psMac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &piMac[0], &piMac[1], &piMac[2], &piMac[3], &piMac[4], &piMac[5]) != 6) {
        return NULL;
    }

    return piMac;
}

//TODO avoid sprintf
char *mac_int2string(char *psMac, uint8_t *piMac)
{
    sprintf(psMac, "%02X:%02X:%02X:%02X:%02X:%02X", piMac[0], piMac[1], piMac[2], piMac[3], piMac[4], piMac[5]);

    return psMac;
}

//TODO return int --> return bool; in utils.[hc] verschieben
int str2long(const char *context, long long int *value, const char *svalue, int base)
{
    char *strend;

    *value = strtoll(svalue, &strend, base);

    if (svalue == strend) {
        logk(LOG_ALWAYS, "UTILS", "%s: number '%s' invalid (no digits found)\n", context, svalue);
        return 1;
    } else if (errno != 0 && *value == 0) {
        logk(LOG_ALWAYS, "UTILS", "%s: number '%lu' invalid (unspecified error occurred)\n", context, *value);
        return 1;
    } else if (errno == 0 && svalue && *strend != 0) {
        logk(LOG_ALWAYS, "UTILS", "%s: number '%lu' valid (but additional characters remain)\n", context, *value);
        return 0;
    }

    return 0;
}

const char *system_name_get()
{
    return "TSN RTOS Fraunhofer";
}

const char *system_desc_get()
{
    return "TSN RTOS for Fraunhofer TSN-EP / TSN-SE / TSN-SW devices";
}
