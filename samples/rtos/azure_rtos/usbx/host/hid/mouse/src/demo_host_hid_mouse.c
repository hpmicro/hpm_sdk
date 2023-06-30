/* This is a small demo of the high-performance FileX FAT file system. It includes setup for
 * a small 34KB RAM disk and a loop that writes and reads a small file.  */
#include "board.h"
/* This example illustrates USBX Device HID Mouse */

#include "ux_api.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hub.h"
#include "ux_host_class_hid_mouse.h"

#include "hpm_usb_host.h"

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
/* Define global variables. */
TX_THREAD demo_thread;
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(64) static ULONG usb_memory[USBX_MEMORY_SIZE / sizeof(ULONG)];
ATTR_PLACE_AT_NONCACHEABLE_INIT_WITH_ALIGNMENT(64) static char usb_memory_cachesafe[USBX_MEMORY_CACHESAFE_SIZE];
ULONG demo_stack[DEMO_STACK_SIZE / sizeof(ULONG)];

extern void usb_hc_low_level_init(void);
extern void usb_hc_low_level2_init(void);

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

static VOID usbx_mem_init(VOID)
{
    ux_system_initialize((VOID *)usb_memory, USBX_MEMORY_SIZE, usb_memory_cachesafe, USBX_MEMORY_CACHESAFE_SIZE);
}

static UINT usbx_host_change_callback(ULONG event, UX_HOST_CLASS *host_class, VOID *instance)
{
    UX_DEVICE *device;

    /* Check if there is a device connection event, make sure the instance is valid.  */
    if ((event == UX_DEVICE_CONNECTION) && (instance != UX_NULL)) {
        /* Get the device instance.  */
        device = (UX_DEVICE *)instance;

        PRINTF("USB device: vid=0x%x, pid=0x%x\r\n", device->ux_device_descriptor.idVendor, device->ux_device_descriptor.idProduct);

        /* Check if the device is configured.  */
        if (device->ux_device_state != UX_DEVICE_CONFIGURED) {
            /* Not configured. Check if there is another configuration.  */
            if ((device->ux_device_first_configuration != UX_NULL) && (device->ux_device_first_configuration->ux_configuration_next_configuration != UX_NULL)) {
                /* Try the second configuration.  */
                ux_host_stack_device_configuration_activate(device->ux_device_first_configuration->ux_configuration_next_configuration);
            }
        }
    }

    return UX_SUCCESS;
}

static UINT demo_class_hid_get(UX_HOST_CLASS_HID **hid_ptr)
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
        if (hid != NULL) {
            if (strcmp(hid->ux_host_class_hid_client->ux_host_class_hid_client_name, _ux_system_host_class_hid_client_mouse_name) == 0)
                break;
        }
        class_index++;
        class_index = class_index % 10;
        tx_thread_sleep(10);
    } while (1);

    /* We still need to wait for the hid status to be live */
    while (hid->ux_host_class_hid_state != UX_HOST_CLASS_INSTANCE_LIVE)
        tx_thread_sleep(10);

    *hid_ptr = hid;

    return UX_SUCCESS;
}

static void demo_thread_entry(ULONG arg)
{
    UX_HOST_CLASS_HID *hid;
    UX_HOST_CLASS_HID_CLIENT *hid_client;
    UX_HOST_CLASS_HID_MOUSE *mouse;
    ULONG mouse_button;
    SLONG mouse_px;
    SLONG mouse_py;
    SLONG pre_wheel;
    SLONG mouse_cx = 0;
    SLONG mouse_cy = 0;
    SLONG cur_wheel = 0;
    UINT status;

    /* Find the HID class */
    status = demo_class_hid_get(&hid);
    if (status != UX_SUCCESS)
        goto err;

    /* Get the HID client */
    hid_client = hid->ux_host_class_hid_client;

    /* Check if the instance of the mouse is live */
    while (hid_client->ux_host_class_hid_client_local_instance == UX_NULL)
        tx_thread_sleep(10);

    /* Get the mouse instance */
    mouse = (UX_HOST_CLASS_HID_MOUSE *)hid_client->ux_host_class_hid_client_local_instance;

    while (1) {
        /* Get a key/state from the keyboard.  */
        status = _ux_host_class_hid_mouse_buttons_get(mouse, &mouse_button);
        if (status == UX_SUCCESS) {
            if (mouse_button & UX_HOST_CLASS_HID_MOUSE_BUTTON_1_PRESSED) {
                PRINTF("Left Click\r\n");
            }
            if (mouse_button & UX_HOST_CLASS_HID_MOUSE_BUTTON_2_PRESSED) {
                PRINTF("Right Click\r\n");
            }
            if (mouse_button & UX_HOST_CLASS_HID_MOUSE_BUTTON_3_PRESSED) {
                PRINTF("Middle Click\r\n");
            }
        }

        mouse_px = mouse_cx;
        mouse_py = mouse_cy;
        status = _ux_host_class_hid_mouse_position_get(mouse, &mouse_cx, &mouse_cy);
        if (status == UX_SUCCESS) {
            if (mouse_cx != mouse_px || mouse_cy != mouse_py) {
                PRINTF("Move to: x:%d y:%d\r\n", mouse_cx, mouse_cy);
            }
        }

        pre_wheel = cur_wheel;
        status = _ux_host_class_hid_mouse_wheel_get(mouse, &cur_wheel);
        if (status == UX_SUCCESS) {
            if (cur_wheel < pre_wheel) {
                PRINTF("Wheel Down\r\n");
            } else if (cur_wheel > pre_wheel) {
                PRINTF("Wheel UP\r\n");
            }
        }

        tx_thread_sleep(10);
    }

err:
    PRINTF("ERROR: 0x%x\r\n", status);

    while (1) {
        tx_thread_sleep(100);
    }
}

void tx_application_define(void *first_unused_memory)
{
    UINT status;

    TX_THREAD_NOT_USED(first_unused_memory);

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
    status = ux_host_class_hid_client_register(_ux_system_host_class_hid_client_mouse_name, ux_host_class_hid_mouse_entry);
    if (status != UX_SUCCESS)
        goto err;

    status = usbx_host_hcd_register();
    if (status != UX_SUCCESS)
        goto err;

    /* Create the main demo thread.  */
    tx_thread_create(&demo_thread, "main thread", demo_thread_entry, 0, (VOID *)demo_stack, DEMO_STACK_SIZE, 20, 20, 1, TX_AUTO_START);

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
    board_init_usb_pins();
    board_init_gpio_pins();

    usb_host_hw_setup();

    PRINTF("USBX host HID Mouse example\r\n");

    /* Enter the ThreadX kernel. */
    tx_kernel_enter();

    return 0;
}