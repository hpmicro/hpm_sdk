/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "chry_ringbuffer.h"
#include "csh.h"
#include "usbd_core.h"
#include "usbd_cdc_acm.h"

static chry_shell_t csh;
static bool login = false;
static chry_ringbuffer_t shell_rb;
static uint8_t mempool[1024];

/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x01
#define CDC_INT_EP 0x83

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01)
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x02, 0x02, 0x01, 0x01)
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "CherryUSB CSH DEMO",         /* Product */
    "2022123456",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_quality_descriptor;
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return other_speed_config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return other_speed_config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;
    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor cdc_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t usb_temp_read_buffer[CDC_MAX_MPS];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t usb_temp_write_buffer[1024];
volatile bool ep_tx_busy_flag = false;

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
    case USBD_EVENT_RESET:
        ep_tx_busy_flag = false;
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        /* setup first out ep read transfer */
        usbd_ep_start_read(busid, CDC_OUT_EP, usb_temp_read_buffer, CDC_MAX_MPS);
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    chry_ringbuffer_write(&shell_rb, usb_temp_read_buffer, nbytes);
    usbd_ep_start_read(busid, ep, usb_temp_read_buffer, CDC_MAX_MPS);
}

void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, ep, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;

void cdc_acm_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, &cdc_descriptor);
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf0));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf1));
    usbd_add_endpoint(busid, &cdc_out_ep);
    usbd_add_endpoint(busid, &cdc_in_ep);
    usbd_initialize(busid, reg_base, usbd_event_handler);
}

static uint16_t csh_sput_cb(chry_readline_t *rl, const void *data, uint16_t size)
{
    (void)rl;

    memcpy(usb_temp_write_buffer, data, size);
    usbd_ep_start_write(0, CDC_IN_EP, usb_temp_write_buffer, size);
    ep_tx_busy_flag = true;
    while(ep_tx_busy_flag){}
    return size;
}

static uint16_t csh_sget_cb(chry_readline_t *rl, void *data, uint16_t size)
{
    (void)rl;
    return chry_ringbuffer_read(&shell_rb, data, size);
}

int shell_init(uint8_t busid, uint32_t regbase, bool need_login)
{
    chry_shell_init_t csh_init = {0};

    if (chry_ringbuffer_init(&shell_rb, mempool, sizeof(mempool))) {
        return -1;
    }

    cdc_acm_init(busid, regbase);

    while(!usb_device_is_configured(busid))
    {
    }

    if (need_login) {
        login = false;
    } else {
        login = true;
    }

    /*!< I/O callback */
    csh_init.sput = csh_sput_cb;
    csh_init.sget = csh_sget_cb;

#if defined(CONFIG_CSH_SYMTAB) && CONFIG_CSH_SYMTAB
#if defined(__GNUC__)
    extern const int __fsymtab_start;
    extern const int __fsymtab_end;
    extern const int __vsymtab_start;
    extern const int __vsymtab_end;

    /*!< get table from ld symbol */
    csh_init.command_table_beg = &__fsymtab_start;
    csh_init.command_table_end = &__fsymtab_end;
    csh_init.variable_table_beg = &__vsymtab_start;
    csh_init.variable_table_end = &__vsymtab_end;
#elif defined(__ICCARM__) || defined(__ICCRX__) || defined(__ICCRISCV__)
#pragma section="FSymTab"
#pragma section="VSymTab"
    csh_init.command_table_beg = __section_begin("FSymTab");
    csh_init.command_table_end = __section_end("FSymTab");
    csh_init.variable_table_beg = __section_begin("VSymTab");
    csh_init.variable_table_end = __section_end("VSymTab");
#endif
#endif

#if defined(CONFIG_CSH_PROMPTEDIT) && CONFIG_CSH_PROMPTEDIT
    static char csh_prompt_buffer[CONFIG_CSH_PROMPTEDIT_BUF_SIZE];

    /*!< set prompt buffer */
    csh_init.prompt_buffer = csh_prompt_buffer;
    csh_init.prompt_buffer_size = sizeof(csh_prompt_buffer);
#endif

#if defined(CONFIG_CSH_HISTORY) && CONFIG_CSH_HISTORY
    static char csh_history_buffer[CONFIG_CSH_HISTORY_BUF_SIZE];

    /*!< set history buffer */
    csh_init.history_buffer = csh_history_buffer;
    csh_init.history_buffer_size = sizeof(csh_history_buffer);
#endif

#if defined(CONFIG_CSH_LNBUFF_STATIC) && CONFIG_CSH_LNBUFF_STATIC
    static char csh_line_buffer[128];

    /*!< set linebuffer */
    csh_init.line_buffer = csh_line_buffer;
    csh_init.line_buffer_size = sizeof(csh_line_buffer);
#endif

    csh_init.uid = 0;
    csh_init.user[0] = "cherry";

    /*!< The port hash function is required,
         and the strcmp attribute is used weakly by default,
         int chry_shell_port_hash_strcmp(const char *hash, const char *str); */
    csh_init.hash[0] = "12345678"; /*!< If there is no password, set to NULL */
    csh_init.host = BOARD_NAME;
    csh_init.user_data = NULL;

    int ret = chry_shell_init(&csh, &csh_init);
    if (ret) {
        return -1;
    }

    return 0;
}

int shell_main(void)
{
    int ret;

restart:

    if (login) {
        goto restart2;
    }

    ret = csh_login(&csh);
    if (ret == 0) {
        login = true;
    } else {
        return 0;
    }

restart2:
    chry_shell_task_exec(&csh);

    ret = chry_shell_task_repl(&csh);

    if (ret == -1) {
        /*!< error */
        return -1;
    } else if (ret == 1) {
        /*!< continue */
        return 0;
    } else {
        /*!< restart */
        goto restart;
    }

    return 0;
}

void shell_lock(void)
{
    chry_readline_erase_line(&csh.rl);
}

void shell_unlock(void)
{
    chry_readline_edit_refresh(&csh.rl);
}

static int csh_exit(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    login = false;
    return 0;
}
CSH_SCMD_EXPORT_ALIAS(csh_exit, exit, );

#define __ENV_PATH "/sbin:/bin"
const char ENV_PATH[] = __ENV_PATH;
CSH_RVAR_EXPORT(ENV_PATH, PATH, sizeof(__ENV_PATH));

#define __ENV_ZERO ""
const char ENV_ZERO[] = __ENV_ZERO;
CSH_RVAR_EXPORT(ENV_ZERO, ZERO, sizeof(__ENV_ZERO));
