/*
* SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
* Copyright (c) 2025 HPMicro
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef __PORT_ESP_HOSTED_HOST_OS_H
#define __PORT_ESP_HOSTED_HOST_OS_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#include "mempool.h"

#include "port_esp_hosted_host_config.h"
#include "esp_hosted_os_abstraction.h"
#include "esp_netif_types.h"
#include "esp_wifi_types.h"

#include "hpm_gpio_drv.h"
#include "hpm_common.h"


typedef unsigned int    uint;

#define MAX_PAYLOAD_SIZE (MAX_TRANSPORT_BUFFER_SIZE-H_ESP_PAYLOAD_HEADER_OFFSET)


typedef enum {
    H_TIMER_TYPE_ONESHOT = 0,
    H_TIMER_TYPE_PERIODIC = 1,
} esp_hosted_timer_type_t;

typedef struct port_esp_hosted_gpio_int_ctx
{
    bool is_registered;
    uint32_t gpio_num;
    uint32_t intr_type;
    void (*handler)(void* arg);
    void *arg;
} port_esp_hosted_gpio_int_ctx_t;


#define HOSTED_BLOCKING                              -1
#define HOSTED_NON_BLOCKING                          0

#define thread_handle_t                              TaskHandle_t
#define queue_handle_t                               QueueHandle_t
#define semaphore_handle_t                           SemaphoreHandle_t
#define mutex_handle_t                               SemaphoreHandle_t

#define spinlock_handle_t                            (void*)
#define gpio_port_handle_t                           (void*)

#define FAST_RAM_ATTR                                ATTR_RAMFUNC
/* this is needed when there is no gpio port being used */
#define H_GPIO_PORT_DEFAULT                          -1

#define gpio_pin_state_t                             int

#define HOSTED_BLOCK_MAX                             portMAX_DELAY

#ifndef CONFIG_ESP_HOSTED_DFLT_TASK_STACK
#define CONFIG_ESP_HOSTED_DFLT_TASK_STACK            512
#endif
#define DFLT_TASK_STACK_SIZE                         CONFIG_ESP_HOSTED_DFLT_TASK_STACK

#ifndef CONFIG_ESP_HOSTED_DFLT_TASK_PRIO
#define CONFIG_ESP_HOSTED_DFLT_TASK_PRIO             23
#endif
#define DFLT_TASK_PRIO                              CONFIG_ESP_HOSTED_DFLT_TASK_PRIO

#ifndef CONFIG_ESP_HOSTED_RPC_TASK_STACK_SIZE
#define CONFIG_ESP_HOSTED_RPC_TASK_STACK_SIZE        1024
#endif
#define RPC_TASK_STACK_SIZE                          CONFIG_ESP_HOSTED_RPC_TASK_STACK_SIZE

#ifndef CONFIG_ESP_HOSTED_RPC_TASK_PRIO
#define CONFIG_ESP_HOSTED_RPC_TASK_PRIO              23
#endif
#define RPC_TASK_PRIO                                CONFIG_ESP_HOSTED_RPC_TASK_PRIO


#define H_GPIO_MODE_DEF_DISABLE                     0x00
#define H_GPIO_MODE_DEF_INPUT                       0x01
#define H_GPIO_MODE_DEF_OUTPUT                      0x02
#define H_GPIO_MODE_DEF_OD                          0x03
#define H_GPIO_MODE_DEF_INPUT_PULLUP                0x04
#define H_GPIO_MODE_DEF_INPUT_PULLDOWN              0x05


#define H_GPIO_PULL_UP                             (1)
#define H_GPIO_PULL_DOWN                           (0)

#define RET_OK                                       0
#define RET_FAIL                                     -1
#define RET_INVALID                                  -2
#define RET_FAIL_MEM                                 -3
#define RET_FAIL4                                    -4
#define RET_FAIL_TIMEOUT                             -5

#define HOSTED_MEM_ALIGNMENT_4      4
#define HOSTED_MEM_ALIGNMENT_32     32
#define HOSTED_MEM_ALIGNMENT_64     64

#ifndef CONFIG_ESP_WIFI_CONSOLEBUF_SIZE
#define CONFIG_ESP_WIFI_CONSOLEBUF_SIZE 256
#endif

/* esp wifi */
#ifndef CONFIG_ESP_WIFI_CSI_ENABLED
#define CONFIG_ESP_WIFI_CSI_ENABLED 0
#endif

#ifndef CONFIG_ESP_WIFI_AMPDU_RX_ENABLED
#define CONFIG_ESP_WIFI_AMPDU_RX_ENABLED 0
#endif

#ifndef CONFIG_ESP_WIFI_AMPDU_TX_ENABLED
#define CONFIG_ESP_WIFI_AMPDU_TX_ENABLED 0
#endif

#ifndef CONFIG_ESP_WIFI_AMSDU_TX_ENABLED
#define CONFIG_ESP_WIFI_AMSDU_TX_ENABLED 0
#endif

#ifndef CONFIG_ESP_WIFI_NVS_ENABLED
#define CONFIG_ESP_WIFI_NVS_ENABLED 0
#endif

#ifndef CONFIG_NEWLIB_NANO_FORMAT
#define CONFIG_NEWLIB_NANO_FORMAT 0
#endif

#ifndef CONFIG_NEWLIB_NANO_FORMAT
#define CONFIG_NEWLIB_NANO_FORMAT 0
#endif

#ifndef CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE
#define CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE 1
#endif

#ifndef CONFIG_ESP_WIFI_ENABLE_WPA3_SAE
#define CONFIG_ESP_WIFI_ENABLE_WPA3_SAE 1
#endif

#ifndef CONFIG_ESP_WIFI_FTM_INITIATOR_SUPPORT
#define CONFIG_ESP_WIFI_FTM_INITIATOR_SUPPORT 0
#endif

#ifndef CONFIG_ESP_WIFI_FTM_RESPONDER_SUPPORT
#define CONFIG_ESP_WIFI_FTM_RESPONDER_SUPPORT 0
#endif

#ifndef CONFIG_ESP_WIFI_GCMP_SUPPORT
#define CONFIG_ESP_WIFI_GCMP_SUPPORT 0
#endif

#ifndef CONFIG_ESP_WIFI_GMAC_SUPPORT
#define CONFIG_ESP_WIFI_GMAC_SUPPORT 0
#endif

#ifndef CONFIG_ESP_WIFI_11R_SUPPORT
#define CONFIG_ESP_WIFI_11R_SUPPORT 0
#endif

#ifndef CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT
#define CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT 0
#endif

#ifndef CONFIG_ESP_WIFI_TASK_PINNED_TO_CORE_1
#define CONFIG_ESP_WIFI_TASK_PINNED_TO_CORE_1 0
#endif

#ifndef CONFIG_ESP_COEX_POWER_MANAGEMENT
#define CONFIG_ESP_COEX_POWER_MANAGEMENT 0
#endif

/** Enumeration **/
enum hardware_type_e {
    HARDWARE_TYPE_ESP32,
    HARDWARE_TYPE_OTHER_ESP_CHIPSETS,
    HARDWARE_TYPE_INVALID,
};

//TODO: redesign common code over




#define MILLISEC_TO_SEC			1000
#define TICKS_PER_SEC(x) (1000*(x) / portTICK_PERIOD_MS)
#define SEC_TO_MILLISEC(x) (1000*(x))
#define SEC_TO_MICROSEC(x) (1000*1000*(x))
#define MILLISEC_TO_MICROSEC(x) (1000*(x))

#define MEM_DUMP(s) 


/* -------- Create handle ------- */
#define HOSTED_CREATE_HANDLE(tYPE, hANDLE) {                                   \
    hANDLE = (tYPE *)g_h.funcs->_h_malloc(sizeof(tYPE));                       \
    if (!hANDLE) {                                                             \
        printf("%s:%u Mem alloc fail while create handle\n", __func__,__LINE__); \
        return NULL;                                                           \
    }                                                                          \
}

#define HOSTED_FREE_HANDLE(handle) { \
    if (handle) { \
        g_h.funcs->_h_free(handle); \
        handle = NULL; \
    } \
}

/* -------- Calloc, Free handle ------- */
#define HOSTED_FREE(buff) if (buff) { g_h.funcs->_h_free(buff); buff = NULL; }
#define HOSTED_CALLOC(struct_name, buff, nbytes, gotosym) do {    \
    buff = (struct_name *)g_h.funcs->_h_calloc(1, nbytes);	  \
    if (!buff) {                                                  \
        printf("%s, Failed to allocate memory \n", __func__);     \
        goto gotosym;                                             \
    }                                                             \
} while(0);

#define HOSTED_MALLOC(struct_name, buff, nbytes, gotosym) do {    \
    buff = (struct_name *)g_h.funcs->_h_malloc(nbytes);		  \
    if (!buff) {                                                  \
        printf("%s, Failed to allocate memory \n", __func__);     \
        goto gotosym;                                             \
    }                                                             \
} while(0);

/* Driver Handle */
struct serial_drv_handle_t;

/* Timer handle */
struct timer_handle_t;
extern struct mempool * nw_mp_g;

void hosted_gpio_isr_handler(uint32_t gpio_num);
#endif
