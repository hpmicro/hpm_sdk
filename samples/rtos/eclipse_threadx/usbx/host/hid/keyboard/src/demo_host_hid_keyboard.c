/* This is a small demo of the high-performance FileX FAT file system. It includes setup for
 * a small 34KB RAM disk and a loop that writes and reads a small file.  */
#include "board.h"
/* This example illustrates USBX Device HID Mouse */

#include "ux_api.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hub.h"
#include "ux_host_class_hid_keyboard.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UX_HCD_NAME "EHCI HOST"
#define CONFIG_USB_EHCI_CAP_BASE (0xF2020000UL + 0x100)
#define PRINTF printf
/* Define constants. */
#define DEMO_STACK_SIZE (4 * 1024)
#define KEYBOARD_QUEUE_SIZE (1024)
#ifndef USBX_MEMORY_SIZE
#define USBX_MEMORY_SIZE (1024 * 32)
#endif
#ifndef USBX_MEMORY_CACHESAFE_SIZE
#define USBX_MEMORY_CACHESAFE_SIZE (32 * 1024)
#endif

typedef struct _keyboard_usr_thread_data {
    UX_HOST_CLASS_HID *hid;
    TX_THREAD thread;
    bool thread_in_use;
    UINT thread_id;
    UX_DEVICE *device;
    INT hid_instance_index;
    bool device_disconnected;
    ULONG demo_stack[DEMO_STACK_SIZE / sizeof(ULONG)];
    ULONG keyboard_queue[KEYBOARD_QUEUE_SIZE / sizeof(ULONG)];
} KEYBOARD_THREAD_DATA; 

/* Define global variables. */
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(64) static ULONG usb_memory[USBX_MEMORY_SIZE / sizeof(ULONG)];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(64) static char usb_memory_cachesafe[USBX_MEMORY_CACHESAFE_SIZE];

KEYBOARD_THREAD_DATA keyboard_data[UX_MAX_DEVICES];
volatile uint32_t current_keyboard_idle_thread_id = 0;

extern void usb_hc_low_level_init(void);
extern void usb_hc_low_level2_init(void);

static void reset_thread_data(KEYBOARD_THREAD_DATA *data, UINT index)
{
    data->thread_id = index;
    data->hid = NULL;
    data->device = NULL;
    data->thread_in_use = false;
    data->hid_instance_index = -1;
}

static UINT demo_class_hid_get(KEYBOARD_THREAD_DATA *data)
{
    UX_HOST_CLASS_HID *hid;
    UX_HOST_CLASS *class;
    UINT status;
    UCHAR class_index = 0;

    /* Find the main HID container */
    status = ux_host_stack_class_get(_ux_system_host_class_hid_name, &class);
    if (status != UX_SUCCESS)
        return status;

    /* We get the first instance of the hid device */
    do {
        hid = NULL;
        status = ux_host_stack_class_instance_get(class, class_index, (void **)&hid);
        if (status != UX_SUCCESS)
            return status;
        if (hid != NULL) {
            if (strcmp((const char*)hid->ux_host_class_hid_client->ux_host_class_hid_client_name, \
                       (const char*)_ux_system_host_class_hid_client_keyboard_name) == 0) {
                for (UINT i = 0; i < UX_MAX_DEVICES; i++) {
                    if ((UINT)&keyboard_data[i] != (UINT)data) {
                        if (keyboard_data[i].hid == hid) {
                            goto try_next;
                        }
                    }
                }
                data->hid = hid;
                data->hid_instance_index = class_index;
                printf("set thread %d hid to %d\n", data->thread_id, data->hid_instance_index);
                break;
            }
        }
        try_next:
        class_index++;
        class_index = class_index % 30;
        tx_thread_sleep(10);
    } while (1);

    /* We still need to wait for the hid status to be live */
    while (hid->ux_host_class_hid_state != UX_HOST_CLASS_INSTANCE_LIVE)
        tx_thread_sleep(10);

    data->device = hid->ux_host_class_hid_device;
    data->device_disconnected = false;

    return UX_SUCCESS;
}

static void demo_thread_entry(ULONG arg)
{
    UX_HOST_CLASS_HID_CLIENT *hid_client;
    UX_HOST_CLASS_HID_KEYBOARD *keyboard;
    ULONG keyboard_char;
    ULONG keyboard_state;
    UINT keyboard_queue_index;
    UINT status;
    KEYBOARD_THREAD_DATA *thread_data = (KEYBOARD_THREAD_DATA *)arg;
    UCHAR *queue = (UCHAR *)(thread_data->keyboard_queue);

    /* Find the HID class */
    status = demo_class_hid_get(thread_data);
    if (status != UX_SUCCESS)
        goto err;

    /* Get the HID client */
    hid_client = thread_data->hid->ux_host_class_hid_client;

    /* Check if the instance of the keyboard is live */
    while (hid_client->ux_host_class_hid_client_local_instance == UX_NULL)
        tx_thread_sleep(10);
    PRINTF("The keyboard is alive\n");

    /* Get the keyboard instance */
    keyboard = (UX_HOST_CLASS_HID_KEYBOARD *)hid_client->ux_host_class_hid_client_local_instance;

    /* Init the keyboard queue index.  */
    keyboard_queue_index = 0;

    while (!thread_data->device_disconnected) {
        /* Get a key/state from the keyboard.  */
        status = ux_host_class_hid_keyboard_key_get(keyboard, &keyboard_char, &keyboard_state);
        if (status == UX_SUCCESS) {
            /* We have a character in the queue.  */
            queue[keyboard_queue_index] = (UCHAR)keyboard_char;

            PRINTF("[keyboard %x]Input: %s\r\n", keyboard, (CHAR *)&keyboard_char);

            /* Can we accept more ?  */
            if (keyboard_queue_index < KEYBOARD_QUEUE_SIZE)
                /* Increment the index.  */
                keyboard_queue_index++;
            else
                /* Back to the beginning of the array.  */
                keyboard_queue_index = 0;
        }

        tx_thread_sleep(10);
    }
    PRINTF("[keyboard %x] disconneted, thread (0x%x)%d exit\n", keyboard, &thread_data->thread, thread_data->thread_id);
    reset_thread_data(thread_data, thread_data->thread_id);
    tx_thread_terminate(&thread_data->thread);
    return;

err:
    PRINTF("ERROR: 0x%x\r\n", status);
    
    while (1) {
        tx_thread_sleep(100);
    }
}

static UINT usbx_host_change_callback(ULONG event, UX_HOST_CLASS *host_class, VOID *instance)
{
    UX_DEVICE *device;

    /* Check if there is a device connection event, make sure the instance is valid.  */
    if ((event == UX_DEVICE_CONNECTION) && (instance != UX_NULL)) {
        /* Get the device instance.  */
        device = (UX_DEVICE *)instance;

        PRINTF("USB device: vid=0x%x, pid=0x%x\r\n", device->ux_device_descriptor.idVendor, device->ux_device_descriptor.idProduct);
        if (host_class != NULL)
            PRINTF("host class name %s\n", host_class->ux_host_class_name);
        /* Check if the device is configured.  */
        if (device->ux_device_state != UX_DEVICE_CONFIGURED) {
            PRINTF("The device is not CONFIGURED\n");
            /* Not configured. Check if there is another configuration.  */
            if ((device->ux_device_first_configuration != UX_NULL) && (device->ux_device_first_configuration->ux_configuration_next_configuration != UX_NULL)) {
                /* Try the second configuration.  */
                ux_host_stack_device_configuration_activate(device->ux_device_first_configuration->ux_configuration_next_configuration);
            }
        } else {

            UINT i = 0;
            for (i = 0; i < UX_MAX_DEVICES; i++) {
                /* Find the next idle thread */
                if (keyboard_data[current_keyboard_idle_thread_id].thread_in_use == false) {
                    if (keyboard_data[current_keyboard_idle_thread_id].thread.tx_thread_state == TX_TERMINATED) {
                        tx_thread_delete(&keyboard_data[current_keyboard_idle_thread_id].thread);
                    }

                    break;
                }
                current_keyboard_idle_thread_id++;
                current_keyboard_idle_thread_id %= UX_MAX_DEVICES;
            }
            if (i < UX_MAX_DEVICES) {
                KEYBOARD_THREAD_DATA *pthread_data = &keyboard_data[current_keyboard_idle_thread_id];
                /* Create the demo thread.  */
            
                UINT status = tx_thread_create(&pthread_data->thread, "keyboard thread", demo_thread_entry, (ULONG)pthread_data, (VOID *)pthread_data->demo_stack, DEMO_STACK_SIZE, 20, 20, 1, TX_AUTO_START);
                if (status != UX_SUCCESS) {
                    printf("thread create failed 0x%x\n", status);
                } else {
                    pthread_data->thread_in_use = true;
                }
            }
        }
    }

    if (event == UX_DEVICE_DISCONNECTION) {
        for (UINT i = 0; i < UX_MAX_DEVICES; i++) {
            if (keyboard_data[i].device == instance) {
                keyboard_data[i].device_disconnected = true;
                break;
            }
        }
    }
    return UX_SUCCESS;
}

static VOID usbx_mem_init(VOID)
{
    ux_system_initialize((VOID *)usb_memory, USBX_MEMORY_SIZE, usb_memory_cachesafe, USBX_MEMORY_CACHESAFE_SIZE);
}

static ULONG usb_host_base(VOID)
{
    /* For EHCI core.  */
    return (HPM_USB0_BASE + 0x100);
}

static UINT usbx_host_hcd_register(VOID)
{
    UINT status;

    status = ux_host_stack_hcd_register((UCHAR *)UX_HCD_NAME, _ux_hcd_ehci_initialize, usb_host_base(), (ULONG)106);

    return status;
}
void tx_application_define(void *first_unused_memory)
{
    UINT status;

    TX_THREAD_NOT_USED(first_unused_memory);

    for (UINT i = 0; i < UX_MAX_DEVICES; i++) {
        reset_thread_data(&keyboard_data[i], i);
    }

    /* Initialize USBX memory. */
    usbx_mem_init();

    /* The code below is required for installing the host portion of USBX */
    status = ux_host_stack_initialize(usbx_host_change_callback);
    if (status != UX_SUCCESS)
        goto err;

    /* Register the HUB class. */
    status = ux_host_stack_class_register(_ux_system_host_class_hub_name, ux_host_class_hub_entry);
    if (status != UX_SUCCESS)
        goto err;

    /* Register the hid class.  */
    status = ux_host_stack_class_register(_ux_system_host_class_hid_name, ux_host_class_hid_entry);
    if (status != UX_SUCCESS)
        goto err;

    /* Register the HID client(s).  */
    status = ux_host_class_hid_client_register(_ux_system_host_class_hid_client_keyboard_name, ux_host_class_hid_keyboard_entry);
    if (status != UX_SUCCESS)
        goto err;

    status = usbx_host_hcd_register();
    if (status != UX_SUCCESS)
        goto err;

    return;

err:
    PRINTF("ERROR: 0x%x\r\n", status);

    while (1) {
        tx_thread_sleep(100);
    }
}

void usb_host_hw_setup(void)
{
    usb_hc_low_level_init();
    usb_hc_low_level2_init();
}

int main(void)
{
    /* Initialize the board. */
    board_init();
    board_init_usb(HPM_USB0);
    board_init_gpio_pins();

    usb_host_hw_setup();

    PRINTF("USBX host HID Keyboard example\r\n");

    /* Enter the ThreadX kernel. */
    tx_kernel_enter();

    return 0;
}