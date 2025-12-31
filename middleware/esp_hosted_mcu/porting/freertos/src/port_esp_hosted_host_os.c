/*
* SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
* copyright (c) 2025, HPMicro
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "string.h"
#include "esp_log.h"
#include <stdlib.h>
#include "port_esp_hosted_host_os.h"
#include "port_esp_hosted_host_config.h"
#include "port_esp_hosted_host_wifi_config.h"
#include "port_esp_hosted_host_log.h"
#include "esp_hosted_power_save.h"
#include "esp_log_level.h"
#include "portmacro.h"
#include "esp_wifi.h"

#include "hpm_clock_drv.h"
#include "hpm_gpiom_drv.h"
#include "hpm_gpio_drv.h"

/* Wi-Fi headers are reused at ESP-Hosted */
#include "esp_wifi_crypto_types.h"

#if H_TRANSPORT_IN_USE == H_TRANSPORT_SDIO
#include "port_esp_hosted_host_sdio.h"
#endif

#if H_TRANSPORT_IN_USE == H_TRANSPORT_SPI
#include "port_esp_hosted_host_spi.h"
#endif

#if H_TRANSPORT_IN_USE == H_TRANSPORT_SPI_HD
#include "port_esp_hosted_host_spi_hd.h"
#endif

#if H_TRANSPORT_IN_USE == H_TRANSPORT_UART
#include "port_esp_hosted_host_uart.h"
#endif

#ifndef CONFIG_ESP_MAX_GPIO_INT
#define CONFIG_ESP_MAX_GPIO_INT 5
#endif

DEFINE_LOG_TAG(os_wrapper_esp);

#define ALIGN_MAGIC 0xA11AA55A
#define ALIGN_MAGIC1 0x5A55A11A

struct mempool * nw_mp_g = NULL;

const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs;

struct hosted_config_t g_h = HOSTED_CONFIG_INIT_DEFAULT();

port_esp_hosted_gpio_int_ctx_t gpio_int_ctx[CONFIG_ESP_MAX_GPIO_INT] = {0};

/* -------- Memory ---------- */

void * hosted_memcpy(void* dest, const void* src, uint32_t size)
{
    if (size && (!dest || !src)) {
        if (!dest)
            ESP_LOGE(TAG, "%s:%u dest is NULL\n", __func__, __LINE__);
        if (!src)
            ESP_LOGE(TAG, "%s:%u dest is NULL\n", __func__, __LINE__);

        assert(dest);
        assert(src);
        return NULL;
    }

    return memcpy(dest, src, size);
}

void * hosted_memset(void* buf, int val, size_t len)
{
    return memset(buf, val, len);
}

void* hosted_malloc(size_t size)
{
    /* without alignment */
    return pvPortMalloc(size);
}

void* hosted_calloc(size_t blk_no, size_t size)
{
    void* ptr = pvPortMalloc(blk_no * size);
    if (ptr) {
        memset(ptr, 0, blk_no * size);
    }
    return ptr;
}

void hosted_free(void* ptr)
{
    if (!ptr) return;
    uintptr_t aligned = (uintptr_t)ptr;
    uint32_t magic1 = ((uint32_t*)aligned)[-3];
    void *raw = ((void**)aligned)[-2];
    uint32_t magic2 = ((uint32_t*)aligned)[-1];
    if (magic1 == ALIGN_MAGIC && magic2 == ALIGN_MAGIC1) {
        vPortFree(raw);
    } else {
        vPortFree(ptr);
    }
}

void *hosted_realloc(void *mem, size_t newsize)
{
    if (newsize == 0) {
        hosted_free(mem);
        return NULL;
    }
    void *p = hosted_malloc(newsize);
    if (p && mem) {
        hosted_memcpy(p, mem, newsize);
        hosted_free(mem);
    }
    return p;
}

void* hosted_malloc_align(size_t size, size_t align)
{
    uintptr_t raw = (uintptr_t)pvPortMalloc(size + align - 1 + sizeof(void*) + 2*sizeof(uint32_t));
    if (!raw) return NULL;
    uintptr_t aligned = (raw + sizeof(void*) + 2*sizeof(uint32_t) + align - 1) & ~(uintptr_t)(align - 1);
    ((uint32_t*)aligned)[-3] = ALIGN_MAGIC;
    ((void**)aligned)[-2] = (void*)raw;
    ((uint32_t*)aligned)[-1] = ALIGN_MAGIC1;
    return (void*)aligned;
}
void hosted_free_align(void* ptr)
{
    hosted_free(ptr);
}

void hosted_init_hook(void)
{
    /* This is hook to initialize port specific contexts, if any */
}


/* -------- Threads ---------- */

void *hosted_thread_create(const char *tname, uint32_t tprio, uint32_t tstack_size, void (*start_routine)(void const *), void *sr_arg)
{
    if (!start_routine) {
        ESP_LOGE(TAG, "start_routine is mandatory for thread create\n");
        return NULL;
    }

    TaskHandle_t handle = NULL;
    BaseType_t rc = xTaskCreate((TaskFunction_t)start_routine, tname, tstack_size, sr_arg, tprio, &handle);
    if ((rc != pdPASS) || (handle == NULL)) {
        ESP_LOGE(TAG, "Failed to create thread: %s\n", tname);
        return NULL;
    }

    return (void *)handle;
}

int hosted_thread_cancel(void *thread_handle)
{
    if (!thread_handle) {
        ESP_LOGE(TAG, "Invalid thread handle\n");
        return RET_INVALID;
    }

    vTaskDelete((TaskHandle_t)thread_handle);
    return RET_OK;
}

/* -------- Sleeps -------------- */
unsigned int hosted_msleep(unsigned int mseconds)
{
    vTaskDelay(pdMS_TO_TICKS(mseconds));
    return 0;
}

unsigned int hosted_usleep(unsigned int useconds)
{
    clock_cpu_delay_us(useconds);
    return 0;
}

unsigned int hosted_sleep(unsigned int seconds)
{
    return hosted_msleep(seconds * 1000UL);
}

/* Non sleepable delays - BLOCKING dead wait */
unsigned int hosted_for_loop_delay(unsigned int number)
{
    volatile int idx = 0;
    for (idx=0; idx<100*number; idx++) {
    }
    return 0;
}



/* -------- Queue --------------- */
/* User expected to pass item's address to this func eg. &item */
int hosted_queue_item(void * queue_handle, void *item, int timeout)
{
    queue_handle_t *q_id = NULL;
    int item_added_in_back = 0;

    if (!queue_handle) {
        ESP_LOGE(TAG, "Uninitialized sem id 3\n");
        return RET_INVALID;
    }

    q_id = (queue_handle_t *)queue_handle;
    item_added_in_back = xQueueSendToBack(*q_id, item, timeout);
    if (pdTRUE == item_added_in_back)
        return RET_OK;

    return RET_FAIL;
}

void * hosted_create_queue(uint32_t qnum_elem, uint32_t qitem_size)
{
    queue_handle_t *q_id = NULL;

    q_id = (queue_handle_t*)hosted_malloc(
            sizeof(queue_handle_t));
    if (!q_id) {
        ESP_LOGE(TAG, "Q allocation failed\n");
        return NULL;
    }

    *q_id = xQueueCreate(qnum_elem, qitem_size);
    if (!*q_id) {
        ESP_LOGE(TAG, "Q create failed\n");
        return NULL;
    }

    return q_id;
}


/* User expected to pass item's address to this func eg. &item */
int hosted_dequeue_item(void * queue_handle, void *item, int timeout)
{
    queue_handle_t *q_id = NULL;
    int item_retrieved = 0;

    if (!queue_handle) {
        ESP_LOGE(TAG, "Uninitialized Q id 1\n\r");
        return RET_INVALID;
    }

    q_id = (queue_handle_t *)queue_handle;
    if (!*q_id) {
        ESP_LOGE(TAG, "Uninitialized Q id 2\n\r");
        return RET_INVALID;
    }

    if (!timeout) {
        /* non blocking */
        item_retrieved = xQueueReceive(*q_id, item, 0);
    } else if (timeout<0) {
        /* Blocking */
        item_retrieved = xQueueReceive(*q_id, item, HOSTED_BLOCK_MAX);
    } else {
        item_retrieved = xQueueReceive(*q_id, item, pdMS_TO_TICKS(SEC_TO_MILLISEC(timeout)));
    }

    if (item_retrieved == pdTRUE)
        return 0;

    return RET_FAIL;
}

int hosted_queue_msg_waiting(void * queue_handle)
{
    queue_handle_t *q_id = NULL;
    if (!queue_handle) {
        ESP_LOGE(TAG, "Uninitialized sem id 9\n");
        return RET_INVALID;
    }

    q_id = (queue_handle_t *)queue_handle;
    return uxQueueMessagesWaiting(*q_id);
}

int hosted_destroy_queue(void * queue_handle)
{
    int ret = RET_OK;
    queue_handle_t *q_id = NULL;

    if (!queue_handle) {
        ESP_LOGE(TAG, "Uninitialized Q id 4\n");
        return RET_INVALID;
    }

    q_id = (queue_handle_t *)queue_handle;

    vQueueDelete(*q_id);

    HOSTED_FREE(queue_handle);

    return ret;
}


int hosted_reset_queue(void * queue_handle)
{
    queue_handle_t *q_id = NULL;

    if (!queue_handle) {
        ESP_LOGE(TAG, "Uninitialized Q id 5\n");
        return RET_INVALID;
    }

    q_id = (queue_handle_t *)queue_handle;

    return xQueueReset(*q_id);
}

/* -------- Mutex --------------- */

int hosted_unlock_mutex(void * mutex_handle)
{
    if (!mutex_handle) {
        ESP_LOGE(TAG, "Uninitialized mut id\n");
        return RET_INVALID;
    }
    return xSemaphoreGive((SemaphoreHandle_t)mutex_handle) == pdTRUE ? RET_OK : RET_FAIL;
}

void * hosted_create_mutex(void)
{
    SemaphoreHandle_t h = xSemaphoreCreateMutex();
    if (!h) {
        ESP_LOGE(TAG, "mut create failed\n");
        return NULL;
    }
    return (void *)h;
}


int hosted_lock_mutex(void * mutex_handle, int timeout)
{
    if (!mutex_handle) {
        ESP_LOGE(TAG, "Uninitialized mut id\n");
        return RET_INVALID;
    }
    TickType_t to = 0;
    if (timeout < 0) {
        to = HOSTED_BLOCK_MAX;
    } else if (timeout > 0) {
        to = pdMS_TO_TICKS(SEC_TO_MILLISEC(timeout));
    }
    return xSemaphoreTake((SemaphoreHandle_t)mutex_handle, to) == pdTRUE ? RET_OK : RET_FAIL_TIMEOUT;
}

int hosted_destroy_mutex(void * mutex_handle)
{
    if (!mutex_handle) {
        ESP_LOGE(TAG, "Uninitialized mut id\n");
        return RET_INVALID;
    }
    vSemaphoreDelete((SemaphoreHandle_t)mutex_handle);
    return RET_OK;
}

/* -------- Semaphores ---------- */
int hosted_post_semaphore(void * semaphore_handle)
{
    semaphore_handle_t *sem_id = NULL;
    int sem_posted = 0;

    if (!semaphore_handle) {
        ESP_LOGE(TAG, "Uninitialized sem id 3\n");
        return RET_INVALID;
    }

    sem_id = (semaphore_handle_t *)semaphore_handle;
    sem_posted = xSemaphoreGive(*sem_id);
    if (pdTRUE == sem_posted)
        return RET_OK;

    return RET_FAIL;
}

FAST_RAM_ATTR int hosted_post_semaphore_from_isr(void * semaphore_handle)
{
    semaphore_handle_t *sem_id = NULL;
    int sem_posted = 0;
    BaseType_t mustYield = false;

    if (!semaphore_handle) {
        ESP_LOGE(TAG, "Uninitialized sem id 3\n");
        return RET_INVALID;
    }

    sem_id = (semaphore_handle_t *)semaphore_handle;

    sem_posted = xSemaphoreGiveFromISR(*sem_id, &mustYield);
    if (mustYield) {
        portYIELD_FROM_ISR(mustYield);
    }
    if (pdTRUE == sem_posted)
        return RET_OK;

    return RET_FAIL;
}

void * hosted_create_semaphore(int maxCount)
{
    semaphore_handle_t *sem_id = NULL;

    sem_id = (semaphore_handle_t*)hosted_malloc(
            sizeof(semaphore_handle_t));
    if (!sem_id) {
        ESP_LOGE(TAG, "Sem allocation failed\n");
        return NULL;
    }

    if (maxCount>1)
        *sem_id = xSemaphoreCreateCounting(maxCount,0);
    else
        *sem_id = xSemaphoreCreateBinary();

    if (!*sem_id) {
        ESP_LOGE(TAG, "sem create failed\n");
        return NULL;
    }

    xSemaphoreGive(*sem_id);

    return sem_id;
}


int hosted_get_semaphore(void * semaphore_handle, int timeout)
{
    semaphore_handle_t *sem_id = NULL;
    int sem_acquired = 0;

    if (!semaphore_handle) {
        ESP_LOGE(TAG, "Uninitialized sem id 1\n\r");
        return RET_INVALID;
    }

    sem_id = (semaphore_handle_t *)semaphore_handle;
    if (!*sem_id) {
        ESP_LOGE(TAG, "Uninitialized sem id 2\n\r");
        return RET_INVALID;
    }

    if (!timeout) {
        /* non blocking */
        sem_acquired = xSemaphoreTake(*sem_id, 0);
    } else if (timeout<0) {
        /* Blocking */
        sem_acquired = xSemaphoreTake(*sem_id, HOSTED_BLOCK_MAX);
    } else {
        sem_acquired = xSemaphoreTake(*sem_id, pdMS_TO_TICKS(SEC_TO_MILLISEC(timeout)));
    }

    if (sem_acquired == pdTRUE)
        return 0;

    return RET_FAIL_TIMEOUT;
}

int hosted_destroy_semaphore(void * semaphore_handle)
{
    int ret = RET_OK;
    semaphore_handle_t *sem_id = NULL;

    if (!semaphore_handle) {
        ESP_LOGE(TAG, "Uninitialized sem id 4\n");
        assert(semaphore_handle);
        return RET_INVALID;
    }

    sem_id = (semaphore_handle_t *)semaphore_handle;

    vSemaphoreDelete(*sem_id);

    HOSTED_FREE(semaphore_handle);

    return ret;
}

#ifdef H_USE_MEMPOOL
static void * hosted_create_spinlock(void)
{
    spinlock_handle_t spin_dummy = portMUX_INITIALIZER_UNLOCKED;
    spinlock_handle_t *spin_id = NULL;

    spin_id = (spinlock_handle_t*)hosted_malloc(
            sizeof(spinlock_handle_t));

    if (!spin_id) {
        ESP_LOGE(TAG, "mut allocation failed\n");
        return NULL;
    }

    *spin_id = spin_dummy;

    //hosted_unlock_mutex(*mut_id);

    return spin_id;
}

void* hosted_create_lock_mempool(void)
{
    return hosted_create_spinlock();
}
void hosted_lock_mempool(void *lock_handle)
{
    assert(lock_handle);
    portENTER_CRITICAL((spinlock_handle_t *)lock_handle);
}

void hosted_unlock_mempool(void *lock_handle)
{
    assert(lock_handle);
    portEXIT_CRITICAL((spinlock_handle_t *)lock_handle);
}
#endif
/* -------- Timers  ---------- */
int hosted_timer_stop(void *timer_handle)
{
    int ret = RET_OK;

    ESP_LOGD(TAG, "Stop the timer\n");
    if (timer_handle) {
        if (xTimerStop((TimerHandle_t)timer_handle, 0) != pdPASS) {
            ESP_LOGE(TAG, "Failed to start the timer\n");
        }
        if (xTimerDelete((TimerHandle_t)timer_handle, 0) != pdPASS) {
            ESP_LOGE(TAG, "Failed to delete the timer\n");
        }
        timer_handle = NULL;
        return ret;
    }
    return RET_FAIL;
}

/* Sample timer_handler looks like this:
*
* void expired(union sigval timer_data){
*     struct mystruct *a = timer_data.sival_ptr;
* 	ESP_LOGE(TAG, "Expired %u\n", a->mydata++);
* }
**/

void *hosted_timer_start(const char *name, int duration_ms, int type,
        void (*timeout_handler)(void *), void *arg)
{
    UBaseType_t uxAutoReload;

    esp_hosted_timer_type_t esp_timer_type = type;

    ESP_LOGD(TAG, "Start the timer %u\n", duration_ms);
    /* Start depending upon timer type */
        if (esp_timer_type == H_TIMER_TYPE_PERIODIC) {
    uxAutoReload = pdTRUE;
    } else if (esp_timer_type == H_TIMER_TYPE_ONESHOT) {
        uxAutoReload = pdFALSE;
    } else {
        ESP_LOGE(TAG, "Unsupported timer type. supported: one_shot, periodic\n");
        return NULL;
    }
    TimerHandle_t timer_handle = xTimerCreate(name, duration_ms, uxAutoReload,( void * ) 0,(TimerCallbackFunction_t) timeout_handler);
    if (!timer_handle) {
        ESP_LOGE(TAG, "Failed to create timer\n");
        if (xTimerDelete(timer_handle, 0) != pdPASS) {
            ESP_LOGE(TAG, "Failed to delete the timer\n");
        }
        return NULL;
    }
    return timer_handle;
}


/* GPIO */

void hosted_gpio_isr_handler(uint32_t gpio_num)
{
    for (uint32_t i = 0; i < sizeof(gpio_int_ctx) / sizeof(port_esp_hosted_gpio_int_ctx_t); i++) {
        if ((gpio_int_ctx[i].is_registered == true) && (gpio_int_ctx[i].gpio_num == gpio_num)) {
            if (gpio_int_ctx[i].handler) {
                gpio_int_ctx[i].handler(gpio_int_ctx[i].arg);
                gpio_clear_pin_interrupt_flag(HPM_GPIO0, GPIO_GET_PORT_INDEX(gpio_num), GPIO_GET_PIN_INDEX(gpio_num));
            }
            break;
        }
    }
}

int hosted_config_gpio(void* gpio_port, uint32_t gpio_num, uint32_t mode)
{
    (void)gpio_port;
    uint32_t gpio_idx = GPIO_GET_PORT_INDEX(gpio_num);
    uint32_t pin_idx = GPIO_GET_PIN_INDEX(gpio_num);

    gpiom_set_pin_controller(HPM_GPIOM, gpio_idx, pin_idx, gpiom_soc_gpio0);

    HPM_IOC->PAD[gpio_num].FUNC_CTL = 0;

    switch (gpio_idx)
    {
    case GPIO_DI_GPIOY :
        HPM_PIOC->PAD[gpio_num].FUNC_CTL = 3;
        break;
#ifdef GPIO_DI_GPIOZ
    case GPIO_DI_GPIOZ :
#ifdef HPM_BIOC
        HPM_BIOC->PAD[gpio_num].FUNC_CTL = 3;
#endif
        break;
#endif
    default :
        break;
    }

    switch (mode)
    {
    case H_GPIO_MODE_DEF_OUTPUT:
        gpio_set_pin_output(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[gpio_num].PAD_CTL &=  ~(IOC_PAD_PAD_CTL_PS_MASK | IOC_PAD_PAD_CTL_PE_MASK | IOC_PAD_PAD_CTL_OD_MASK);
        break;
    case H_GPIO_MODE_DEF_INPUT:
        gpio_set_pin_input(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[gpio_num].PAD_CTL &= ~(IOC_PAD_PAD_CTL_PS_MASK | IOC_PAD_PAD_CTL_PE_MASK);
        break;
    case H_GPIO_MODE_DEF_INPUT_PULLDOWN:
        gpio_set_pin_input(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[gpio_num].PAD_CTL = (HPM_IOC->PAD[gpio_num].PAD_CTL & ~IOC_PAD_PAD_CTL_PS_MASK) | IOC_PAD_PAD_CTL_PE_SET(1);
        break;
    case H_GPIO_MODE_DEF_INPUT_PULLUP:
        gpio_set_pin_input(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[gpio_num].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
        break;
    case H_GPIO_MODE_DEF_OD:
        gpio_set_pin_output(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[gpio_num].PAD_CTL = (HPM_IOC->PAD[gpio_num].PAD_CTL & ~(IOC_PAD_PAD_CTL_PS_MASK | IOC_PAD_PAD_CTL_PE_MASK)) | IOC_PAD_PAD_CTL_OD_SET(1);
        break;
    default:
        /* Invalid mode */
        break;
    }
    HPM_IOC->PAD[gpio_num].FUNC_CTL = IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
    return 0;
}

int hosted_setup_gpio_interrupt(void* gpio_port, uint32_t gpio_num, uint32_t intr_type, void (*fn)(void *), void *arg)
{
    int ret = -1;
    hosted_config_gpio(gpio_port, gpio_num, H_GPIO_MODE_DEF_INPUT);
    gpio_interrupt_trigger_t trigger = (gpio_interrupt_trigger_t)intr_type;
    uint32_t gpio_idx = GPIO_GET_PORT_INDEX(gpio_num);
    uint32_t pin_idx = GPIO_GET_PIN_INDEX(gpio_num);
    for (uint32_t i = 0; i < sizeof(gpio_int_ctx) / sizeof(port_esp_hosted_gpio_int_ctx_t); i++) {
        if (gpio_int_ctx[i].is_registered == false) {
            gpio_int_ctx[i].is_registered = true;
            gpio_int_ctx[i].gpio_num = gpio_num;
            gpio_int_ctx[i].intr_type = intr_type;
            gpio_int_ctx[i].handler = fn;
            gpio_int_ctx[i].arg = arg;
            gpio_set_pin_input(HPM_GPIO0, gpio_idx, pin_idx);
            gpio_config_pin_interrupt(HPM_GPIO0, gpio_idx, pin_idx, trigger);
            gpio_enable_pin_interrupt(HPM_GPIO0, gpio_idx, pin_idx);
            ret = 0;
            break;
        }
    }
    return ret;
}

int hosted_teardown_gpio_interrupt(void* gpio_port, uint32_t gpio_num)
{
    int ret = -1;
    (void)gpio_port;
    uint32_t gpio_idx = GPIO_GET_PORT_INDEX(gpio_num);
    uint32_t pin_idx = GPIO_GET_PIN_INDEX(gpio_num);
    for (uint32_t i = 0; i < sizeof(gpio_int_ctx) / sizeof(port_esp_hosted_gpio_int_ctx_t); i++) {
        if ((gpio_int_ctx[i].is_registered == true) && (gpio_int_ctx[i].gpio_num == gpio_num)) {
            gpio_int_ctx[i].is_registered = false;
            gpio_int_ctx[i].gpio_num = 0;
            gpio_int_ctx[i].intr_type = 0;
            gpio_int_ctx[i].handler = NULL;
            gpio_int_ctx[i].arg = NULL;
            gpio_disable_pin_interrupt(HPM_GPIO0, gpio_idx, pin_idx);
            ret = 0;
            break;
        }
    }
    return ret;
    return 0;
}

int hosted_read_gpio(void*gpio_port, uint32_t gpio_num)
{
    (void)gpio_port;
    return gpio_read_pin(HPM_GPIO0, GPIO_GET_PORT_INDEX(gpio_num), GPIO_GET_PIN_INDEX(gpio_num));
}

int hosted_write_gpio(void* gpio_port, uint32_t gpio_num, uint32_t value)
{
    (void)gpio_port;
    gpio_write_pin(HPM_GPIO0, GPIO_GET_PORT_INDEX(gpio_num), GPIO_GET_PIN_INDEX(gpio_num), value);
    return 0;
}
int hosted_hold_gpio(void* gpio_port, uint32_t gpio_num, uint32_t hold_value)
{
    (void)gpio_port;
#if defined(IOC_PAD_PAD_CTL_KE_MASK)
    HPM_IOC->PAD[gpio_num].PAD_CTL = (HPM_IOC->PAD[gpio_num].PAD_CTL & ~IOC_PAD_PAD_CTL_KE_MASK) | IOC_PAD_PAD_CTL_KE_SET(hold_value);
#endif
    return 0;
}

int hosted_pull_gpio(void* gpio_port, uint32_t gpio_num, uint32_t pull_value, uint32_t enable)
{
    int ret = 0;
    (void)gpio_port;
    HPM_IOC->PAD[gpio_num].PAD_CTL = (HPM_IOC->PAD[gpio_num].PAD_CTL & ~IOC_PAD_PAD_CTL_PE_MASK) | IOC_PAD_PAD_CTL_PE_SET(enable);
    switch (pull_value)
    {
    case H_GPIO_PULL_UP:
        HPM_IOC->PAD[gpio_num].PAD_CTL = (HPM_IOC->PAD[gpio_num].PAD_CTL & ~IOC_PAD_PAD_CTL_PS_MASK) | IOC_PAD_PAD_CTL_PS_SET(1);
        break;
    case H_GPIO_PULL_DOWN:
        HPM_IOC->PAD[gpio_num].PAD_CTL = (HPM_IOC->PAD[gpio_num].PAD_CTL & ~IOC_PAD_PAD_CTL_PS_MASK);
        break;
    default:
        ret = -1;
        break;
    }
    return ret;
}


ATTR_WEAK int hosted_wifi_event_post(int32_t event_id, void* event_data, size_t event_data_size, uint32_t ticks_to_wait)
{
    (void)ticks_to_wait;
    ESP_LOGV(TAG, "event %ld recvd --> event_data:%p event_data_size: %u\n",event_id, event_data, event_data_size);
    return 0;
}

void hosted_log_write(int  level,
                    const char *tag,
                    const char *format, ...)
{
    va_list args;
    uint32_t length = 0;
    char level_char;
    static char rt_log_buf[CONFIG_ESP_WIFI_CONSOLEBUF_SIZE];
    esp_log_level_t level_for_tag = esp_log_level_get(tag);
    if ((ESP_LOG_NONE != level_for_tag) && (level <= level_for_tag)) {
        va_start(args, format);
        length = vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, format, args);
        if (length > CONFIG_ESP_WIFI_CONSOLEBUF_SIZE - 1) {
            length = CONFIG_ESP_WIFI_CONSOLEBUF_SIZE - 1;
        }
        switch (level) {
        case ESP_LOG_INFO:
            level_char = 'I'; 
            break;
        case ESP_LOG_WARN:
            level_char = 'W';
            break;
        case ESP_LOG_ERROR:
            level_char = 'E';
            break;
        default:
            level_char = 'D';
            break;
        }
        printf("[%c/%s] %.*s\n", level_char, tag, length, rt_log_buf);
        va_end(args);
    }
}

int hosted_restart_host(void)
{
    ESP_LOGI(TAG, "Restarting host");
    return 0;
}


int hosted_config_host_power_save(uint32_t power_save_type, void* gpio_port, uint32_t gpio_num, int level)
{
    return -1;
}

int hosted_start_host_power_save(uint32_t power_save_type)
{
    return -1;
}


int hosted_get_host_wakeup_or_reboot_reason(void)
{
    return HOSTED_WAKEUP_NORMAL_REBOOT;
}


hosted_osi_funcs_t g_hosted_osi_funcs = {
    ._h_memcpy                   =  hosted_memcpy                  ,
    ._h_memset                   =  hosted_memset                  ,
    ._h_malloc                   =  hosted_malloc                  ,
    ._h_calloc                   =  hosted_calloc                  ,
    ._h_free                     =  hosted_free                    ,
    ._h_realloc                  =  hosted_realloc                 ,
    ._h_malloc_align             =  hosted_malloc_align            ,
    ._h_free_align               =  hosted_free_align              ,
    ._h_thread_create            =  hosted_thread_create           ,
    ._h_thread_cancel            =  hosted_thread_cancel           ,
    ._h_msleep                   =  hosted_msleep                  ,
    ._h_usleep                   =  hosted_usleep                  ,
    ._h_sleep                    =  hosted_sleep                   ,
    ._h_blocking_delay           =  hosted_for_loop_delay          ,
    ._h_queue_item               =  hosted_queue_item              ,
    ._h_create_queue             =  hosted_create_queue            ,
    ._h_queue_msg_waiting        =  hosted_queue_msg_waiting       ,
    ._h_dequeue_item             =  hosted_dequeue_item            ,
    ._h_destroy_queue            =  hosted_destroy_queue           ,
    ._h_reset_queue              =  hosted_reset_queue             ,
    ._h_unlock_mutex             =  hosted_unlock_mutex            ,
    ._h_create_mutex             =  hosted_create_mutex            ,
    ._h_lock_mutex               =  hosted_lock_mutex              ,
    ._h_destroy_mutex            =  hosted_destroy_mutex           ,
    ._h_post_semaphore           =  hosted_post_semaphore          ,
    ._h_post_semaphore_from_isr  =  hosted_post_semaphore_from_isr ,
    ._h_create_semaphore         =  hosted_create_semaphore        ,
    ._h_get_semaphore            =  hosted_get_semaphore           ,
    ._h_destroy_semaphore        =  hosted_destroy_semaphore       ,
    ._h_timer_stop               =  hosted_timer_stop              ,
    ._h_timer_start              =  hosted_timer_start             ,
#ifdef H_USE_MEMPOOL
    ._h_create_lock_mempool      =  hosted_create_lock_mempool     ,
    ._h_lock_mempool             =  hosted_lock_mempool            ,
    ._h_unlock_mempool           =  hosted_unlock_mempool          ,
#endif
    ._h_config_gpio              =  hosted_config_gpio             ,
    ._h_config_gpio_as_interrupt =  hosted_setup_gpio_interrupt,
    ._h_teardown_gpio_interrupt  = hosted_teardown_gpio_interrupt,
    ._h_hold_gpio                = hosted_hold_gpio,
    ._h_read_gpio                =  hosted_read_gpio               ,
    ._h_write_gpio               =  hosted_write_gpio              ,
    ._h_pull_gpio                = hosted_pull_gpio,

    ._h_get_host_wakeup_or_reboot_reason = hosted_get_host_wakeup_or_reboot_reason,

#if H_TRANSPORT_IN_USE == H_TRANSPORT_SPI
    ._h_bus_init                 =  hosted_spi_init                ,
    ._h_bus_deinit               =  hosted_spi_deinit              ,
    ._h_do_bus_transfer          =  hosted_do_spi_transfer         ,
#endif
    ._h_event_wifi_post          =  hosted_wifi_event_post         ,
    ._h_printf                   =  hosted_log_write               ,
    ._h_hosted_init_hook         =  hosted_init_hook               ,
#if H_TRANSPORT_IN_USE == H_TRANSPORT_SDIO
    ._h_bus_init                 =  hosted_sdio_init               ,
    ._h_bus_deinit               =  hosted_sdio_deinit             ,
    ._h_sdio_card_init           =  hosted_sdio_card_init          ,
    ._h_sdio_read_reg            =  hosted_sdio_read_reg           ,
    ._h_sdio_write_reg           =  hosted_sdio_write_reg          ,
    ._h_sdio_read_block          =  hosted_sdio_read_block         ,
    ._h_sdio_write_block         =  hosted_sdio_write_block        ,
    ._h_sdio_wait_slave_intr     =  hosted_sdio_wait_slave_intr    ,
#endif
#if H_TRANSPORT_IN_USE == H_TRANSPORT_SPI_HD
    ._h_bus_init                 =  hosted_spi_hd_init               ,
    ._h_bus_deinit               =  hosted_spi_hd_deinit             ,
    ._h_spi_hd_read_reg          =  hosted_spi_hd_read_reg           ,
    ._h_spi_hd_write_reg         =  hosted_spi_hd_write_reg          ,
    ._h_spi_hd_read_dma          =  hosted_spi_hd_read_dma           ,
    ._h_spi_hd_write_dma         =  hosted_spi_hd_write_dma          ,
    ._h_spi_hd_set_data_lines    =  hosted_spi_hd_set_data_lines     ,
    ._h_spi_hd_send_cmd9         =  hosted_spi_hd_send_cmd9          ,
#endif
#if H_TRANSPORT_IN_USE == H_TRANSPORT_UART
    ._h_bus_init                 = hosted_uart_init                ,
    ._h_bus_deinit               = hosted_uart_deinit              ,
    ._h_uart_read                = hosted_uart_read                ,
    ._h_uart_write               = hosted_uart_write               ,
#endif
    ._h_restart_host             = hosted_restart_host             ,

    ._h_config_host_power_save_hal_impl = hosted_config_host_power_save,
    ._h_start_host_power_save_hal_impl = hosted_start_host_power_save,
};
