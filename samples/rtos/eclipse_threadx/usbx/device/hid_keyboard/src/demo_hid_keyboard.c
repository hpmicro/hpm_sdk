/* This is a small demo of the high-performance FileX FAT file system. It includes setup for
 * a small 34KB RAM disk and a loop that writes and reads a small file.  */
#include "board.h"
/* This example illustrates USBX Device HID Mouse */

#include "ux_api.h"
#include "ux_device_class_hid.h"
#include "hpm_usb_device.h"

#define HID_NUM_LOCK_MASK 1
#define HID_CAPS_LOCK_MASK 2

#define UX_DEMO_STACK_SIZE (1024 * 4)

#ifndef USBX_MEMORY_SIZE
#define USBX_MEMORY_SIZE (1024 * 32)
#endif

/* Define global data structures.  */
TX_THREAD demo_thread;
ULONG num_lock_flag = UX_FALSE;
ULONG caps_lock_flag = UX_FALSE;

static UX_SLAVE_CLASS_HID_PARAMETER hid_parameter;

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(64) static ULONG usb_memory[USBX_MEMORY_SIZE / sizeof(ULONG)];

static ULONG demo_thread_stack[UX_DEMO_STACK_SIZE / sizeof(ULONG)];

#define DEVICE_FRAMEWORK_LENGTH_FULL_SPEED (sizeof(device_framework_full_speed))
UCHAR device_framework_full_speed[] = {
    /* Device descriptor */
    0x12,
    0x01,
    0x10,
    0x01,
    0x00,
    0x00,
    0x00,
    0x40,
    0xB7,
    0x34,
    0xB6,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,

    /* Configuration descriptor */
    0x09,
    0x02,
    0x22,
    0x00,
    0x01,
    0x01,
    0x00,
    0xc0,
    0x32,

    /* Interface descriptor */
    0x09,
    0x04,
    0x00,
    0x00,
    0x01,
    0x03,
    0x00,
    0x01,
    0x00,

    /* HID descriptor */
    0x09,
    0x21,
    0x10,
    0x01,
    0x21,
    0x01,
    0x22,
    0x3f,
    0x00,

    /* Endpoint descriptor (Interrupt) */
    0x07,
    0x05,
    0x81,
    0x03,
    0x08,
    0x00,
    0x08
};

#define DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED (sizeof(device_framework_high_speed))
UCHAR device_framework_high_speed[] = {
    /* Device descriptor */
    0x12,
    0x01,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0xB7,
    0x34,
    0xB6,
    0x00,
    0x01,
    0x00,
    0x01,
    0x02,
    0x03,
    0x01,

    /* Device qualifier descriptor */
    0x0a,
    0x06,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,

    /* Configuration descriptor */
    0x09,
    0x02,
    0x22,
    0x00,
    0x01,
    0x01,
    0x00,
    0xc0,
    0x32,

    /* Interface descriptor */
    0x09,
    0x04,
    0x00,
    0x00,
    0x01,
    0x03,
    0x00,
    0x01,
    0x00,

    /* HID descriptor */
    0x09,
    0x21,
    0x10,
    0x01,
    0x21,
    0x01,
    0x22,
    0x3f,
    0x00,

    /* Endpoint descriptor (Interrupt) */
    0x07,
    0x05,
    0x81,
    0x03,
    0x08,
    0x00,
    0x08
};

/* String Device Framework :
 Byte 0 and 1 : Word containing the language ID : 0x0904 for US
 Byte 2       : Byte containing the index of the descriptor
 Byte 3       : Byte containing the length of the descriptor string
*/

#define STRING_FRAMEWORK_LENGTH (sizeof(string_framework))
UCHAR string_framework[] = {
    /* Manufacturer string descriptor : Index 1 */
    0x09,
    0x04,
    0x01,
    21U,
    'H',
    'P',
    'M',
    'I',
    'C',
    'R',
    'O',
    ' ',
    'S',
    'E',
    'M',
    'I',
    'C',
    'O',
    'N',
    'D',
    'U',
    'C',
    'T',
    'O',
    'R',

    /* Product string descriptor : Index 2 */
    0x09,
    0x04,
    0x02,
    19U,
    'H',
    'I',
    'D',
    ' ',
    'k',
    'e',
    'y',
    'b',
    'o',
    'a',
    'r',
    'd',
    ' ',
    'D',
    'E',
    'V',
    'I',
    'C',
    'E',

    /* Serial Number string descriptor : Index 3 */
    0x09,
    0x04,
    0x03,
    0x04,
    0x30,
    0x30,
    0x30,
    0x31
};

/* Multiple languages are supported on the device, to add
   a language besides english, the unicode language code must
   be appended to the language_id_framework array and the length
   adjusted accordingly. */
#define LANGUAGE_ID_FRAMEWORK_LENGTH (sizeof(language_id_framework))
UCHAR language_id_framework[] = {
    /* English. */
    0x09,
    0x04
};

#define HID_KEYBOARD_REPORT_LENGTH 63
UCHAR hid_keyboard_report[HID_KEYBOARD_REPORT_LENGTH] = {
    0x05,
    0x01, /* USAGE_PAGE (Generic Desktop) */
    0x09,
    0x06, /* USAGE (Keyboard) */
    0xa1,
    0x01, /* COLLECTION (Application) */
    0x05,
    0x07, /*   USAGE_PAGE (Keyboard) */
    0x19,
    0xe0, /*   USAGE_MINIMUM (Keyboard LeftControl) */
    0x29,
    0xe7, /*   USAGE_MAXIMUM (Keyboard Right GUI) */
    0x15,
    0x00, /*   LOGICAL_MINIMUM (0) */
    0x25,
    0x01, /*   LOGICAL_MAXIMUM (1) */
    0x75,
    0x01, /*   REPORT_SIZE (1) */
    0x95,
    0x08, /*   REPORT_COUNT (8) */
    0x81,
    0x02, /*   INPUT (Data,Var,Abs) */
    0x95,
    0x01, /*   REPORT_COUNT (1) */
    0x75,
    0x08, /*   REPORT_SIZE (8) */
    0x81,
    0x03, /*   INPUT (Cnst,Var,Abs) */
    0x95,
    0x05, /*   REPORT_COUNT (5) */
    0x75,
    0x01, /*   REPORT_SIZE (1) */
    0x05,
    0x08, /*   USAGE_PAGE (LEDs) */
    0x19,
    0x01, /*   USAGE_MINIMUM (Num Lock) */
    0x29,
    0x05, /*   USAGE_MAXIMUM (Kana) */
    0x91,
    0x02, /*   OUTPUT (Data,Var,Abs) */
    0x95,
    0x01, /*   REPORT_COUNT (1) */
    0x75,
    0x03, /*   REPORT_SIZE (3) */
    0x91,
    0x03, /*   OUTPUT (Cnst,Var,Abs) */
    0x95,
    0x06, /*   REPORT_COUNT (6) */
    0x75,
    0x08, /*   REPORT_SIZE (8) */
    0x15,
    0x00, /*   LOGICAL_MINIMUM (0) */
    0x25,
    0x65, /*   LOGICAL_MAXIMUM (101) */
    0x05,
    0x07, /*   USAGE_PAGE (Keyboard) */
    0x19,
    0x00, /*   USAGE_MINIMUM (Reserved (no event indicated)) */
    0x29,
    0x65, /*   USAGE_MAXIMUM (Keyboard Application) */
    0x81,
    0x00, /*   INPUT (Data,Ary,Abs) */
    0xc0  /* END_COLLECTION */
};

static UINT demo_thread_hid_callback(UX_SLAVE_CLASS_HID *hid, UX_SLAVE_CLASS_HID_EVENT *hid_event)
{
    (void)hid;
    /* There was an event.  Analyze it.  Is it NUM LOCK ? */
    if (hid_event->ux_device_class_hid_event_buffer[0] & HID_NUM_LOCK_MASK)
        /* Set the Num lock flag.  */
        num_lock_flag = UX_TRUE;
    else
        /* Reset the Num lock flag.  */
        num_lock_flag = UX_FALSE;

    /* There was an event.  Analyze it.  Is it CAPS LOCK ? */
    if (hid_event->ux_device_class_hid_event_buffer[0] & HID_CAPS_LOCK_MASK)
        /* Set the Caps lock flag.  */
        caps_lock_flag = UX_TRUE;
    else
        /* Reset the Caps lock flag.  */
        caps_lock_flag = UX_FALSE;

    return UX_SUCCESS;
}

static VOID demo_hid_instance_activate(VOID *hid_instance)
{
    (void)hid_instance;
    printf("HID device activate\r\n");
}

static void demo_thread_entry(ULONG arg)
{
    UX_SLAVE_DEVICE *device;
    UX_SLAVE_INTERFACE *interface;
    UX_SLAVE_CLASS_HID *hid;
    UX_SLAVE_CLASS_HID_EVENT hid_event;
    UCHAR key;
    (void)arg;
    /* Get the pointer to the device.  */
    device = &_ux_system_slave->ux_system_slave_device;

    /* Set the first key to 'a' which is 04.  */
    key = 0x04;

    /* reset the HID event structure.  */
    ux_utility_memory_set(&hid_event, 0, sizeof(UX_SLAVE_CLASS_HID_EVENT));

    while (1) {
        /* Is the device configured ? */
        while (device->ux_slave_device_state != UX_DEVICE_CONFIGURED) {
            /* Then wait.  */
            tx_thread_sleep(10);
        }

        /* Until the device stays configured.  */
        while (device->ux_slave_device_state == UX_DEVICE_CONFIGURED) {
            /* Get the interface.  We use the first interface, this is a simple device.  */
            interface = device->ux_slave_device_first_interface;

            /* Form that interface, derive the HID owner.  */
            hid = interface->ux_slave_interface_class_instance;

            /* Wait for 2 seconds. */
            tx_thread_sleep(2 * TX_TIMER_TICKS_PER_SECOND);

            /* Then insert a key into the keyboard event.  Length is fixed to 8.  */
            hid_event.ux_device_class_hid_event_length = 8;

            /* First byte is a modifier byte.  */
            hid_event.ux_device_class_hid_event_buffer[0] = 0;

            /* Second byte is reserved. */
            hid_event.ux_device_class_hid_event_buffer[1] = 0;

            /* The 6 next bytes are keys. We only have one key here.  */
            hid_event.ux_device_class_hid_event_buffer[2] = key;

            /* Set the keyboard event.  */
            ux_device_class_hid_event_set(hid, &hid_event);

            /* Next event has the key depressed.  */
            hid_event.ux_device_class_hid_event_buffer[2] = 0;

            /* Length is fixed to 8.  */
            hid_event.ux_device_class_hid_event_length = 8;

            /* Set the keyboard event.  */
            ux_device_class_hid_event_set(hid, &hid_event);

            /* Are we at the end of alphabet ?  */
            if (key != (0x04 + 26))
                /* Next key.  */
                key++;
            else
                /* Start over again.  */
                key = 0x04;
        }
    }
}

usb_device_handle_t usb_handle;
static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(USB_SOC_DCD_DATA_RAM_ADDRESS_ALIGNMENT) dcd_data_t _dcd_data;
static void usb_device_hw_setup(void)
{
    usb_handle.regs = (USB_Type *)HPM_USB0_BASE;
    usb_handle.dcd_data = &_dcd_data;
    uint32_t int_mask;
    int_mask = (USB_USBINTR_UE_MASK | USB_USBINTR_UEE_MASK | USB_USBINTR_PCE_MASK | USB_USBINTR_URE_MASK);

    usb_device_init(&usb_handle, int_mask);
}
extern void usb_device_setup(void);

int main(void)
{
    /* Initialize the board. */
    board_init();
    board_init_usb(HPM_USB0);
    board_init_gpio_pins();

    usb_device_hw_setup();

    printf("USBX device HID Keyboard example\r\n");

    /* Enter the ThreadX kernel. */
    tx_kernel_enter();

    return 0;
}

void tx_application_define(void *first_unused_memory)
{
    UINT status;

    UX_PARAMETER_NOT_USED(first_unused_memory);

    /* Initialize USBX Memory */
    status = ux_system_initialize((VOID *)usb_memory, USBX_MEMORY_SIZE, UX_NULL, 0);
    if (status != UX_SUCCESS)
        goto err;

    usb_device_setup();

    /* The code below is required for installing the device portion of USBX. No call back for
        device status change in this example. */
    status = ux_device_stack_initialize(device_framework_high_speed,
        DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED,
        device_framework_full_speed,
        DEVICE_FRAMEWORK_LENGTH_FULL_SPEED,
        string_framework,
        STRING_FRAMEWORK_LENGTH,
        language_id_framework,
        LANGUAGE_ID_FRAMEWORK_LENGTH,
        UX_NULL);
    if (status != UX_SUCCESS)
        goto err;

    /* Initialize the hid class parameters for a keyboard.  */
    hid_parameter.ux_device_class_hid_parameter_report_address = hid_keyboard_report;
    hid_parameter.ux_device_class_hid_parameter_report_length = HID_KEYBOARD_REPORT_LENGTH;
    hid_parameter.ux_device_class_hid_parameter_callback = demo_thread_hid_callback;
    hid_parameter.ux_slave_class_hid_instance_activate = demo_hid_instance_activate;

    /* Initialize the device hid class. The class is connected with interface 0 */
    status = ux_device_stack_class_register(_ux_system_slave_class_hid_name, ux_device_class_hid_entry, 1, 0, (VOID *)&hid_parameter);

    if (status != UX_SUCCESS)
        goto err;

    /* Create the main demo thread.  */
    tx_thread_create(&demo_thread, "USBX demo", demo_thread_entry, 0, (VOID *)demo_thread_stack, UX_DEMO_STACK_SIZE, 20, 20, 1, TX_AUTO_START);

    /* Then wait.  */
    tx_thread_sleep(100);

    return;

err:
    printf("tx_application_define: ERROR 0x%x\r\n", status);
    while (1) {
    }
}